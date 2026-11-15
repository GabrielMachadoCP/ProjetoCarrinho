# Central DrenaHub

## Papel no MVP

A Central é a interface web hospedada no próprio ESP32. Ela simula o núcleo do aplicativo futuro sem acrescentar backend ou nuvem desnecessários à evolução do carrinho. O celular se conecta diretamente à rede do DrenaBot e recebe uma atualização de estado a cada 800 ms.

## Fluxo da ocorrência

```mermaid
sequenceDiagram
    participant R as DrenaBot
    participant E as ESP32
    participant C as Central Web
    participant O as Operador
    R->>E: sensor cruza 30%
    E->>R: parar patrulha
    E->>C: água + nível + ocorrência
    C-->>O: alerta visual (e vibração, se suportada)
    O->>C: reconhecer alerta
    C->>E: POST /alert/ack
    E->>R: aguardar sensor seco
    R->>E: nível abaixo de 15%
    E->>R: retomar patrulha
```

## Informações apresentadas

- conexão do robô;
- modo manual ou patrulha autônoma;
- ação atual;
- nível de água e valor ADC;
- distância frontal;
- classe e confiança TinyML;
- contador de ocorrências;
- último nível detectado;
- último evento operacional;
- referências de calibração.

## Regras simples

1. A água é detectada a partir de 30% do intervalo calibrado.
2. O alerta fica registrado mesmo que a leitura diminua.
3. O operador precisa reconhecer a ocorrência.
4. O ponto só é considerado seco abaixo de 15%.
5. A patrulha retoma quando o alerta foi reconhecido e o sensor está seco.
6. A condição molhada usada na calibração não conta como ocorrência.

## Limite do protótipo

O aviso é local e funciona enquanto a Central está aberta no celular conectado à rede do ESP32. Notificação remota, mapa, banco de dados e múltiplos robôs ficam como evolução de produto, não como dependência desta entrega de Maker Lab.
