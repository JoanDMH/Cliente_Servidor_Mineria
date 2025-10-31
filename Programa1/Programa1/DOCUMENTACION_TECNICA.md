# Documentación Técnica - Simulador de Blockchain

## Arquitectura del Sistema

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

### Diseño

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

### Números Primos Utilizados

- **PRIMO1**: 1,000,000,007
- **PRIMO2**: 1,000,000,009
- **PRIMO3**: 999,999,937

### Características

1. **Determinista**: Mismo input → mismo output
2. **Dispersión**: Pequeños cambios generan hashes diferentes
3. **Formato**: Hexadecimal (sin prefijo 0x)
4. **Rango**: 0 hasta PRIMO1-1

### Ejemplo

```
Entrada: "Universidad de los Llanos, sede Barcelona00"
Proceso:
  - U (85): hash = (0 * 1000000009 + 85 * 1) % 1000000007 = 85
  - U (85): hash = (85 + 85²) % 999999937 = 7310
  - n (110): hash = (7310 * 1000000009 + 110 * 2) % 1000000007 = ...
  ...
  hash_final = ... % 1000000007
  
Salida: "3a5f2e00" (ejemplo - depende del cálculo completo)
```

## Generación de Combinaciones

### Sistema Alfanumérico Base-62

Alfabeto: `0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz`

Índice → Combinación (similar a conversión decimal a base-62):

```
Índice 0: "000...0"
Índice 1: "000...1"
...
Índice 61: "000...z"
Índice 62: "000...10"
...
```

### Ejemplo con longitud 3

```
Índice      Combinación
0       →   "000"
1       →   "001"
62      →   "010"
3844    →   "100"
238327  →   "zzz"
```

## Protocolo de Comunicación

### Mensajes Servidor → Worker

1. **CONFIG** (al conectar):
   ```
   CONFIG|texto_base|texto_relleno|num_ceros|rango_inicio|rango_fin
   ```
   Ejemplo:
   ```
   CONFIG|Universidad de los Llanos|AAA|2|0|61999
   ```

2. **DETENER** (cuando hay solución):
   ```
   DETENER
   ```

### Mensajes Worker → Servidor

1. **SOLUCION** (hash válido encontrado):
   ```
   SOLUCION|hash_hex|nonce
   ```
   Ejemplo:
   ```
   SOLUCION|3a5f2e00|12345
   ```

2. **NO_ENCONTRADO** (rango completado sin éxito):
   ```
   NO_ENCONTRADO
   ```

3. **PROGRESO** (opcional, cada 10,000 intentos):
   ```
   PROGRESO|porcentaje|intentos
   ```

## Flujo de Ejecución

### Secuencia Completa

```
1. SERVIDOR inicia
   ↓
2. Lee Archivo.txt
   ↓
3. Solicita parámetros al usuario
   ↓
4. Divide espacio de búsqueda en N rangos
   ↓
5. Espera N conexiones de workers
   ↓
6. Por cada worker que se conecta:
   - Envía CONFIG con su rango
   - Crea thread para escuchar mensajes
   ↓
7. WORKERS reciben CONFIG
   ↓
8. WORKERS inician búsqueda en paralelo:
   Para nonce en [inicio, fin]:
     - Generar combinación
     - Calcular hash de (texto_base + combinación)
     - Si hash termina en N ceros:
       ✓ Enviar SOLUCION al servidor
       ✓ Detener búsqueda
     - Si tiempo_limite alcanzado:
       ✓ Detener búsqueda
   ↓
9. SERVIDOR recibe primer SOLUCION
   ↓
10. SERVIDOR envía DETENER a todos los workers
    ↓
11. Workers restantes detienen búsqueda
    ↓
12. SERVIDOR muestra resultado final
```

## Concurrencia y Sincronización

### Threads en Servidor

- **Thread principal**: Acepta conexiones, coordina workers
- **Threads workers**: Un thread por cada worker conectado, escucha mensajes

### Sincronización

```c
CRITICAL_SECTION seccion_critica;

// Proteger variable compartida
EnterCriticalSection(&seccion_critica);
if (!solucion_encontrada) {
    solucion_encontrada = 1;
    // ... procesar solución
}
LeaveCriticalSection(&seccion_critica);
```

### Variables Compartidas Protegidas

- `solucion_encontrada`: Flag global
- `hash_solucion`: Hash ganador
- `nonce_solucion`: Nonce ganador

## Espacio de Búsqueda

### Cálculo de Combinaciones

Total = 62^longitud

| Longitud | Combinaciones | Tamaño Aproximado      |
|----------|---------------|------------------------|
| 1        | 62            | ~62 intentos           |
| 2        | 3,844         | ~4 mil intentos        |
| 3        | 238,328       | ~238 mil intentos      |
| 4        | 14,776,336    | ~15 millones           |
| 5        | 916,132,832   | ~916 millones          |

### División de Trabajo

Para N workers:
```
tamaño_rango = total_combinaciones / N
worker[i].inicio = i * tamaño_rango
worker[i].fin = (i+1) * tamaño_rango - 1

// Último worker toma el resto
worker[N-1].fin = total_combinaciones - 1
```

## Optimizaciones Implementadas

1. **Distribución equitativa**: Rangos balanceados entre workers
2. **Detección temprana**: Stop inmediato al encontrar solución
3. **Tiempo límite**: Previene búsquedas infinitas (300s)
4. **Reporte de progreso**: Cada 10,000 intentos
5. **Hash eficiente**: Operaciones modulares en vez de criptográficas

## Limitaciones Conocidas

1. **No persistence**: Si se cae el servidor, se pierde todo
2. **Sin recuperación**: Workers no pueden reconectarse
3. **Memoria**: Límite de 10 workers simultáneos (configurable)
4. **Puerto fijo**: 8888 (hardcoded)
5. **Sin autenticación**: Cualquiera puede conectarse

## Posibles Mejoras Futuras

1. Guardar estado de búsqueda (checkpoints)
2. Balanceo dinámico de carga
3. Reconexión automática de workers
4. Configuración de puerto por parámetro
5. Sistema de autenticación
6. Métricas y estadísticas detalladas
7. Interfaz gráfica
8. Soporte para múltiples algoritmos de hash

## Consideraciones de Rendimiento

### Factores que Afectan el Rendimiento

1. **Número de workers**: Más workers = búsqueda más rápida (hasta cierto límite)
2. **Dificultad (ceros)**: Más ceros = exponencialmente más difícil
3. **Longitud del relleno**: Mayor longitud = más combinaciones
4. **CPU**: Operaciones modulares intensivas en CPU
5. **Red**: Latencia mínima (localhost), puede afectar en red

### Benchmarks Estimados (CPU moderna, localhost)

| Config                  | Tiempo Estimado |
|-------------------------|-----------------|
| 2 chars, 1 cero, 2 wrk  | < 1 segundo     |
| 3 chars, 2 ceros, 4 wrk | 1-30 segundos   |
| 4 chars, 3 ceros, 8 wrk | 1-10 minutos    |
| 5 chars, 4 ceros, 10wrk | 10+ minutos     |

**Nota**: Tiempos son aproximados y dependen del hardware.
