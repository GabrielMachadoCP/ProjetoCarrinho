# Checklist da Global Solution — Maker Lab

**Prazo informado no documento:** 19/11/2026 às 23:55

**Forma oficial:** um integrante envia o link do repositório GitHub.
**Atenção:** o documento informa que ZIP, PDF ou DOC não substituem o repositório.

Legenda: ✅ pronto no repositório · 🟨 depende do carrinho/grupo · ⬜ falta fazer

## Entregáveis obrigatórios do documento

| Exigência | Evidência | Estado |
|---|---|---:|
| Evoluir o robô inteligente desenvolvido anteriormente | histórico preservado + comparação no README | ✅ |
| Adicionar pelo menos um novo sensor | sensor analógico de água no GPIO 34 | 🟨 montar fisicamente |
| Melhorar a integração com IA | MLP com três entradas, pipeline e inferência local | ✅ software / 🟨 validar com dados reais |
| Código-fonte completo | `firmware/DrenaHub` e `tinyml` | ✅ |
| Instruções para instalar e executar | README principal | ✅ |
| Lista de componentes | `hardware/componentes.md` | ✅ |
| Integração sensor + IA documentada | arquitetura, firmware e TinyML | ✅ |
| Central Web com alerta de água | painel embarcado, contador e reconhecimento | ✅ software / 🟨 demonstrar no carrinho |
| Vídeo de demonstração com no máximo 6 minutos | roteiro pronto; link ainda vazio | 🟨 gravar |
| Repositório GitHub acessível | repositório público; validar em janela anônima | ✅ |
| Nomes dos integrantes no README | nomes e RMs no topo | ✅ |
| Apenas um integrante realizar a submissão | decisão operacional do grupo | ⬜ definir quem envia |
| Entrega dentro do prazo | submissão até 19/11/2026 23:55 | ⬜ enviar no portal |

## O que o vídeo precisa mostrar

| Item pedido | Como mostrar | Estado |
|---|---|---:|
| Problema CitizenTech | explicar acúmulo de água em áreas urbanas/operacionais | 🟨 |
| Solução proposta | apresentar DrenaHub e DrenaBot | 🟨 |
| Robô funcionando | controle e modo autônomo no piso | 🟨 |
| Novo sensor | close da montagem e leitura mudando | 🟨 |
| Integração com IA | probabilidades e classe na Central | 🟨 |
| Alerta de água | mostrar parada, aviso e reconhecimento | 🟨 |
| Principais funções | manual, patrulha, calibração, detecção e reação | 🟨 |
| Duração máxima de 6 min | seguir roteiro de 5min50s | 🟨 |
| Link no GitHub ou YouTube | colar no README | 🟨 |

## Critérios de avaliação citados no documento

| Critério | O que já foi preparado | O que falta provar |
|---|---|---|
| Funcionamento do novo sensor | leitura, filtro e calibração implementados | teste físico e evidência |
| Uso e integração da IA | treino, exportação e inferência C++ | modelo retreinado com leituras reais |
| Integração hardware/software | pinagem, firmware, API e painel | montagem sem falhas |
| Relevância CitizenTech | problema, públicos, ODS e escala descritos | comunicação clara no vídeo |
| Autonomia e tomada de decisão | máquina de estados e fail-safe | demonstração no carrinho |
| Acabamento e organização | suporte CAD e guia de montagem | fixação e cabos organizados |
| Testes, erros e limitações | plano T01–T12 e limitações honestas | preencher resultados reais |
| Criatividade e inovação | inspeção móvel + plataforma escalável | demonstrar caso concreto |

## Tarefas físicas na ordem recomendada

- [ ] Comprar/receber o sensor analógico de água.
- [ ] Imprimir ou adaptar o suporte e fixar o sensor.
- [ ] Fazer as três ligações do novo sensor.
- [ ] Compilar e gravar o firmware no ESP32.
- [ ] Calibrar seco e molhado na Central.
- [ ] Testar a parada, o alerta e o botão de reconhecimento.
- [ ] Executar os testes T01–T12.
- [ ] Tirar fotos das ligações e do protótipo final.
- [ ] Coletar dados reais das três classes.
- [ ] Retreinar, revisar métricas e regravar o modelo.
- [ ] Atualizar a tabela de testes e as evidências.
- [ ] Gravar o vídeo seguindo o roteiro.
- [ ] Publicar o vídeo como não listado ou adicionar ao repositório.
- [ ] Colar o link do vídeo no README.
- [ ] Conferir o GitHub em uma janela anônima.
- [ ] Um integrante submeter o link e citar os três membros antes do prazo.

## Conferência final de cinco minutos

- [ ] O README abre e identifica projeto, turma, nomes e RMs.
- [ ] O link do vídeo funciona sem solicitar permissão.
- [ ] O vídeo tem no máximo seis minutos.
- [ ] O código não contém senha pessoal, token ou segredo.
- [ ] A pinagem documentada é a mesma da montagem.
- [ ] O repositório contém o firmware final retreinado.
- [ ] Nenhum resultado físico foi declarado sem evidência.
