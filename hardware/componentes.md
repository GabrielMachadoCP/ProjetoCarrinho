# Componentes do DrenaBot 01

## Materiais

| Componente | Qtd. | Papel no protótipo | Situação |
|---|---:|---|---|
| ESP32 ESP-WROOM-32 DevKit V1 | 1 | controle, Wi-Fi e TinyML | reaproveitado |
| Sensor analógico de nível/presença de água | 1 | detectar contato e intensidade de água | **novo sensor** |
| HC-SR04 | 1 | medir distância | reaproveitado |
| L298N | 1 | controlar os motores | reaproveitado |
| Motor DC com redução | 2 | tração | reaproveitado |
| Rodas + roda boba | 3 | deslocamento 2WD | reaproveitado |
| Chassi 2WD | 1 | base mecânica | reaproveitado |
| Resistores 1 kΩ e 2 kΩ | 1 de cada | divisor de tensão do ECHO | reaproveitados |
| Protoboard e jumpers | diversos | interligação | reaproveitados |
| Power bank | 1 | ESP32 e sensores | reaproveitado |
| Suporte 4xAA + pilhas | 1 | motores | reaproveitado |
| Suporte do sensor | 1 | manter somente a ponta sensível baixa | novo; arquivo CAD incluso |
| Buzzer ativo 3,3 V | 1 | alerta sonoro | opcional |

## Especificação de compra do novo sensor

Procure por **sensor analógico de nível de água 3,3–5 V para Arduino/ESP32**, normalmente uma placa estreita com trilhas expostas e pinos `S/+/-` ou `AO/VCC/GND`.

Para este firmware, o sensor precisa oferecer:

- saída analógica;
- alimentação compatível com 3,3 V;
- leitura inferior a 4095 no ADC do ESP32;
- área sensível que possa tocar pouca água sem submergir os conectores.

Não use módulo que forneça apenas saída digital. Sensores resistivos simples oxidam com uso prolongado; são adequados à prova acadêmica, mas a versão comercial deverá migrar para sensor capacitivo, boia ou sonda industrial conforme o ambiente.

## Pinagem resumida

| Origem | Destino |
|---|---|
| Sensor de água VCC | 3V3 do ESP32 |
| Sensor de água GND | GND |
| Sensor de água AO/S | GPIO 34 |
| HC-SR04 TRIG | GPIO 18 |
| HC-SR04 ECHO | GPIO 19 por divisor 1 kΩ/2 kΩ |
| L298N IN1 / IN2 | GPIO 25 / 26 |
| L298N IN3 / IN4 | GPIO 33 / 32 |
| ESP32 GND | L298N GND |

As ligações detalhadas e o procedimento de validação estão em [ligacoes_drenahub.md](ligacoes_drenahub.md).
