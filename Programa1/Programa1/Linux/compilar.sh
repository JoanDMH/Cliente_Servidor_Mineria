#!/bin/bash
# Script de compilacion para Linux
# Simulador de Blockchain

echo "========================================"
echo "Compilando Simulador de Blockchain"
echo "Plataforma: Linux"
echo "========================================"
echo ""

# Verificar si existe gcc
if ! command -v gcc &> /dev/null; then
    echo "ERROR: gcc no está instalado"
    echo "Instala gcc con uno de estos comandos:"
    echo "  Ubuntu/Debian: sudo apt install build-essential"
    echo "  Fedora/RHEL:   sudo dnf install gcc"
    echo "  Arch:          sudo pacman -S gcc"
    exit 1
fi

echo "GCC encontrado: $(gcc --version | head -n1)"
echo ""

# Compilar utilidades de hash
echo "Compilando utilidades de hash..."
gcc -c utilidades_hash.c -o utilidades_hash.o
if [ $? -ne 0 ]; then
    echo "ERROR al compilar utilidades_hash.c"
    exit 1
fi
echo "[OK] utilidades_hash.o"

# Compilar servidor
echo "Compilando servidor..."
gcc -c servidor_linux.c -o servidor_linux.o -pthread
if [ $? -ne 0 ]; then
    echo "ERROR al compilar servidor_linux.c"
    exit 1
fi
echo "[OK] servidor_linux.o"

gcc servidor_linux.o utilidades_hash.o -o servidor -lpthread
if [ $? -ne 0 ]; then
    echo "ERROR al enlazar servidor"
    exit 1
fi
chmod +x servidor
echo "[OK] servidor"

# Compilar worker
echo "Compilando worker..."
gcc -c worker_linux.c -o worker_linux.o -pthread
if [ $? -ne 0 ]; then
    echo "ERROR al compilar worker_linux.c"
    exit 1
fi
echo "[OK] worker_linux.o"

gcc worker_linux.o utilidades_hash.o -o worker -lpthread
if [ $? -ne 0 ]; then
    echo "ERROR al enlazar worker"
    exit 1
fi
chmod +x worker
echo "[OK] worker"

echo ""
echo "========================================"
echo "Compilacion exitosa!"
echo "========================================"
echo ""
echo "Ejecutables generados:"
echo "  ./servidor  - Lider/Coordinador"
echo "  ./worker    - Cliente/Minero"
echo ""
echo "Para ejecutar:"
echo "  Terminal 1: ./servidor"
echo "  Terminales adicionales: ./worker"

