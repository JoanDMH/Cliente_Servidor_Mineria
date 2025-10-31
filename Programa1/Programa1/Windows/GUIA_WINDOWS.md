# Joan David Martínez Hernández - 160004716
#  Guía Completa para Windows con PowerShell


##  Requisitos

- **Sistema Operativo**: Windows 7 o superior
- **Compilador**: GCC (MinGW-w64)
- **Terminal**: PowerShell


##  Verificar Instalación de GCC en Windows
Abrir PowerShell nueva y ejecutar:

gcc --version


##  Archivos del Proyecto

Programa1/Windows/
├── Archivo.txt           # Texto base (ya existe)

├── utilidades_hash.h     # Header de funciones

├── utilidades_hash.c     # Implementación hash
├── servidor.c            # Servidor Windows
├── worker.c              # Worker Windows
├── compilar.ps1          # Script compilación
├── ejecutar_servidor.ps1 # Script servidor
├── ejecutar_worker.ps1   # Script worker
├── limpiar.ps1           # Script limpieza
└── GUIA_WINDOWS.md       # Esta guía


##  Compilación

### Opción 1: Usando el Script PowerShell (Recomendado)

```powershell
# Navegar al directorio Windows
cd C:\Users\Admin\OneDrive\Escritorio\Programa1\Windows

# Ejecutar script de compilación
.\compilar.ps1
```


### Opción 2: Compilación Manual

Abrir **PowerShell** en el directorio del proyecto:

```powershell
# Navegar al directorio Windows
cd C:\Users\Admin\OneDrive\Escritorio\Programa1\Windows

```
# 1. Compilar utilidades hash
gcc -c utilidades_hash.c -o utilidades_hash.o

# 2. Compilar servidor
gcc -c servidor.c -o servidor.o

# 3. Enlazar servidor con librería de sockets
gcc servidor.o utilidades_hash.o -o servidor.exe -lws2_32

# 4. Compilar worker
gcc -c worker.c -o worker.o

# 5. Enlazar worker con librería de sockets
gcc worker.o utilidades_hash.o -o worker.exe -lws2_32
```

### Verificar Compilación

Deberías tener estos archivos:

```
servidor.exe
worker.exe
```


##  Ejecución

### Configuración del Sistema

El simulador necesita:
- **1 servidor** (coordinador)
- **N workers** (mineros) - tú decides cuántos

### Paso 1: Abrir PowerShell para el Servidor

```powershell
# Navegar al directorio Windows
cd C:\Users\Admin\OneDrive\Escritorio\Programa1\Windows

# Ejecutar servidor
.\servidor.exe
```

O usar el script:

```powershell
.\ejecutar_servidor.ps1
```

### Paso 2: Configurar Parámetros

El servidor preguntará:

```
Ingrese el texto de relleno (caracteres alfanumericos): AAA
Ingrese la cantidad de ceros consecutivos al final del hash: 2
Ingrese el numero de workers (1-10): 3
```

### Paso 3: Abrir PowerShell para cada Worker

**Importante**: abrir tantas terminales como workers especificados

#### En cada terminal de worker:

```powershell
# Navegar al directorio Windows
cd C:\Users\Admin\OneDrive\Escritorio\Programa1\Windows

# Ejecutar worker
.\worker.exe
```

O usar el script:


.\ejecutar_worker.ps1


##  Ejemplo Completo - Paso a Paso

### Escenario: 2 workers

#### Terminal 1 - Servidor
```powershell
PS C:\Users\Admin\OneDrive\Escritorio\Programa1\Windows> .\servidor.exe

=== SIMULADOR DE BLOCKCHAIN - SERVIDOR ===

Texto original del archivo: Universidad de los Llanos, sede Barcelona

Ingrese el texto de relleno: AA
Ingrese la cantidad de ceros: 1
Ingrese el numero de workers: 2

=== CONFIGURACION ===
Texto de relleno: AA (longitud: 2)
Condicion: Hash terminado en 1 ceros
Numero de workers: 2

Total de combinaciones: 3844
Combinaciones por worker: 1922

Worker 1: rango [0 - 1921]
Worker 2: rango [1922 - 3843]

Socket creado.
Bind realizado.
Esperando conexiones en el puerto 8888...
```

#### Terminal 2 - Worker 1
```powershell
PS C:\Users\Admin\OneDrive\Escritorio\Programa1\Windows> .\worker.exe

=== WORKER - CLIENTE DE MINERIA ===
Conectando al servidor 127.0.0.1:8888...
Conectado al servidor!

=== CONFIGURACION RECIBIDA ===
Texto base: Universidad de los Llanos, sede Barcelona
Relleno: AA
Ceros requeridos: 1
Rango asignado: [0 - 1921]
Total de combinaciones a probar: 1922

Iniciando busqueda...

Progreso: 52.08% - Intentos: 10000 - Ultimo hash: 3a5f2e12
```

#### Terminal 3 - Worker 2
```powershell
PS C:\Users\Admin\OneDrive\Escritorio\Programa1> .\worker.exe

=== WORKER - CLIENTE DE MINERIA ===
Conectando al servidor 127.0.0.1:8888...
Conectado al servidor!

=== CONFIGURACION RECIBIDA ===
Texto base: Universidad de los Llanos, sede Barcelona
Relleno: AA
Ceros requeridos: 1
Rango asignado: [1922 - 3843]
Total de combinaciones a probar: 1922

Iniciando busqueda...
```

### Resultado Esperado

Cuando un worker encuentre la solución:

```
*** SOLUCION ENCONTRADA! ***
Nonce: 2456
Combinacion: Az
Texto completo: Universidad de los Llanos, sede BarcelonaAz
Hash: 3a5f2e0

[SERVIDOR] Orden de detencion recibida (otro worker encontro la solucion)
```

En el servidor:

```
*** Worker 2 encontro la solucion! ***
Hash: 3a5f2e0
Nonce: 2456

=== RESULTADO FINAL ===
SOLUCION ENCONTRADA!
Hash: 3a5f2e0
Nonce: 2456
```





## Conectar a Servidor Remoto

Si el servidor está en otra máquina:

### En la máquina del Servidor:

```powershell
# Obtener la IP
ipconfig

# Buscar "Dirección IPv4", ejemplo: 192.168.1.100
```

### En la máquina del Worker:

```powershell
# Conectar especificando la IP del servidor
.\worker.exe 192.168.1.100
```

### Configurar Firewall

```powershell
# Ejecutar como Administrador
New-NetFirewallRule -DisplayName "Blockchain Server" -Direction Inbound -LocalPort 8888 -Protocol TCP -Action Allow
```


### Ejecutar Múltiples Workers Automáticamente

Crear script `iniciar_workers.ps1`:

```powershell
# iniciar_workers.ps1
param(
    [int]$cantidad = 2
)

for ($i = 1; $i -le $cantidad; $i++) {
    Start-Process powershell -ArgumentList "-NoExit", "-Command", "cd '$PWD'; .\worker.exe"
    Start-Sleep -Seconds 1
}
```

Usar:

.\iniciar_workers.ps1 -cantidad 4




