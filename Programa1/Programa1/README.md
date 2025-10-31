# Joan David Martínez Hernández - 160004716

Sistema distribuido que simula el proceso de minería en una blockchain utilizando arquitectura cliente-servidor.


### Si usa Windows:
    Leer: GUIA_WINDOWS.md

### Si usa Linux:
    Leer: GUIA_LINUX.md


## Descripcion General

El sistema consta de:
- **Líder (Servidor)**: Coordina el trabajo y divide el espacio de búsqueda
- **Workers (Clientes)**: Realizan la búsqueda de soluciones en paralelo

### Componentes Principales

```
┌─────────────────────────────────────────────────────────┐
│                       SERVIDOR                          │
│  - Coordina workers                                     │
│  - Divide espacio de búsqueda                           │
│  - Recibe resultados                                    │
│  - Notifica detención                                   │
└───────────┬─────────────────────────────┬───────────────┘
            │                             │
            │      Socket TCP/IP          │
            │      Puerto 8888            │
            │                             │
    ┌───────▼─────────┐         ┌────────▼────────┐
    │    WORKER 1     │   ...   │    WORKER N     │
    │  Rango: [0-X]   │         │  Rango: [Y-Z]   │
    │  Busca hash     │         │  Busca hash     │
    └─────────────────┘         └─────────────────┘
```

## Algoritmo de Hash Simple

# Diseño

El hash NO utiliza SHA-256 ni MD5. Implementa un algoritmo propio basado en:

```c
hash = 0
Para cada carácter c en posición i:
    ascii = valor_ASCII(c)
    hash = (hash * PRIMO2 + ascii * (i+1)) % PRIMO1
    hash = (hash + ascii²) % PRIMO3

hash_final = (hash * PRIMO2 + longitud_texto) % PRIMO1
resultado_hex = convertir_a_hexadecimal(hash_final)
```

# Números Primos Utilizados

- PRIMO1: 1,000,000,007
- PRIMO2: 1,000,000,009
- PRIMO3: 999,999,937


## Archivos del Proyecto

# Código Fuente Compartido
```
Archivo.txt           # Texto base
utilidades_hash.h     # Header funciones hash
utilidades_hash.c     # Implementación hash
```

# Windows
```
servidor.c            # Servidor (Winsock2)
worker.c              # Worker (Winsock2)
compilar.ps1          # Script compilación PowerShell
ejecutar_servidor.ps1 # Script servidor
ejecutar_worker.ps1   # Script worker
limpiar.ps1           # Script limpieza
GUIA_WINDOWS.md       # Guia completa Windows
```

# Linux
```
servidor_linux.c      # Servidor (POSIX)
worker_linux.c        # Worker (POSIX)
compilar.sh           # Script compilación Bash
ejecutar_servidor.sh  # Script servidor
ejecutar_worker.sh    # Script worker
limpiar.sh            # Script limpieza
GUIA_LINUX.md         # Guia completa Linux
```

# Caracteristicas Principales

- Algoritmo de hash simple
- Comunicacion por sockets TCP/IP
- Concurrencia multi-threading
- Division automatica del trabajo
- Detencion coordinada al encontrar solucion
- Limite de tiempo por worker (300s)


## Configuraciones Sugeridas

| Configuración| Texto | Ceros | Workers | Tiempo Estimado |
|--------------|-------|-------|---------|-----------------|
| Ultra Rápida | A     | 1     | 2       | < 10 segundos   |
| Rápida       | AA    | 1     | 2       | < 1 minuto      |
| Media        | AAA   | 2     | 4       | 1-5 minutos     |
| Intensiva    | AAAA  | 2     | 8       | 5-30 minutos    |



