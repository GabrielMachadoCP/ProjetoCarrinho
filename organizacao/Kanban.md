# 🗂️ Kanban — Carrinho Robô 2WD

O Kanban foi atualizado para refletir o projeto realmente executado.

```text
BACKLOG → TO DO → IN PROGRESS → REVIEW / TEST → DONE
```

---

## 📥 Backlog

| ID | Tarefa | Status |
|---|---|---|
| K20 | Documentar etapa TinyML | Próxima entrega |
| K21 | Organizar dataset e métricas do Edge Impulse | Próxima entrega |
| K22 | Documentar modelo embarcado no ESP32 | Próxima entrega |

---

## 📌 To Do

| ID | Tarefa | Status |
|---|---|---|
| K19 | Revisão final da documentação atual | Em andamento |

---

## 🚧 In Progress

| ID | Tarefa | Status |
|---|---|---|
| K18 | Atualização dos arquivos do repositório para a arquitetura real | Em andamento |

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

A evolução com TinyML será documentada separadamente e incluirá coleta de dados, pré-processamento, treinamento no Edge Impulse, avaliação do classificador, deployment e execução do modelo no ESP32.
