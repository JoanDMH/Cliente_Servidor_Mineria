# Joan David Martínez Hernández - 160004716
#  Guía Completa para Linux con Bash


##  Requisitos

- **Sistema Operativo**: Linux (Ubuntu, Debian)
- **Compilador**: GCC
- **Terminal**: Bash


# Verificar instalación de GCC
gcc --version


## Archivos del Proyecto

```
Programa1/Linux/
├── Archivo.txt            # Texto base (ya existe)
├── utilidades_hash.h      # Header de funciones
├── utilidades_hash.c      # Implementación hash
├── servidor_linux.c       # Servidor Linux
├── worker_linux.c         # Worker Linux
├── compilar.sh            # Script compilación
├── ejecutar_servidor.sh   # Script servidor
├── ejecutar_worker.sh     # Script worker
├── limpiar.sh             # Script limpieza
└── GUIA_LINUX.md          # Esta guía
```


##  Compilación

### Opción 1: (Recomendado)

```
# Navegar al directorio Linux
cd ~/Programa1/Linux

# Dar permisos de ejecución (solo primera vez)
chmod +x compilar.sh

# Dar permisos de escritura al directorio actual
chmod u+w .

# Compilar
./compilar.sh
```

### Opción 2: Compilación Manual

Abrir terminal en el directorio del proyecto:

```bash
# 1. Compilar utilidades hash
gcc -c utilidades_hash.c -o utilidades_hash.o

# 2. Compilar servidor
gcc -c servidor_linux.c -o servidor_linux.o -pthread

# 3. Enlazar servidor con pthread
gcc servidor_linux.o utilidades_hash.o -o servidor -lpthread

# 4. Compilar worker
gcc -c worker_linux.c -o worker_linux.o -pthread

# 5. Enlazar worker con pthread
gcc worker_linux.o utilidades_hash.o -o worker -lpthread

# 6. Dar permisos de ejecución
chmod +x servidor worker
```

### Verificar Compilación


```
servidor
worker
```

Verificar permisos:

```
ls -lh servidor worker

# -rwxr-xr-x  1 user user  servidor
# -rwxr-xr-x  1 user user  worker
```



##  Ejecución

### Configuración del Sistema

El simulador necesita:
- **1 servidor** (coordinador)
- **N workers** (mineros) - tú decides cuántos

### Paso 1: Abrir Terminal para el Servidor

```bash
# Navegar al directorio Linux
cd ~/Programa1/Linux

# Ejecutar servidor
./servidor
```

O usar el script:

```bash
chmod +x ejecutar_servidor.sh
./ejecutar_servidor.sh
```

### Paso 2: Configurar Parámetros

El servidor pregunta:

```
Ingrese el texto de relleno (caracteres alfanumericos): AAA
Ingrese la cantidad de ceros consecutivos al final del hash: 2
Ingrese el numero de workers (1-10): 3
```

**Recomendaciones para prueba rápida:**
- Texto: `AA` (2 caracteres)
- Ceros: `1`
- Workers: `2`

### Paso 3: Abrir Terminales para cada Worker

**Importante**: Es necesario abrir tantas terminales como workers especificados


#### En cada terminal de worker:

```bash
# Navegar al directorio Linux
cd ~/Programa1/Linux

# Ejecutar worker
./worker
```

O usar el script:

```bash
chmod +x ejecutar_worker.sh
./ejecutar_worker.sh
```



##  Ejemplo Completo - Paso a Paso

### Escenario: 2 workers

#### Terminal 1 - Servidor
```bash
user@linux:~/Programa1/Linux$ ./servidor

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
```bash
user@linux:~/Programa1/Linux$ ./worker

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
```bash
user@linux:~/Programa1/Linux$ ./worker

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



##  Configuraciones Recomendadas

### Prueba Ultra Rápida (< 10 segundos)
```
Texto: A
Ceros: 1
Workers: 2
Combinaciones: 62
```



##  Conectar a Servidor Remoto

Si el servidor está en otra máquina:

### En la máquina del Servidor:

```bash
# Obtener la IP
ip addr show


### En la máquina del Worker:
cd ~/Programa1/Linux

# Dar permisos de ejecución
sudo chmod +x compilar.sh

#dar permisos de escritura al directorio
sudo chmod u+w . 

# Conectar especificando la IP del servidor
./worker 192.168.1.100

### Configurar Firewall

#### UFW (Ubuntu/Debian)

```bash
# Permitir puerto 8888
sudo ufw allow 8888/tcp

# Verificar
sudo ufw status
```

#### firewalld (Fedora/RHEL)

```bash
# Permitir puerto 8888
sudo firewall-cmd --permanent --add-port=8888/tcp
sudo firewall-cmd --reload

# Verificar
sudo firewall-cmd --list-ports
```

#### iptables

```bash
# Permitir puerto 8888
sudo iptables -A INPUT -p tcp --dport 8888 -j ACCEPT

# Guardar reglas
sudo netfilter-persistent save
```



##  Limpiar Archivos Compilados

```bash
# Manual
rm -f *.o servidor worker

# O usar script
chmod +x limpiar.sh
./limpiar.sh
```

