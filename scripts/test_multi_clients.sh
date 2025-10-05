#!/bin/bash
# Executa múltiplos clientes em paralelo para teste de concorrência

echo "[TEST] Iniciando múltiplos clientes..."
for i in {1..5}; do
  ./client "Mensagem $i do cliente" &
done

wait
echo "[TEST] Todos os clientes concluíram."

