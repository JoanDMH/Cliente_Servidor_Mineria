# Script para ejecutar el servidor - PowerShell

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Simulador de Blockchain - Servidor" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Verificar que el ejecutable exista
if (-not (Test-Path "servidor.exe")) {
    Write-Host "ERROR: servidor.exe no encontrado" -ForegroundColor Red
    Write-Host "Compila primero ejecutando: .\compilar.ps1" -ForegroundColor Yellow
    Write-Host ""
    pause
    exit 1
}

Write-Host "INSTRUCCIONES:" -ForegroundColor Yellow
Write-Host ""
Write-Host "1. Este script iniciará el SERVIDOR (coordinador)" -ForegroundColor White
Write-Host "2. El servidor te pedirá:" -ForegroundColor White
Write-Host "   - Texto de relleno (ej: AA, AAA)" -ForegroundColor White
Write-Host "   - Cantidad de ceros (ej: 1, 2)" -ForegroundColor White
Write-Host "   - Número de workers (ej: 2, 4)" -ForegroundColor White
Write-Host ""
Write-Host "3. Después debes abrir OTRAS terminales y ejecutar:" -ForegroundColor White
Write-Host "   .\worker.exe" -ForegroundColor Cyan
Write-Host ""
Write-Host "Configuración sugerida para prueba rápida:" -ForegroundColor Yellow
Write-Host "  - Texto: AA" -ForegroundColor White
Write-Host "  - Ceros: 1" -ForegroundColor White
Write-Host "  - Workers: 2" -ForegroundColor White
Write-Host ""
Write-Host "Presiona cualquier tecla para iniciar el servidor..." -ForegroundColor Green
pause

Write-Host ""
Write-Host "Iniciando servidor..." -ForegroundColor Cyan
Write-Host ""

# Ejecutar servidor
.\servidor.exe

Write-Host ""
pause
