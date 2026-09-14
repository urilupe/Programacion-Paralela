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


    void depositar(int cantidad, int hilo, int operacion) {

        #pragma omp critical(banco)
        {
            saldo += cantidad;

            cout << "\nHilo " << hilo
                 << " - Operacion " << operacion
                 << " - DEPOSITO" << endl;

            cout << "Deposito de $" << cantidad
                 << " | Saldo actual: $" << saldo << endl;
        }
    }


    void retirar(int cantidad, int hilo, int operacion) {

        #pragma omp critical(banco)
        {
            cout << "\nHilo " << hilo
                 << " - Operacion " << operacion
                 << " - RETIRO" << endl;

            if (saldo >= cantidad) {

                saldo -= cantidad;

                cout << "Retiro de $" << cantidad
                     << " | Saldo actual: $" << saldo << endl;
            }
            else {

                cout << "Retiro rechazado de $" << cantidad
                     << " | Fondos insuficientes" << endl;
            }
        }
    }


    int consultarSaldo(int hilo, int operacion) {

        int saldoActual;

        #pragma omp critical(banco)
        {
            saldoActual = saldo;

            cout << "\nHilo " << hilo
                 << " - Operacion " << operacion
                 << " - CONSULTA" << endl;

            cout << "Saldo consultado: $"
                 << saldoActual << endl;
        }

        return saldoActual;
    }


    int obtenerSaldoFinal() {

        int saldoActual;

        #pragma omp critical(banco)
        {
            saldoActual = saldo;
        }

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
        cout << "       SISTEMA BANCARIO - CRITICAL" << endl;
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

                // Protegemos rand()
                #pragma omp critical(aleatorio)
                {
                    opcion = rand() % 3;

                    cantidad = (rand() % 10 + 1) * 10;
                }


                if (opcion == 0) {

                    banco.depositar(cantidad, id, i);
                }

                else if (opcion == 1) {

                    banco.retirar(cantidad, id, i);
                }

                else {

                    banco.consultarSaldo(id, i);
                }
            }
        }


        double fin = omp_get_wtime();


        cout << "\n======================================" << endl;
        cout << "RESULTADOS FINALES" << endl;
        cout << "======================================" << endl;

        cout << "Saldo final: $"
             << banco.obtenerSaldoFinal() << endl;

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