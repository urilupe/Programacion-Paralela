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

public:

    Banco(int saldoInicial) {
        saldo = saldoInicial;
    }


    void depositar(int cantidad) {

        #pragma omp atomic
        saldo += cantidad;

        int saldoActual;

        #pragma omp atomic read
        saldoActual = saldo;

        #pragma omp critical(pantalla)
        {
            cout << "Deposito de $" << cantidad
                 << " | Saldo actual: $"
                 << saldoActual << endl;
        }
    }


    void retirar(int cantidad) {

        // Un retiro requiere comprobar el saldo
        // y posteriormente modificarlo.
        #pragma omp critical(retiro)
        {
            int saldoActual;

            #pragma omp atomic read
            saldoActual = saldo;

            if (saldoActual >= cantidad) {

                #pragma omp atomic
                saldo -= cantidad;

                #pragma omp atomic read
                saldoActual = saldo;

                #pragma omp critical(pantalla)
                {
                    cout << "Retiro de $" << cantidad
                         << " | Saldo actual: $"
                         << saldoActual << endl;
                }
            }

            else {

                #pragma omp critical(pantalla)
                {
                    cout << "Retiro rechazado de $"
                         << cantidad
                         << " | Fondos insuficientes" << endl;
                }
            }
        }
    }


    int consultarSaldo() {

        int saldoActual;

        #pragma omp atomic read
        saldoActual = saldo;

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
        cout << "        SISTEMA BANCARIO - ATOMIC" << endl;
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