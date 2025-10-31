# Script de compilación para Windows - PowerShell
# Simulador de Blockchain

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Compilando Simulador de Blockchain" -ForegroundColor Cyan
Write-Host "Plataforma: Windows" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Verificar si existe gcc
$gccExists = Get-Command gcc -ErrorAction SilentlyContinue

if (-not $gccExists) {
    Write-Host "ERROR: gcc no está instalado o no está en el PATH" -ForegroundColor Red
    Write-Host "Instala MinGW-w64 desde: https://www.msys2.org/" -ForegroundColor Yellow
    Write-Host "Ver GUIA_WINDOWS.md para instrucciones detalladas" -ForegroundColor Yellow
    pause
    exit 1
}

Write-Host "GCC encontrado: $((gcc --version)[0])" -ForegroundColor Green
Write-Host ""

# Compilar utilidades de hash
Write-Host "Compilando utilidades de hash..." -ForegroundColor Yellow
gcc -c utilidades_hash.c -o utilidades_hash.o

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR al compilar utilidades_hash.c" -ForegroundColor Red
    pause
    exit 1
}
Write-Host "[OK] utilidades_hash.o" -ForegroundColor Green

# Compilar servidor
Write-Host "Compilando servidor..." -ForegroundColor Yellow
gcc -c servidor.c -o servidor.o

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR al compilar servidor.c" -ForegroundColor Red
    pause
    exit 1
}
Write-Host "[OK] servidor.o" -ForegroundColor Green

gcc servidor.o utilidades_hash.o -o servidor.exe -lws2_32

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR al enlazar servidor.exe" -ForegroundColor Red
    pause
    exit 1
}
Write-Host "[OK] servidor.exe" -ForegroundColor Green

# Compilar worker
Write-Host "Compilando worker..." -ForegroundColor Yellow
gcc -c worker.c -o worker.o

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR al compilar worker.c" -ForegroundColor Red
    pause
    exit 1
}
Write-Host "[OK] worker.o" -ForegroundColor Green

gcc worker.o utilidades_hash.o -o worker.exe -lws2_32

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR al enlazar worker.exe" -ForegroundColor Red
    pause
    exit 1
}
Write-Host "[OK] worker.exe" -ForegroundColor Green

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "Compilación exitosa!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "Ejecutables generados:" -ForegroundColor Cyan
Write-Host "  servidor.exe  - Líder/Coordinador" -ForegroundColor White
Write-Host "  worker.exe    - Cliente/Minero" -ForegroundColor White
Write-Host ""
Write-Host "Para ejecutar:" -ForegroundColor Cyan
Write-Host "  Terminal 1: .\servidor.exe" -ForegroundColor White
Write-Host "  Terminales adicionales: .\worker.exe" -ForegroundColor White

pause
