#!/bin/bash
# Script para ejecutar un worker - Linux

SERVIDOR_IP=${1:-"127.0.0.1"}

echo "========================================"
echo "Worker - Cliente de Minería"
echo "========================================"
echo ""

# Verificar que el ejecutable exista
if [ ! -f "./worker" ]; then
    echo "ERROR: worker no encontrado"
    echo "Compila primero ejecutando: ./compilar.sh"
    echo ""
    read -p "Presiona Enter para salir..."
    exit 1
fi

# Verificar permisos de ejecución
if [ ! -x "./worker" ]; then
    echo "ADVERTENCIA: worker no tiene permisos de ejecucion"
    echo "Agregando permisos..."
    chmod +x worker
fi

if [ "$SERVIDOR_IP" = "127.0.0.1" ]; then
    echo "Conectando al servidor en localhost..."
    echo ""
    ./worker
else
    echo "Conectando al servidor en $SERVIDOR_IP..."
    echo ""
    ./worker $SERVIDOR_IP
fi

echo ""
read -p "Presiona Enter para salir..."
