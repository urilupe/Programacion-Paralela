#include <iostream>
#include <cstdlib>
#include <ctime>
#include <omp.h>

using namespace std;


// =====================================================
// CLASE OPERACIONES ARREGLOS
// =====================================================
class OperacionesArreglos {

public:

    // -------------------------------------------------
    // SUMAR ARREGLOS
    // -------------------------------------------------
    void sumarArreglosOpenMP(
        const int* A,
        const int* B,
        int* R,
        int tamano
    ) {
        #pragma omp parallel for
        for (int i = 0; i < tamano; i++) {
            R[i] = A[i] + B[i];
        }
    }


    // -------------------------------------------------
    // RESTAR ARREGLOS
    // -------------------------------------------------
    void restarArreglosOpenMP(
        const int* A,
        const int* B,
        int* R,
        int tamano
    ) {
        #pragma omp parallel for
        for (int i = 0; i < tamano; i++) {
            R[i] = A[i] - B[i];
        }
    }


    // -------------------------------------------------
    // MULTIPLICAR ARREGLOS
    // -------------------------------------------------
    void multiplicarArreglosOpenMP(
        const int* A,
        const int* B,
        int* R,
        int tamano
    ) {
        #pragma omp parallel for
        for (int i = 0; i < tamano; i++) {
            R[i] = A[i] * B[i];
        }
    }


    // -------------------------------------------------
    // CUADRADO DE UN ARREGLO
    // -------------------------------------------------
    void cuadradoArregloOpenMP(
        const int* A,
        int* R,
        int tamano
    ) {
        #pragma omp parallel for
        for (int i = 0; i < tamano; i++) {
            R[i] = A[i] * A[i];
        }
    }
};


// =====================================================
// LLENAR ARREGLOS PARA LA DEMOSTRACION
// A = ascendente
// B = descendente
// =====================================================
void llenarDemostracion(int* A, int* B, int tamano) {

    for (int i = 0; i < tamano; i++) {

        A[i] = i + 1;
        B[i] = tamano - i;
    }
}


// =====================================================
// LLENAR ARREGLOS CON VALORES ALEATORIOS
// =====================================================
void llenarAleatorios(int* A, int* B, int tamano) {

    for (int i = 0; i < tamano; i++) {

        A[i] = rand() % 100;
        B[i] = rand() % 100;
    }
}


// =====================================================
// IMPRIMIR ARREGLO
// =====================================================
void imprimirArreglo(const int* arreglo, int tamano) {

    for (int i = 0; i < tamano; i++) {

        cout << arreglo[i] << " ";
    }

    cout << endl;
}


// =====================================================
// MAIN
// =====================================================
int main() {

    srand(static_cast<unsigned int>(time(nullptr)));

    OperacionesArreglos operaciones;

    int modo;
    int tamano;
    int opcion;

    bool arreglosLlenos = false;


    // =================================================
    // SELECCIONAR TIPO DE EJECUCION
    // =================================================
    cout << "============================================\n";
    cout << "       PRACTICA DE ARREGLOS CON OPENMP\n";
    cout << "============================================\n\n";

    cout << "Hermosillo Prado Carlos\n";
    cout << "Montes de Oca del Risco Rafael Alejandro\n";
    cout << "Ramirez Andrade Uriel Ismael Guadalupe\n";


    cout << "Seleccione el tipo de ejecucion:\n\n";

    cout << "1. Demostracion con 100 elementos\n";
    cout << "2. Rendimiento con 10,000,000 elementos\n";

    cout << "\nOpcion: ";
    cin >> modo;


    // =================================================
    // VALIDAR MODO
    // =================================================
    if (modo == 1) {

        tamano = 100;

    }
    else if (modo == 2) {

        tamano = 10000000;

    }
    else {

        cout << "\nOpcion no valida.\n";

        return 1;
    }


    // =================================================
    // CREAR ARREGLOS DINAMICAMENTE
    // =================================================
    int* A = new int[tamano];
    int* B = new int[tamano];
    int* R = new int[tamano];


    // =================================================
    // MENU PRINCIPAL
    // =================================================
    do {

        cout << "\n============================================\n";
        cout << "                   MENU\n";
        cout << "============================================\n";

        cout << "1. Llenar arreglos\n";
        cout << "2. Sumar arreglos\n";
        cout << "3. Restar arreglos\n";
        cout << "4. Multiplicar arreglos\n";
        cout << "5. Cuadrado de un arreglo\n";
        cout << "6. Salir\n";

        cout << "============================================\n";

        cout << "Seleccione una opcion: ";
        cin >> opcion;


        // =================================================
        // OPCION 1 - LLENAR ARREGLOS
        // =================================================
        if (opcion == 1) {

            // Primera ejecución: 100 elementos
            if (modo == 1) {

                llenarDemostracion(A, B, tamano);

                cout << "\nArreglos llenados correctamente.\n";

                cout << "\nArreglo A (ascendente):\n";
                imprimirArreglo(A, tamano);

                cout << "\nArreglo B (descendente):\n";
                imprimirArreglo(B, tamano);
            }

            // Segunda ejecución: 10 millones
            else {

                llenarAleatorios(A, B, tamano);

                cout << "\nArreglos de "
                     << tamano
                     << " elementos llenados correctamente.\n";

                cout << "Los arreglos no se imprimen debido a su tamano.\n";
            }

            arreglosLlenos = true;
        }


        // =================================================
        // OPCIONES 2 A 5
        // =================================================
        else if (opcion >= 2 && opcion <= 5) {

            // Verificar que primero se hayan llenado
            if (!arreglosLlenos) {

                cout << "\nPrimero debe llenar los arreglos "
                     << "utilizando la opcion 1.\n";

                continue;
            }


            // -------------------------------------------------
            // En la demostración mostrar A y B antes
            // de cada operación
            // -------------------------------------------------
            if (modo == 1) {

                cout << "\nArreglo A:\n";
                imprimirArreglo(A, tamano);

                cout << "\nArreglo B:\n";
                imprimirArreglo(B, tamano);
            }


            double inicio;
            double fin;


            // =================================================
            // OPCION 2 - SUMA
            // =================================================
            if (opcion == 2) {

                inicio = omp_get_wtime();

                operaciones.sumarArreglosOpenMP(
                    A,
                    B,
                    R,
                    tamano
                );

                fin = omp_get_wtime();

                cout << "\nOperacion realizada: SUMA\n";
            }


            // =================================================
            // OPCION 3 - RESTA
            // =================================================
            else if (opcion == 3) {

                inicio = omp_get_wtime();

                operaciones.restarArreglosOpenMP(
                    A,
                    B,
                    R,
                    tamano
                );

                fin = omp_get_wtime();

                cout << "\nOperacion realizada: RESTA\n";
            }


            // =================================================
            // OPCION 4 - MULTIPLICACION
            // =================================================
            else if (opcion == 4) {

                inicio = omp_get_wtime();

                operaciones.multiplicarArreglosOpenMP(
                    A,
                    B,
                    R,
                    tamano
                );

                fin = omp_get_wtime();

                cout << "\nOperacion realizada: MULTIPLICACION\n";
            }


            // =================================================
            // OPCION 5 - CUADRADO
            // =================================================
            else if (opcion == 5) {

                inicio = omp_get_wtime();

                operaciones.cuadradoArregloOpenMP(
                    A,
                    R,
                    tamano
                );

                fin = omp_get_wtime();

                cout << "\nOperacion realizada: CUADRADO DEL ARREGLO A\n";
            }


            // =================================================
            // MOSTRAR RESULTADO SOLO CON 100 ELEMENTOS
            // =================================================
            if (modo == 1) {

                cout << "\nArreglo R:\n";
                imprimirArreglo(R, tamano);
            }


            // =================================================
            // MOSTRAR TIEMPO
            // =================================================
            cout << "\nTiempo de ejecucion en paralelo: "
                 << (fin - inicio)
                 << " segundos\n";
        }


        // =================================================
        // OPCION 6 - SALIR
        // =================================================
        else if (opcion == 6) {

            cout << "\nFinalizando programa...\n";

            cout << "Hermosillo Prado Carlos\n";
            cout << "Montes de Oca del Risco Rafael Alejandro\n";
            cout << "Ramirez Andrade Uriel Ismael Guadalupe\n";
        }

        // =================================================
        // OPCION INCORRECTA
        // =================================================
        else {

            cout << "\nOpcion no valida.\n";
        }

    } while (opcion != 6);


    // =================================================
    // LIBERAR MEMORIA
    // =================================================
    delete[] A;
    delete[] B;
    delete[] R;

    return 0;
}