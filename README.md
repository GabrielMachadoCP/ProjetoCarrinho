# 🤖 Carrinho Robô 2WD com ESP32 + TinyML

Projeto de um **carrinho robótico 2WD controlado por Wi-Fi**, desenvolvido com **ESP32 ESP-WROOM-32 DevKit V1**, ponte H L298N, dois motores DC, sensor ultrassônico HC-SR04 e um modelo **TinyML treinado no Edge Impulse**.

A versão final utiliza **duas rodas motorizadas e uma roda boba**. O ESP32 é o único microcontrolador do sistema, cria a rede Wi-Fi utilizada pelo celular, hospeda a interface web e executa localmente o modelo de classificação.

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
- Utilizar o HC-SR04 para medir a distância de obstáculos;
- Pré-processar as leituras com média móvel e normalização;
- Executar um modelo TinyML diretamente no ESP32;
- Classificar a situação à frente como **Livre, Atenção ou Obstáculo**;
- Alternar entre **Modo Manual** e **Modo Autônomo TinyML**;
- Operar sem conexão com um computador após a gravação do firmware.

---

## 🧰 Componentes Utilizados

| Componente | Quantidade | Função |
|---|---:|---|
| ESP32 ESP-WROOM-32 DevKit V1 | 1 | Controlador principal, servidor Wi-Fi e inferência TinyML |
| Ponte H L298N | 1 | Acionamento dos dois motores |
| Motores DC com redução | 2 | Movimentação do carrinho |
| Rodas | 2 | Tração |
| Roda boba | 1 | Apoio e giro livre |
| Chassi 2WD | 1 | Estrutura física |
| Sensor ultrassônico HC-SR04 | 1 | Medição de distância |
| Protoboard | 1 | Organização das conexões |
| Resistor de 1 kΩ | 1 | Divisor de tensão do ECHO |
| Resistor de 2 kΩ | 1 | Divisor de tensão do ECHO |
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
- Suporte para 4 pilhas AA.

A configuração 2WD substituiu a proposta inicial de 4WD e foi a configuração realmente utilizada na montagem.

---

## 🧠 Arquitetura Final

```text
                    iPhone
                      │
                      │ Wi-Fi
                      ↓
                    ESP32
                  /       \
                 /         \
          HC-SR04           Interface Web
              │
              ↓
        Média móvel
              │
              ↓
        Normalização
              │
              ↓
       Modelo TinyML
              │
              ↓
     Livre / Atenção / Obstáculo
              │
              ↓
            L298N
              │
              ↓
         2 motores DC
```

---

## 🎮 Controle Manual

O próprio ESP32 cria a rede:

- **SSID:** `CarrinhoESP32`
- **Senha:** `12345678`
- **Endereço:** `192.168.4.1`

O celular se conecta diretamente à rede e acessa uma página web hospedada no ESP32.

A interface possui os comandos:

- Frente;
- Ré;
- Esquerda;
- Direita;
- Parar;
- **Modo Manual**;
- **Modo Autônomo TinyML**.

Não é necessário roteador ou acesso à internet.

---

## 🤖 Modo Autônomo TinyML

O HC-SR04 envia as leituras ao ESP32. Depois do pré-processamento, o modelo do Edge Impulse classifica a situação em três classes:

| Classe | Situação | Comportamento |
|---|---|---|
| 0 | LIVRE | Segue em frente |
| 1 | ATENÇÃO | Para |
| 2 | OBSTÁCULO | Para, dá ré e realiza uma curva |

O modelo final atingiu **85,3% de accuracy** no conjunto de validação, após uma segunda coleta de dados para melhorar o reconhecimento da classe intermediária.

📘 **Documentação completa da segunda entrega:** [TinyML/README.md](./TinyML/README.md)

---

## 🔋 Alimentação

A alimentação foi separada para evitar instabilidade causada pelo consumo dos motores:

- **Power bank → ESP32 via USB**;
- **4 pilhas AA → L298N → motores**;
- **GND do ESP32 e GND do L298N em comum**.

---

## 📏 Sensor HC-SR04

| HC-SR04 | ESP32 |
|---|---|
| VCC | 5V |
| GND | GND |
| TRIG | GPIO 18 |
| ECHO | GPIO 19 através de divisor de tensão |

O divisor utiliza **1 kΩ e 2 kΩ** antes da entrada do GPIO 19.

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
- ENA e ENB permanecem habilitados pelos jumpers na montagem utilizada.

---

## 📁 Estrutura do Repositório

```text
ProjetoCarrinho/
│
├── README.md
├── cad/
│   └── arquivos CAD da etapa inicial
│
├── hardware/
│   ├── arquitetura.md
│   └── componentes.md
│
├── organizacao/
│   ├── Backlog.md
│   ├── Dependencias.md
│   ├── Kanban.md
│   ├── MVP.md
│   └── MoSCoW.md
│
├── src/
│   └── carrinho_esp32.ino
│
└── TinyML/
    ├── README.md
    ├── dados/
    │   └── dados_carrinho.csv
    ├── evidencias/
    │   ├── 01-importacao-csv.png
    │   ├── 02-create-impulse.png
    │   ├── 03-deployment-int8.png
    │   ├── 04-treinamento-inicial-71.png
    │   └── 05-treinamento-final-85-3.png
    └── modelo/
        └── biblioteca Arduino gerada pelo Edge Impulse
```

> A pasta `cad/` foi mantida como material da etapa inicial e não representa a configuração mecânica final utilizada no protótipo.

---

## 💻 Firmware

O firmware final está em:

**[src/carrinho_esp32.ino](./src/carrinho_esp32.ino)**

Ele reúne:

- controle dos motores;
- servidor Wi-Fi;
- interface web;
- leitura do HC-SR04;
- média móvel;
- normalização;
- inferência do modelo Edge Impulse;
- modo manual;
- modo autônomo TinyML;
- rotina de segurança para obstáculo muito próximo.

---

## 🧪 Evolução do Modelo

No primeiro treinamento, o modelo atingiu **71,0% de accuracy** e não conseguiu reconhecer corretamente a classe 1 (Atenção).

Após analisar a matriz de confusão, foram coletadas mais amostras na faixa intermediária. O modelo foi treinado novamente e atingiu:

- **Accuracy:** 85,3%;
- **Loss:** 0,50;
- **Weighted Precision:** 0,88;
- **Weighted Recall:** 0,85;
- **Weighted F1 Score:** 0,84;
- **AUC:** 1,00.

As evidências e o passo a passo completo estão disponíveis na pasta **[TinyML](./TinyML/README.md)**.
