# Script para ejecutar un worker - PowerShell

param(
    [string]$ServidorIP = "127.0.0.1"
)

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Worker - Cliente de Minería" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Verificar que el ejecutable exista
if (-not (Test-Path "worker.exe")) {
    Write-Host "ERROR: worker.exe no encontrado" -ForegroundColor Red
    Write-Host "Compila primero ejecutando: .\compilar.ps1" -ForegroundColor Yellow
    Write-Host ""
    pause
    exit 1
}

if ($ServidorIP -eq "127.0.0.1") {
    Write-Host "Conectando al servidor en localhost..." -ForegroundColor Yellow
    Write-Host ""
    .\worker.exe
} else {
    Write-Host "Conectando al servidor en $ServidorIP..." -ForegroundColor Yellow
    Write-Host ""
    .\worker.exe $ServidorIP
}

Write-Host ""
pause
