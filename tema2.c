#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>

#define MAX 20  // Tamaño del vector

// Variables globales
int vector[MAX];           // Vector de números aleatorios
int numero_buscado;        // Número que se busca en el vector
int num_hilos;             // Número de hilos para la búsqueda
int posicion_encontrada = -1;  // Posición donde se encuentra el número (-1 si no se encuentra)
bool encontrado = false;   // Bandera para indicar si se encontró el número
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // Mutex para sincronización

/*
Función que realiza la búsqueda en una sección del vector.
Esta función es ejecutada por cada hilo. Busca el número en su sección
asignada del vector y actualiza las variables globales si lo encuentra.
 */
void* buscar(void* arg) {
    int id = *(int*)arg;
    int inicio = id * (MAX / num_hilos);
    int fin = (id == num_hilos - 1) ? MAX : (id + 1) * (MAX / num_hilos);

    for (int i = inicio; i < fin && !encontrado; i++) {
        if (vector[i] == numero_buscado) {
            pthread_mutex_lock(&mutex);
            if (!encontrado) {
                encontrado = true;
                posicion_encontrada = i;
            }
            pthread_mutex_unlock(&mutex);
            break;
        }
    }

    pthread_exit(NULL);
}

/*
Función principal del programa.
Inicializa el vector con números aleatorios, crea los hilos para la búsqueda,
espera a que terminen y muestra el resultado.
 */
int main(int argc, char *argv[]) {
    // Verificar argumentos de línea de comandos
    if (argc != 3) {
        printf("Uso: %s <numero_a_buscar> <numero_de_hilos>\n", argv[0]);
        return 1;
    }

    // Obtener parámetros de los argumentos
    numero_buscado = atoi(argv[1]);
    num_hilos = atoi(argv[2]);

    // Inicializar el generador de números aleatorios y llenar el vector
    srand(time(NULL));
    for (int i = 0; i < MAX; i++) {
        vector[i] = rand() % 10;
        printf("%d ", vector[i]);
    }
    printf("\n");

    // Crear e iniciar los hilos
    pthread_t hilos[num_hilos];
    int ids[num_hilos];
    for (int i = 0; i < num_hilos; i++) {
        ids[i] = i;
        pthread_create(&hilos[i], NULL, buscar, &ids[i]);
    }

    // Esperar a que todos los hilos terminen
    for (int i = 0; i < num_hilos; i++) {
        pthread_join(hilos[i], NULL);
    }

    // Mostrar el resultado de la búsqueda
    if (encontrado) {
        printf("El número %d se encontró en la posición %d\n", numero_buscado, posicion_encontrada);
    } else {
        printf("El número %d no se encontró en el vector\n", numero_buscado);
    }

    return 0;
}