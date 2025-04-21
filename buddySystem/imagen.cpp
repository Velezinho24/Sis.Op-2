#include "imagen.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;

Imagen::Imagen(const std::string &nombreArchivo, BuddyAllocator *allocador)
    : allocador(allocador),
      anchoOriginal(0),
      altoOriginal(0),
      ancho(0),
      alto(0),
      canales(0),
      pixeles(nullptr) {
    
    unsigned char* buffer = stbi_load(nombreArchivo.c_str(), &ancho, &alto, &canales, 0);
    if (!buffer) {
        cerr << "Error: No se pudo cargar la imagen '" << nombreArchivo << "'.\n";
        exit(1);
    }

    anchoOriginal = ancho;  
    altoOriginal = alto;

    convertirBufferAMatriz(buffer);
    stbi_image_free(buffer);
}

Imagen::~Imagen() {
    if (!allocador) {
        for (int y = 0; y < alto; y++) {
            for (int x = 0; x < ancho; x++) {
                delete[] pixeles[y][x];
            }
            delete[] pixeles[y];
        }
        delete[] pixeles;
    }
}

void Imagen::convertirBufferAMatriz(unsigned char* buffer) {
    int indice = 0;
    pixeles = new unsigned char**[alto];

    for (int y = 0; y < alto; y++) {
        pixeles[y] = new unsigned char*[ancho];
        for (int x = 0; x < ancho; x++) {
            pixeles[y][x] = new unsigned char[canales];
            for (int c = 0; c < canales; c++) {
                pixeles[y][x][c] = buffer[indice++];
            }
        }
    }
}

void Imagen::guardarImagen(const std::string &nombreArchivo) const {
    unsigned char* buffer = new unsigned char[alto * ancho * canales];
    int indice = 0;

    for (int y = 0; y < alto; y++) {
        for (int x = 0; x < ancho; x++) {
            for (int c = 0; c < canales; c++) {
                buffer[indice++] = pixeles[y][x][c];
            }
        }
    }

    if (!stbi_write_png(nombreArchivo.c_str(), ancho, alto, canales, buffer, ancho * canales)) {
        cerr << "Error: No se pudo guardar la imagen en '" << nombreArchivo << "'.\n";
        delete[] buffer;
        exit(1);
    }

    delete[] buffer;
}


void Imagen::rotar(float anguloGrados) {
    float radianes = anguloGrados * M_PI / 180.0f;
    float coseno = cos(radianes);
    float seno = sin(radianes);

    // Calculamos las nuevas dimensiones
    int newAncho = abs(ancho * coseno) + abs(alto * seno);
    int newAlto = abs(ancho * seno) + abs(alto * coseno);

    unsigned char*** nuevaMatriz = new unsigned char**[newAlto];
    for (int y = 0; y < newAlto; y++) {
        nuevaMatriz[y] = new unsigned char*[newAncho];
        for (int x = 0; x < newAncho; x++) {
            nuevaMatriz[y][x] = new unsigned char[canales];
            int origX = (x - newAncho / 2) * coseno - (y - newAlto / 2) * seno + ancho / 2;
            int origY = (x - newAncho / 2) * seno + (y - newAlto / 2) * coseno + alto / 2;
            
            if (origX >= 0 && origX < ancho && origY >= 0 && origY < alto) {
                for (int c = 0; c < canales; c++) {
                    nuevaMatriz[y][x][c] = pixeles[origY][origX][c];
                }
            } else {
                for (int c = 0; c < canales; c++) {
                    nuevaMatriz[y][x][c] = (c == 3) ? 0 : 0;
                }
            }
        }
    }

    // Liberar memoria antigua
    for (int y = 0; y < alto; y++) {
        for (int x = 0; x < ancho; x++) {
            delete[] pixeles[y][x];
        }
        delete[] pixeles[y];
    }
    delete[] pixeles;

    // Actualizar dimensiones actuales
    ancho = newAncho;
    alto = newAlto;
    pixeles = nuevaMatriz;
}

void Imagen::escalar(float factor) {
    if (factor <= 0) {
        cerr << "Error: Factor de escalado debe ser mayor que 0.\n";
        return;
    }

    // Calculamos las nuevas dimensiones
    int newAncho = static_cast<int>(ancho * factor);
    int newAlto = static_cast<int>(alto * factor);

    // Crear nueva matriz
    unsigned char*** nuevaMatriz = new unsigned char**[newAlto];
    for (int y = 0; y < newAlto; y++) {
        nuevaMatriz[y] = new unsigned char*[newAncho];
        for (int x = 0; x < newAncho; x++) {
            nuevaMatriz[y][x] = new unsigned char[canales];
            float origX = (x + 0.5f) / factor - 0.5f;
            float origY = (y + 0.5f) / factor - 0.5f;

            int x0 = static_cast<int>(origX);
            int y0 = static_cast<int>(origY);
            int x1 = min(x0 + 1, ancho - 1);
            int y1 = min(y0 + 1, alto - 1);

            float dx = origX - x0;
            float dy = origY - y0;

            for (int c = 0; c < canales; c++) {
                float val = 
                    pixeles[y0][x0][c] * (1 - dx) * (1 - dy) +
                    pixeles[y0][x1][c] * dx * (1 - dy) +
                    pixeles[y1][x0][c] * (1 - dx) * dy +
                    pixeles[y1][x1][c] * dx * dy;

                nuevaMatriz[y][x][c] = static_cast<unsigned char>(val);
            }
        }
    }

    // Liberar memoria antigua
    for (int y = 0; y < alto; y++) {
        for (int x = 0; x < ancho; x++) {
            delete[] pixeles[y][x];
        }
        delete[] pixeles[y];
    }
    delete[] pixeles;

    // Actualizar dimensiones actuales
    ancho = newAncho;
    alto = newAlto;
    pixeles = nuevaMatriz;
}

void Imagen::mostrarInfo() const {
    cout << "Dimensiones originales: " << anchoOriginal << " x " << altoOriginal << endl;
    cout << "Dimensiones finales: " << ancho << " x " << alto << endl;
    cout << "Canales: " << canales << " (RGB) " << endl;
}