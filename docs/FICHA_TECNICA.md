# Ficha técnica — DrenaHub / DrenaBot 01

## Identificação

| Campo | Conteúdo |
|---|---|
| Tema | CitizenTech |
| Disciplina | Project-Based Maker Lab |
| Turma | 4ESPY |
| Projeto | DrenaHub |
| Protótipo | DrenaBot 01 |
| Plataforma | ESP32 + robô 2WD |
| Novo sensor | sensor analógico de presença/nível de água |
| IA | rede neural TinyML embarcada |
| Operação | manual e autônoma |
| Interface | Central Web local com alerta e confirmação da obstrução |

## Problema

Pequenos acúmulos de água em garagens, docas, depósitos e áreas técnicas podem ser percebidos tarde. A inspeção manual é pontual, depende de disponibilidade humana e não cria uma leitura padronizada do risco.

## Proposta

Um robô de baixo custo percorre pontos de inspeção e combina presença de água, tendência recente e distância de obstáculos. O processamento ocorre no próprio ESP32. O protótipo reage localmente e apresenta os dados ao operador na Central DrenaHub, acessível pelo celular. Quando encontra água, ele interrompe a patrulha e mantém um alerta visível até a confirmação humana.

## Hipótese de valor

Se equipes de operação puderem verificar áreas vulneráveis de forma mais frequente e padronizada, poderão priorizar intervenções antes que um pequeno acúmulo gere dano, interrupção ou risco.

## Públicos iniciais

- administradoras e síndicos de condomínios;
- operadores de galpões e centros logísticos;
- estacionamentos e shopping centers;
- facilities de hospitais, escolas e indústrias;
- defesa civil e prefeituras em futuras versões.

## Escalabilidade

O DrenaBot e a Central Web são o primeiro ponto de entrada. O mesmo software pode integrar sensores fixos, dispositivos adicionais, mapas de inspeção, histórico em nuvem, notificações remotas e ordens de manutenção. Assim, a solução começa com detecção móvel local e pode evoluir para uma plataforma de operação hídrica.

## Indicadores para um piloto futuro

| Indicador | Como medir |
|---|---|
| Pontos inspecionados | contagem por rota |
| Alertas por classe | NORMAL / ATENÇÃO / CRÍTICO |
| Tempo até inspeção humana | horário do alerta versus verificação |
| Falsos alertas | alerta sem água confirmada |
| Eventos encontrados antecipadamente | ocorrência antes de dano/interdição |
| Disponibilidade do dispositivo | horas operacionais / horas previstas |

## Limites do MVP

O protótipo não mede profundidade de enchente, não navega por mapas e não envia dados à nuvem. O sensor resistivo de baixo custo é adequado à prova acadêmica, mas deve ser substituído por solução industrial em operação prolongada.
