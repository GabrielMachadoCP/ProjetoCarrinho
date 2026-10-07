/*
  =========================================================
  CARRINHO ROBÔ 2WD - ESP32 + Wi-Fi + HC-SR04 + TinyML
  =========================================================

  Hardware:
  - ESP32 ESP-WROOM-32 DevKit V1
  - Ponte H L298N
  - 2 motores DC
  - Chassi 2WD com roda boba
  - HC-SR04

  Modos:
  - Manual: controle pelo navegador do celular
  - Autônomo TinyML: classificação local de risco de colisão

  Classes do modelo:
  0 = LIVRE
  1 = ATENCAO
  2 = OBSTACULO

  Alimentação:
  - ESP32: power bank via USB
  - Motores: 4 pilhas AA através da L298N
  - GND ESP32 e GND L298N em comum
  =========================================================
*/

#include <gabrielmacapi-project-1_inferencing.h>

#include <WiFi.h>
#include <WebServer.h>

// =========================================================
// MOTORES - L298N
// =========================================================

const int IN1 = 25;
const int IN2 = 26;
const int IN3 = 33;
const int IN4 = 32;

// =========================================================
// HC-SR04
// =========================================================

const int TRIG = 18;
const int ECHO = 19;

const int JANELA = 5;

float leituras[JANELA];
int indice = 0;
bool janelaCompleta = false;

// =========================================================
// WI-FI
// =========================================================

const char* ssid = "CarrinhoESP32";
const char* password = "12345678";

WebServer server(80);

// true = controle pelo celular
// false = controle pelo modelo TinyML
bool modoManual = true;

// =========================================================
// MOVIMENTOS
// =========================================================

void parar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void frente() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void re() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void esquerda() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void direita() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

// =========================================================
// HC-SR04
// =========================================================

float lerDistancia() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duracao = pulseIn(ECHO, HIGH, 30000);

  if (duracao == 0) {
    return -1;
  }

  return duracao * 0.0343 / 2.0;
}

// =========================================================
// MEDIA MOVEL - JANELA DE 5 LEITURAS
// =========================================================

float mediaMovel(float novaLeitura) {
  leituras[indice] = novaLeitura;
  indice++;

  if (indice >= JANELA) {
    indice = 0;
    janelaCompleta = true;
  }

  int quantidade = janelaCompleta ? JANELA : indice;

  float soma = 0;

  for (int i = 0; i < quantidade; i++) {
    soma += leituras[i];
  }

  return soma / quantidade;
}

// =========================================================
// NORMALIZACAO 0 - 1
// MESMA REGRA UTILIZADA NA CRIACAO DO DATASET
// =========================================================

float normalizar(float distancia) {
  if (distancia < 0) {
    distancia = 0;
  }

  if (distancia > 100) {
    distancia = 100;
  }

  return distancia / 100.0;
}

// =========================================================
// TINYML
// =========================================================

int classificarTinyML(float feature) {
  float features[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];

  // O modelo foi treinado utilizando a feature de distancia
  // normalizada. Caso o buffer esperado possua mais de uma
  // posicao, o valor atual e replicado no frame.
  for (int i = 0; i < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE; i++) {
    features[i] = feature;
  }

  signal_t signal;

  int err = numpy::signal_from_buffer(
    features,
    EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE,
    &signal
  );

  if (err != 0) {
    Serial.println("Erro ao criar signal.");
    return -1;
  }

  ei_impulse_result_t result = {0};

  EI_IMPULSE_ERROR res =
    run_classifier(&signal, &result, false);

  if (res != EI_IMPULSE_OK) {
    Serial.println("Erro ao executar o modelo.");
    return -1;
  }

  float maiorProbabilidade = -1.0;
  int melhorClasse = -1;

  Serial.println();
  Serial.println("--- RESULTADO TINYML ---");

  for (size_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
    Serial.print(result.classification[i].label);
    Serial.print(": ");
    Serial.println(result.classification[i].value, 4);

    if (result.classification[i].value > maiorProbabilidade) {
      maiorProbabilidade = result.classification[i].value;
      melhorClasse = atoi(result.classification[i].label);
    }
  }

  Serial.print("Classe escolhida: ");
  Serial.println(melhorClasse);

  return melhorClasse;
}

// =========================================================
// MODO AUTONOMO
// =========================================================

void executarAutonomo() {
  float distancia = lerDistancia();

  if (distancia < 0) {
    Serial.println("Falha na leitura do HC-SR04.");
    parar();
    return;
  }

  float distanciaMedia = mediaMovel(distancia);

  // Aguarda a janela de media movel estar preenchida.
  if (!janelaCompleta) {
    parar();
    return;
  }

  float feature = normalizar(distanciaMedia);

  Serial.print("Distancia media: ");
  Serial.print(distanciaMedia);
  Serial.println(" cm");

  Serial.print("Feature: ");
  Serial.println(feature, 4);

  // Camada extra de seguranca:
  // abaixo de 10 cm o afastamento e executado
  // independentemente da previsao do modelo.
  if (distanciaMedia < 10) {
    Serial.println("FAILSAFE: obstaculo muito proximo.");

    parar();
    delay(150);

    re();
    delay(450);

    direita();
    delay(450);

    parar();
    return;
  }

  int classe = classificarTinyML(feature);

  if (classe == 0) {
    // LIVRE
    Serial.println("LIVRE -> FRENTE");
    frente();
  }
  else if (classe == 1) {
    // ATENCAO
    Serial.println("ATENCAO -> PARAR");
    parar();
  }
  else if (classe == 2) {
    // OBSTACULO
    Serial.println("OBSTACULO -> DESVIAR");

    parar();
    delay(150);

    re();
    delay(450);

    direita();
    delay(450);

    parar();
  }
  else {
    // Qualquer erro de inferencia deixa o carrinho parado.
    parar();
  }
}

// =========================================================
// INTERFACE WEB
// =========================================================

String pagina = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">

<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">

  <style>
    body {
      font-family: Arial, sans-serif;
      text-align: center;
      background: #f2f2f2;
      margin-top: 35px;
    }

    h1 {
      color: #001E50;
    }

    button {
      width: 95px;
      height: 75px;
      margin: 7px;
      font-size: 32px;
      border: none;
      border-radius: 15px;
      background: #001E50;
      color: white;
      touch-action: none;
    }

    .stop {
      background: #c62828;
    }

    .mode {
      width: 220px;
      height: 55px;
      font-size: 17px;
      margin-top: 20px;
    }

    .manual {
      background: #001E50;
    }

    .auto {
      background: #197b4d;
    }

    #status {
      margin: 20px;
      font-size: 20px;
      font-weight: bold;
    }
  </style>
</head>

<body>

  <h1>Carrinho VWCO</h1>

  <div id="status">
    Modo Manual
  </div>

  <div>
    <button
      ontouchstart="cmd('/frente')"
      ontouchend="cmd('/parar')"
      onmousedown="cmd('/frente')"
      onmouseup="cmd('/parar')">
      ↑
    </button>
  </div>

  <div>
    <button
      ontouchstart="cmd('/esquerda')"
      ontouchend="cmd('/parar')"
      onmousedown="cmd('/esquerda')"
      onmouseup="cmd('/parar')">
      ←
    </button>

    <button
      class="stop"
      onclick="cmd('/parar')">
      ■
    </button>

    <button
      ontouchstart="cmd('/direita')"
      ontouchend="cmd('/parar')"
      onmousedown="cmd('/direita')"
      onmouseup="cmd('/parar')">
      →
    </button>
  </div>

  <div>
    <button
      ontouchstart="cmd('/re')"
      ontouchend="cmd('/parar')"
      onmousedown="cmd('/re')"
      onmouseup="cmd('/parar')">
      ↓
    </button>
  </div>

  <button
    class="mode manual"
    onclick="manual()">
    MODO MANUAL
  </button>

  <br>

  <button
    class="mode auto"
    onclick="automatico()">
    MODO AUTÔNOMO TinyML
  </button>

  <script>
    function cmd(comando) {
      fetch(comando);
    }

    function manual() {
      fetch('/manual');
      document.getElementById("status").innerHTML = "Modo Manual";
    }

    function automatico() {
      fetch('/auto');
      document.getElementById("status").innerHTML = "Modo Autônomo TinyML";
    }
  </script>

</body>
</html>
)rawliteral";

// =========================================================
// SETUP
// =========================================================

void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  parar();

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  WiFi.softAP(ssid, password);

  Serial.println();
  Serial.println("============================");
  Serial.println(" CARRINHO 2WD + TINYML");
  Serial.println("============================");

  Serial.print("Rede Wi-Fi: ");
  Serial.println(ssid);

  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", []() {
    server.send(200, "text/html", pagina);
  });

  // Comandos manuais somente movimentam o carrinho
  // quando o modo manual esta ativo.
  server.on("/frente", []() {
    if (modoManual) {
      frente();
    }

    server.send(200, "text/plain", "OK");
  });

  server.on("/re", []() {
    if (modoManual) {
      re();
    }

    server.send(200, "text/plain", "OK");
  });

  server.on("/esquerda", []() {
    if (modoManual) {
      esquerda();
    }

    server.send(200, "text/plain", "OK");
  });

  server.on("/direita", []() {
    if (modoManual) {
      direita();
    }

    server.send(200, "text/plain", "OK");
  });

  server.on("/parar", []() {
    parar();
    server.send(200, "text/plain", "OK");
  });

  server.on("/manual", []() {
    modoManual = true;
    parar();

    Serial.println("MODO MANUAL");

    server.send(200, "text/plain", "MANUAL");
  });

  server.on("/auto", []() {
    modoManual = false;
    parar();

    Serial.println("MODO AUTONOMO TINYML");

    server.send(200, "text/plain", "AUTONOMO");
  });

  server.begin();

  Serial.println("Servidor web iniciado.");
}

// =========================================================
// LOOP
// =========================================================

void loop() {
  server.handleClient();

  if (!modoManual) {
    executarAutonomo();
    delay(250);
  }
}
