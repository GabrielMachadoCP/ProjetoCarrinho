/*
  ======================================================================
  DRENAHUB - DRENABOT 2WD | ESP32 + L298N + HC-SR04 + SENSOR DE AGUA
  ======================================================================

  Global Solution CitizenTech - Project-Based Maker Lab - FIAP 4ESPY

  O firmware evolui o carrinho 2WD anterior para inspecao preventiva de
  areas sujeitas a acumulacao de agua. Toda decisao basica e executada
  localmente, sem depender de internet.

  Modos:
  - Manual: direcao pelo painel web embarcado.
  - Autonomo: leitura dos sensores, inferencia TinyML e reacao do robo.

  Classes TinyML:
  0 = NORMAL
  1 = ATENCAO
  2 = CRITICO

  IMPORTANTE:
  - Calibre o sensor seco e molhado no painel antes da demonstracao.
  - O ECHO do HC-SR04 deve passar pelo divisor de tensao 1k/2k.
  - Eletronica e agua devem permanecer fisicamente separadas.
  ======================================================================
*/

#include <Preferences.h>
#include <WebServer.h>
#include <WiFi.h>

#include "drenahub_model.h"

// ----------------------------------------------------------------------
// PINAGEM CONFIRMADA DO CARRINHO ANTERIOR
// ----------------------------------------------------------------------
constexpr int IN1 = 25;
constexpr int IN2 = 26;
constexpr int IN3 = 33;
constexpr int IN4 = 32;

constexpr int TRIG_PIN = 18;
constexpr int ECHO_PIN = 19;

// ADC1: funciona normalmente enquanto o Wi-Fi do ESP32 esta ativo.
constexpr int WATER_SENSOR_PIN = 34;

constexpr int STATUS_LED_PIN = 2;   // LED embarcado na maioria dos DevKit.
constexpr int BUZZER_PIN = 27;      // Opcional.
constexpr bool BUZZER_INSTALLED = false;

// ----------------------------------------------------------------------
// REDE LOCAL DO PROTOTIPO
// ----------------------------------------------------------------------
const char* WIFI_SSID = "DrenaHub-DrenaBot";
const char* WIFI_PASSWORD = "DrenaHub2026";

WebServer server(80);
Preferences preferences;

// ----------------------------------------------------------------------
// CONFIGURACAO E ESTADO
// ----------------------------------------------------------------------
enum OperatingMode { MODE_MANUAL, MODE_AUTONOMOUS };
enum RiskClass { RISK_NORMAL = 0, RISK_ATTENTION = 1, RISK_CRITICAL = 2, RISK_FAULT = 3 };

OperatingMode operatingMode = MODE_MANUAL;
RiskClass currentRisk = RISK_NORMAL;

constexpr int FILTER_WINDOW = 8;
int waterReadings[FILTER_WINDOW] = {0};
int waterIndex = 0;
bool waterWindowReady = false;

int dryCalibration = 450;
int wetCalibration = 3000;

int waterRaw = 0;
float waterFiltered = 0.0f;
float waterLevel = 0.0f;
float previousWaterLevel = 0.0f;
float riseRate = 0.0f;
float distanceCm = -1.0f;
float obstacleRisk = 0.0f;
float classProbabilities[3] = {1.0f, 0.0f, 0.0f};

bool waterPresent = false;
bool waterAlertActive = false;
bool waterAlertAcknowledged = false;
bool suppressWaterAlertUntilDry = false;
unsigned int waterEventCount = 0;
unsigned long lastWaterEventAt = 0;
float lastWaterEventLevel = 0.0f;

String currentAction = "PARADO";
String lastEvent = "Sistema iniciado";
unsigned long lastSensorUpdate = 0;
unsigned long lastSerialLog = 0;

constexpr unsigned long SENSOR_INTERVAL_MS = 200;
constexpr float WATER_DETECTION_THRESHOLD = 0.30f;
constexpr float WATER_CLEAR_THRESHOLD = 0.15f;
// 15 cm mantém o desvio simples sem confundir paredes ou objetos distantes
// com um bloqueio imediato da patrulha.
constexpr float OBSTACLE_AVOID_DISTANCE_CM = 15.0f;

// ----------------------------------------------------------------------
// PAGINA WEB EMBARCADA
// ----------------------------------------------------------------------
const char PAGE_HTML[] PROGMEM = R"rawliteral(
<!doctype html>
<html lang="pt-BR">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Central DrenaHub</title>
  <style>
    :root{--navy:#082744;--blue:#0d6e9f;--cyan:#20a9bd;--bg:#edf4f7;--card:#fff;--text:#173042;--muted:#607783;--green:#168657;--yellow:#d58a11;--red:#c43c45;--softred:#fff0f1}
    *{box-sizing:border-box}body{margin:0;background:var(--bg);font-family:Arial,sans-serif;color:var(--text)}
    header{background:linear-gradient(135deg,var(--navy),var(--blue));color:#fff;padding:22px 18px}
    .head{max-width:980px;margin:auto;display:flex;justify-content:space-between;gap:16px;align-items:center}.head h1{font-size:27px;margin:0}.head p{margin:6px 0 0;opacity:.86}.connection{font-size:13px;background:#ffffff1c;padding:9px 12px;border-radius:999px;white-space:nowrap}.dot{display:inline-block;width:9px;height:9px;border-radius:50%;background:#6ee7a7;margin-right:7px}
    main{max-width:980px;margin:auto;padding:18px}.card{background:var(--card);border-radius:14px;padding:16px;box-shadow:0 4px 14px #17304212}.section{margin-top:14px}.grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(205px,1fr));gap:13px}.label{font-size:12px;color:var(--muted);text-transform:uppercase;letter-spacing:.06em}.value{font-size:26px;font-weight:700;margin-top:6px}.note{font-size:13px;color:var(--muted);line-height:1.45}.small{font-size:14px;font-weight:600}.statusline{display:flex;gap:10px;flex-wrap:wrap;align-items:center}.risk{display:inline-block;padding:8px 13px;border-radius:999px;color:#fff;font-weight:bold}.NORMAL{background:var(--green)}.ATENCAO{background:var(--yellow)}.CRITICO,.FALHA{background:var(--red)}
    .alert-panel{display:none;border:2px solid var(--red);background:var(--softred);border-radius:15px;padding:17px;grid-template-columns:1fr auto;gap:12px;align-items:center}.alert-panel.show{display:grid}.alert-panel.ack{border-color:var(--yellow);background:#fff8e8}.alert-title{color:var(--red);font-size:20px;font-weight:800;margin-bottom:5px}.alert-panel.ack .alert-title{color:#a36909}.alert-button{border:0;background:var(--red);color:#fff;border-radius:10px;padding:13px 16px;font-weight:bold;cursor:pointer}.alert-button:disabled{background:#9a9a9a;cursor:default}
    .bar{height:12px;background:#dae5ea;border-radius:8px;overflow:hidden;margin-top:10px}.fill{height:100%;width:0;background:var(--cyan);transition:.3s}.water-found{color:var(--red)}
    .patrol{display:grid;grid-template-columns:1fr 1fr;gap:10px}.action{border:0;border-radius:11px;background:var(--navy);color:#fff;min-height:48px;padding:0 14px;font-size:14px;font-weight:bold;cursor:pointer;transition:transform .08s,opacity .2s}.action:active,.controls button:active{transform:scale(.98)}.action:disabled,.controls button:disabled{opacity:.45;cursor:not-allowed}.action.auto{background:var(--green)}.action.emergency{background:var(--red);grid-column:1/-1;min-height:56px;font-size:16px}.action.cal{background:var(--blue);margin:4px}.action.secondary{background:#61727d}.safety-note{margin:12px 0 0;padding:10px 12px;background:#f3f7f9;border-radius:10px;color:var(--muted);font-size:13px;line-height:1.4}
    .controls{display:grid;grid-template-columns:repeat(3,82px);gap:8px;justify-content:center;margin-top:13px}.controls button{border:0;border-radius:11px;background:var(--navy);color:#fff;min-height:58px;font-size:20px;font-weight:bold;cursor:pointer}.controls .stop{background:var(--red)}
    .eventbox{border-left:4px solid var(--cyan);padding-left:12px;margin-top:10px}.footer{text-align:center;color:var(--muted);font-size:12px;padding:22px}
    @media(max-width:600px){header{padding:17px 14px}.head{align-items:flex-start}.head h1{font-size:23px}.connection{font-size:0}.connection .dot{margin:0}main{padding:12px}.patrol{grid-template-columns:1fr}.action.emergency{grid-column:auto}.alert-panel{grid-template-columns:1fr}.alert-button{width:100%}}
  </style>
</head>
<body>
  <header>
    <div class="head">
      <div><h1>Central DrenaHub</h1><p>DrenaBot 01 · patrulha autônoma e alerta de água</p></div>
      <div id="connection" class="connection"><span class="dot"></span>Robô conectado</div>
    </div>
  </header>
  <main>
    <section id="alertPanel" class="alert-panel">
      <div>
        <div id="alertTitle" class="alert-title">ÁGUA DETECTADA</div>
        <div id="alertMessage" class="small">O DrenaBot encontrou água e interrompeu a patrulha.</div>
        <div id="alertDetail" class="note">Aguardando dados...</div>
      </div>
      <button id="ackButton" class="alert-button" onclick="command('/alert/ack')">RECONHECER ALERTA</button>
    </section>

    <div class="card statusline section">
      <span id="risk" class="risk NORMAL">NORMAL</span>
      <span id="mode" class="small">Patrulha manual</span>
      <span id="action" class="small">Ação: PARADO</span>
    </div>

    <div class="card section">
      <div class="label">Comando da patrulha</div>
      <p id="modeHint" class="note">O carrinho está parado e aguarda um comando.</p>
      <div class="patrol">
        <button id="autoButton" class="action auto" onclick="confirmAutonomous()">INICIAR MODO AUTÔNOMO</button>
        <button id="manualButton" class="action secondary" onclick="command('/mode/manual')">ASSUMIR CONTROLE MANUAL</button>
        <button class="action emergency" onclick="emergencyStop()">■ PARADA DE EMERGÊNCIA</button>
      </div>
      <div class="safety-note">No modo autônomo: caminho livre = avança · obstáculo a até 15 cm = recua e vira · água = para e alerta.</div>
    </div>

    <div class="grid section">
      <div class="card">
        <div class="label">Sensor de água</div>
        <div id="waterState" class="value">SECO</div>
        <div id="water" class="small">Nível 0%</div>
        <div class="bar"><div id="waterBar" class="fill"></div></div>
        <p id="raw" class="note">ADC: --</p>
      </div>
      <div class="card"><div class="label">Caminho à frente</div><div id="distance" class="value">-- cm</div><p class="note">HC-SR04 frontal</p></div>
      <div class="card"><div class="label">Decisão TinyML</div><div id="confidence" class="value">--%</div><p id="probs" class="note">N -- · A -- · C --</p></div>
      <div class="card"><div class="label">Ocorrências de água</div><div id="eventCount" class="value">0</div><p id="lastDetection" class="note">Nenhuma ocorrência</p></div>
    </div>

    <div class="card section">
      <div class="label">Movimentação manual</div>
      <div class="controls">
        <span></span><button data-hold="/move/forward">↑</button><span></span>
        <button data-hold="/move/left">←</button><button class="stop" onclick="emergencyStop()">■</button><button data-hold="/move/right">→</button>
        <span></span><button data-hold="/move/reverse">↓</button><span></span>
      </div>
    </div>

    <div class="card section">
      <div class="label">Atividade da central</div>
      <div class="eventbox"><div id="event" class="small">Sistema iniciado</div><div id="uptime" class="note">Tempo ativo: --</div></div>
    </div>

    <div class="card section">
      <div class="label">Calibração do sensor</div>
      <p class="note">Registre primeiro a leitura completamente seca. Depois encoste somente a área sensível na referência molhada da maquete.</p>
      <button class="action cal" onclick="calibrate('/calibrate/dry','Registrar a leitura atual como SECO?')">REGISTRAR SECO</button>
      <button class="action cal" onclick="calibrate('/calibrate/wet','Registrar a leitura atual como MOLHADO?')">REGISTRAR MOLHADO</button>
      <span id="calibration" class="note">Seco: -- · Molhado: --</span>
    </div>
  </main>
  <div class="footer">FIAP 4ESPY · Global Solution CitizenTech · Project-Based Maker Lab</div>
  <script>
    let seenWaterEvents = -1;
    async function command(path){
      try{
        const response=await fetch(path,{method:'POST',cache:'no-store'});
        if(!response.ok){alert(await response.text());return false}
        await update();
        return true;
      }catch(error){
        alert('Não foi possível enviar o comando ao carrinho. Verifique a conexão com a rede DrenaHub-DrenaBot.');
        return false;
      }
    }
    async function confirmAutonomous(){
      const confirmed=confirm('CONFIRMAR MODO AUTÔNOMO\n\nO carrinho começará a andar imediatamente quando o caminho estiver livre e seco. Deixe espaço à frente e mantenha acesso à parada de emergência.');
      if(confirmed){await command('/mode/auto')}
    }
    async function emergencyStop(){await command('/emergency-stop')}
    async function calibrate(path,message){if(confirm(message)){await command(path)}}
    document.querySelectorAll('[data-hold]').forEach(btn=>{
      const start=e=>{e.preventDefault();command(btn.dataset.hold)};
      const stop=e=>{e.preventDefault();command('/move/stop')};
      btn.addEventListener('mousedown',start);btn.addEventListener('mouseup',stop);btn.addEventListener('mouseleave',stop);
      btn.addEventListener('touchstart',start,{passive:false});btn.addEventListener('touchend',stop,{passive:false});
    });
    async function update(){
      try{
        const response=await fetch('/api/status',{cache:'no-store'});
        const d=await response.json();
        document.getElementById('connection').innerHTML='<span class="dot"></span>Robô conectado';
        const risk=document.getElementById('risk');risk.textContent=d.risk;risk.className='risk '+d.risk;
        document.getElementById('mode').textContent=d.mode==='autonomo'?'Patrulha autônoma':'Patrulha manual';
        document.getElementById('action').textContent='Ação: '+d.action;
        const autonomous=d.mode==='autonomo';
        const autoButton=document.getElementById('autoButton');
        autoButton.disabled=autonomous;
        autoButton.textContent=autonomous?'MODO AUTÔNOMO ATIVO':'INICIAR MODO AUTÔNOMO';
        document.getElementById('manualButton').disabled=!autonomous;
        document.getElementById('modeHint').textContent=autonomous?'Patrulha ativa. Use a parada de emergência para interromper imediatamente.':'Controle manual ativo. Confirme o modo autônomo para iniciar a patrulha.';
        document.querySelectorAll('[data-hold]').forEach(button=>button.disabled=autonomous);

        const waterPercent=Math.round(d.water_level*100);
        const state=document.getElementById('waterState');
        state.textContent=d.water_present?'ÁGUA DETECTADA':'SECO';
        state.className='value'+(d.water_present?' water-found':'');
        document.getElementById('water').textContent='Nível '+waterPercent+'%';
        document.getElementById('waterBar').style.width=waterPercent+'%';
        document.getElementById('waterBar').style.background=d.water_present?'var(--red)':'var(--cyan)';
        document.getElementById('raw').textContent='ADC: '+d.water_raw+' · Subida: '+d.rise_rate.toFixed(2);
        document.getElementById('distance').textContent=d.distance_cm<0?'Falha':d.distance_cm.toFixed(1)+' cm';
        document.getElementById('confidence').textContent=Math.round(d.confidence*100)+'%';
        document.getElementById('probs').textContent='N '+Math.round(d.probabilities[0]*100)+'% · A '+Math.round(d.probabilities[1]*100)+'% · C '+Math.round(d.probabilities[2]*100)+'%';
        document.getElementById('eventCount').textContent=d.alert.events;
        document.getElementById('lastDetection').textContent=d.alert.events?'Último nível: '+Math.round(d.alert.last_level*100)+'%':'Nenhuma ocorrência';

        const panel=document.getElementById('alertPanel');
        const button=document.getElementById('ackButton');
        if(d.alert.active){
          panel.className='alert-panel show'+(d.alert.acknowledged?' ack':'');
          document.getElementById('alertTitle').textContent=d.alert.acknowledged?'ALERTA RECONHECIDO':'ÁGUA DETECTADA';
          document.getElementById('alertMessage').textContent=d.water_present?'O DrenaBot encontrou água e interrompeu a patrulha.':'A água não está mais presente; confirme a ocorrência.';
          document.getElementById('alertDetail').textContent='Ocorrência #'+d.alert.events+' · nível '+Math.round(d.alert.last_level*100)+'% · registrada com '+Math.floor(d.alert.last_at_s)+' s de operação';
          button.disabled=d.alert.acknowledged;
          button.textContent=d.alert.acknowledged?'ALERTA RECONHECIDO':'RECONHECER ALERTA';
          document.title=(d.alert.acknowledged?'Alerta reconhecido':'⚠ Água detectada')+' | DrenaHub';
          if((seenWaterEvents<0||d.alert.events>seenWaterEvents)&&!d.alert.acknowledged&&navigator.vibrate){navigator.vibrate([250,120,250])}
        }else{
          panel.className='alert-panel';
          document.title='Central DrenaHub';
        }
        seenWaterEvents=d.alert.events;
        document.getElementById('event').textContent=d.last_event;
        document.getElementById('uptime').textContent='Tempo ativo: '+Math.floor(d.uptime_s)+' s';
        document.getElementById('calibration').textContent='Seco: '+d.calibration.dry+' · Molhado: '+d.calibration.wet;
      }catch(error){
        document.getElementById('connection').innerHTML='<span class="dot" style="background:#f16b72"></span>Sem conexão';
        document.getElementById('event').textContent='Sem comunicação com o DrenaBot';
      }
    }
    update();setInterval(update,800);
  </script>
</body>
</html>
)rawliteral";

// ----------------------------------------------------------------------
// UTILITARIOS
// ----------------------------------------------------------------------
float clamp01(float value) {
  if (value < 0.0f) return 0.0f;
  if (value > 1.0f) return 1.0f;
  return value;
}

const char* riskName(RiskClass risk) {
  switch (risk) {
    case RISK_NORMAL: return "NORMAL";
    case RISK_ATTENTION: return "ATENCAO";
    case RISK_CRITICAL: return "CRITICO";
    default: return "FALHA";
  }
}

const char* modeName() {
  return operatingMode == MODE_MANUAL ? "manual" : "autonomo";
}

void setEvent(const String& message) {
  lastEvent = message;
  Serial.print("[EVENTO] ");
  Serial.println(message);
}

void beep(int durationMs) {
  if (!BUZZER_INSTALLED) return;
  digitalWrite(BUZZER_PIN, HIGH);
  delay(durationMs);
  digitalWrite(BUZZER_PIN, LOW);
}

void updateWaterAlert() {
  if (!waterWindowReady) return;

  // A referência molhada da calibração não deve virar uma ocorrência.
  if (suppressWaterAlertUntilDry) {
    waterPresent = waterLevel >= WATER_CLEAR_THRESHOLD;
    if (!waterPresent) suppressWaterAlertUntilDry = false;
    return;
  }

  // Histerese: ativa em 30% e considera seco somente abaixo de 15%.
  const bool detectedNow = waterPresent
      ? waterLevel >= WATER_CLEAR_THRESHOLD
      : waterLevel >= WATER_DETECTION_THRESHOLD;

  if (detectedNow && !waterPresent) {
    waterPresent = true;
    waterAlertActive = true;
    waterAlertAcknowledged = false;
    waterEventCount++;
    lastWaterEventAt = millis();
    lastWaterEventLevel = waterLevel;
    setEvent("Agua detectada; alerta enviado para a Central DrenaHub");
    beep(250);
  } else if (detectedNow && waterPresent) {
    if (waterLevel > lastWaterEventLevel) lastWaterEventLevel = waterLevel;
  } else if (!detectedNow && waterPresent) {
    waterPresent = false;
    if (waterAlertAcknowledged) {
      waterAlertActive = false;
      setEvent("Ponto seco; ocorrencia encerrada");
    } else {
      setEvent("Agua nao esta mais presente; alerta aguarda reconhecimento");
    }
  }
}

// ----------------------------------------------------------------------
// MOTORES
// ----------------------------------------------------------------------
void stopMotors() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  currentAction = "PARADO";
}

void moveForward() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  currentAction = "FRENTE";
}

void moveReverse() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  currentAction = "RE";
}

void turnLeft() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  currentAction = "ESQUERDA";
}

void turnRight() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  currentAction = "DIREITA";
}

// ----------------------------------------------------------------------
// SENSORES E PRE-PROCESSAMENTO
// ----------------------------------------------------------------------
float readDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  const unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return -1.0f;
  return duration * 0.0343f / 2.0f;
}

float movingWaterAverage(int newReading) {
  waterReadings[waterIndex] = newReading;
  waterIndex++;
  if (waterIndex >= FILTER_WINDOW) {
    waterIndex = 0;
    waterWindowReady = true;
  }

  const int count = waterWindowReady ? FILTER_WINDOW : waterIndex;
  if (count <= 0) return static_cast<float>(newReading);

  long sum = 0;
  for (int i = 0; i < count; ++i) sum += waterReadings[i];
  return static_cast<float>(sum) / count;
}

float normalizeWater(float reading) {
  const int span = wetCalibration - dryCalibration;
  if (span < 100) return 0.0f;
  return clamp01((reading - dryCalibration) / static_cast<float>(span));
}

float calculateObstacleRisk(float distance) {
  if (distance < 0.0f) return 1.0f;
  if (distance >= 50.0f) return 0.0f;
  return clamp01((50.0f - distance) / 50.0f);
}

void updateSensorsAndInference(bool forceUpdate = false) {
  if (!forceUpdate && millis() - lastSensorUpdate < SENSOR_INTERVAL_MS) return;
  lastSensorUpdate = millis();

  waterRaw = analogRead(WATER_SENSOR_PIN);
  waterFiltered = movingWaterAverage(waterRaw);
  waterLevel = normalizeWater(waterFiltered);
  riseRate = clamp01((waterLevel - previousWaterLevel) * 8.0f);
  previousWaterLevel = waterLevel;

  distanceCm = readDistanceCm();
  obstacleRisk = calculateObstacleRisk(distanceCm);

  const float features[3] = {waterLevel, riseRate, obstacleRisk};
  const int predicted = DrenaHubModel::predict(features, classProbabilities);
  currentRisk = static_cast<RiskClass>(predicted);

  // Camadas deterministicas de seguranca acima do modelo.
  if (distanceCm < 0.0f || waterRaw >= 4090) {
    currentRisk = RISK_FAULT;
  } else if (waterLevel >= 0.75f || distanceCm <= 10.0f) {
    currentRisk = RISK_CRITICAL;
  } else if (waterLevel >= 0.30f && currentRisk == RISK_NORMAL) {
    currentRisk = RISK_ATTENTION;
  }

  updateWaterAlert();
  digitalWrite(STATUS_LED_PIN, currentRisk == RISK_NORMAL ? LOW : HIGH);

  if (millis() - lastSerialLog >= 1000) {
    lastSerialLog = millis();
    Serial.print("telemetry,");
    Serial.print(millis()); Serial.print(',');
    Serial.print(waterRaw); Serial.print(',');
    Serial.print(waterLevel, 4); Serial.print(',');
    Serial.print(riseRate, 4); Serial.print(',');
    Serial.print(distanceCm, 2); Serial.print(',');
    Serial.print(obstacleRisk, 4); Serial.print(',');
    Serial.println(riskName(currentRisk));
  }
}

// ----------------------------------------------------------------------
// DECISAO AUTONOMA
// ----------------------------------------------------------------------
void executeEscapeManeuver(const String& reason) {
  stopMotors();
  beep(180);
  delay(150);
  moveReverse();
  delay(450);
  turnRight();
  delay(450);
  stopMotors();
  setEvent(reason);
}

void runAutonomousMode() {
  // Mantem o comportamento do carrinho original: com leitura valida ele
  // sempre avanca ou desvia. O unico bloqueio novo e agua presente agora.
  if (waterLevel >= WATER_DETECTION_THRESHOLD) {
    stopMotors();
    currentAction = "AGUA DETECTADA - PARADO";
    return;
  }

  if (distanceCm < 0.0f) {
    stopMotors();
    currentAction = "FALHA NO HC-SR04";
    return;
  }

  if (distanceCm <= OBSTACLE_AVOID_DISTANCE_CM) {
    executeEscapeManeuver("Obstaculo detectado; desvio executado");
    return;
  }

  moveForward();
}

// ----------------------------------------------------------------------
// API E ROTAS WEB
// ----------------------------------------------------------------------
void sendOk(const String& response = "OK") {
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "text/plain; charset=utf-8", response);
}

void requireManual(void (*movement)()) {
  if (operatingMode == MODE_MANUAL) movement();
  sendOk();
}

String statusJson() {
  float confidence = classProbabilities[0];
  if (classProbabilities[1] > confidence) confidence = classProbabilities[1];
  if (classProbabilities[2] > confidence) confidence = classProbabilities[2];

  String json = "{";
  json += "\"device_id\":\"DRENABOT-01\",";
  json += "\"mode\":\"" + String(modeName()) + "\",";
  json += "\"risk\":\"" + String(riskName(currentRisk)) + "\",";
  json += "\"action\":\"" + currentAction + "\",";
  json += "\"water_raw\":" + String(waterRaw) + ",";
  json += "\"water_level\":" + String(waterLevel, 4) + ",";
  json += "\"water_present\":";
  json += waterPresent ? "true," : "false,";
  json += "\"rise_rate\":" + String(riseRate, 4) + ",";
  json += "\"distance_cm\":" + String(distanceCm, 2) + ",";
  json += "\"obstacle_risk\":" + String(obstacleRisk, 4) + ",";
  json += "\"confidence\":" + String(confidence, 4) + ",";
  json += "\"probabilities\":[" + String(classProbabilities[0], 4) + "," + String(classProbabilities[1], 4) + "," + String(classProbabilities[2], 4) + "],";
  json += "\"last_event\":\"" + lastEvent + "\",";
  json += "\"uptime_s\":" + String(millis() / 1000.0f, 1) + ",";
  json += "\"alert\":{";
  json += "\"active\":";
  json += waterAlertActive ? "true," : "false,";
  json += "\"acknowledged\":";
  json += waterAlertAcknowledged ? "true," : "false,";
  json += "\"events\":" + String(waterEventCount) + ",";
  json += "\"last_at_s\":" + String(lastWaterEventAt / 1000.0f, 1) + ",";
  json += "\"last_level\":" + String(lastWaterEventLevel, 4) + "},";
  json += "\"calibration\":{\"dry\":" + String(dryCalibration) + ",\"wet\":" + String(wetCalibration) + "}";
  json += "}";
  return json;
}

void configureWebServer() {
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Cache-Control", "no-store, no-cache, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "0");
    server.send_P(200, "text/html; charset=utf-8", PAGE_HTML);
  });

  server.on("/api/status", HTTP_GET, []() {
    server.sendHeader("Cache-Control", "no-store");
    server.send(200, "application/json", statusJson());
  });

  server.on("/mode/manual", HTTP_POST, []() {
    operatingMode = MODE_MANUAL;
    stopMotors();
    setEvent("Controle manual assumido pelo operador");
    sendOk("MANUAL");
  });

  server.on("/mode/auto", HTTP_POST, []() {
    operatingMode = MODE_AUTONOMOUS;
    setEvent("Modo autonomo confirmado pelo operador");

    // Não espera o próximo loop: lê os sensores e aciona os motores dentro
    // da própria requisição que confirmou o modo autônomo.
    updateSensorsAndInference(true);
    runAutonomousMode();
    sendOk("AUTONOMO");
  });

  server.on("/emergency-stop", HTTP_POST, []() {
    operatingMode = MODE_MANUAL;
    stopMotors();
    setEvent("Parada de emergencia acionada");
    sendOk("PARADA_DE_EMERGENCIA");
  });

  server.on("/alert/ack", HTTP_POST, []() {
    if (!waterAlertActive) {
      sendOk("SEM_ALERTA");
      return;
    }
    waterAlertAcknowledged = true;
    if (!waterPresent) waterAlertActive = false;
    setEvent("Alerta de agua reconhecido pelo operador");
    sendOk("ALERTA_RECONHECIDO");
  });

  server.on("/move/forward", HTTP_POST, []() { requireManual(moveForward); });
  server.on("/move/reverse", HTTP_POST, []() { requireManual(moveReverse); });
  server.on("/move/left", HTTP_POST, []() { requireManual(turnLeft); });
  server.on("/move/right", HTTP_POST, []() { requireManual(turnRight); });
  server.on("/move/stop", HTTP_POST, []() {
    // Um comando de parada sempre cancela a patrulha, evitando que o loop
    // autônomo volte a ligar os motores logo em seguida.
    operatingMode = MODE_MANUAL;
    stopMotors();
    sendOk("PARADO");
  });

  server.on("/calibrate/dry", HTTP_POST, []() {
    stopMotors();
    dryCalibration = waterRaw;
    preferences.putInt("dry", dryCalibration);
    setEvent("Calibracao seca registrada: " + String(dryCalibration));
    sendOk("SECO=" + String(dryCalibration));
  });

  server.on("/calibrate/wet", HTTP_POST, []() {
    stopMotors();
    if (waterRaw <= dryCalibration + 100) {
      server.send(422, "text/plain; charset=utf-8", "Leitura molhada deve ser maior que a seca");
      return;
    }
    wetCalibration = waterRaw;
    preferences.putInt("wet", wetCalibration);
    suppressWaterAlertUntilDry = true;
    waterAlertActive = false;
    waterAlertAcknowledged = false;
    setEvent("Calibracao molhada registrada: " + String(wetCalibration));
    sendOk("MOLHADO=" + String(wetCalibration));
  });

  server.onNotFound([]() {
    server.send(404, "application/json", "{\"error\":\"rota_nao_encontrada\"}");
  });

  server.begin();
}

// ----------------------------------------------------------------------
// SETUP E LOOP
// ----------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(WATER_SENSOR_PIN, INPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  stopMotors();

  analogReadResolution(12);
  analogSetPinAttenuation(WATER_SENSOR_PIN, ADC_11db);

  preferences.begin("drenahub", false);
  dryCalibration = preferences.getInt("dry", 450);
  wetCalibration = preferences.getInt("wet", 3000);

  WiFi.mode(WIFI_AP);
  const bool apStarted = WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
  configureWebServer();

  Serial.println();
  Serial.println("================================================");
  Serial.println(" DRENAHUB - DRENABOT 01");
  Serial.println("================================================");
  Serial.print("Wi-Fi: "); Serial.println(WIFI_SSID);
  Serial.print("IP: "); Serial.println(WiFi.softAPIP());
  Serial.print("AP iniciado: "); Serial.println(apStarted ? "SIM" : "NAO");
  Serial.print("Calibracao seco/molhado: ");
  Serial.print(dryCalibration); Serial.print('/'); Serial.println(wetCalibration);
  Serial.println("CSV: tipo,millis,adc,nivel,subida,distancia,obstaculo,risco");
}

void loop() {
  server.handleClient();
  updateSensorsAndInference();

  if (operatingMode == MODE_AUTONOMOUS) {
    runAutonomousMode();
  }

  delay(5);
}
