# 🧠 Arquitetura do Projeto

A versão executada do projeto utiliza **um único ESP32 ESP-WROOM-32** como controlador principal. O Arduino Mega, o joystick físico, um segundo ESP32 e a comunicação ESP-NOW, considerados na concepção inicial, **não fazem parte da montagem final desta etapa**.

## 📡 Fluxo de Controle

O ESP32 cria uma rede Wi-Fi própria e hospeda uma página web de controle.

```text
iPhone / navegador
        ↓
Wi-Fi "CarrinhoESP32"
        ↓
ESP32 ESP-WROOM-32
        ↓
Ponte H L298N
        ↓
2 motores DC
```

O usuário acessa o endereço **192.168.4.1** no navegador e utiliza os botões da interface para executar os comandos:

- Frente;
- Ré;
- Esquerda;
- Direita;
- Parar.

A comunicação ocorre diretamente entre o celular e o ESP32, sem necessidade de roteador ou acesso à internet.

---

## 🏎️ Estrutura Mecânica

O projeto utiliza um **chassi 2WD**, composto por:

- 2 motores DC com redução;
- 2 rodas motorizadas;
- 1 roda boba para apoio e giro;
- Base do chassi;
- Suporte para 4 pilhas AA.

A configuração 2WD substituiu a proposta inicial de um chassi 4WD. Ela atende aos movimentos necessários com menor peso e menor complexidade.

A carenagem do protótipo foi construída em papelão com aparência inspirada em um caminhão da Volkswagen Caminhões e Ônibus.

---

## ⚙️ Controle dos Motores

A ponte H utilizada é a **L298N**.

| Função | ESP32 | L298N |
|---|---|---|
| Motor esquerdo A | GPIO 25 | IN1 |
| Motor esquerdo B | GPIO 26 | IN2 |
| Motor direito A | GPIO 33 | IN3 |
| Motor direito B | GPIO 32 | IN4 |
| Referência elétrica | GND | GND |

Os jumpers **ENA** e **ENB** permanecem instalados na configuração básica utilizada, mantendo os canais habilitados.

### Sentido utilizado no firmware

- Frente: os dois motores avançam;
- Ré: os dois motores invertem;
- Esquerda: motores giram em sentidos opostos para realizar a curva;
- Direita: motores giram em sentidos opostos para realizar a curva;
- Parar: todas as entradas da ponte H ficam em nível baixo.

---

## 📏 Sensor Ultrassônico HC-SR04

O sensor HC-SR04 foi instalado na parte frontal do carrinho para medir a distância até obstáculos.

| HC-SR04 | Ligação |
|---|---|
| VCC | 5V do ESP32 |
| GND | GND |
| TRIG | GPIO 18 |
| ECHO | GPIO 19 por divisor de tensão |

Como o sinal ECHO do HC-SR04 pode chegar a aproximadamente 5 V, foi utilizado um divisor resistivo antes do GPIO 19:

```text
ECHO do HC-SR04
      ↓
     1 kΩ
      ↓
      +────────→ GPIO 19
      ↓
     2 kΩ
      ↓
     GND
```

---

## 🔋 Alimentação

A alimentação foi separada para melhorar a estabilidade do sistema:

```text
Power bank
   ↓ USB
 ESP32

4 pilhas AA
    ↓
  L298N
    ↓
2 motores DC
```

O **GND do ESP32 e o GND do L298N são interligados**, garantindo uma referência elétrica comum.

Essa configuração evita alimentar os motores diretamente pelo ESP32 e reduz problemas causados por quedas de tensão durante a partida dos motores.

---

## 📐 Fixação dos Componentes

| Componente | Posição / Fixação |
|---|---|
| ESP32 | Sobre a base do chassi, fixado de forma a permitir acesso à porta USB |
| L298N | Sobre a base do chassi, próximo aos motores |
| HC-SR04 | Parte frontal do carrinho |
| Suporte 4xAA | Base do chassi |
| Power bank | Posicionado sobre o chassi/carenagem conforme distribuição de peso |
| Motores DC | Laterais do chassi 2WD |
| Roda boba | Ponto de apoio livre do chassi |
| Carenagem | Estrutura em papelão sobre a base |

> A evolução com **TinyML foi implementada** e está documentada em [TinyML/README.md](../TinyML/README.md).
