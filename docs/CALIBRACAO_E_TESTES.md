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

Registre no quadro:

| Repetição | ADC seco | ADC molhado | Diferença | Observação |
|---:|---:|---:|---:|---|
| 1 |  |  |  |  |
| 2 |  |  |  |  |
| 3 |  |  |  |  |

## Casos de teste

| ID | Cenário | Procedimento | Resultado esperado | Real | Status |
|---|---|---|---|---|---|
| T01 | inicialização | ligar ESP32 | rede e painel disponíveis |  | ⬜ |
| T02 | controle manual | usar todos os botões | movimentos coerentes e parada |  | ⬜ |
| T03 | obstáculo | aproximar objeto | distância diminui e robô evita |  | ⬜ |
| T04 | sensor seco | manter área sensível seca | nível próximo de 0% |  | ⬜ |
| T05 | água em patrulha | aplicar umidade intermediária | parada e alerta na Central |  | ⬜ |
| T06 | nível crítico | tocar referência molhada | classe CRÍTICO e robô parado |  | ⬜ |
| T07 | subida rápida | molhar progressivamente | `rise_rate` aumenta |  | ⬜ |
| T08 | falha ultrassônica | bloquear/desconectar em bancada | classe FALHA e parada |  | ⬜ |
| T09 | reinício | desligar e ligar | calibração preservada |  | ⬜ |
| T10 | autonomia | executar sequência completa | decisão sem comando manual |  | ⬜ |
| T11 | confirmação | clicar em Confirmar drenagem obstruída | alerta fecha e patrulha retoma imediatamente |  | ⬜ |
| T12 | retomada da patrulha | retirar o sensor da água | carrinho volta a andar abaixo de 15% |  | ⬜ |
| T13 | confirmação | iniciar modo autônomo e cancelar/confirmar | cancelar não move; confirmar inicia imediatamente |  | ⬜ |
| T14 | emergência | acionar parada durante a patrulha | motores param e modo volta para manual |  | ⬜ |
| T15 | nova água com sensor úmido | confirmar T11 e elevar a leitura em pelo menos 12 pontos | nova ocorrência e nova parada |  | ⬜ |
| T16 | sensor saturado | manter leitura em 95% ou mais após T11 | Central solicita secagem do sensor |  | ⬜ |

Não marque como aprovado sem observar o carrinho.

## Coleta real para IA

Colete pelo menos 90 amostras no total e mantenha as três classes representadas. Para um resultado mais robusto, faça 90 segundos por classe em mais de uma condição de distância.

```bash
python tinyml/collect_serial.py --port COM3 --label NORMAL --seconds 90
python tinyml/collect_serial.py --port COM3 --label ATENCAO --seconds 90
python tinyml/collect_serial.py --port COM3 --label CRITICO --seconds 90
python tinyml/train_drenahub_model.py --input tinyml/data/drenahub_real.csv
```

Depois do retreinamento:

1. confira se `firmware/DrenaHub/drenahub_model.h` foi atualizado;
2. anote accuracy, precision e recall por classe;
3. grave novamente o ESP32;
4. repita T04, T05, T06 e T10;
5. versione o CSV, a matriz e as métricas, sem inventar resultados.

## Evidências mínimas

- foto geral do carrinho;
- close do novo sensor e da ligação no GPIO 34;
- Central mostrando cada classe e o alerta de água;
- obstrução confirmada e contador de ocorrências;
- matriz de confusão com dados reais;
- tabela de testes preenchida;
- vídeo do funcionamento real.
