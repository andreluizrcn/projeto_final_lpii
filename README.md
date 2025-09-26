# v1-logging - libtslog
./test_cli <num_threads> <messages_per_thread>
```


Com CMake:


```bash
mkdir build && cd build
cmake ..
make
./test_cli 16 200
```


## Verificações sugeridas
- Executar com muitos threads (ex.: 64) e verificar se o arquivo `test.log` contém todas as linhas.
- Checar formato de timestamp e id da thread.
- Forçar erro de abertura de arquivo (permissões) para verificar tratamento de exceção.


## Tags e release
- Commit e criar tag `v1-logging` com release contendo: `include/libtslog.h`, `src/libtslog.cpp`, `tests/test_cli.cpp`, `Makefile`, `docs/architecture.puml`, `README.md`.


```bash
git add .
git commit -m "v1-logging: libtslog + test CLI"
git tag -a v1-logging -m "v1: libtslog and architecture"
git push origin main --tags
```
```


---


## Checklist para o professor (texto para o corpo do Release/GitHub)


- [x] libtslog: header e implementação.
- [x] Test CLI: simula N threads gravando logs concorrentes.
- [x] Makefile e CMakeLists.txt funcionais.
- [x] Documento de arquitetura (sequence diagram - PlantUML).
- [x] Instruções de build e teste no README.


---



# Relatório de Análise Crítica com IA

Em cada etapa, foi utilizado **auxílio de LLMs (IA)** para revisar o código e identificar possíveis problemas de concorrência:

### Pergunta feita à IA
> "Revise este código de logging concorrente em C++ e identifique possíveis problemas de concorrência (race conditions, deadlocks, starvation). Sugira melhorias."

### Resposta resumida da IA
- O uso de `std::mutex` garante exclusão mútua nas operações de escrita.
- Não há risco de **deadlock**, pois o código não possui múltiplos locks encadeados.
- Não há risco de **starvation**, pois todas as threads competem de forma justa pelo mutex.
- Possível melhoria: considerar uso de uma fila assíncrona de mensagens para reduzir contenção em cenários de altíssima concorrência.
- Garantir tratamento de exceções ao abrir/fechar arquivos de log.

### Mitigações aplicadas
- Confirmação do uso correto de `std::mutex` para exclusão mútua.
- Escrita imediata em arquivo para evitar lock prolongado.
- Tratamento de erros de I/O incluído no teste CLI.
- Planejamento para avaliar fila assíncrona (ThreadSafeQueue) em versões futuras.





