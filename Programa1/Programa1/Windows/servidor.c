#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include "utilidades_hash.h"

#pragma comment(lib, "ws2_32.lib")

#define PUERTO 8888
#define MAX_WORKERS 10
#define BUFFER_SIZE 1024
#define MAX_TEXTO 256

typedef struct {
    SOCKET socket;
    uint64_t rango_inicio;
    uint64_t rango_fin;
    int id;
    int activo;
} InfoWorker;

// Variables globales
InfoWorker workers[MAX_WORKERS];
int num_workers_total = 0;
int solucion_encontrada = 0;
char hash_solucion[BUFFER_SIZE];
uint64_t nonce_solucion = 0;
CRITICAL_SECTION seccion_critica;

// Prototipos
void inicializar_winsock();
void leer_archivo_texto(char* contenido);
void configurar_parametros(char* texto_relleno, int* num_ceros, int* num_workers);
void dividir_trabajo(int longitud_relleno, int num_workers);
DWORD WINAPI manejar_worker(LPVOID param);
void notificar_todos_workers(const char* mensaje);

int main() {
    WSADATA wsa;
    SOCKET socket_servidor, socket_worker;
    struct sockaddr_in servidor, cliente;
    int tamanio_cliente;
    char texto_original[MAX_TEXTO];
    char texto_relleno[MAX_TEXTO];
    int num_ceros, num_workers;
    
    printf("=== SIMULADOR DE BLOCKCHAIN - SERVIDOR ===\n\n");
    
    // Inicializar sección crítica
    InitializeCriticalSection(&seccion_critica);
    
    // Leer archivo de texto
    leer_archivo_texto(texto_original);
    printf("Texto original del archivo: %s\n\n", texto_original);
    
    // Solicitar parámetros al usuario
    configurar_parametros(texto_relleno, &num_ceros, &num_workers);
    num_workers_total = num_workers;
    
    printf("\n=== CONFIGURACION ===\n");
    printf("Texto de relleno: %s (longitud: %d)\n", texto_relleno, (int)strlen(texto_relleno));
    printf("Condicion: Hash terminado en %d ceros\n", num_ceros);
    printf("Numero de workers: %d\n\n", num_workers);
    
    // Dividir el trabajo
    dividir_trabajo(strlen(texto_relleno), num_workers);
    
    // Inicializar Winsock
    inicializar_winsock();
    
    // Crear socket
    if ((socket_servidor = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Error al crear socket: %d\n", WSAGetLastError());
        return 1;
    }
    
    printf("Socket creado.\n");
    
    // Configurar dirección del servidor
    servidor.sin_family = AF_INET;
    servidor.sin_addr.s_addr = INADDR_ANY;
    servidor.sin_port = htons(PUERTO);
    
    // Bind
    if (bind(socket_servidor, (struct sockaddr*)&servidor, sizeof(servidor)) == SOCKET_ERROR) {
        printf("Error en bind: %d\n", WSAGetLastError());
        return 1;
    }
    
    printf("Bind realizado.\n");
    
    // Listen
    listen(socket_servidor, MAX_WORKERS);
    printf("Esperando conexiones en el puerto %d...\n\n", PUERTO);
    
    tamanio_cliente = sizeof(struct sockaddr_in);
    
    // Aceptar conexiones de workers
    for (int i = 0; i < num_workers && !solucion_encontrada; i++) {
        socket_worker = accept(socket_servidor, (struct sockaddr*)&cliente, &tamanio_cliente);
        
        if (socket_worker == INVALID_SOCKET) {
            printf("Error al aceptar conexion\n");
            continue;
        }
        
        printf("Worker %d conectado desde %s:%d\n",  i + 1,  inet_ntoa(cliente.sin_addr),  ntohs(cliente.sin_port));
        
        workers[i].socket = socket_worker;
        workers[i].id = i;
        workers[i].activo = 1;
        
        // Enviar configuración al worker
        char mensaje[BUFFER_SIZE];
        sprintf(mensaje, "CONFIG|%s|%s|%d|%llu|%llu", 
                texto_original, 
                texto_relleno, 
                num_ceros,
                workers[i].rango_inicio,
                workers[i].rango_fin);
        send(socket_worker, mensaje, strlen(mensaje), 0);
        
        // Crear thread para manejar el worker
        HANDLE hThread = CreateThread(NULL, 0, manejar_worker, &workers[i], 0, NULL);
        if (hThread) {
            CloseHandle(hThread);
        }
    }
    
    // Esperar a que todos los workers terminen
    printf("\nTodos los workers conectados. Iniciando busqueda...\n\n");
    
    while (1) {
        Sleep(1000);
        
        int workers_activos = 0;
        for (int i = 0; i < num_workers; i++) {
            if (workers[i].activo) {
                workers_activos++;
            }
        }
        
        if (workers_activos == 0 || solucion_encontrada) {
            break;
        }
    }
    
    // Resultados finales
    printf("\n=== RESULTADO FINAL ===\n");
    if (solucion_encontrada) {
        printf("SOLUCION ENCONTRADA!\n");
        printf("Hash: %s\n", hash_solucion);
        printf("Nonce: %llu\n", nonce_solucion);
    } else {
        printf("No se encontro solucion en el espacio de busqueda.\n");
    }
    
    // Cerrar sockets
    for (int i = 0; i < num_workers; i++) {
        closesocket(workers[i].socket);
    }
    closesocket(socket_servidor);
    WSACleanup();
    DeleteCriticalSection(&seccion_critica);
    
    return 0;
}

void inicializar_winsock() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Error al inicializar Winsock: %d\n", WSAGetLastError());
        exit(1);
    }
}

void leer_archivo_texto(char* contenido) {
    FILE* archivo = fopen("Archivo.txt", "r");
    if (archivo == NULL) {
        printf("Error: No se pudo abrir Archivo.txt\n");
        strcpy(contenido, "Texto por defecto");
        return;
    }
    
    if (fgets(contenido, MAX_TEXTO, archivo) == NULL) {
        strcpy(contenido, "Texto por defecto");
    } else {
        // Remover salto de línea si existe
        size_t len = strlen(contenido);
        if (len > 0 && contenido[len-1] == '\n') {
            contenido[len-1] = '\0';
        }
    }
    
    fclose(archivo);
}

void configurar_parametros(char* texto_relleno, int* num_ceros, int* num_workers) {
    printf("Ingrese el texto de relleno (caracteres alfanumericos): ");
    scanf("%s", texto_relleno);
    
    printf("Ingrese la cantidad de ceros consecutivos al final del hash: ");
    scanf("%d", num_ceros);
    
    printf("Ingrese el numero de workers (1-%d): ", MAX_WORKERS);
    scanf("%d", num_workers);
    
    if (*num_workers < 1) *num_workers = 1;
    if (*num_workers > MAX_WORKERS) *num_workers = MAX_WORKERS;
}

void dividir_trabajo(int longitud_relleno, int num_workers) {
    uint64_t total_combinaciones = calcular_total_combinaciones(longitud_relleno);
    uint64_t combinaciones_por_worker = total_combinaciones / num_workers;
    
    printf("Total de combinaciones: %llu\n", total_combinaciones);
    printf("Combinaciones por worker: %llu\n\n", combinaciones_por_worker);
    
    for (int i = 0; i < num_workers; i++) {
        workers[i].rango_inicio = i * combinaciones_por_worker;
        workers[i].rango_fin = (i == num_workers - 1) ? 
                                total_combinaciones - 1 : 
                                (i + 1) * combinaciones_por_worker - 1;
        
        printf("Worker %d: rango [%llu - %llu]\n", 
               i + 1, 
               workers[i].rango_inicio, 
               workers[i].rango_fin);
    }
}

DWORD WINAPI manejar_worker(LPVOID param) {
    InfoWorker* worker = (InfoWorker*)param;
    char buffer[BUFFER_SIZE];
    int bytes_recibidos;
    
    while ((bytes_recibidos = recv(worker->socket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_recibidos] = '\0';
        
        if (strncmp(buffer, "SOLUCION", 8) == 0) {
            // Formato: SOLUCION|hash|nonce
            char* token = strtok(buffer, "|");
            token = strtok(NULL, "|");
            strcpy(hash_solucion, token);
            token = strtok(NULL, "|");
            nonce_solucion = strtoull(token, NULL, 10);
            
            EnterCriticalSection(&seccion_critica);
            if (!solucion_encontrada) {
                solucion_encontrada = 1;
                printf("\n*** Worker %d encontro la solucion! ***\n", worker->id + 1);
                printf("Hash: %s\n", hash_solucion);
                printf("Nonce: %llu\n", nonce_solucion);
                
                // Notificar a todos los workers que paren
                notificar_todos_workers("DETENER");
            }
            LeaveCriticalSection(&seccion_critica);
            break;
        }
        else if (strncmp(buffer, "NO_ENCONTRADO", 13) == 0) {
            printf("Worker %d termino sin encontrar solucion en su rango.\n", worker->id + 1);
            break;
        }
        else if (strncmp(buffer, "PROGRESO", 8) == 0) {
            // Mostrar progreso
            printf("Worker %d: %s\n", worker->id + 1, buffer);
        }
    }
    
    worker->activo = 0;
    return 0;
}

void notificar_todos_workers(const char* mensaje) {
    for (int i = 0; i < num_workers_total; i++) {
        if (workers[i].activo && workers[i].socket != INVALID_SOCKET) {
            send(workers[i].socket, mensaje, strlen(mensaje), 0);
        }
    }
}
