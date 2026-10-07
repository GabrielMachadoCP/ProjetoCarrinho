# 🤖 Carrinho Robô 2WD com ESP32

Projeto de um **carrinho robótico 2WD controlado por Wi-Fi**, desenvolvido com **ESP32 ESP-WROOM-32 DevKit V1**, ponte H L298N, dois motores DC e sensor ultrassônico HC-SR04.

A versão final do projeto utiliza **duas rodas motorizadas e uma roda boba**, substituindo o conceito inicial de chassi 4WD. O ESP32 é o único microcontrolador do sistema e também cria a rede Wi-Fi usada para controlar o carrinho pelo navegador do celular.

## 👥 Integrantes do Grupo

- **Gabriel Machado** — RM 99880
- **Lourenzo Ramos** — RM 99951
- **Vitor Hugo Rodrigues** — RM 97758

---

## 🎯 Objetivo do Projeto

Desenvolver um carrinho robótico capaz de:

- Movimentar-se para frente e para trás;
- Realizar curvas para esquerda e direita;
- Parar remotamente;
- Ser controlado por um celular conectado diretamente ao Wi-Fi criado pelo ESP32;
- Controlar dois motores DC por meio de uma ponte H L298N;
- Utilizar um sensor ultrassônico HC-SR04 para medir a distância de obstáculos;
- Operar de forma independente do computador após a gravação do firmware;
- Utilizar uma carenagem simples de papelão inspirada em um caminhão da Volkswagen Caminhões e Ônibus.

> A documentação específica da evolução com TinyML será adicionada em uma etapa posterior do projeto.

---

## 🧰 Componentes Utilizados

| Componente | Quantidade | Função |
|---|---:|---|
| ESP32 ESP-WROOM-32 DevKit V1 | 1 | Controlador principal e servidor Wi-Fi |
| Ponte H L298N | 1 | Acionamento dos dois motores |
| Motores DC com redução | 2 | Movimentação do carrinho |
| Rodas | 2 | Tração |
| Roda boba | 1 | Apoio e giro livre |
| Chassi 2WD | 1 | Estrutura física |
| Sensor ultrassônico HC-SR04 | 1 | Medição de distância |
| Protoboard | 1 | Organização das conexões |
| Resistores de 1 kΩ e 2 kΩ | 1 de cada | Divisor de tensão do pino ECHO |
| Fios jumper | Diversos | Conexões elétricas |
| Suporte para 4 pilhas AA | 1 | Alimentação dos motores |
| Pilhas AA | 4 | Alimentação da ponte H e motores |
| Power bank | 1 | Alimentação estável do ESP32 via USB |
| Papelão | Conforme necessário | Carenagem do caminhão |

---

## 🏎️ Chassi

O projeto utiliza um **chassi 2WD**, composto por:

- 2 motores DC com redução;
- 2 rodas motorizadas;
- 1 roda boba;
- Base do chassi;
- Suporte para 4 pilhas AA;
- Parafusos e elementos de fixação.

Essa configuração foi escolhida por ser mais simples, leve e adequada ao protótipo, mantendo todos os movimentos necessários para o projeto.

---

## 🧠 Arquitetura do Projeto

O ESP32 concentra o controle do sistema.

### Controle remoto

O próprio ESP32 cria uma rede Wi-Fi chamada **CarrinhoESP32**. O celular se conecta diretamente a essa rede e acessa uma página web hospedada no microcontrolador.

```text
Celular
   ↓
Wi-Fi criado pelo ESP32
   ↓
ESP32
   ↓
L298N
   ↓
2 motores DC
```

A interface web possui comandos de:

- Frente;
- Ré;
- Esquerda;
- Direita;
- Parar.

Não é necessário roteador ou acesso à internet para controlar o carrinho.

### Alimentação

Para evitar instabilidade causada pelo consumo dos motores, a alimentação foi separada:

- **Power bank → ESP32 via USB**;
- **4 pilhas AA → L298N → motores**;
- **GND do ESP32 e GND do L298N em comum**.

### Sensor ultrassônico

O HC-SR04 utiliza:

| HC-SR04 | ESP32 |
|---|---|
| VCC | 5V |
| GND | GND |
| TRIG | GPIO 18 |
| ECHO | GPIO 19 através de divisor de tensão |

O divisor de tensão usa resistores de **1 kΩ e 2 kΩ**, reduzindo o sinal do ECHO antes de chegar ao GPIO 19 do ESP32.

---

## 🔌 Ligações do L298N

| ESP32 | L298N |
|---|---|
| GPIO 25 | IN1 |
| GPIO 26 | IN2 |
| GPIO 33 | IN3 |
| GPIO 32 | IN4 |
| GND | GND |

- OUT1 / OUT2 → motor esquerdo;
- OUT3 / OUT4 → motor direito;
- ENA e ENB permanecem habilitados pelos jumpers para o controle básico.

---

## 📁 Estrutura do Repositório

- **cad/** — arquivos CAD mantidos como material de referência da etapa inicial;
- **hardware/** — documentação da arquitetura e dos componentes realmente utilizados;
- **organizacao/** — documentação de planejamento adaptada ao projeto executado;
- **src/** — código utilizado no ESP32.

---

## 💻 Firmware

O firmware principal está em:

**src/carrinho_esp32.ino**

Ele implementa o controle via Wi-Fi e hospeda a interface web acessada pelo celular.

Configuração da rede:

- Rede: **CarrinhoESP32**
- Senha: **12345678**
- Endereço padrão: **192.168.4.1**

Após o código ser gravado, o computador não é necessário para a operação do carrinho.
