//Ramirez Andrade Uriel Ismael GUadalupe

#include <mpi.h>
#include <iostream>
 
int main(int argc, char* argv[]) {
    int rank, size;
   
    MPI_Init(&argc, &argv); // Inicialización de MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Obtener el rango del proceso
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Obtener el tamaño del comunicador
   
    std::cout << "Hola Mundo desde el proceso " << rank << " de " << size << std::endl;
   
    MPI_Finalize(); // Finalización de MPI
   
    return 0;
}