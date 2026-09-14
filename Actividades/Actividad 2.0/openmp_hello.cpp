//Ramirez Andrade Uriel Ismael GUadalupe
#include <omp.h>
#include <stdio.h>

int main() {
#pragma omp parallel
    {
        int id = omp_get_thread_num();
        printf("Hola desde el hilo %d\n", id);
    }

    return 0;
}
