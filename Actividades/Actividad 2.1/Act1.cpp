#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define HILOS 5
#define TAM 10

int main() {

    int arreglos[HILOS][TAM];

    omp_set_num_threads(HILOS);

    #pragma omp parallel
    {
        #pragma omp for
        for (int h = 0; h < HILOS; h++) {

            // Cada hilo comienza con una semilla diferente
            srand(h + 1);

            printf("Hilo %d iniciado\n", h);

            for (int i = 0; i < TAM; i++) {

                int numero;
                int repetido;

                do {
                    numero = rand() % 1000;
                    repetido = 0;

                    // Revisar que no se repita dentro del mismo arreglo
                    for (int j = 0; j < i; j++) {

                        if (arreglos[h][j] == numero) {
                            repetido = 1;
                        }
                    }

                } while (repetido == 1);

                arreglos[h][i] = numero;

                int avance = (i + 1) * 10;

                printf("Hilo %d - numero %d - avance %d%%\n",
                       h, numero, avance);
            }

            printf("Hilo %d terminado al 100%%\n\n", h);
        }

        #pragma omp single
        {
            printf("\n--- ARREGLOS FINALES ---\n");

            for (int h = 0; h < HILOS; h++) {

                printf("Hilo %d: ", h);

                for (int i = 0; i < TAM; i++) {
                    printf("%d ", arreglos[h][i]);
                }

                printf("\n");
            }
        }
    }

    return 0;
}
