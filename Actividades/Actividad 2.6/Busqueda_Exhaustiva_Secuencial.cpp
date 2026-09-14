/*
    PRACTICA: Busqueda exhaustiva - VERSION SECUENCIAL
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
#include <omp.h>   // omp_get_wtime()

using namespace std;
using ull = unsigned long long;

struct ResultadoSecuencial {
    bool encontrada;
    ull posicion;
    ull revisadas;
    double tiempo;
};

class BusquedaExhaustivaSecuencial {
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
    BusquedaExhaustivaSecuencial() {
        cantidadCaracteres = 36;
        caracteres = new char[cantidadCaracteres + 1];
        strcpy(caracteres, "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
    }

    ~BusquedaExhaustivaSecuencial() {
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

        int longitudReal = static_cast<int>(strlen(clave));
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

    // Convierte una posicion numerica a su combinacion en base 36.
    // No se almacenan todas las combinaciones simultaneamente.
    void posicionAClave(ull posicion, int longitud, char* salida) const {
        for (int i = longitud - 1; i >= 0; --i) {
            int digito = static_cast<int>(posicion % static_cast<ull>(cantidadCaracteres));
            salida[i] = caracteres[digito];
            posicion /= static_cast<ull>(cantidadCaracteres);
        }
        salida[longitud] = '\0';
    }

    ResultadoSecuencial buscar(const char* objetivo, int longitud, ull total) const {
        ResultadoSecuencial resultado{};
        resultado.encontrada = false;
        resultado.posicion = 0;
        resultado.revisadas = 0;
        resultado.tiempo = 0.0;

        char* combinacion = new char[longitud + 1];

        cout << "\n============================================================\n";
        cout << "                 BUSQUEDA SECUENCIAL\n";
        cout << "============================================================\n";
        cout << "Estado: INICIO DE BUSQUEDA\n";

        const double inicio = omp_get_wtime();

        for (ull posicion = 0; posicion < total; ++posicion) {
            posicionAClave(posicion, longitud, combinacion);
            ++resultado.revisadas;

            if (strcmp(combinacion, objetivo) == 0) {
                resultado.encontrada = true;
                resultado.posicion = posicion;
                break;
            }
        }

        const double fin = omp_get_wtime();
        resultado.tiempo = fin - inicio;

        cout << "Estado: FIN DE BUSQUEDA\n";
        cout << "Combinaciones revisadas: " << formatoNumero(resultado.revisadas) << "\n";

        if (resultado.encontrada) {
            posicionAClave(resultado.posicion, longitud, combinacion);
            cout << "Clave encontrada: " << combinacion << "\n";
            cout << "Posicion numerica: " << formatoNumero(resultado.posicion) << "\n";
            cout << "Verificacion exacta: "
                 << (strcmp(combinacion, objetivo) == 0 ? "CORRECTA" : "ERROR") << "\n";
        } else {
            cout << "Clave no encontrada.\n";
        }

        cout << fixed << setprecision(9);
        cout << "Tiempo secuencial: " << resultado.tiempo << " segundos\n";
        cout << "============================================================\n";

        delete[] combinacion;
        return resultado;
    }

    void mostrarDatosEspacio(int longitud, ull total) const {
        char* primera = new char[longitud + 1];
        char* ultima = new char[longitud + 1];

        posicionAClave(0, longitud, primera);
        posicionAClave(total - 1, longitud, ultima);

        cout << "\n================ DATOS DE LA PRUEBA ================\n";
        cout << "Version: SECUENCIAL\n";
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

char* leerClave(const BusquedaExhaustivaSecuencial& buscador, int longitud) {
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
    cout << " PRACTICA - BUSQUEDA EXHAUSTIVA / VERSION SECUENCIAL\n";
    cout << " Uso exclusivo de claves de prueba academicas\n";
    cout << "============================================================\n";
    cout << "Integrantes:\n";
    cout << " - Hermosillo Prado Carlos\n";
    cout << " - Montes de Oca del Risco Rafael Alejandro\n";
    cout << " - Ramirez Andrade Uriel Ismael Guadalupe\n";

    BusquedaExhaustivaSecuencial buscador;

    const int opcion = leerTipoEjecucion();
    const int longitud = (opcion == 1) ? 3 : leerLongitudMayor();
    const ull total = buscador.calcularTamanoEspacio(longitud);

    buscador.mostrarDatosEspacio(longitud, total);

    char* clave = leerClave(buscador, longitud);
    cout << "\nClave de prueba introducida: " << clave << "\n";

    if (longitud >= 6) {
        cout << "ADVERTENCIA: 36^" << longitud
             << " genera un espacio muy grande y la ejecucion puede tardar bastante.\n";
    }

    ResultadoSecuencial resultado = buscador.buscar(clave, longitud, total);

    cout << "\nRESUMEN PARA COMPARACION POSTERIOR\n";
    cout << "Clave: " << clave << "\n";
    cout << "Longitud: " << longitud << "\n";
    cout << "Espacio: " << BusquedaExhaustivaSecuencial::formatoNumero(total) << " combinaciones\n";
    cout << fixed << setprecision(9);
    cout << "Tiempo secuencial: " << resultado.tiempo << " s\n";
    cout << "Resultado: " << (resultado.encontrada ? "ENCONTRADA" : "NO ENCONTRADA") << "\n";

    delete[] clave;
    return 0;
}
