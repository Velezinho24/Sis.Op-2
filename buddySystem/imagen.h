#ifndef IMAGEN_H
#define IMAGEN_H

#include <string>
#include "buddy_allocator.h"

class Imagen {
public:
    Imagen(const std::string &nombreArchivo, BuddyAllocator *allocador = nullptr);
    ~Imagen();

    void mostrarInfo() const;
    void guardarImagen(const std::string &nombreArchivo) const;
    void rotar(float anguloGrados);
    void escalar(float factor);

private:
    // Orden correcto (primero los que no dependen de otros)
    BuddyAllocator* allocador;  // Primero porque se recibe en constructor
    int anchoOriginal, altoOriginal;     // Luego estos
    int ancho, alto, canales;   // Finalmente estos
    unsigned char*** pixeles;

    void convertirBufferAMatriz(unsigned char* buffer);
};

#endif // IMAGEN_H