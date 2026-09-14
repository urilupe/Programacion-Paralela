/*
    PRACTICA: Busqueda exhaustiva - VERSION PARALELA CON OpenMP
    Materia: Programacion Paralela

    Integrantes (apellidos y nombres en orden alfabetico):
    Hermosillo Prado Carlos
    Montes de Oca del Risco Rafael Alejandro
    Ramirez Andrade Uriel Ismael Guadalupe

    IMPORTANTE:
    Este programa trabaja UNICAMENTE con claves de prueba academicas
    introducidas por el usuario y formadas por A-Z y 0-9.
    No accede a cuentas, archivos protegidos, credenciales, redes
    ni sistemas externos.
*/

#include <iostream>
#include <iomanip>
#include <cstring>
#include <limits>
#include <string>
#include <omp.h>

using namespace std;
using ull = unsigned long long;

struct EstadoHilo {
    ull inicio;
    ull fin;
    ull asignadas;
    ull revisadas;
    bool encontro;
    bool finalizo;
    bool detenidoPorOtro;
};

struct ResultadoParalelo {
    bool encontrada;
    ull posicion;
    ull revisadas;
    double tiempo;
    int hiloGanador;
    int hilosUtilizados;
};

class BusquedaExhaustivaParalela {
private:
    char* caracteres;
    int cantidadCaracteres;

    int indiceCaracter(char c) const {
        for (int i = 0; i < cantidadCaracteres; ++i) {
            if (caracteres[i] == c) {
                return i;
            }
        }
        return -1;
    }

public:
    BusquedaExhaustivaParalela() {
        cantidadCaracteres = 36;
        caracteres = new char[cantidadCaracteres + 1];
        strcpy(caracteres, "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
    }

    ~BusquedaExhaustivaParalela() {
        delete[] caracteres;
    }

    ull calcularTamanoEspacio(int longitud) const {
        ull total = 1;
        for (int i = 0; i < longitud; ++i) {
            total *= static_cast<ull>(cantidadCaracteres);
        }
        return total;
    }

    bool validarClave(const char* clave, int longitudEsperada) const {
        if (clave == nullptr || clave[0] == '\0') {
            cout << "ERROR: La clave no puede estar vacia.\n";
            return false;
        }

        const int longitudReal = static_cast<int>(strlen(clave));
        if (longitudReal != longitudEsperada) {
            cout << "ERROR: La clave debe tener exactamente "
                 << longitudEsperada << " caracteres.\n";
            return false;
        }

        for (int i = 0; i < longitudReal; ++i) {
            if (indiceCaracter(clave[i]) == -1) {
                cout << "ERROR: El caracter '" << clave[i]
                     << "' no esta permitido. Use solo A-Z y 0-9, sin espacios.\n";
                return false;
            }
        }

        return true;
    }

    void posicionAClave(ull posicion, int longitud, char* salida) const {
        for (int i = longitud - 1; i >= 0; --i) {
            const int digito = static_cast<int>(posicion % static_cast<ull>(cantidadCaracteres));
            salida[i] = caracteres[digito];
            posicion /= static_cast<ull>(cantidadCaracteres);
        }
        salida[longitud] = '\0';
    }

    ResultadoParalelo buscar(const char* objetivo,
                             int longitud,
                             ull total,
                             int hilosSolicitados) const {
        ResultadoParalelo resultado{};
        resultado.encontrada = false;
        resultado.posicion = 0;
        resultado.revisadas = 0;
        resultado.tiempo = 0.0;
        resultado.hiloGanador = -1;
        resultado.hilosUtilizados = hilosSolicitados;

        // Arreglo dinamico: no se utiliza vector.
        EstadoHilo* estados = new EstadoHilo[hilosSolicitados];
        for (int i = 0; i < hilosSolicitados; ++i) {
            estados[i] = {0, 0, 0, 0, false, false, false};
        }

        // 'encontrada' se comparte como bandera de comunicacion.
        // 0 = continuar, 1 = algun hilo ya encontro la clave.
        int encontrada = 0;
        ull posicionEncontrada = 0;
        int hiloGanador = -1;
        int hilosReales = hilosSolicitados;

        cout << "\n============================================================\n";
        cout << "            BUSQUEDA PARALELA UTILIZANDO OpenMP\n";
        cout << "============================================================\n";
        cout << "Estado general: INICIO DE BUSQUEDA\n";

        omp_set_dynamic(0);
        const double inicioTiempo = omp_get_wtime();

        #pragma omp parallel num_threads(hilosSolicitados) \
            shared(estados, encontrada, posicionEncontrada, hiloGanador, hilosReales)
        {
            const int id = omp_get_thread_num();
            const int totalHilos = omp_get_num_threads();

            #pragma omp single
            {
                hilosReales = totalHilos;
            }

            // Distribucion explicita y equilibrada del espacio.
            const ull base = total / static_cast<ull>(totalHilos);
            const ull residuo = total % static_cast<ull>(totalHilos);

            ull cantidad = 0;
            ull rangoInicio = 0;

            if (static_cast<ull>(id) < residuo) {
                cantidad = base + 1;
                rangoInicio = static_cast<ull>(id) * cantidad;
            } else {
                cantidad = base;
                rangoInicio = residuo * (base + 1)
                            + (static_cast<ull>(id) - residuo) * base;
            }

            const ull rangoFin = (cantidad == 0) ? rangoInicio : (rangoInicio + cantidad - 1);

            estados[id].inicio = rangoInicio;
            estados[id].fin = rangoFin;
            estados[id].asignadas = cantidad;

            char* claveInicio = new char[longitud + 1];
            char* claveFin = new char[longitud + 1];
            char* combinacion = new char[longitud + 1];

            posicionAClave(rangoInicio, longitud, claveInicio);
            posicionAClave(rangoFin, longitud, claveFin);

            // Se protege la salida para que las lineas no se mezclen.
            #pragma omp critical(salida_hilos)
            {
                cout << "Hilo " << id
                     << " -> Total hilos: " << totalHilos
                     << " -> Inicio: " << claveInicio
                     << " -> Fin: " << claveFin
                     << " -> Cantidad: " << formatoNumero(cantidad)
                     << " -> Estado: INICIADO\n";
            }

            ull revisadasLocal = 0;
            bool encontroLocal = false;
            bool detenidoPorOtro = false;

            for (ull posicion = rangoInicio; cantidad > 0 && posicion <= rangoFin; ++posicion) {
                int detener = 0;

                // Comunicacion segura de la bandera compartida.
                #pragma omp atomic read
                detener = encontrada;

                if (detener != 0) {
                    detenidoPorOtro = true;
                    break;
                }

                posicionAClave(posicion, longitud, combinacion);
                ++revisadasLocal;

                if (strcmp(combinacion, objetivo) == 0) {
                    // Primero se registra de forma protegida toda la informacion
                    // del hilo ganador.
                    #pragma omp critical(registro_ganador)
                    {
                        posicionEncontrada = posicion;
                        hiloGanador = id;
                    }

                    // Despues se publica la bandera para detener a los demas.
                    #pragma omp atomic write
                    encontrada = 1;

                    encontroLocal = true;
                    break;
                }

                if (posicion == rangoFin) {
                    break;
                }
            }

            estados[id].revisadas = revisadasLocal;
            estados[id].encontro = encontroLocal;
            estados[id].finalizo = true;
            estados[id].detenidoPorOtro = detenidoPorOtro;

            #pragma omp critical(salida_hilos)
            {
                cout << "Hilo " << id
                     << " -> Estado: FINALIZADO"
                     << " -> Revisadas: " << formatoNumero(revisadasLocal)
                     << " -> Encontro clave: " << (encontroLocal ? "SI" : "NO")
                     << " -> Detenido por otro hilo: " << (detenidoPorOtro ? "SI" : "NO")
                     << "\n";
            }

            delete[] claveInicio;
            delete[] claveFin;
            delete[] combinacion;
        }

        const double finTiempo = omp_get_wtime();

        resultado.encontrada = (encontrada != 0);
        resultado.posicion = posicionEncontrada;
        resultado.hiloGanador = hiloGanador;
        resultado.hilosUtilizados = hilosReales;
        resultado.tiempo = finTiempo - inicioTiempo;

        ull totalRevisadas = 0;
        for (int i = 0; i < hilosReales; ++i) {
            totalRevisadas += estados[i].revisadas;
        }
        resultado.revisadas = totalRevisadas;

        cout << "\n---------------- RESUMEN DE HILOS ----------------\n";
        for (int i = 0; i < hilosReales; ++i) {
            char* inicioTexto = new char[longitud + 1];
            char* finTexto = new char[longitud + 1];

            posicionAClave(estados[i].inicio, longitud, inicioTexto);
            posicionAClave(estados[i].fin, longitud, finTexto);

            cout << "Hilo " << i
                 << " | Rango: " << inicioTexto << " - " << finTexto
                 << " | Asignadas: " << formatoNumero(estados[i].asignadas)
                 << " | Revisadas: " << formatoNumero(estados[i].revisadas)
                 << " | Inicio: SI"
                 << " | Finalizo: " << (estados[i].finalizo ? "SI" : "NO")
                 << " | Encontro: " << (estados[i].encontro ? "SI" : "NO")
                 << "\n";

            delete[] inicioTexto;
            delete[] finTexto;
        }
        cout << "---------------------------------------------------\n";
        cout << "Estado general: FIN DE BUSQUEDA\n";

        if (resultado.encontrada) {
            char* encontradaTexto = new char[longitud + 1];
            posicionAClave(resultado.posicion, longitud, encontradaTexto);

            cout << "Clave encontrada: " << encontradaTexto << "\n";
            cout << "Clave encontrada por el hilo: " << resultado.hiloGanador << "\n";
            cout << "Posicion numerica: " << formatoNumero(resultado.posicion) << "\n";
            cout << "Verificacion exacta: "
                 << (strcmp(encontradaTexto, objetivo) == 0 ? "CORRECTA" : "ERROR") << "\n";

            delete[] encontradaTexto;
        } else {
            cout << "Clave no encontrada.\n";
        }

        cout << "Combinaciones revisadas entre todos los hilos: "
             << formatoNumero(resultado.revisadas) << "\n";
        cout << fixed << setprecision(9);
        cout << "Tiempo paralelo: " << resultado.tiempo << " segundos\n";
        cout << "Numero de hilos utilizados: " << resultado.hilosUtilizados << "\n";
        cout << "============================================================\n";

        delete[] estados;
        return resultado;
    }

    void mostrarDatosEspacio(int longitud, ull total) const {
        char* primera = new char[longitud + 1];
        char* ultima = new char[longitud + 1];

        posicionAClave(0, longitud, primera);
        posicionAClave(total - 1, longitud, ultima);

        cout << "\n================ DATOS DE LA PRUEBA ================\n";
        cout << "Version: PARALELA CON OpenMP\n";
        cout << "Conjunto permitido: " << caracteres << "\n";
        cout << "Cantidad de caracteres: " << cantidadCaracteres << "\n";
        cout << "Longitud seleccionada: " << longitud << "\n";
        cout << "Calculo realizado por el programa: 36^" << longitud << "\n";
        cout << "Numero total de combinaciones: " << formatoNumero(total) << "\n";
        cout << "Primera combinacion: " << primera << "\n";
        cout << "Ultima combinacion: " << ultima << "\n";
        cout << "=====================================================\n";

        delete[] primera;
        delete[] ultima;
    }

    static string formatoNumero(ull numero) {
        string texto = to_string(numero);
        string salida;
        int contador = 0;

        for (int i = static_cast<int>(texto.size()) - 1; i >= 0; --i) {
            salida.insert(salida.begin(), texto[i]);
            ++contador;
            if (contador == 3 && i != 0) {
                salida.insert(salida.begin(), ',');
                contador = 0;
            }
        }
        return salida;
    }
};

int leerTipoEjecucion() {
    int opcion;

    while (true) {
        cout << "\nSeleccione la ejecucion requerida:\n";
        cout << "1. Primera ejecucion: clave de 3 caracteres\n";
        cout << "2. Segunda ejecucion: clave de mayor longitud\n";
        cout << "Opcion: ";

        if (cin >> opcion && (opcion == 1 || opcion == 2)) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return opcion;
        }

        cout << "ERROR: Introduzca 1 o 2.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

int leerLongitudMayor() {
    int longitud;

    while (true) {
        cout << "Longitud para la segunda ejecucion (5, 6, 7 o 10): ";

        if (cin >> longitud &&
            (longitud == 5 || longitud == 6 || longitud == 7 || longitud == 10)) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return longitud;
        }

        cout << "ERROR: Seleccione 5, 6, 7 o 10.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

int leerNumeroHilos() {
    const int maximo = omp_get_max_threads();
    int hilos;

    while (true) {
        cout << "Numero de hilos a utilizar (1-" << maximo << "): ";

        if (cin >> hilos && hilos >= 1 && hilos <= maximo) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return hilos;
        }

        cout << "ERROR: Introduzca un numero entre 1 y " << maximo << ".\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

char* leerClave(const BusquedaExhaustivaParalela& buscador, int longitud) {
    const int capacidad = 128;
    char* entrada = new char[capacidad];

    while (true) {
        cout << "Introduzca una clave DE PRUEBA de " << longitud
             << " caracteres usando solo A-Z y 0-9: ";

        cin.getline(entrada, capacidad);

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "ERROR: La entrada es demasiado larga.\n";
            continue;
        }

        if (buscador.validarClave(entrada, longitud)) {
            char* clave = new char[longitud + 1];
            strcpy(clave, entrada);
            delete[] entrada;
            return clave;
        }
    }
}

int main() {
    cout << "============================================================\n";
    cout << " PRACTICA - BUSQUEDA EXHAUSTIVA / VERSION PARALELA OpenMP\n";
    cout << " Uso exclusivo de claves de prueba academicas\n";
    cout << "============================================================\n";
    cout << "Integrantes:\n";
    cout << " - Hermosillo Prado Carlos\n";
    cout << " - Montes de Oca del Risco Rafael Alejandro\n";
    cout << " - Ramirez Andrade Uriel Ismael Guadalupe\n";

    BusquedaExhaustivaParalela buscador;

    const int opcion = leerTipoEjecucion();
    const int longitud = (opcion == 1) ? 3 : leerLongitudMayor();
    const ull total = buscador.calcularTamanoEspacio(longitud);

    buscador.mostrarDatosEspacio(longitud, total);

    char* clave = leerClave(buscador, longitud);
    const int hilos = leerNumeroHilos();

    cout << "\nClave de prueba introducida: " << clave << "\n";
    cout << "Hilos solicitados: " << hilos << "\n";

    if (longitud >= 6) {
        cout << "ADVERTENCIA: 36^" << longitud
             << " genera un espacio muy grande y la ejecucion puede tardar bastante.\n";
    }

    ResultadoParalelo resultado = buscador.buscar(clave, longitud, total, hilos);

    cout << "\nRESUMEN PARA COMPARACION POSTERIOR\n";
    cout << "Clave: " << clave << "\n";
    cout << "Longitud: " << longitud << "\n";
    cout << "Espacio: " << BusquedaExhaustivaParalela::formatoNumero(total) << " combinaciones\n";
    cout << fixed << setprecision(9);
    cout << "Tiempo paralelo: " << resultado.tiempo << " s\n";
    cout << "Numero de hilos: " << resultado.hilosUtilizados << "\n";
    cout << "Hilo ganador: " << resultado.hiloGanador << "\n";
    cout << "Resultado: " << (resultado.encontrada ? "ENCONTRADA" : "NO ENCONTRADA") << "\n";

    delete[] clave;
    return 0;
}
