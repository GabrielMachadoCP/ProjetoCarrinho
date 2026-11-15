# Roteiro do vídeo — máximo 6 minutos

Meta sugerida: **5min30s a 5min50s**, deixando margem para cortes e tela final.

## 0:00–0:35 — problema

**Imagem:** pontos baixos, ralo/garagem da maquete ou uma foto própria do contexto.

**Fala sugerida:**

“Pequenos acúmulos de água em condomínios, estacionamentos e galpões costumam ser percebidos apenas em inspeções pontuais. Quando a água chega a estoques, veículos ou equipamentos, um problema inicialmente simples pode gerar dano e interrupção. Para a Global Solution CitizenTech, nós criamos o DrenaHub.”

## 0:35–1:05 — proposta

**Imagem:** plano geral do DrenaBot.

**Fala sugerida:**

“O DrenaHub é uma proposta de monitoramento preventivo de drenagem. Este é o DrenaBot 01, nosso MVP móvel. Ele evolui o carrinho das etapas anteriores para detectar água, evitar obstáculos, classificar o risco localmente e reagir sem depender de internet.”

## 1:05–1:50 — hardware e novo sensor

**Imagem:** close no ESP32, HC-SR04, sensor de água e L298N.

Explique:

- sensor analógico de água no GPIO 34;
- HC-SR04 nos GPIOs 18 e 19;
- L298N e motores;
- fontes separadas e GND comum;
- cuidado para manter eletrônica longe da água.

Mostre a leitura do ADC mudando no painel ao tocar uma esponja úmida.

## 1:50–2:35 — IA

**Imagem:** painel e, por poucos segundos, matriz de confusão/código.

**Fala sugerida:**

“O ESP32 filtra e normaliza as leituras. A rede TinyML usa nível de água, velocidade de subida e risco de obstáculo para estimar três classes: normal, atenção e crítico. A inferência roda no próprio microcontrolador. Regras de segurança têm prioridade em falha ou nível extremo.”

Substitua qualquer métrica mencionada pela obtida no dataset **real**. Não apresente o baseline sintético como validação física.

## 2:35–4:55 — demonstração

Grave em plano contínuo sempre que possível:

1. conexão à rede e abertura do painel;
2. calibração seca e molhada;
3. controle manual;
4. modo autônomo em condição normal;
5. objeto à frente e reação do robô;
6. condição intermediária e parada;
7. condição crítica e manobra de afastamento;
8. probabilidades e classe mudando no painel.

Uma pessoa pode segurar o celular enquanto outra opera a maquete.

## 4:55–5:25 — testes e limitações

**Imagem:** tabela preenchida ou montagem.

**Fala sugerida:**

“Validamos movimentos, distância, calibração, persistência, três classes e parada segura. Como limitação, este sensor é apropriado ao protótipo e não a uma enchente real. Uma versão comercial usaria proteção IP, sensores industriais e histórico em nuvem.”

Só diga que um teste foi validado se ele realmente foi executado.

## 5:25–5:50 — impacto e encerramento

**Fala sugerida:**

“O MVP começa em condomínios, galpões e estacionamentos, mas a arquitetura pode receber sensores fixos, mapas, alertas e manutenção preditiva. Assim, o DrenaHub transforma inspeções reativas em acompanhamento preventivo e escalável.”

Finalize com:

- nome DrenaHub;
- nomes e RMs;
- GitHub;
- disciplinas/turma.

## Checklist de gravação

- [ ] usar vídeo horizontal;
- [ ] limpar lente e ambiente;
- [ ] garantir luz sobre o sensor;
- [ ] deixar tela do celular legível;
- [ ] gravar uma tomada reserva de cada cenário;
- [ ] evitar música alta;
- [ ] remover pausas e erros na edição;
- [ ] conferir duração abaixo de 6 minutos;
- [ ] testar o link sem estar logado.
