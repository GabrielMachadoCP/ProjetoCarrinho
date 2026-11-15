# Validação de software

Execução realizada em **15/11/2026** antes da publicação.

## Resultados

| Verificação | Comando | Resultado |
|---|---|---|
| treinamento reproduzível | `python3 tinyml/train_drenahub_model.py` | aprovado |
| exportação C++ | geração de `drenahub_model.h` | aprovado |
| inferência isolada | `g++ ... tests/model_smoke.cpp` | aprovado |
| contrato Central/firmware | `python3 tests/dashboard_contract.py` | aprovado |
| compilação ESP32 | `pio run` | aprovado |

## Build ESP32

- plataforma: `espressif32@6.9.0`;
- placa: `esp32dev`;
- framework: Arduino;
- RAM: 45.484 bytes de 327.680 (**13,9%**);
- flash: 806.301 bytes de 1.310.720 (**61,5%**);
- saída: `SUCCESS`.

## Baseline TinyML

- amostras sintéticas: 3.600;
- treino: 2.700;
- teste: 900;
- accuracy: 0,9967;
- smoke test: `TinyML smoke test: OK`.

A métrica acima comprova a reprodução do pipeline, não a assertividade física do produto. A validação com o sensor real continua explicitamente pendente.

## Integração contínua

O workflow `.github/workflows/validate.yml` repete o treinamento, testa o comportamento da inferência C++ e compila o firmware gerado a cada push ou pull request. Ele não compara os pesos bit a bit, pois pequenas diferenças de otimizador e biblioteca numérica podem alterar os pesos entre sistemas sem mudar as classes previstas.
