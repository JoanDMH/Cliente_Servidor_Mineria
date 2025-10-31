#!/bin/bash
# Script para ejecutar el servidor - Linux

echo "========================================"
echo "Simulador de Blockchain - Servidor"
echo "========================================"
echo ""

# Verificar que el ejecutable exista
if [ ! -f "./servidor" ]; then
    echo "ERROR: servidor no encontrado"
    echo "Compila primero ejecutando: ./compilar.sh"
    echo ""
    read -p "Presiona Enter para salir..."
    exit 1
fi

# Verificar permisos de ejecución
if [ ! -x "./servidor" ]; then
    echo "ADVERTENCIA: servidor no tiene permisos de ejecucion"
    echo "Agregando permisos..."
    chmod +x servidor
fi

echo "INSTRUCCIONES:"
echo ""
echo "1. Este script iniciará el SERVIDOR (coordinador)"
echo "2. El servidor te pedirá:"
echo "   - Texto de relleno (ej: AA, AAA)"
echo "   - Cantidad de ceros (ej: 1, 2)"
echo "   - Número de workers (ej: 2, 4)"
echo ""
echo "3. Después debes abrir OTRAS terminales y ejecutar:"
echo "   ./worker"
echo ""
echo "Configuración sugerida para prueba rápida:"
echo "  - Texto: AA"
echo "  - Ceros: 1"
echo "  - Workers: 2"
echo ""
read -p "Presiona Enter para iniciar el servidor..."

echo ""
echo "Iniciando servidor..."
echo ""

# Ejecutar servidor
./servidor

echo ""
read -p "Presiona Enter para salir..."
