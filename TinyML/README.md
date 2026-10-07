# 🧠 TinyML — Classificação de Obstáculos no Carrinho 2WD

Esta pasta documenta a **segunda etapa do projeto do carrinho robótico**, na qual o protótipo passou a utilizar **TinyML embarcado no ESP32** para classificar a situação à frente a partir das leituras do sensor ultrassônico HC-SR04.

O objetivo foi evoluir o carrinho já funcional — ESP32, chassi 2WD, ponte H L298N, dois motores e controle via Wi-Fi — para que ele também pudesse tomar decisões localmente e executar um **modo autônomo**.

## 📌 Resultado da etapa

Ao final desta etapa, o sistema passou a possuir dois modos de funcionamento:

- **Modo Manual:** o usuário dirige o carrinho pelo navegador do celular;
- **Modo Autônomo TinyML:** o ESP32 lê o HC-SR04, pré-processa a distância, executa o modelo treinado no Edge Impulse e decide a ação do carrinho.

As classes utilizadas foram:

| Classe | Significado | Faixa utilizada na coleta |
|---|---|---|
| **0** | LIVRE | distância maior que 40 cm |
| **1** | ATENÇÃO | distância entre 20 e 40 cm |
| **2** | OBSTÁCULO | distância menor que 20 cm |

---

# 1. Preparação do sensor HC-SR04

O primeiro passo foi instalar e validar o sensor ultrassônico na parte frontal do carrinho.

## Ligações

| HC-SR04 | ESP32 |
|---|---|
| VCC | 5V |
| GND | GND |
| TRIG | GPIO 18 |
| ECHO | GPIO 19 através de divisor de tensão |

O pino ECHO do HC-SR04 pode fornecer um sinal próximo de 5 V, enquanto os GPIOs do ESP32 trabalham em 3,3 V. Por isso, foi utilizado um **divisor resistivo** antes do GPIO 19:

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

Também foi mantido o **GND comum** entre ESP32, sensor e ponte H.

---

# 2. Teste inicial de distância

Antes de coletar qualquer dado para Machine Learning, foi realizado um teste simples para confirmar que o HC-SR04 estava funcionando.

```cpp
const int TRIG = 18;
const int ECHO = 19;

void setup() {
  Serial.begin(115200);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  delay(1000);
  Serial.println("Teste HC-SR04");
}

void loop() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duracao = pulseIn(ECHO, HIGH, 30000);

  if (duracao == 0) {
    Serial.println("Sem leitura");
  }
  else {
    float distancia = duracao * 0.0343 / 2.0;

    Serial.print("Distancia: ");
    Serial.print(distancia);
    Serial.println(" cm");
  }

  delay(500);
}
```

Com o Serial Monitor em **115200 baud**, foram feitos testes aproximando e afastando objetos. Depois de validar a estabilidade das leituras, iniciou-se a preparação do dataset.

---

# 3. Pré-processamento dos dados

Para não enviar ao modelo apenas a leitura bruta do sensor, foram aplicadas duas etapas principais de pré-processamento:

1. **Média móvel com janela de 5 leituras**, reduzindo oscilações e ruído do HC-SR04;
2. **Normalização entre 0 e 1**, considerando uma faixa útil de 0 a 100 cm.

Também foi aplicado um limite superior de 100 cm. Assim:

```text
distância de 75 cm → feature 0.7500
distância de 35 cm → feature 0.3500
distância de 10 cm → feature 0.1000
```

## Código utilizado para gerar os dados

```cpp
const int TRIG = 18;
const int ECHO = 19;

const int JANELA = 5;

float leituras[JANELA];
int indice = 0;
bool preenchida = false;

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

float mediaMovel(float novaLeitura) {
  leituras[indice] = novaLeitura;
  indice++;

  if (indice >= JANELA) {
    indice = 0;
    preenchida = true;
  }

  int quantidade = preenchida ? JANELA : indice;

  float soma = 0;

  for (int i = 0; i < quantidade; i++) {
    soma += leituras[i];
  }

  return soma / quantidade;
}

float normalizar(float distancia) {
  if (distancia < 0) distancia = 0;
  if (distancia > 100) distancia = 100;

  return distancia / 100.0;
}

int gerarLabel(float distancia) {
  if (distancia < 20) return 2;
  if (distancia <= 40) return 1;

  return 0;
}

void setup() {
  Serial.begin(115200);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  delay(1500);

  Serial.println("feature,label");
}

void loop() {
  float distanciaBruta = lerDistancia();

  if (distanciaBruta < 0) {
    return;
  }

  float distanciaMedia = mediaMovel(distanciaBruta);

  if (!preenchida) {
    delay(100);
    return;
  }

  float feature = normalizar(distanciaMedia);
  int label = gerarLabel(distanciaMedia);

  Serial.print(feature, 4);
  Serial.print(",");
  Serial.println(label);

  delay(200);
}
```

---

# 4. Criação do CSV

O Serial Monitor passou a produzir dados diretamente no formato:

```csv
feature,label
0.7629,0
0.7574,0
0.7740,0
0.3980,1
0.3150,1
0.1850,2
0.1020,2
```

Cada linha representa:

- **feature:** distância média já normalizada;
- **label:** classe correspondente à situação detectada.

Durante a coleta, o objeto foi colocado em diferentes distâncias dentro de cada classe para evitar um dataset formado por valores praticamente repetidos.

O arquivo utilizado está disponível em:

➡️ [dados_carrinho.csv](./dados/dados_carrinho.csv)

---

# 5. Importação no Edge Impulse

Foi criado um projeto no **Edge Impulse Studio** para treinar o classificador.

Na primeira tentativa, o upload direto do CSV falhou porque o Edge Impulse tentou interpretar o arquivo sem uma configuração adequada para as várias linhas de amostras.

![Tentativa inicial de importação do CSV](./evidencias/01-importacao-csv.png)

Para resolver, foi utilizado o **CSV Wizard**.

A configuração utilizada foi:

- dados **não são time-series**;
- coluna de valor: **feature**;
- coluna de rótulo: **label**;
- divisão dos dados entre treinamento e teste realizada no projeto.

Depois dessa configuração, as linhas do CSV passaram a ser reconhecidas como amostras individuais.

---

# 6. Criação do Impulse

Em **Impulse Design → Create Impulse**, foi criada a estrutura do modelo.

![Tela de criação do Impulse](./evidencias/02-create-impulse.png)

A configuração utilizada foi:

```text
Features
   ↓
Raw Data
   ↓
Classification
   ↓
Output
```

O bloco **Raw Data** foi utilizado para trabalhar com a feature numérica já pré-processada pelo ESP32.

O bloco de aprendizado utilizado foi **Classification**, pois o objetivo é decidir entre três classes discretas: 0, 1 e 2.

Após salvar o Impulse, as features foram geradas e o classificador foi treinado.

---

# 7. Configuração do treinamento

No treinamento do classificador foram utilizados:

| Parâmetro | Valor |
|---|---:|
| Number of training cycles | **20** |
| Learning rate | **0.005** |
| Classes | **3** |
| Feature de entrada | distância normalizada |

---

# 8. Primeiro treinamento e identificação do problema

O primeiro treinamento atingiu:

- **Accuracy: 71,0%**
- **Loss: 0,62**
- **Weighted Precision: 0,52**
- **Weighted Recall: 0,71**
- **Weighted F1 Score: 0,60**
- **Area under ROC Curve: 0,98**

![Primeiro treinamento - 71% de accuracy](./evidencias/04-treinamento-inicial-71.png)

A matriz de confusão mostrou claramente o principal problema:

- classe **0 (LIVRE): 100%** de acerto;
- classe **2 (OBSTÁCULO): 100%** de acerto;
- classe **1 (ATENÇÃO): 0%** de acerto.

Das amostras reais da classe 1:

- **77,8%** foram classificadas incorretamente como classe 0;
- **22,2%** foram classificadas incorretamente como classe 2.

Ou seja, o modelo separava bem os extremos, mas ainda não havia aprendido adequadamente a região intermediária.

---

# 9. Melhoria do dataset

Com base na matriz de confusão, foi realizada uma **nova coleta de dados**, dando mais atenção à classe 1.

Foram coletadas mais amostras principalmente em:

- faixa central de **25 a 35 cm**;
- região próxima de **20 cm**;
- região próxima de **40 cm**;
- diferentes posições dentro da faixa de ATENÇÃO.

Durante a coleta, o objeto era posicionado, aguardava-se a estabilização da média móvel e então eram registradas novas leituras.

Essa etapa foi importante porque o problema não estava no ESP32 ou no sensor: o primeiro modelo simplesmente não possuía exemplos suficientes para representar bem a classe intermediária.

Depois da inclusão das novas amostras, as features foram geradas novamente e o classificador foi treinado outra vez com os mesmos parâmetros.

---

# 10. Resultado após a nova coleta

O segundo treinamento apresentou uma melhora significativa:

- **Accuracy: 85,3%**
- **Loss: 0,50**
- **Weighted Precision: 0,88**
- **Weighted Recall: 0,85**
- **Weighted F1 Score: 0,84**
- **Area under ROC Curve: 1,00**

![Treinamento final - 85,3% de accuracy](./evidencias/05-treinamento-final-85-3.png)

A nova matriz de confusão ficou:

| Classe real | Previsto 0 | Previsto 1 | Previsto 2 |
|---|---:|---:|---:|
| **0 - LIVRE** | **100%** | 0% | 0% |
| **1 - ATENÇÃO** | 30% | **50%** | 20% |
| **2 - OBSTÁCULO** | 0% | 0% | **100%** |

O F1 Score por classe também melhorou:

| Classe | F1 Score |
|---|---:|
| 0 | 0,88 |
| 1 | 0,67 |
| 2 | 0,93 |

A classe intermediária ainda é a mais difícil do problema, mas deixou de ter **0% de acerto** e passou a ser reconhecida pelo modelo. A accuracy geral evoluiu de **71,0% para 85,3%**.

---

# 11. Quantização e Deployment

Com o modelo treinado, foi utilizada a etapa **Deployment** do Edge Impulse.

Foi selecionada a opção de modelo **Quantized (int8)**:

![Modelo Quantized int8](./evidencias/03-deployment-int8.png)

Em seguida foi escolhida a opção:

```text
Arduino Library
```

e executado o **Build**.

O Edge Impulse gerou a biblioteca do projeto em formato ZIP.

Arquivo utilizado:

➡️ [Biblioteca Arduino gerada pelo Edge Impulse](./modelo/ei-gabrielmacapi-project-1-arduino-1.0.1-impulse-1.zip)

---

# 12. Instalação da biblioteca no Arduino IDE

No Arduino IDE, o ZIP gerado pelo Edge Impulse foi instalado por:

```text
Sketch
  → Include Library
      → Add .ZIP Library...
```

Depois da instalação, o projeto passou a poder incluir a biblioteca de inferência e executar o modelo diretamente no ESP32.

Essa é a etapa que transforma o modelo treinado no Edge Impulse em um modelo **TinyML embarcado**, sem depender de computador ou internet durante a inferência.

---

# 13. Inferência no ESP32

A lógica executada no ESP32 segue o mesmo pré-processamento utilizado para construir o dataset:

```text
HC-SR04
   ↓
leitura da distância
   ↓
média móvel de 5 amostras
   ↓
limitação entre 0 e 100 cm
   ↓
normalização 0–1
   ↓
modelo Edge Impulse
   ↓
classe prevista
```

O classificador retorna probabilidades para as três classes e a de maior probabilidade é utilizada como decisão.

```text
0 → LIVRE
1 → ATENÇÃO
2 → OBSTÁCULO
```

---

# 14. Integração com o carrinho

O modelo foi integrado ao firmware que já controlava os motores e hospedava a página web.

A interface passou a possuir dois botões de modo:

```text
[ MODO MANUAL ]

[ MODO AUTÔNOMO TinyML ]
```

## Modo Manual

Mantém o funcionamento original:

```text
iPhone
  ↓
Wi-Fi do ESP32
  ↓
Página de controle
  ↓
Frente / Ré / Esquerda / Direita / Parar
```

## Modo Autônomo TinyML

Quando ativado, o modelo passa a controlar as decisões do carrinho.

A estratégia implementada é:

```text
LIVRE (classe 0)
      ↓
seguir em frente


ATENÇÃO (classe 1)
      ↓
parar


OBSTÁCULO (classe 2)
      ↓
parar
      ↓
dar ré
      ↓
virar para a direita
      ↓
realizar uma nova análise
```

Também foi mantida uma camada de segurança no firmware: se a distância medida estiver **abaixo de 10 cm**, o carrinho executa a manobra de afastamento independentemente da previsão do modelo.

---

# 15. Arquitetura final

```text
                     ┌──────────────────┐
                     │      iPhone      │
                     └────────┬─────────┘
                              │ Wi-Fi
                              ↓
                     ┌──────────────────┐
                     │      ESP32       │
                     │ ESP-WROOM-32     │
                     └───────┬──────────┘
                             │
              ┌──────────────┴──────────────┐
              │                             │
              ↓                             ↓
       ┌─────────────┐              ┌──────────────┐
       │   HC-SR04   │              │ Interface Web│
       └──────┬──────┘              └──────────────┘
              │
              ↓
       Média móvel
              │
              ↓
       Normalização
              │
              ↓
       ┌──────────────┐
       │ Modelo TinyML│
       │ Edge Impulse │
       └──────┬───────┘
              │
              ↓
       Classe 0 / 1 / 2
              │
              ↓
       ┌──────────────┐
       │    L298N     │
       └──────┬───────┘
              │
              ↓
         2 motores DC
```

---

# 16. Evidências

| Evidência | Descrição |
|---|---|
| [01-importacao-csv.png](./evidencias/01-importacao-csv.png) | Primeira tentativa de upload do CSV e necessidade de configurar o CSV Wizard |
| [02-create-impulse.png](./evidencias/02-create-impulse.png) | Tela de criação do Impulse antes de adicionar Raw Data e Classification |
| [03-deployment-int8.png](./evidencias/03-deployment-int8.png) | Modelo selecionado em Quantized (int8) para deployment |
| [04-treinamento-inicial-71.png](./evidencias/04-treinamento-inicial-71.png) | Primeiro treinamento: 71,0% de accuracy e falha na classe 1 |
| [05-treinamento-final-85-3.png](./evidencias/05-treinamento-final-85-3.png) | Treinamento após nova coleta: 85,3% de accuracy e melhora da classe 1 |

---

# 17. Arquivos da entrega

| Arquivo | Finalidade |
|---|---|
| [dados_carrinho.csv](./dados/dados_carrinho.csv) | Dataset utilizado para treinamento |
| [Biblioteca Arduino do Edge Impulse](./modelo/ei-gabrielmacapi-project-1-arduino-1.0.1-impulse-1.zip) | Modelo exportado para execução no ESP32 |
| [Firmware final](../src/carrinho_esp32.ino) | Integração do controle Wi-Fi, motores, HC-SR04 e TinyML |

---

# 18. Conclusão

A segunda etapa evoluiu o protótipo de um carrinho apenas controlado remotamente para um sistema capaz de realizar **inferência local no ESP32**.

O processo completo envolveu:

1. instalação e teste do HC-SR04;
2. pré-processamento das leituras;
3. criação do CSV;
4. importação dos dados no Edge Impulse;
5. configuração do Impulse;
6. treinamento do classificador;
7. análise da matriz de confusão;
8. coleta adicional para melhorar a classe intermediária;
9. novo treinamento e validação;
10. quantização do modelo;
11. exportação como Arduino Library;
12. instalação da biblioteca no Arduino IDE;
13. execução da inferência no ESP32;
14. integração do modelo ao controle dos motores;
15. criação do **Modo Autônomo TinyML**.

O resultado final foi um modelo com **85,3% de accuracy** e execução embarcada diretamente no microcontrolador, mantendo também o modo de direção manual pelo celular.
