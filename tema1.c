#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_HILOS 100

// Estructura para pasar datos a los hilos
typedef struct {
    int numero;
    int orden;
    unsigned long long resultado;
} DatosHilo;

// Variables globales
DatosHilo datos_hilo[MAX_HILOS];
pthread_t hilos[MAX_HILOS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int impresion_actual = 0;
int num_hilos = 0;

// Función para calcular el factorial de un número
unsigned long long factorial(int n) {
    if (n == 0 || n == 1) return 1;
    return n * factorial(n - 1);
}

// Función que ejecutará cada hilo
void *calcular_factorial(void *arg) {
    DatosHilo *datos = (DatosHilo *)arg;
    
    // Calcula el factorial
    datos->resultado = factorial(datos->numero);

    // Sección crítica para imprimir el resultado en orden
    pthread_mutex_lock(&mutex);
    while (datos->orden != impresion_actual) {
        // Espera si no es el turno de este hilo para imprimir
        pthread_cond_wait(&cond, &mutex);
    }
    // Imprime el resultado
    printf("Factorial de %d: %llu\n", datos->numero, datos->resultado);
    // Actualiza el contador de impresión
    impresion_actual++;
    // Notifica a todos los hilos que el estado ha cambiado
    pthread_cond_broadcast(&cond);
    
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(int argc, char *argv[]) {
    // Verifica si se proporcionaron argumentos
    if (argc < 2) {
        printf("Uso: %s <numero1> <numero2> ... <numeroN>\n", argv[0]);
        return 1;
    }

    // Calcula el número de hilos basado en los argumentos
    num_hilos = argc - 1;
    if (num_hilos > MAX_HILOS) {
        printf("Error: Demasiados argumentos. Máximo %d.\n", MAX_HILOS);
        return 1;
    }

    // Crea un hilo para cada número proporcionado
    for (int i = 0; i < num_hilos; i++) {
        datos_hilo[i].numero = atoi(argv[i + 1]);
        datos_hilo[i].orden = i;
        if (pthread_create(&hilos[i], NULL, calcular_factorial, &datos_hilo[i]) != 0) {
            perror("Error al crear el hilo");
            return 1;
        }
    }

    // Espera a que todos los hilos terminen
    for (int i = 0; i < num_hilos; i++) {
        if (pthread_join(hilos[i], NULL) != 0) {
            perror("Error al esperar el hilo");
            return 1;
        }
    }

    return 0;
}