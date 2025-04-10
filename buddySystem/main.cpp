#include "imagen.h"
#include "buddy_allocator.h"
#include <iostream>
#include <chrono>
#include <string>
#include <cstring>

using namespace std;
using namespace std::chrono;

// Muestra cómo usar el programa
void mostrarUso() {
    cout << "Uso: ./main <archivo_entrada> <archivo_salida> [opciones]\n";
    cout << "Opciones:\n";
    cout << "  -buddy              Usar Buddy Allocator\n";
    cout << "  -no-buddy           Usar new/delete (predeterminado)\n";
    cout << "  -angulo <grados>    Rotar la imagen (ej: -angulo 45)\n";
    cout << "  -escalar <ancho> <alto>  Redimensionar (ej: -escalar 800 600)\n";
}

// Procesa los argumentos de línea de comandos
void procesarArgumentos(int argc, char* argv[], bool& usarBuddy, float& angulo, bool& rotar, bool& escalar, float& factorEscala) {
    for (int i = 3; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-buddy") {
            usarBuddy = true;
        } else if (arg == "-no-buddy") {
            usarBuddy = false;
        } else if (arg == "-angulo" && i + 1 < argc) {
            angulo = stof(argv[i + 1]);
            rotar = true;
            i++;
        } else if (arg == "-escalar" && i + 1 < argc) {
            factorEscala = stof(argv[i + 1]);
            escalar = true;
            i++;
        }
    }
}

int main(int argc, char* argv[]) {
    // Verificar argumentos mínimos
    if (argc < 3) {
        mostrarUso();
        return 1;
    }

    string archivoEntrada = argv[1];
    string archivoSalida = argv[2];
    bool usarBuddy = false;
    float angulo = 0.0f;
    bool rotar = false;
    bool escalar = false;
    float factorEscala = 1.0f;

    // Procesar argumentos
    procesarArgumentos(argc, argv, usarBuddy, angulo, rotar, escalar, factorEscala);

    // Mostrar configuración
    cout << "=== PROCESAMIENTO DE IMAGEN ===" << endl;
    cout << "Archivo de entrada: " << archivoEntrada << endl;
    cout << "Archivo de salida: " << archivoSalida << endl;
    cout << "Modo de asignación de memoria: " << (usarBuddy ? "Buddy System" : "new/delete") << endl;
    cout << "---------------------" << endl;

    // Medir tiempo
    auto inicio = high_resolution_clock::now();

    // Procesar imagen
    if (usarBuddy) {
        BuddyAllocator allocador(32 * 1024 * 1024);  // 32 MB
        Imagen img(archivoEntrada, &allocador);
        

        if (rotar) {
            img.rotar(angulo);
        }
        if (escalar) {
            img.escalar(factorEscala);
        }
        img.mostrarInfo();
        if (rotar) {
            cout << "Angulo de rotación: " << angulo << " grados" << endl;
        }
        if (escalar) {
            cout << "Factor de escalado: " << factorEscala << endl;
        }
        cout << "-------------------------------" << endl;
        img.guardarImagen(archivoSalida);
    } else {
        Imagen img(archivoEntrada);

        if (rotar) {
            img.rotar(angulo);
        }
        if (escalar) {
            img.escalar(factorEscala);
        }
        img.mostrarInfo();
        if (rotar) {
            cout << "Angulo de rotación: " << angulo << " grados" << endl;
        }
        if (escalar) {
            cout << "Factor de escalado: " << factorEscala << endl;
        }
        cout << "-------------------------------" << endl;
        img.guardarImagen(archivoSalida);
    }

    // Mostrar tiempo
    auto fin = high_resolution_clock::now();
    auto duracion = duration_cast<milliseconds>(fin - inicio).count();
    cout << "\nTiempo total: " << duracion << " ms" << endl;

    return 0;
}