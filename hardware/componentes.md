# 🧰 Componentes Utilizados

Este documento lista os componentes efetivamente utilizados na montagem do carrinho.

| Componente | Quantidade | Função |
|---|---:|---|
| ESP32 ESP-WROOM-32 DevKit V1 | 1 | Microcontrolador principal, servidor Wi-Fi e controle do sistema |
| Ponte H L298N | 1 | Acionamento dos dois motores DC |
| Motores DC com redução | 2 | Movimentação do carrinho |
| Rodas | 2 | Tração |
| Roda boba | 1 | Apoio e liberdade de giro |
| Chassi 2WD | 1 | Estrutura mecânica principal |
| Sensor ultrassônico HC-SR04 | 1 | Medição da distância até obstáculos |
| Protoboard | 1 | Apoio às conexões do circuito |
| Resistor 1 kΩ | 1 | Parte do divisor de tensão do ECHO |
| Resistor 2 kΩ | 1 | Parte do divisor de tensão do ECHO |
| Fios jumper | Diversos | Conexões entre ESP32, sensor e ponte H |
| Suporte para 4 pilhas AA | 1 | Alimentação do conjunto de motores |
| Pilhas AA | 4 | Fonte da ponte H e motores |
| Power bank | 1 | Alimentação do ESP32 via USB |
| Papelão | Conforme necessário | Construção da carenagem do caminhão |

---

## 🔌 Pinagem Utilizada

### Ponte H L298N

| Entrada L298N | GPIO ESP32 |
|---|---:|
| IN1 | 25 |
| IN2 | 26 |
| IN3 | 33 |
| IN4 | 32 |

- OUT1 / OUT2 → motor esquerdo;
- OUT3 / OUT4 → motor direito;
- ENA / ENB → mantidos habilitados pelos jumpers.

### HC-SR04

| Pino HC-SR04 | ESP32 |
|---|---|
| VCC | 5V |
| GND | GND |
| TRIG | GPIO 18 |
| ECHO | GPIO 19 através de divisor 1 kΩ + 2 kΩ |

---

## 🔋 Alimentação Utilizada

- **ESP32:** power bank conectado pela porta USB;
- **Motores:** 4 pilhas AA conectadas à entrada de alimentação da L298N;
- **Referência comum:** GND do ESP32 conectado ao GND da L298N.

---

## ❌ Componentes da proposta inicial que não foram utilizados

A primeira concepção do projeto considerava outros componentes que foram descartados na montagem real. Por isso, eles não fazem parte desta versão da documentação:

- Arduino Mega 2560;
- Joystick Shield;
- Segundo ESP32 dedicado à transmissão;
- Comunicação ESP-NOW entre dois ESP32;
- Quatro motores;
- Chassi 4WD;
- LEDs traseiros de freio.

> A parte de TinyML será documentada em uma etapa própria posteriormente.
