# Buddy System

## Introducción

Es una herramienta de línea de comandos desarrollada en C++ que permite procesar imágenes aplicando dos transformaciones principales: **rotación** y **escalado**. Además, el programa permite comparar el uso de memoria entre el método convencional (`new/delete`) y una implementación personalizada del **Buddy System**, lo cual permite observar diferencias en rendimiento y consumo de recursos.

Está diseñado para sistemas operativos tipo Unix (Linux, WSL, etc.) y utiliza la biblioteca OpenCV para manipulación de imágenes.

## Ejecución

```sh
./programa_buddy <entrada.png> <salida.png> -escalar <factor> -angulo <grados> [-buddy]
```

Donde:

- `<entrada.png>` es la imagen de entrada.
- `<salida.png>` es el nombre del archivo de imagen de salida.
- `-escalar` Factor de escalado (ej: 0.5, 1.2, etc.).
- `-angulo` Ángulo de rotación en grados (ej: 45, 90, etc.).
- `buddy` (opcional) Si se incluye, activa la asignación con Buddy System. Si se omite, se usará new/delete.
  
### Ejemplo

```sh
./programa_buddy entrada.png salida.png -escalar 1.5 -angulo 45 -buddy
```

## Opciones disponibles

- `-angulo <valor>`: Rota la imagen respecto a su centro.
- `-escalar <valor>`: Escala la imagen manteniendo su proporción.
- `-buddy`: Activa el uso del sistema Buddy System para la gestión de memoria.

## Requisitos

- C++17 o superior
- OpenCV (preferiblemente versión 4.x)

### Instalación en Ubuntu

```sh
sudo apt update
sudo apt install build-essential libopencv-dev
```

## Compilación

```sh
make
```

Esto generará un ejecutable llamado `programa_buddy`.


### Arquitectura del programa

A continuación se describe la responsabilidad de cada componente clave:

### `main.cpp`

- Es el **punto de entrada** del programa.
- Se encarga de:
  - Parsear los argumentos de la línea de comandos.
  - Cargar la imagen de entrada.
  - Invocar las funciones para rotar y escalar.
  - Elegir el modo de asignación de memoria (`BuddySystem` o `new/delete`).
  - Medir tiempos de procesamiento y memoria.
  - Mostrar los resultados por consola.
  - Guardar la imagen de salida.

---

### `image_processor.cpp` / `image_processor.h`

- Contiene las funciones relacionadas con el **procesamiento de imágenes**, incluyendo:
  - Carga de la imagen usando OpenCV.
  - Rotación de imagen sobre su centro usando **interpolación bilineal**.
  - Escalado proporcional de la imagen.
- Utiliza estructuras como matrices tridimensionales `[alto][ancho][canales]`.

---

### `buddy_system.cpp` / `buddy_system.h`

- Implementa una versión simplificada del **algoritmo de asignación de memoria Buddy System**.
- Provee funciones para:
  - Reservar bloques de memoria del tamaño adecuado.
  - Dividir o fusionar bloques según se necesite.
  - Liberar memoria correctamente.
- Se utiliza cuando el usuario activa el modo `-buddy`.


## Autores

- Santiago Palacio García
- Santiago Vélez Blandón
- Erick Ochoa
- Silvia Katheryn Cáceres Vásquez
