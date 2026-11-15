# Validação de software

Execução realizada em **15/11/2026** antes da publicação.

## Resultados

| Verificação | Comando | Resultado |
|---|---|---|
| treinamento reproduzível | `python3 tinyml/train_drenahub_model.py` | aprovado |
| exportação C++ | geração de `drenahub_model.h` | aprovado |
| inferência isolada | `g++ ... tests/model_smoke.cpp` | aprovado |
| compilação ESP32 | `pio run` | aprovado |

## Build ESP32

- plataforma: `espressif32@6.9.0`;
- placa: `esp32dev`;
- framework: Arduino;
- RAM: 45.476 bytes de 327.680 (**13,9%**);
- flash: 799.901 bytes de 1.310.720 (**61,0%**);
- saída: `SUCCESS`.

## Baseline TinyML

- amostras sintéticas: 3.600;
- treino: 2.700;
- teste: 900;
- accuracy: 0,9967;
- smoke test: `TinyML smoke test: OK`.

A métrica acima comprova a reprodução do pipeline, não a assertividade física do produto. A validação com o sensor real continua explicitamente pendente.

## Integração contínua

O workflow `.github/workflows/validate.yml` repete o treinamento, confere se os artefatos estão sincronizados, testa a inferência C++ e compila o firmware a cada push ou pull request.
