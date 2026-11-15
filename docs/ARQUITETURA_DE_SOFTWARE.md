# Arquitetura de software

## Componentes

| Camada | Responsabilidade | Implementação |
|---|---|---|
| Aquisição | ler água e distância | ADC + pulso ultrassônico |
| Pré-processamento | filtrar e normalizar | média móvel e calibração |
| Inferência | estimar classe de risco | MLP TinyML em C++ |
| Segurança | garantir limites críticos | regras determinísticas |
| Controle | acionar movimento | GPIOs da L298N |
| Interface | comandos e telemetria | WebServer local |
| Persistência | manter calibração | Preferences/NVS |
| Coleta | formar dataset real | telemetria serial + Python |

## Máquina de decisão

```mermaid
stateDiagram-v2
    [*] --> Manual
    Manual --> Autonomo: ativar IA
    Autonomo --> Normal: risco normal
    Autonomo --> Atencao: risco moderado
    Autonomo --> Critico: risco alto
    Autonomo --> Falha: sensor inválido
    Normal --> Avancar
    Atencao --> Parar
    Critico --> Afastar
    Falha --> Parar
    Autonomo --> Manual: assumir controle
```

## API local

| Método | Rota | Uso |
|---|---|---|
| GET | `/` | painel web |
| GET | `/api/status` | estado e telemetria em JSON |
| POST | `/mode/manual` | ativar modo manual |
| POST | `/mode/auto` | ativar modo autônomo |
| POST | `/move/forward` | avançar no modo manual |
| POST | `/move/reverse` | recuar no modo manual |
| POST | `/move/left` | girar à esquerda |
| POST | `/move/right` | girar à direita |
| POST | `/move/stop` | parar |
| POST | `/calibrate/dry` | salvar referência seca |
| POST | `/calibrate/wet` | salvar referência molhada |

## Telemetria

Uma linha por segundo é enviada na serial:

```text
telemetry,millis,adc,nivel,subida,distancia,obstaculo,risco
```

O coletor converte as variáveis em CSV rotulado. A classificação real é informada pelo operador durante cada execução do script.

## Escolhas técnicas

- GPIO 34 pertence ao ADC1 e não conflita com o Wi-Fi.
- Inferência local mantém o funcionamento sem internet.
- O cabeçalho gerado evita dependências externas no ESP32.
- A camada de segurança tem prioridade sobre a rede neural.
- A calibração persistente absorve variações entre unidades do sensor.
