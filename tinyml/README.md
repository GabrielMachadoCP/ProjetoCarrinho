# TinyML do DrenaHub

## Objetivo

Classificar o risco de um ponto de inspeção usando três variáveis já normalizadas:

| Entrada | Intervalo | Origem |
|---|---:|---|
| `water_level` | 0–1 | sensor de água após calibração |
| `rise_rate` | 0–1 | aumento recente do nível |
| `obstacle_risk` | 0–1 | distância do HC-SR04 |

Saídas: `0 = NORMAL`, `1 = ATENCAO`, `2 = CRITICO`.

## Modelo

O treinamento usa `StandardScaler` e uma `MLPClassifier` com:

- 3 entradas;
- 1 camada oculta com 6 neurônios ReLU;
- 3 logits e softmax no firmware;
- semente fixa para reprodução;
- separação estratificada 75% treino / 25% teste.

O script exporta pesos, vieses e parâmetros do scaler para `firmware/DrenaHub/drenahub_model.h`. A inferência final é C++ puro.

## Baseline incluído

O CSV `data/drenahub_training_synthetic.csv` é **sintético**. Ele valida o pipeline de ponta a ponta antes da disponibilidade do sensor, mas não prova desempenho físico. A alta accuracy do baseline é esperada porque as classes derivam da política usada para gerar os dados.

Arquivos gerados:

- `model/metrics.json`;
- `evidencias/matriz_confusao_baseline.png`;
- `../firmware/DrenaHub/drenahub_model.h`.

## Executar o baseline

```bash
python -m pip install -r tinyml/requirements.txt
python tinyml/train_drenahub_model.py
```

## Coletar dados reais

O firmware envia telemetria a 115200 baud. O operador informa a classe real de cada sessão:

```bash
python tinyml/collect_serial.py --port COM3 --label NORMAL --seconds 90
python tinyml/collect_serial.py --port COM3 --label ATENCAO --seconds 90
python tinyml/collect_serial.py --port COM3 --label CRITICO --seconds 90
```

No Linux/macOS, substitua `COM3` por algo como `/dev/ttyUSB0` ou `/dev/cu.usbserial-...`.

O coletor cria/acrescenta linhas em `data/drenahub_real.csv`. Faça as sessões em diferentes distâncias e umidades. Evite coletar todas as amostras de uma classe em uma única condição estática.

## Retreinar com dados reais

```bash
python tinyml/train_drenahub_model.py --input tinyml/data/drenahub_real.csv
```

O CSV precisa conter:

```text
water_level,rise_rate,obstacle_risk,label
```

Requisitos mínimos impostos pelo script:

- 90 amostras;
- labels 0, 1 e 2 presentes;
- valores numéricos;
- mesmas variáveis consumidas pelo firmware.

Depois, grave novamente o ESP32 para incorporar o novo cabeçalho.

## Validação da exportação C++

```bash
g++ -std=c++17 tests/model_smoke.cpp -o model_smoke
./model_smoke
```

O teste confirma que:

- uma entrada seca e livre tende a NORMAL;
- uma entrada intermediária não vira NORMAL;
- uma entrada molhada tende a CRÍTICO;
- as probabilidades somam aproximadamente 1.

## Interpretação das métricas

Use no relatório/vídeo somente as métricas do dataset real. Além de accuracy, observe recall de CRÍTICO: um recall baixo nessa classe significa risco de não detectar uma condição importante.

A matriz de confusão deve ser lida por linha como classe real e por coluna como classe prevista.

## IA com proteção

O modelo apoia a decisão, mas não substitui regras de segurança. O firmware força CRÍTICO ou FALHA em limites objetivos, evitando que uma previsão incerta permita o avanço em uma condição extrema.
