# Script para limpiar archivos compilados - PowerShell

Write-Host "Limpiando archivos compilados..." -ForegroundColor Yellow
Write-Host ""

# Eliminar archivos objeto
$archivosO = Get-ChildItem -Filter "*.o" -ErrorAction SilentlyContinue
if ($archivosO) {
    Remove-Item -Path "*.o" -Force
    Write-Host "Eliminados archivos .o" -ForegroundColor Green
}

# Eliminar ejecutables
$archivosExe = Get-ChildItem -Filter "*.exe" -ErrorAction SilentlyContinue
if ($archivosExe) {
    Remove-Item -Path "*.exe" -Force
    Write-Host "Eliminados archivos .exe" -ForegroundColor Green
}

Write-Host ""
Write-Host "Limpieza completa." -ForegroundColor Green
Write-Host ""
Write-Host "Archivos eliminados:" -ForegroundColor Cyan
Write-Host "  - *.o (archivos objeto)" -ForegroundColor White
Write-Host "  - *.exe (ejecutables)" -ForegroundColor White
Write-Host ""
pause
