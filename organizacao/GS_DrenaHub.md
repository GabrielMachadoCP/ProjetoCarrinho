# Organização da Global Solution — DrenaHub

## Objetivo da sprint

Transformar o carrinho 2WD em um protótipo CitizenTech capaz de patrulhar, identificar água, parar e avisar o operador pela Central DrenaHub.

## Responsabilidades sugeridas

| Frente | Responsável sugerido | Evidência |
|---|---|---|
| Firmware, TinyML e GitHub | Gabriel | código, build e métricas |
| Montagem, pinagem e acabamento | Lourenzo | fotos e teste elétrico |
| Cenários, coleta e vídeo | Vitor | CSV real, tabela e edição |
| Teste final | trio | checklist assinado/revisado |

A divisão é apenas operacional; todos devem compreender problema, arquitetura e demonstração.

## Definition of Done

Uma tarefa só está concluída quando:

- foi executada no hardware quando aplicável;
- o alerta apareceu e foi reconhecido pela Central quando aplicável;
- possui evidência;
- está versionada;
- não contradiz a pinagem final;
- pode ser explicada por qualquer integrante;
- não depende de resultado inventado.

## Riscos

| Risco | Mitigação |
|---|---|
| sensor chegar tarde | adaptar sensor compatível já disponível e manter interface analógica |
| leitura instável | média móvel e calibração no painel |
| ESP32 reiniciar com motores | fontes separadas e GND comum |
| água alcançar eletrônica | bandeja rasa, suporte e teste estacionário |
| classe crítica com pouco recall | balancear coleta e revisar matriz |
| vídeo ultrapassar limite | roteiro de 5min50s e edição objetiva |
| link sem acesso | validar em janela anônima |
