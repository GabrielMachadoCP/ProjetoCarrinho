# 🔗 Dependências do Projeto

Este documento representa as dependências da versão realmente construída do carrinho 2WD.

## 📊 Dependências Principais

| Tarefa | Depende de | Motivo |
|---|---|---|
| Instalação dos motores | Montagem do chassi 2WD | Os motores precisam estar fixados na estrutura |
| Conexão da L298N | Instalação dos motores | A ponte H controla diretamente os motores |
| Teste dos motores | L298N + alimentação | É necessário validar sentido e funcionamento |
| Funções de movimentação | Motores funcionando | O firmware precisa comandar hardware já validado |
| Controle via Wi-Fi | ESP32 configurado | O ESP32 cria a rede e hospeda a interface |
| Controle pelo iPhone | Servidor web funcionando | O navegador acessa a página criada pelo ESP32 |
| Instalação do HC-SR04 | ESP32 e alimentação prontos | O sensor depende da alimentação e GPIOs |
| Leitura segura do ECHO | Divisor resistivo 1 kΩ + 2 kΩ | O GPIO do ESP32 deve receber tensão reduzida |
| Teste de distância | HC-SR04 conectado | O sensor deve estar fisicamente integrado |
| Teste completo | Motores + Wi-Fi + alimentação | Todos os elementos básicos precisam funcionar juntos |
| Operação sem computador | Power bank + pilhas AA | O carrinho precisa funcionar de forma independente |
| TinyML | Sensor validado + dataset + modelo treinado | Evolução prevista para a segunda entrega |

---

## 🔄 Fluxo Principal

```text
Montagem do chassi 2WD
        ↓
Motores + roda boba
        ↓
Ponte H L298N
        ↓
Alimentação dos motores
        ↓
Teste de movimentação
        ↓
ESP32 + firmware
        ↓
Rede Wi-Fi + interface web
        ↓
Controle pelo iPhone
        ↓
Integração do HC-SR04
        ↓
Teste completo
```

---

## 🔋 Dependência de Alimentação

O projeto utiliza duas fontes de energia:

```text
Power bank → ESP32

4 pilhas AA → L298N → motores
```

Os dois circuitos compartilham o **GND**, necessário para que os sinais de controle do ESP32 sejam interpretados corretamente pela ponte H.

---

## 🚨 Caminho Crítico

As atividades que bloqueiam o funcionamento principal são:

1. Montagem do chassi;
2. Instalação dos dois motores;
3. Conexão da L298N;
4. Alimentação;
5. Firmware de movimentação;
6. Wi-Fi do ESP32;
7. Interface de controle;
8. Teste integrado.

A integração do HC-SR04 é necessária para a etapa de sensoriamento e serve de base para a evolução com TinyML.
