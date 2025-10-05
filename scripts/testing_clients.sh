#!/bin/bash
# Testa múltiplos clientes simultaneamente

for i in {1..5}; do
  ./cliente "Mensagem $i do cliente" &
done

wait
echo "Testes finalizados."

