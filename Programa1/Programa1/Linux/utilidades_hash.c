#include "utilidades_hash.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

#define PRIMO1 1000000007ULL
#define PRIMO2 1000000009ULL
#define PRIMO3 999999937ULL

// Alfabeto alfanumérico: 0-9, A-Z, a-z
static const char ALFABETO[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
static const int TAMANIO_ALFABETO = 62;

void calcular_hash_simple(const char* texto, char* hash_hex) {
    uint64_t valor_hash = 0;
    int longitud = strlen(texto);
    
    // Procesar cada carácter
    for (int i = 0; i < longitud; i++) {
        unsigned char valor_ascii = (unsigned char)texto[i];
        
        // Aplicar operaciones con primos y posición del carácter
        valor_hash = (valor_hash * PRIMO2 + valor_ascii * (i + 1)) % PRIMO1;
        valor_hash = (valor_hash + (uint64_t)valor_ascii * valor_ascii) % PRIMO3;
    }
    
    // Aplicar una última operación para mayor dispersión
    valor_hash = (valor_hash * PRIMO2 + longitud) % PRIMO1;
    
    // Convertir a hexadecimal
    sprintf(hash_hex, "%llx", (unsigned long long)valor_hash);
}

int verificar_condicion_hash(const char* hash_hex, int num_ceros) {
    if (num_ceros <= 0) {
        return 1;
    }
    
    int longitud = strlen(hash_hex);
    if (longitud < num_ceros) {
        return 0;
    }
    
    // Verificar que los últimos num_ceros caracteres sean '0'
    for (int i = 0; i < num_ceros; i++) {
        if (hash_hex[longitud - num_ceros + i] != '0') {
            return 0;
        }
    }
    
    return 1;
}

uint64_t calcular_total_combinaciones(int longitud_relleno) {
    uint64_t total = 1;
    for (int i = 0; i < longitud_relleno; i++) {
        total *= TAMANIO_ALFABETO;
        // Prevenir overflow (retornar máximo si es muy grande)
        if (total > 1000000000000ULL) {
            return 1000000000000ULL;
        }
    }
    return total;
}

void generar_combinacion(uint64_t indice, int longitud, char* resultado) {
    // Generar la combinación en base 62
    for (int i = longitud - 1; i >= 0; i--) {
        resultado[i] = ALFABETO[indice % TAMANIO_ALFABETO];
        indice /= TAMANIO_ALFABETO;
    }
    resultado[longitud] = '\0';
}
