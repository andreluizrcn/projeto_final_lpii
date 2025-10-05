# README - Sistema Cliente/Servidor TCP com Logging Thread-Safe

## Descrição do Projeto

Sistema completo de comunicação cliente/servidor TCP implementado em C++, com biblioteca de logging thread-safe (`libtslog`) e suporte a múltiplos clientes concorrentes. O projeto demonstra o uso prático de conceitos de programação concorrente: threads, exclusão mútua, variáveis de condição, monitores e sockets.

## Estrutura do Projeto

```
.
├── include/
│   └── libtslog.h          # API da biblioteca de logging thread-safe
├── src/
│   ├── libtslog.cpp        # Implementação da libtslog
│   ├── server.cpp          # Servidor TCP concorrente
│   └── client.cpp          # Cliente TCP
├── tests/
│   └── test_cli.cpp        # Teste de logging concorrente
├── scripts/
│   ├── test_multi_clients.sh    # Script de teste com múltiplos clientes
│   └── testing_clients.sh       # Script alternativo de teste
├── docs/
│   ├── architecture.puml        # Diagramas de arquitetura (PlantUML)
│   ├── sequence_ver2_cli.puml   # Diagrama v2-cli
│   └── sequence_ver3_cli.puml   # Diagrama v3-final
├── Makefile                # Build com Make
├── CMakeLists.txt          # Build com CMake
├── README.md               # Este arquivo
└── RELATORIO_FINAL_ANALISE_CRITICA_COM_IA.pdf  # Relatório de análise com IA
```

## Funcionalidades Implementadas

### Biblioteca libtslog (v1-logging)
- Logger singleton thread-safe
- Fila de mensagens protegida por mutex e variáveis de condição
- Thread worker dedicada para escrita em arquivo
- Suporte a níveis de log: DEBUG, INFO, WARN, ERROR
- Timestamps automáticos com ID da thread
- Shutdown gracioso com flush de mensagens pendentes

### Sistema Cliente/Servidor (v2-cli e v3-final)
- **Servidor TCP:**
  - Aceita múltiplos clientes simultaneamente (porta 8080)
  - Thread dedicada por cliente
  - Echo de mensagens com logging completo
  - Gerenciamento automático de recursos (RAII)
  
- **Cliente TCP:**
  - Conexão ao servidor via socket
  - Envio de mensagens por linha de comando
  - Recepção e exibição de respostas
  - Logging de todas as operações

### Mecanismos de Concorrência

1. **Threads:** Uso de `std::thread` para:
   - Worker de logging assíncrono
   - Atendimento concorrente de clientes no servidor

2. **Exclusão Mútua:** `std::mutex` protegendo:
   - Fila de mensagens do logger
   - Arquivo de log durante escrita

3. **Variáveis de Condição:** `std::condition_variable` para:
   - Sincronização entre produtor (threads de log) e consumidor (worker)
   - Notificação de shutdown gracioso

4. **Monitores:** Classe `ThreadSafeQueue` encapsula sincronização

5. **Sockets:** API POSIX para comunicação TCP cliente/servidor

6. **RAII:** Classe `SocketGuard` para:
   - Fechamento automático de sockets
   - Gerenciamento seguro de recursos

## Como Compilar

### Opção 1: Makefile (Recomendado para Linux)

```bash
make
```

**Executáveis gerados:**
- `server` - Servidor TCP
- `client` - Cliente TCP
- `test_cli` - Teste de logging concorrente

**Limpar build:**
```bash
make clean
```

### Opção 2: CMake (Multiplataforma)

```bash
mkdir -p build && cd build
cmake ..
make
cd ..
```

**Executáveis gerados em:** `build/server`, `build/client`, `build/test_cli`

## Como Executar

### 1. Testar Biblioteca de Logging (v1-logging)

```bash
./test_cli [num_threads] [messages_per_thread]
```

**Exemplo:**
```bash
./test_cli 16 200
# Cria 16 threads, cada uma enviando 200 mensagens de log
# Resultado salvo em: test.log
```

**Verificações:**
- Arquivo `test.log` deve conter todas as mensagens (3200 linhas no exemplo)
- Formato: `YYYY-MM-DD HH:MM:SS [T<thread_id>] [INFO] worker X message Y`
- Nenhuma linha deve estar corrompida ou incompleta

### 2. Executar Sistema Cliente/Servidor (v2-cli e v3-final)

**Terminal 1 - Servidor:**
```bash
./server
# Aguarda conexões na porta 8080
# Logs salvos em: server.log
```

**Terminal 2 - Cliente único:**
```bash
./client "Olá, servidor!"
# Envia mensagem e exibe resposta
# Logs salvos em: client.log
```

**Teste com múltiplos clientes simultâneos:**
```bash
chmod +x scripts/test_multi_clients.sh
./scripts/test_multi_clients.sh
```

**Ou manualmente:**
```bash
./client "Cliente 1" &
./client "Cliente 2" &
./client "Cliente 3" &
wait
```

### 3. Análise de Logs

**Verificar logs do servidor:**
```bash
cat server.log
```

**Verificar logs dos clientes:**
```bash
cat client.log
```

**Verificar ordem temporal e IDs de threads:**
```bash
grep "\[T" server.log | sort
```

## Diagramas de Arquitetura

### Diagrama 1: Logging Thread-Safe
```plantuml
@startuml
actor Client
participant "Client Thread" as CT
participant "TSLogger API" as L
participant "Logger Worker" as W

Client -> CT: send message
CT -> L: TSLogger::instance().info(msg)
L -> L: enqueue message (ThreadSafeQueue)
L -> W: notify
W -> W: pop messages
W -> file: write log
@enduml
```

### Diagrama 2: Comunicação Cliente/Servidor
```plantuml
@startuml
title Diagrama de Sequência - Chat TCP

actor User
participant "Cliente CLI" as C
participant "Servidor" as S
participant "TSLogger" as L

User -> C: Digita mensagem
C -> S: Envia mensagem (TCP)
S -> L: Log de recebimento
S -> C: Resposta (echo)
C -> L: Log da resposta recebida
@enduml
```

## Requisitos Atendidos

### Requisitos Funcionais
- [x] Threads para concorrência
- [x] Exclusão mútua com `std::mutex`
- [x] Variáveis de condição para sincronização
- [x] Monitores (classe `ThreadSafeQueue`)
- [x] Comunicação via sockets TCP
- [x] Gerenciamento de recursos com RAII
- [x] Tratamento de erros com exceções
- [x] Logging concorrente obrigatório

### Requisitos de Qualidade
- [x] Modularidade (biblioteca separada)
- [x] Código limpo e documentado
- [x] Build funcional (Makefile e CMake)
- [x] Scripts de teste automatizado

### Requisitos de Documentação
- [x] README completo
- [x] Diagramas de sequência
- [x] Instruções de build e execução
- [x] Análise crítica com IA

## Testes de Concorrência

### Teste 1: Logging com Alta Concorrência
```bash
./test_cli 64 500
# 64 threads, 500 mensagens cada = 32.000 linhas
# Tempo esperado: < 5 segundos
```

**Verificação:**
```bash
wc -l test.log  # Deve mostrar exatamente 32000 linhas
```

### Teste 2: Múltiplos Clientes Simultâneos
```bash
for i in {1..20}; do ./client "Teste $i" & done; wait
```

**Verificação:**
```bash
grep "Novo cliente conectado" server.log | wc -l  # Deve mostrar 20
```

### Teste 3: Stress Test
```bash
# Terminal 1
./server

# Terminal 2
for i in {1..100}; do 
  ./client "Stress test $i" & 
done
wait
```

## Dependências

- **Compilador:** GCC 7+ ou Clang 6+ (suporte a C++17)
- **Sistema Operacional:** Linux (testado em Ubuntu 20.04+)
- **Bibliotecas:** 
  - `pthread` (POSIX threads)
  - Sockets POSIX (arpa/inet.h)
