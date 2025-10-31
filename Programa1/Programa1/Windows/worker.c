#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <time.h>
#include "utilidades_hash.h"

#pragma comment(lib, "ws2_32.lib")

#define PUERTO 8888
#define BUFFER_SIZE 1024
#define MAX_TEXTO 256
#define TIEMPO_LIMITE_SEGUNDOS 300  // 5 minutos máximo por worker

// Variables globales
int debe_detener = 0;
CRITICAL_SECTION seccion_critica;

// Prototipos
void inicializar_winsock();
DWORD WINAPI escuchar_servidor(LPVOID param);
void buscar_solucion(SOCKET sock, const char* texto_base, const char* texto_relleno, int num_ceros, uint64_t rango_inicio, uint64_t rango_fin);

int main(int argc, char* argv[]) {
    SOCKET sock;
    struct sockaddr_in servidor;
    char buffer[BUFFER_SIZE];
    char servidor_ip[50] = "127.0.0.1";
    int bytes_recibidos;
    
    // Permitir especificar IP del servidor como argumento
    if (argc > 1) {
        strcpy(servidor_ip, argv[1]);
    }
    
    printf("=== WORKER - CLIENTE DE MINERIA ===\n");
    printf("Conectando al servidor %s:%d...\n", servidor_ip, PUERTO);
    
    InitializeCriticalSection(&seccion_critica);
    inicializar_winsock();
    
    // Crear socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Error al crear socket: %d\n", WSAGetLastError());
        return 1;
    }
    
    // Configurar dirección del servidor
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(PUERTO);
    servidor.sin_addr.s_addr = inet_addr(servidor_ip);
    
    // Conectar al servidor
    if (connect(sock, (struct sockaddr*)&servidor, sizeof(servidor)) < 0) {
        printf("Error al conectar con el servidor: %d\n", WSAGetLastError());
        printf("Asegurese de que el servidor este ejecutandose.\n");
        return 1;
    }
    
    printf("Conectado al servidor!\n");
    
    // Recibir configuración del servidor
    bytes_recibidos = recv(sock, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_recibidos <= 0) {
        printf("Error al recibir configuracion\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    
    buffer[bytes_recibidos] = '\0';
    
    // Parsear configuración: CONFIG|texto_original|texto_relleno|num_ceros|rango_inicio|rango_fin
    char texto_original[MAX_TEXTO];
    char texto_relleno[MAX_TEXTO];
    int num_ceros;
    uint64_t rango_inicio, rango_fin;
    
    char* token = strtok(buffer, "|");
    if (strcmp(token, "CONFIG") != 0) {
        printf("Error: Mensaje de configuracion invalido\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    
    token = strtok(NULL, "|");
    strcpy(texto_original, token);
    
    token = strtok(NULL, "|");
    strcpy(texto_relleno, token);
    
    token = strtok(NULL, "|");
    num_ceros = atoi(token);
    
    token = strtok(NULL, "|");
    rango_inicio = strtoull(token, NULL, 10);
    
    token = strtok(NULL, "|");
    rango_fin = strtoull(token, NULL, 10);
    
    printf("\n=== CONFIGURACION RECIBIDA ===\n");
    printf("Texto base: %s\n", texto_original);
    printf("Relleno: %s\n", texto_relleno);
    printf("Ceros requeridos: %d\n", num_ceros);
    printf("Rango asignado: [%llu - %llu]\n", rango_inicio, rango_fin);
    printf("Total de combinaciones a probar: %llu\n\n", rango_fin - rango_inicio + 1);
    
    // Crear thread para escuchar mensajes del servidor
    HANDLE hThread = CreateThread(NULL, 0, escuchar_servidor, (LPVOID)sock, 0, NULL);
    
    // Iniciar búsqueda
    printf("Iniciando busqueda...\n\n");
    buscar_solucion(sock, texto_original, texto_relleno, num_ceros, rango_inicio, rango_fin);
    
    // Esperar a que el thread termine
    if (hThread) {
        WaitForSingleObject(hThread, 2000);
        CloseHandle(hThread);
    }
    
    closesocket(sock);
    WSACleanup();
    DeleteCriticalSection(&seccion_critica);
    
    printf("\nWorker finalizado.\n");
    return 0;
}

void inicializar_winsock() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Error al inicializar Winsock: %d\n", WSAGetLastError());
        exit(1);
    }
}

DWORD WINAPI escuchar_servidor(LPVOID param) {
    SOCKET sock = (SOCKET)param;
    char buffer[BUFFER_SIZE];
    int bytes_recibidos;
    
    while ((bytes_recibidos = recv(sock, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_recibidos] = '\0';
        
        if (strcmp(buffer, "DETENER") == 0) {
            EnterCriticalSection(&seccion_critica);
            debe_detener = 1;
            LeaveCriticalSection(&seccion_critica);
            printf("\n[SERVIDOR] Orden de detencion recibida (otro worker encontro la solucion)\n");
            break;
        }
    }
    
    return 0;
}

void buscar_solucion(SOCKET sock, const char* texto_base, const char* texto_relleno, 
                     int num_ceros, uint64_t rango_inicio, uint64_t rango_fin) {
    char texto_completo[BUFFER_SIZE];
    char combinacion[MAX_TEXTO];
    char hash_resultado[BUFFER_SIZE];
    char mensaje[BUFFER_SIZE];
    int longitud_relleno = strlen(texto_relleno);
    uint64_t intentos = 0;
    uint64_t ultimo_progreso = 0;
    time_t tiempo_inicio = time(NULL);
    time_t tiempo_actual;
    int encontrado = 0;
    
    for (uint64_t nonce = rango_inicio; nonce <= rango_fin; nonce++) {
        // Verificar si debe detener
        EnterCriticalSection(&seccion_critica);
        int detener = debe_detener;
        LeaveCriticalSection(&seccion_critica);
        
        if (detener) {
            printf("Deteniendo busqueda...\n");
            break;
        }
        
        // Verificar tiempo límite
        tiempo_actual = time(NULL);
        if (difftime(tiempo_actual, tiempo_inicio) > TIEMPO_LIMITE_SEGUNDOS) {
            printf("\nTiempo limite alcanzado (%d segundos)\n", TIEMPO_LIMITE_SEGUNDOS);
            break;
        }
        
        // Generar combinación actual
        generar_combinacion(nonce, longitud_relleno, combinacion);
        
        // Construir texto completo: texto_base + combinacion
        sprintf(texto_completo, "%s%s", texto_base, combinacion);
        
        // Calcular hash
        calcular_hash_simple(texto_completo, hash_resultado);
        
        // Verificar condición
        if (verificar_condicion_hash(hash_resultado, num_ceros)) {
            printf("\n*** SOLUCION ENCONTRADA! ***\n");
            printf("Nonce: %llu\n", nonce);
            printf("Combinacion: %s\n", combinacion);
            printf("Texto completo: %s\n", texto_completo);
            printf("Hash: %s\n", hash_resultado);
            
            // Notificar al servidor
            sprintf(mensaje, "SOLUCION|%s|%llu", hash_resultado, nonce);
            send(sock, mensaje, strlen(mensaje), 0);
            encontrado = 1;
            break;
        }
        
        intentos++;
        
        // Reportar progreso cada 10000 intentos
        if (intentos - ultimo_progreso >= 10000) {
            double porcentaje = ((double)(nonce - rango_inicio) / (rango_fin - rango_inicio + 1)) * 100.0;
            printf("Progreso: %.2f%% - Intentos: %llu - Ultimo hash: %s\n", 
                   porcentaje, intentos, hash_resultado);
            ultimo_progreso = intentos;
        }
    }
    
    if (!encontrado && !debe_detener) {
        printf("\nBusqueda completada. No se encontro solucion en el rango asignado.\n");
        printf("Total de combinaciones probadas: %llu\n", intentos);
        sprintf(mensaje, "NO_ENCONTRADO");
        send(sock, mensaje, strlen(mensaje), 0);
    }
    
    tiempo_actual = time(NULL);
    printf("Tiempo de ejecucion: %.0f segundos\n", difftime(tiempo_actual, tiempo_inicio));
}
