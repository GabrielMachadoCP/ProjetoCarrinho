# 📋 Product Backlog — Carrinho Robô 2WD

## 👥 Integrantes

- **Gabriel Machado**
- **Lourenzo Ramos**
- **Vitor Hugo Rodrigues**

O backlog foi atualizado para refletir a versão realmente construída do projeto: chassi 2WD, um único ESP32, controle por Wi-Fi, ponte H L298N e sensor HC-SR04.

---

## 🎯 Backlog do Projeto

| ID | Tarefa | Prioridade | Status |
|---|---|---|---|
| B01 | Montar o chassi 2WD | Alta | Concluído |
| B02 | Instalar os dois motores DC | Alta | Concluído |
| B03 | Instalar a roda boba | Alta | Concluído |
| B04 | Conectar os motores à ponte H L298N | Alta | Concluído |
| B05 | Definir a alimentação dos motores com 4 pilhas AA | Alta | Concluído |
| B06 | Alimentar o ESP32 via power bank | Alta | Concluído |
| B07 | Configurar o ESP32 ESP-WROOM-32 | Alta | Concluído |
| B08 | Implementar os comandos frente, ré, esquerda, direita e parar | Alta | Concluído |
| B09 | Criar rede Wi-Fi própria no ESP32 | Alta | Concluído |
| B10 | Criar interface web de controle | Alta | Concluído |
| B11 | Testar controle pelo navegador do iPhone | Alta | Concluído |
| B12 | Instalar o HC-SR04 na parte frontal | Alta | Concluído |
| B13 | Montar divisor de tensão no pino ECHO | Alta | Concluído |
| B14 | Testar leitura de distância do HC-SR04 | Alta | Concluído |
| B15 | Integrar e organizar o cabeamento | Média | Concluído |
| B16 | Construir a carenagem de papelão | Média | Concluído |
| B17 | Realizar testes de movimentação | Alta | Concluído |
| B18 | Validar alimentação independente do computador | Alta | Concluído |
| B19 | Atualizar documentação do GitHub para a montagem real | Média | Em andamento |
| B20 | Documentar a evolução com TinyML | Alta | Próxima etapa |

---

## 🧩 Organização por Área

| Área | Principais atividades |
|---|---|
| Software | Firmware do ESP32, servidor Wi-Fi, interface web e comandos dos motores |
| Hardware | Chassi 2WD, L298N, motores, alimentação, HC-SR04 e cabeamento |
| Estrutura | Fixação dos componentes e carenagem do caminhão |
| Testes | Validação de direção, alcance do controle, sensor e alimentação |
| Documentação | README, arquitetura, componentes e organização do projeto |

---

## 🔄 Sequência Real de Desenvolvimento

1. Montagem do chassi 2WD;
2. Instalação dos motores e roda boba;
3. Ligação da ponte H;
4. Testes individuais dos motores;
5. Configuração do ESP32;
6. Implementação dos comandos de movimento;
7. Criação do controle via Wi-Fi;
8. Teste pelo iPhone;
9. Instalação do HC-SR04;
10. Teste do sensor;
11. Organização da alimentação;
12. Construção da carenagem;
13. Testes integrados;
14. Atualização da documentação;
15. Evolução para TinyML na segunda entrega.
