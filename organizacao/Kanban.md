# 🗂️ Kanban — Carrinho Robô 2WD

O Kanban foi atualizado para refletir o projeto realmente executado.

```text
BACKLOG → TO DO → IN PROGRESS → REVIEW / TEST → DONE
```

---

## 📥 Backlog

| ID | Tarefa | Status |
|---|---|---|
| — | Nenhuma tarefa pendente desta etapa | — |

---

## 📌 To Do

| ID | Tarefa | Status |
|---|---|---|
| — | Nenhuma tarefa pendente nesta etapa | — |

---

## 🚧 In Progress

| ID | Tarefa | Status |
|---|---|---|
| — | Nenhuma tarefa pendente nesta etapa | — |

---

## 🧪 Review / Test

| ID | Tarefa | Status |
|---|---|---|
| — | Nenhuma tarefa pendente nesta etapa | — |

---

## ✅ Done

| ID | Tarefa |
|---|---|
| K01 | Montagem do chassi 2WD |
| K02 | Instalação dos dois motores DC |
| K03 | Instalação da roda boba |
| K04 | Conexão da ponte H L298N |
| K05 | Alimentação dos motores com 4 pilhas AA |
| K06 | Alimentação do ESP32 por power bank |
| K07 | Configuração do ESP32 ESP-WROOM-32 |
| K08 | Implementação de frente, ré, esquerda, direita e parada |
| K09 | Criação da rede Wi-Fi CarrinhoESP32 |
| K10 | Criação da página web de controle |
| K11 | Controle do carrinho pelo iPhone |
| K12 | Instalação do HC-SR04 |
| K13 | Divisor de tensão do pino ECHO |
| K14 | Teste de leitura de distância |
| K15 | Testes dos dois motores |
| K16 | Organização básica do cabeamento |
| K17 | Construção da carenagem do caminhão |
| K18 | Atualização da documentação para a arquitetura real |
| K19 | Revisão final da documentação da primeira etapa |
| K20 | Documentação da etapa TinyML |
| K21 | Organização do dataset e métricas do Edge Impulse |
| K22 | Modelo TinyML embarcado no ESP32 e modo autônomo |

---

## 🎯 Primeira Etapa — Base Funcional

A primeira etapa foi concluída com:

- Chassi 2WD;
- Dois motores;
- Ponte H L298N;
- ESP32 como controlador único;
- Controle via Wi-Fi;
- Interface web acessível pelo iPhone;
- Movimentos de frente, ré, esquerda, direita e parada;
- Alimentação independente;
- HC-SR04 instalado e testado.

---

## 🧠 Segunda Etapa — TinyML

A segunda etapa foi concluída e está documentada em [TinyML/README.md](../TinyML/README.md). Ela incluiu coleta e pré-processamento dos dados, treinamento no Edge Impulse, análise da matriz de confusão, coleta adicional para melhorar o classificador, deployment quantizado e execução do modelo no ESP32 com um novo **Modo Autônomo TinyML**.
