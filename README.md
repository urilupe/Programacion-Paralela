# Práctica 2.6 - Búsqueda Exhaustiva Secuencial y Paralela con OpenMP

## Integrantes

> Agregar los nombres completos de los integrantes en orden alfabético.

- [Integrante 1]
- [Integrante 2]
- [Integrante 3]

**Equipo:** [Número de equipo]

---

## Descripción de la solución

Esta práctica implementa un algoritmo de **búsqueda exhaustiva** sobre un espacio de claves de prueba con fines exclusivamente académicos.

Se desarrollaron dos versiones independientes:

1. **Versión secuencial:** recorre el espacio de búsqueda desde la primera combinación hasta encontrar la clave de prueba introducida por el usuario.
2. **Versión paralela con OpenMP:** divide explícitamente el espacio de búsqueda entre varios hilos para que cada uno revise un rango diferente de combinaciones.

El programa no almacena todas las combinaciones simultáneamente en memoria. Cada combinación se representa mediante una **posición numérica** que se convierte a una cadena utilizando un sistema de base 36.

Ambas versiones solicitan y validan una clave de prueba, calculan el espacio total de búsqueda, generan combinaciones a partir de posiciones numéricas, miden el tiempo con `omp_get_wtime()` y verifican que la clave encontrada coincida exactamente con la entrada.

La versión paralela también distribuye el espacio entre hilos, registra el hilo ganador y comunica el hallazgo al resto para evitar trabajo innecesario.

> **Importante:** este proyecto utiliza únicamente claves de prueba creadas para la práctica. No trabaja con contraseñas reales, credenciales, archivos protegidos ni sistemas externos.

---

## Caracteres utilizados

El espacio de búsqueda utiliza los siguientes **36 caracteres**:

```text
ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789
```

El número total de combinaciones para una clave de longitud `n` se calcula como:

```text
36^n
```

Ejemplos:

| Longitud | Número de combinaciones |
|---:|---:|
| 3 | 46,656 |
| 5 | 60,466,176 |
| 7 | 78,364,164,096 |
| 10 | 3,656,158,440,062,976 |

---

## Algoritmos implementados

### 1. Búsqueda exhaustiva secuencial

La versión secuencial recorre las posiciones del espacio de búsqueda una por una.

Para cada posición:

1. Convierte la posición numérica a una combinación.
2. Compara la combinación con la clave de prueba.
3. Incrementa el contador de combinaciones revisadas.
4. Si encuentra la clave, detiene la búsqueda.
5. Registra la posición y el tiempo de ejecución.

Archivo:

```text
Busqueda_Exhaustiva_Secuencial.cpp
```

### 2. Búsqueda exhaustiva paralela con OpenMP

La versión paralela divide el espacio total de búsqueda entre los hilos disponibles.

Cada hilo conoce:

- su identificador,
- la cantidad total de hilos,
- el inicio de su rango,
- el final de su rango,
- y la cantidad de combinaciones asignadas.

Los rangos se distribuyen de forma equilibrada mediante cociente y residuo.

Cuando un hilo encuentra la clave:

1. Registra la posición encontrada.
2. Registra su identificador como hilo ganador.
3. Modifica una bandera compartida.
4. Los demás hilos consultan la bandera y detienen su búsqueda.

Archivo:

```text
Busqueda_Exhaustiva_Paralela_OpenMP.cpp
```

---

## Directivas OpenMP utilizadas

### `#pragma omp parallel`

Crea la región paralela donde varios hilos ejecutan la búsqueda simultáneamente.

```cpp
#pragma omp parallel num_threads(hilosSolicitados)
```

### `omp_get_thread_num()`

Obtiene el identificador del hilo actual.

### `omp_get_num_threads()`

Obtiene la cantidad total de hilos de la región paralela.

### `#pragma omp atomic read`

Permite consultar de forma segura la bandera compartida que indica si la clave ya fue encontrada.

### `#pragma omp atomic write`

Permite modificar de forma segura la bandera compartida cuando un hilo encuentra la clave.

### `#pragma omp critical`

Protege la información compartida relacionada con el hilo ganador para evitar condiciones de carrera.

### `#pragma omp barrier`

Sincroniza los hilos en puntos determinados del programa.

### `#pragma omp single`

Permite que una sola hebra ejecute ciertas operaciones una única vez dentro de la región paralela.

### `omp_get_wtime()`

Mide el tiempo de ejecución tanto en la versión secuencial como en la paralela.

---

## Estructura del repositorio

```text
.
├── Busqueda_Exhaustiva_Secuencial.cpp
├── Busqueda_Exhaustiva_Paralela_OpenMP.cpp
└── README.md
```

---

## Requisitos

- Compilador compatible con C++17.
- Soporte para OpenMP.
- En Windows puede utilizarse **MSYS2 UCRT64 con g++**.

---

## Compilación

### Versión secuencial

```powershell
g++ -std=c++17 -O2 -Wall -Wextra -fopenmp Busqueda_Exhaustiva_Secuencial.cpp -o Busqueda_Exhaustiva_Secuencial.exe
```

### Versión paralela

```powershell
g++ -std=c++17 -O2 -Wall -Wextra -fopenmp Busqueda_Exhaustiva_Paralela_OpenMP.cpp -o Busqueda_Exhaustiva_Paralela_OpenMP.exe
```

Si `g++` no está agregado al `PATH`, puede utilizarse directamente la ruta de MSYS2:

```powershell
& "C:\msys64\ucrt64\bin\g++.exe" -std=c++17 -O2 -Wall -Wextra -fopenmp Busqueda_Exhaustiva_Secuencial.cpp -o Busqueda_Exhaustiva_Secuencial.exe
```

```powershell
& "C:\msys64\ucrt64\bin\g++.exe" -std=c++17 -O2 -Wall -Wextra -fopenmp Busqueda_Exhaustiva_Paralela_OpenMP.cpp -o Busqueda_Exhaustiva_Paralela_OpenMP.exe
```

---

## Ejecución

### Versión secuencial

```powershell
.\Busqueda_Exhaustiva_Secuencial.exe
```

### Versión paralela

```powershell
.\Busqueda_Exhaustiva_Paralela_OpenMP.exe
```

El programa solicitará el tipo de ejecución, la longitud de la clave cuando corresponda, una clave de prueba válida y, en la versión paralela, el número de hilos.

Para comparar correctamente ambas versiones se debe utilizar la misma longitud, la misma clave, el mismo equipo y condiciones equivalentes de ejecución.

El rendimiento puede compararse con:

```text
Speedup = Tiempo secuencial / Tiempo paralelo
```

Un valor mayor que `1` indica que la versión paralela fue más rápida.

---

## Ejecuciones realizadas

Durante la práctica se realizaron pruebas con:

- **3 caracteres**, para observar claramente el funcionamiento del algoritmo.
- **7 caracteres**, para analizar el rendimiento sobre un espacio de búsqueda considerablemente mayor.

En espacios pequeños, el costo de crear y sincronizar hilos puede ser mayor que el beneficio del paralelismo. En espacios grandes, la distribución del trabajo permite aprovechar mejor varios núcleos del procesador.

---

## Nota académica

Este repositorio fue desarrollado exclusivamente para la materia de **Programación Paralela** con el propósito de demostrar conceptos de búsqueda exhaustiva, programación secuencial, programación paralela, distribución de trabajo, hilos, sincronización y OpenMP.
