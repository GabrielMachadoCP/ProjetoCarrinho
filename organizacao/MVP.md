# 🚀 MVP — Carrinho Robô 2WD

## 1. Objetivo

O MVP consiste em um carrinho robótico 2WD capaz de ser controlado sem fio por um celular, utilizando um ESP32 como controlador principal e uma ponte H L298N para acionar dois motores DC.

A arquitetura final não utiliza Arduino Mega, joystick físico, segundo ESP32 ou ESP-NOW.

---

## 🟢 Produto Minimamente Viável

O projeto é considerado funcional quando o usuário consegue conectar o celular diretamente à rede criada pelo ESP32 e comandar o carrinho de forma estável.

### Requisitos mínimos atingidos

- Chassi 2WD montado;
- Dois motores DC funcionando;
- Duas rodas motorizadas e uma roda boba;
- Ponte H L298N conectada;
- Alimentação dos motores com 4 pilhas AA;
- ESP32 ESP-WROOM-32 alimentado via power bank;
- Rede Wi-Fi criada pelo próprio ESP32;
- Interface web de controle;
- Movimento para frente;
- Movimento para trás;
- Curva para esquerda;
- Curva para direita;
- Parada;
- Controle pelo iPhone;
- Sensor HC-SR04 conectado e testado;
- Funcionamento sem conexão ao computador.

---

## 🔄 Fluxo do MVP

```text
iPhone
  ↓
Rede Wi-Fi do ESP32
  ↓
Página web de controle
  ↓
ESP32
  ↓
L298N
  ↓
2 motores DC
```

Sensoriamento:

```text
HC-SR04
   ↓
ESP32
   ↓
Leitura de distância
```

---

## ✅ Critério de Conclusão

O MVP é considerado concluído quando o carrinho responde de forma confiável aos cinco comandos principais:

1. Frente;
2. Ré;
3. Esquerda;
4. Direita;
5. Parar.

Além disso, o sistema deve operar com alimentação própria, sem depender do computador.

---

## 🏁 Estado Atual

A base funcional do projeto foi concluída e validada. O sensor ultrassônico também foi instalado e testado.

A próxima evolução será tratada como uma etapa própria do projeto: **TinyML**, usando os dados do HC-SR04 para classificação embarcada no ESP32.

---

## 📊 Etapas

| Etapa | Resultado |
|---|---|
| Montagem | Chassi 2WD, motores e roda boba instalados |
| Eletrônica | ESP32, L298N e alimentação funcionando |
| Controle | Página web via Wi-Fi funcionando no celular |
| Sensoriamento | HC-SR04 conectado e validado |
| MVP | Carrinho movimentando-se remotamente de forma estável |
| Evolução | TinyML embarcado no ESP32 |
