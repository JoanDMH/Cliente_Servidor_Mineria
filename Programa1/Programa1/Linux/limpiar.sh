#!/bin/bash
# Script para limpiar archivos compilados - Linux

echo "Limpiando archivos compilados..."
echo ""

# Eliminar archivos objeto
rm -f *.o
echo "Eliminados archivos .o"

# Eliminar ejecutables
rm -f servidor worker
echo "Eliminados ejecutables (servidor, worker)"

echo ""
echo "Limpieza completa"
echo ""
echo "Archivos eliminados:"
echo "  - *.o (archivos objeto)"
echo "  - servidor (ejecutable)"
echo "  - worker (ejecutable)"
echo ""
