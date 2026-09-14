#include <iostream>
#include <cstdlib>
#include <ctime>
#include <omp.h>

using namespace std;


// =========================
// CLASE BANCO
// =========================
class Banco {
private:

    int saldo;
    omp_lock_t bloqueo;

public:

    Banco(int saldoInicial) {

        saldo = saldoInicial;

        // Inicializar el lock
        omp_init_lock(&bloqueo);
    }


    ~Banco() {

        // Liberar el recurso utilizado
        omp_destroy_lock(&bloqueo);
    }


    void depositar(int cantidad) {

        omp_set_lock(&bloqueo);

        saldo += cantidad;

        cout << "Deposito de $" << cantidad
             << " | Saldo actual: $"
             << saldo << endl;

        omp_unset_lock(&bloqueo);
    }


    void retirar(int cantidad) {

        omp_set_lock(&bloqueo);

        if (saldo >= cantidad) {

            saldo -= cantidad;

            cout << "Retiro de $" << cantidad
                 << " | Saldo actual: $"
                 << saldo << endl;

        }

        else {

            cout << "Retiro rechazado de $"
                 << cantidad
                 << " | Fondos insuficientes" << endl;
        }

        omp_unset_lock(&bloqueo);
    }


    int consultarSaldo() {

        omp_set_lock(&bloqueo);

        int saldoActual = saldo;

        omp_unset_lock(&bloqueo);

        return saldoActual;
    }
};


// =========================
// CLASE TESTBANCO
// =========================
class TestBanco {

public:

    void ejecutar() {

        int saldoInicial = 1000;
        int numeroHilos = 4;

        Banco banco(saldoInicial);

        srand(time(NULL));

        omp_set_num_threads(numeroHilos);

        cout << "======================================" << endl;
        cout << "         SISTEMA BANCARIO - LOCK" << endl;
        cout << "======================================" << endl;

        cout << "Saldo inicial: $" << saldoInicial << endl;
        cout << "Numero de hilos: " << numeroHilos << endl;
        cout << "Operaciones por hilo: 5" << endl;
        cout << "======================================" << endl;

        double inicio = omp_get_wtime();

        #pragma omp parallel
        {
            int id = omp_get_thread_num();

            for (int i = 1; i <= 5; i++) {

                int opcion;
                int cantidad;

                #pragma omp critical(aleatorio)
                {
                    opcion = rand() % 3;
                    cantidad = (rand() % 10 + 1) * 10;
                }

                if (opcion == 0) {

                    #pragma omp critical(pantalla)
                    {
                        cout << "\nHilo " << id
                             << " - Operacion " << i
                             << " - DEPOSITO" << endl;
                    }

                    banco.depositar(cantidad);
                }

                else if (opcion == 1) {

                    #pragma omp critical(pantalla)
                    {
                        cout << "\nHilo " << id
                             << " - Operacion " << i
                             << " - RETIRO" << endl;
                    }

                    banco.retirar(cantidad);
                }

                else {

                    int saldo = banco.consultarSaldo();

                    #pragma omp critical(pantalla)
                    {
                        cout << "\nHilo " << id
                             << " - Operacion " << i
                             << " - CONSULTA" << endl;

                        cout << "Saldo consultado: $"
                             << saldo << endl;
                    }
                }
            }
        }

        double fin = omp_get_wtime();

        cout << "\n======================================" << endl;
        cout << "RESULTADOS FINALES" << endl;
        cout << "======================================" << endl;

        cout << "Saldo final: $"
             << banco.consultarSaldo() << endl;

        cout << "Tiempo de ejecucion: "
             << fin - inicio
             << " segundos" << endl;

        cout << "======================================" << endl;
    }
};


// =========================
// MAIN
// =========================
int main() {

    TestBanco prueba;

    prueba.ejecutar();

    return 0;
}