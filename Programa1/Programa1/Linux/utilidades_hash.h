#ifndef UTILIDADES_HASH_H
#define UTILIDADES_HASH_H

#include <stdint.h>

/**
 * Calcula un hash simple del texto dado
 * Usa operaciones con números primos y valores ASCII
 */
void calcular_hash_simple(const char* texto, char* hash_hex);

/**
 * Verifica si el hash termina con la cantidad especificada de ceros
 */
int verificar_condicion_hash(const char* hash_hex, int num_ceros);

/**
 * Calcula el número total de combinaciones posibles
 * Para alfanuméricos: 62^longitud
 */
uint64_t calcular_total_combinaciones(int longitud_relleno);

/**
 * Genera una combinación alfanumérica basada en un índice
 * Alfabeto: 0-9, A-Z, a-z (62 caracteres)
 */
void generar_combinacion(uint64_t indice, int longitud, char* resultado);

#endif // UTILIDADES_HASH_H
