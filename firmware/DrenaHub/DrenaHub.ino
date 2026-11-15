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

String currentAction = "PARADO";
String lastEvent = "Sistema iniciado";
unsigned long lastSensorUpdate = 0;
unsigned long lastAutonomousManeuver = 0;
unsigned long lastSerialLog = 0;

constexpr unsigned long SENSOR_INTERVAL_MS = 200;
constexpr unsigned long MANEUVER_COOLDOWN_MS = 4500;

// ----------------------------------------------------------------------
// PAGINA WEB EMBARCADA
// ----------------------------------------------------------------------
const char PAGE_HTML[] PROGMEM = R"rawliteral(
<!doctype html>
<html lang="pt-BR">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>DrenaHub | DrenaBot</title>
  <style>
    :root{--navy:#082744;--blue:#0d6e9f;--cyan:#26b6c7;--bg:#edf4f7;--card:#fff;--text:#173042;--muted:#607783;--green:#178a58;--yellow:#d69316;--red:#c33b43}
    *{box-sizing:border-box} body{margin:0;background:var(--bg);font-family:Arial,sans-serif;color:var(--text)}
    header{background:linear-gradient(130deg,var(--navy),var(--blue));color:white;padding:22px 18px} header h1{margin:0;font-size:27px} header p{margin:6px 0 0;opacity:.86}
    main{max-width:950px;margin:auto;padding:18px}.grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(210px,1fr));gap:13px}.card{background:var(--card);border-radius:14px;padding:16px;box-shadow:0 4px 14px #17304212}.label{font-size:12px;color:var(--muted);text-transform:uppercase;letter-spacing:.06em}.value{font-size:27px;font-weight:700;margin-top:6px}.risk{display:inline-block;padding:8px 13px;border-radius:999px;color:white;font-weight:bold}.NORMAL{background:var(--green)}.ATENCAO{background:var(--yellow)}.CRITICO,.FALHA{background:var(--red)}
    .bar{height:12px;background:#dae5ea;border-radius:8px;overflow:hidden;margin-top:10px}.fill{height:100%;width:0;background:var(--cyan);transition:.3s}.section{margin-top:15px}.controls{display:grid;grid-template-columns:repeat(3,82px);gap:8px;justify-content:center}.controls button,.action{border:0;border-radius:11px;background:var(--navy);color:white;min-height:58px;font-size:20px;font-weight:bold;cursor:pointer}.controls .stop{background:var(--red)}.action{font-size:14px;min-height:44px;padding:0 14px;margin:5px}.action.auto{background:var(--green)}.action.cal{background:var(--blue)}.note{font-size:13px;color:var(--muted);line-height:1.45}.statusline{display:flex;gap:10px;flex-wrap:wrap;align-items:center}.small{font-size:14px;font-weight:600}.footer{text-align:center;color:var(--muted);font-size:12px;padding:22px}
  </style>
</head>
<body>
  <header><h1>DrenaHub</h1><p>DrenaBot 01 · monitoramento preventivo de drenagem</p></header>
  <main>
    <div class="card statusline"><span id="risk" class="risk NORMAL">NORMAL</span><span id="mode" class="small">Modo manual</span><span id="action" class="small">Ação: PARADO</span></div>
    <div class="grid section">
      <div class="card"><div class="label">Nível de água</div><div id="water" class="value">0%</div><div class="bar"><div id="waterBar" class="fill"></div></div><p id="raw" class="note">ADC: --</p></div>
      <div class="card"><div class="label">Distância livre</div><div id="distance" class="value">-- cm</div><p class="note">HC-SR04 frontal</p></div>
      <div class="card"><div class="label">Confiança TinyML</div><div id="confidence" class="value">--%</div><p id="probs" class="note">N -- · A -- · C --</p></div>
      <div class="card"><div class="label">Último evento</div><div id="event" class="small" style="margin-top:10px">Sistema iniciado</div><p id="uptime" class="note">Tempo ativo: --</p></div>
    </div>

    <div class="card section">
      <div class="label">Controle manual</div>
      <div class="controls" style="margin-top:12px">
        <span></span><button data-hold="/move/forward">↑</button><span></span>
        <button data-hold="/move/left">←</button><button class="stop" onclick="command('/move/stop')">■</button><button data-hold="/move/right">→</button>
        <span></span><button data-hold="/move/reverse">↓</button><span></span>
      </div>
      <div style="text-align:center;margin-top:10px">
        <button class="action" onclick="command('/mode/manual')">MODO MANUAL</button>
        <button class="action auto" onclick="command('/mode/auto')">MODO AUTÔNOMO</button>
      </div>
    </div>

    <div class="card section">
      <div class="label">Calibração do sensor</div>
      <p class="note">1. Completamente seco: pressione “Registrar seco”. 2. Molhe o sensor até o nível máximo usado na maquete e pressione “Registrar molhado”.</p>
      <button class="action cal" onclick="calibrate('/calibrate/dry','Registrar a leitura atual como SECO?')">REGISTRAR SECO</button>
      <button class="action cal" onclick="calibrate('/calibrate/wet','Registrar a leitura atual como MOLHADO?')">REGISTRAR MOLHADO</button>
      <span id="calibration" class="note">Seco: -- · Molhado: --</span>
    </div>
  </main>
  <div class="footer">FIAP 4ESPY · Global Solution CitizenTech · Project-Based Maker Lab</div>
  <script>
    async function command(path){await fetch(path,{method:'POST'});await update()}
    async function calibrate(path,message){if(confirm(message)){await command(path)}}
    document.querySelectorAll('[data-hold]').forEach(btn=>{
      const start=e=>{e.preventDefault();command(btn.dataset.hold)};
      const stop=e=>{e.preventDefault();command('/move/stop')};
      btn.addEventListener('mousedown',start);btn.addEventListener('mouseup',stop);btn.addEventListener('mouseleave',stop);
      btn.addEventListener('touchstart',start,{passive:false});btn.addEventListener('touchend',stop,{passive:false});
    });
    async function update(){
      try{
        const r=await fetch('/api/status');const d=await r.json();
        const risk=document.getElementById('risk');risk.textContent=d.risk;risk.className='risk '+d.risk;
        document.getElementById('mode').textContent='Modo '+d.mode;
        document.getElementById('action').textContent='Ação: '+d.action;
        document.getElementById('water').textContent=Math.round(d.water_level*100)+'%';
        document.getElementById('waterBar').style.width=Math.round(d.water_level*100)+'%';
        document.getElementById('raw').textContent='ADC: '+d.water_raw+' · Subida: '+d.rise_rate.toFixed(2);
        document.getElementById('distance').textContent=d.distance_cm<0?'Falha':d.distance_cm.toFixed(1)+' cm';
        document.getElementById('confidence').textContent=Math.round(d.confidence*100)+'%';
        document.getElementById('probs').textContent='N '+Math.round(d.probabilities[0]*100)+'% · A '+Math.round(d.probabilities[1]*100)+'% · C '+Math.round(d.probabilities[2]*100)+'%';
        document.getElementById('event').textContent=d.last_event;
        document.getElementById('uptime').textContent='Tempo ativo: '+Math.floor(d.uptime_s)+' s';
        document.getElementById('calibration').textContent='Seco: '+d.calibration.dry+' · Molhado: '+d.calibration.wet;
      }catch(e){document.getElementById('event').textContent='Sem comunicação com o DrenaBot'}
    }
    update();setInterval(update,1000);
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

void updateSensorsAndInference() {
  if (millis() - lastSensorUpdate < SENSOR_INTERVAL_MS) return;
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
  if (millis() - lastAutonomousManeuver < MANEUVER_COOLDOWN_MS) {
    stopMotors();
    return;
  }
  lastAutonomousManeuver = millis();

  stopMotors();
  beep(180);
  delay(150);
  moveReverse();
  delay(380);
  turnRight();
  delay(420);
  stopMotors();
  setEvent(reason);
}

void runAutonomousMode() {
  if (!waterWindowReady) {
    stopMotors();
    currentAction = "CALIBRANDO LEITURAS";
    return;
  }

  switch (currentRisk) {
    case RISK_NORMAL:
      if (distanceCm > 25.0f) {
        moveForward();
      } else {
        executeEscapeManeuver("Obstaculo detectado; desvio executado");
      }
      break;

    case RISK_ATTENTION:
      stopMotors();
      currentAction = "INSPECAO NECESSARIA";
      break;

    case RISK_CRITICAL:
      executeEscapeManeuver("Risco critico; area evitada pelo DrenaBot");
      break;

    case RISK_FAULT:
    default:
      stopMotors();
      currentAction = "FALHA DE SENSOR";
      break;
  }
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
  json += "\"rise_rate\":" + String(riseRate, 4) + ",";
  json += "\"distance_cm\":" + String(distanceCm, 2) + ",";
  json += "\"obstacle_risk\":" + String(obstacleRisk, 4) + ",";
  json += "\"confidence\":" + String(confidence, 4) + ",";
  json += "\"probabilities\":[" + String(classProbabilities[0], 4) + "," + String(classProbabilities[1], 4) + "," + String(classProbabilities[2], 4) + "],";
  json += "\"last_event\":\"" + lastEvent + "\",";
  json += "\"uptime_s\":" + String(millis() / 1000.0f, 1) + ",";
  json += "\"calibration\":{\"dry\":" + String(dryCalibration) + ",\"wet\":" + String(wetCalibration) + "}";
  json += "}";
  return json;
}

void configureWebServer() {
  server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html; charset=utf-8", PAGE_HTML);
  });

  server.on("/api/status", HTTP_GET, []() {
    server.sendHeader("Cache-Control", "no-store");
    server.send(200, "application/json", statusJson());
  });

  server.on("/mode/manual", HTTP_POST, []() {
    operatingMode = MODE_MANUAL;
    stopMotors();
    setEvent("Modo manual ativado");
    sendOk("MANUAL");
  });

  server.on("/mode/auto", HTTP_POST, []() {
    operatingMode = MODE_AUTONOMOUS;
    stopMotors();
    setEvent("Modo autonomo TinyML ativado");
    sendOk("AUTONOMO");
  });

  server.on("/move/forward", HTTP_POST, []() { requireManual(moveForward); });
  server.on("/move/reverse", HTTP_POST, []() { requireManual(moveReverse); });
  server.on("/move/left", HTTP_POST, []() { requireManual(turnLeft); });
  server.on("/move/right", HTTP_POST, []() { requireManual(turnRight); });
  server.on("/move/stop", HTTP_POST, []() { stopMotors(); sendOk(); });

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
