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
    cout << "  -escalar <ancho> <alto>  Redimensionar (ej: -escalar 800 600)\n";
}

void procesarArgumentos(int argc, char* argv[], bool& usarBuddy, float& angulo, bool& rotar, bool& escalar, float& factorEscala) {
    for (int i = 3; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-buddy") {
            usarBuddy = true;
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

    size_t memInicio = getMemoryUsage();
    auto inicio = high_resolution_clock::now();

    if (usarBuddy) {
        BuddyAllocator allocador(4 * 1024 * 1024);
        Imagen img(archivoEntrada, &allocador);

        if (rotar) img.rotar(angulo);
        if (escalar) img.escalar(factorEscala);
        
        img.mostrarInfo();
        if (rotar) cout << "Angulo de rotación: " << angulo << " grados" << endl;
        if (escalar) cout << "Factor de escalado: " << factorEscala << endl;
        cout << "-------------------------------" << endl;
        if (rotar) cout << "[INFO] Imagen rotada correctamente.\n";
        if (escalar) cout << "[INFO] Imagen escalada correctamente.\n";
        cout << "-------------------------------" << endl;

        size_t memBuddy = getMemoryUsage() - memInicio;
        auto tiempoBuddy = duration_cast<milliseconds>(high_resolution_clock::now() - inicio).count();

        // Benchmark tradicional (silencioso)
        size_t memInicioNormal = getMemoryUsage();
        auto inicioNormal = high_resolution_clock::now();
        
        Imagen imgNormal(archivoEntrada);
        if (rotar) imgNormal.rotar(angulo);
        if (escalar) imgNormal.escalar(factorEscala);
        
        size_t memNormal = getMemoryUsage() - memInicioNormal;
        auto tiempoNormal = duration_cast<milliseconds>(high_resolution_clock::now() - inicioNormal).count();

        // Tus mensajes originales SIN MODIFICAR
        cout << "TIEMPO DE PROCESAMIENTO:" << endl;
        cout << " - Sin Buddy System: " << tiempoNormal << " ms" << endl;
        cout << " - Con Buddy System: " << tiempoBuddy << " ms" << endl;
        cout << "" << endl;
        cout << "MEMORIA UTILIZADA:" << endl;
        cout << fixed << setprecision(2);
        cout << " - Sin Buddy System: " << memNormal/1024.0 << " MB" << endl;
        cout << " - Con buddy System: " << memBuddy/1024.0 << " MB" << endl;
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
        
        // Mensajes originales SIN MODIFICAR
        cout << "TIEMPO DE PROCESAMIENTO:" << endl;
        cout << " - Sin Buddy System: " << duration_cast<milliseconds>(high_resolution_clock::now() - inicio).count() << " ms" << endl;
        cout << "" << endl;
        cout << "MEMORIA UTILIZADA:" << endl;
        cout << fixed << setprecision(2);
        cout << " - Sin Buddy System: " << (getMemoryUsage() - memInicio)/1024.0 << " MB" << endl;
        cout << "-------------------------------" << endl;
        
        img.guardarImagen(archivoSalida);
    }
    
    return 0;
}