# 📊 Priorização MoSCoW — Carrinho Robô 2WD

A técnica MoSCoW foi atualizada para refletir a versão realmente desenvolvida do projeto.

---

## 🔴 Must Have

| Funcionalidade | Justificativa |
|---|---|
| Chassi 2WD | Estrutura principal do carrinho |
| Dois motores DC | Responsáveis pela movimentação |
| Duas rodas + roda boba | Configuração mecânica usada |
| Ponte H L298N | Controle elétrico dos motores |
| ESP32 ESP-WROOM-32 | Controlador principal do sistema |
| Alimentação dos motores | Necessária para o movimento |
| Power bank para o ESP32 | Permite funcionamento independente |
| Movimento para frente | Função básica |
| Movimento para trás | Necessário para manobras |
| Curva para esquerda | Controle de direção |
| Curva para direita | Controle de direção |
| Parada | Segurança e controle |
| Wi-Fi criado pelo ESP32 | Meio de comunicação com o usuário |
| Interface web | Permite controlar o carrinho pelo celular |
| Integração hardware + software | Necessária para o funcionamento completo |

---

## 🟠 Should Have

| Funcionalidade | Justificativa |
|---|---|
| HC-SR04 | Medição da distância até obstáculos |
| Divisor de tensão no ECHO | Proteção da entrada do ESP32 |
| Organização do cabeamento | Melhora confiabilidade e montagem |
| Carenagem | Melhora acabamento e identidade visual |
| Testes de integração | Valida o funcionamento conjunto |
| Documentação técnica | Registra a solução realmente construída |

---

## 🟡 Could Have

| Funcionalidade | Benefício |
|---|---|
| Controle de velocidade por PWM | Movimento mais suave |
| Indicador de bateria | Facilita monitoramento |
| Buzzer | Feedback sonoro |
| Faróis ou LEDs | Sinalização visual |
| Telemetria de distância na página web | Exibe dados do HC-SR04 ao usuário |
| Melhor acabamento da carenagem | Evolução estética do protótipo |

---

## 🔵 Próxima Entrega — TinyML

A parte de TinyML não é detalhada neste documento porque será registrada como a segunda entrega do projeto. Essa evolução utiliza o HC-SR04, pré-processamento dos dados, treinamento no Edge Impulse e execução do modelo no ESP32.

---

## ⚪ Fora do Escopo Atual

| Funcionalidade | Motivo |
|---|---|
| GPS | Não necessário para o objetivo do protótipo |
| Câmera com transmissão de vídeo | Não necessária nesta versão |
| Mapeamento completo do ambiente | Complexidade acima do escopo |
| Reconhecimento visual de objetos | Exigiria câmera e processamento adicional |
| Aplicativo mobile nativo | A interface web já atende ao controle remoto |

---

## 🎯 Ordem de Prioridade

```text
1. Estrutura 2WD
        ↓
2. Motores + L298N
        ↓
3. ESP32
        ↓
4. Controle Wi-Fi
        ↓
5. Integração e testes
        ↓
6. HC-SR04
        ↓
7. Documentação
        ↓
8. Evolução TinyML
```
