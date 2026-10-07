/*
  =========================================================
  CARRINHO ROBÔ 2WD - ESP32 + L298N + CONTROLE VIA WI-FI
  =========================================================

  Hardware utilizado:
  - ESP32 ESP-WROOM-32 DevKit V1
  - Ponte H L298N
  - 2 motores DC
  - Chassi 2WD com roda boba

  Controle:
  - O ESP32 cria a rede Wi-Fi "CarrinhoESP32"
  - O celular acessa 192.168.4.1 pelo navegador
  - A página permite frente, ré, esquerda, direita e parar

  Alimentação:
  - ESP32: power bank via USB
  - Motores: 4 pilhas AA através da L298N
  - GND do ESP32 e GND da L298N em comum
  =========================================================
*/

#include <WiFi.h>
#include <WebServer.h>

// =========================================================
// PINOS DA PONTE H L298N
// =========================================================

const int IN1 = 25;
const int IN2 = 26;
const int IN3 = 33;
const int IN4 = 32;

// =========================================================
// REDE WI-FI CRIADA PELO ESP32
// =========================================================

const char* ssid = "CarrinhoESP32";
const char* password = "12345678";

WebServer server(80);

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
// INTERFACE WEB
// =========================================================

String pagina = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta charset="UTF-8">

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

    p {
      color: #444;
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
  </style>
</head>

<body>

  <h1>Carrinho VWCO</h1>
  <p>Controle Wi-Fi via ESP32</p>

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

  <script>
    function cmd(comando) {
      fetch(comando);
    }
  </script>

</body>
</html>
)rawliteral";

// =========================================================
// ROTAS DO SERVIDOR
// =========================================================

void configurarRotas() {

  server.on("/", []() {
    server.send(200, "text/html", pagina);
  });

  server.on("/frente", []() {
    frente();
    server.send(200, "text/plain", "FRENTE");
  });

  server.on("/re", []() {
    re();
    server.send(200, "text/plain", "RE");
  });

  server.on("/esquerda", []() {
    esquerda();
    server.send(200, "text/plain", "ESQUERDA");
  });

  server.on("/direita", []() {
    direita();
    server.send(200, "text/plain", "DIREITA");
  });

  server.on("/parar", []() {
    parar();
    server.send(200, "text/plain", "PARAR");
  });
}

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

  WiFi.softAP(ssid, password);

  Serial.println();
  Serial.println("============================");
  Serial.println(" CARRINHO 2WD - ESP32");
  Serial.println("============================");

  Serial.print("Rede Wi-Fi: ");
  Serial.println(ssid);

  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  configurarRotas();

  server.begin();

  Serial.println("Servidor web iniciado.");
}

// =========================================================
// LOOP
// =========================================================

void loop() {
  server.handleClient();
}
