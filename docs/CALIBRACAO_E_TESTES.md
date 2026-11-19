# Plano de calibração e testes físicos

Este documento separa o que pode ser validado por software do que precisa ser executado com o carrinho real.

## Preparação

- mantenha o chassi sobre superfície seca;
- use uma bandeja rasa ou esponja, nunca uma poça profunda;
- deixe ESP32, L298N, motores e fontes longe da água;
- suspenda as rodas no primeiro teste de movimento;
- mantenha um cabo USB disponível para regravação;
- seque e limpe o sensor após cada sessão.

## Calibração

1. Ligue o ESP32 e aguarde a rede `DrenaHub-DrenaBot`.
2. Abra `192.168.4.1`.
3. Espere pelo menos dois segundos para preencher a média móvel.
4. Com o sensor seco, registre **SECO**.
5. Toque a área sensível na condição máxima da maquete e registre **MOLHADO**.
6. Volte o sensor ao seco; a referência molhada da calibração não será contada como ocorrência.
7. Repita seco e molhado três vezes e confirme estabilidade.
8. Reinicie o ESP32 e confira se os valores persistiram.


## Coleta real para IA

Colete pelo menos 90 amostras no total e mantenha as três classes representadas. Para um resultado mais robusto, faça 90 segundos por classe em mais de uma condição de distância.

```bash
python tinyml/collect_serial.py --port COM3 --label NORMAL --seconds 90
python tinyml/collect_serial.py --port COM3 --label ATENCAO --seconds 90
python tinyml/collect_serial.py --port COM3 --label CRITICO --seconds 90
python tinyml/train_drenahub_model.py --input tinyml/data/drenahub_real.csv
```
