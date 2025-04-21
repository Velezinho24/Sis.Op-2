#include "imagen.h"
#include "buddy_allocator.h"
#include <iostream>
#include <chrono>
#include <string>
#include <cstring>
#include <sys/resource.h>
#include <iomanip>

using namespace std;
using namespace std::chrono;

void mostrarUso() {
    cout << "Uso: ./main <archivo_entrada> <archivo_salida> [opciones]\n";
    cout << "Opciones:\n";
    cout << "  -buddy              Usar Buddy Allocator\n";
    cout << "  -angulo <grados>    Rotar la imagen (ej: -angulo 45)\n";
    cout << "  -escalar <factor>   Escalar imagen (ej: -escalar 0.5)\n";
}

void procesarArgumentos(int argc, char* argv[], bool& usarBuddy, float& angulo, bool& rotar, bool& escalar, float& factorEscala) {
    for (int i = 3; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-buddy") {
            usarBuddy = true;
        } else if (arg == "-angulo" && i + 1 < argc) {
            angulo = stof(argv[++i]);
            rotar = true;
        } else if (arg == "-escalar" && i + 1 < argc) {
            factorEscala = stof(argv[++i]);
            escalar = true;
        }
    }
}

size_t getMemoryUsage() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss; // KB
}

int main(int argc, char* argv[]) {
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

    procesarArgumentos(argc, argv, usarBuddy, angulo, rotar, escalar, factorEscala);

    cout << "=== PROCESAMIENTO DE IMAGEN ===" << endl;
    cout << "Archivo de entrada: " << archivoEntrada << endl;
    cout << "Archivo de salida: " << archivoSalida << endl;
    cout << "Modo de asignación de memoria: " << (usarBuddy ? "Buddy System" : "new/delete") << endl;
    cout << "-------------------------------" << endl;

    long tiempoNormal = 0;
    size_t memNormal = 0;

    // Benchmark SIN Buddy (Siempre se hace primero)
    {
        size_t memInicio = getMemoryUsage();
        auto inicio = high_resolution_clock::now();

        Imagen imgNormal(archivoEntrada);
        if (rotar) imgNormal.rotar(angulo);
        if (escalar) imgNormal.escalar(factorEscala);

        tiempoNormal = duration_cast<milliseconds>(high_resolution_clock::now() - inicio).count();
        memNormal = getMemoryUsage() - memInicio;
    }

    long tiempoBuddy = 0;
    size_t memBuddy = 0;

    if (usarBuddy) {
        size_t memInicio = getMemoryUsage();
        auto inicio = high_resolution_clock::now();

        BuddyAllocator allocador(4 * 1024 * 1024); // 4 MB
        Imagen img(archivoEntrada, &allocador);
        if (rotar) img.rotar(angulo);
        if (escalar) img.escalar(factorEscala);

        tiempoBuddy = duration_cast<milliseconds>(high_resolution_clock::now() - inicio).count();
        memBuddy = getMemoryUsage() - memInicio;

        img.mostrarInfo();
        if (rotar) cout << "Angulo de rotación: " << angulo << " grados" << endl;
        if (escalar) cout << "Factor de escalado: " << factorEscala << endl;
        cout << "-------------------------------" << endl;
        if (rotar) cout << "[INFO] Imagen rotada correctamente.\n";
        if (escalar) cout << "[INFO] Imagen escalada correctamente.\n";
        cout << "-------------------------------" << endl;

        img.guardarImagen(archivoSalida);
    } else {
        Imagen img(archivoEntrada);
        if (rotar) img.rotar(angulo);
        if (escalar) img.escalar(factorEscala);

        img.mostrarInfo();
        if (rotar) cout << "Angulo de rotación: " << angulo << " grados" << endl;
        if (escalar) cout << "Factor de escalado: " << factorEscala << endl;
        cout << "-------------------------------" << endl;
        if (rotar) cout << "[INFO] Imagen rotada correctamente.\n";
        if (escalar) cout << "[INFO] Imagen escalada correctamente.\n";
        cout << "-------------------------------" << endl;

        img.guardarImagen(archivoSalida);
    }

    // Mostrar comparación
    cout << "TIEMPO DE PROCESAMIENTO:" << endl;
    cout << " - Sin Buddy System: " << tiempoNormal << " ms" << endl;
    if (usarBuddy) cout << " - Con Buddy System: " << tiempoBuddy << " ms" << endl;
    cout << endl;

    cout << "MEMORIA UTILIZADA:" << endl;
    cout << fixed << setprecision(2);
    cout << " - Sin Buddy System: " << memNormal / 1024.0 << " MB" << endl;
    if (usarBuddy) cout << " - Con Buddy System: " << memBuddy / 1024.0 << " MB" << endl;
    cout << "-------------------------------" << endl;

    cout << "[INFO] Imagen guardada correctamente en '" << archivoSalida << "'." << endl;
    return 0;
}
