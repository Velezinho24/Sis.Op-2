#include "buddy_allocator.h"
#include <cstdlib>
#include <iostream>

using namespace std;

BuddyAllocator::BuddyAllocator(size_t size) {
    this->size = size;
    memoriaBase = std::malloc(size);
    if (!memoriaBase) {
        cerr << "Error: No se pudo asignar memoria base con Buddy System.\n";
        exit(1);
    }
}

BuddyAllocator::~BuddyAllocator() {
    std::free(memoriaBase);
}

void* BuddyAllocator::alloc(size_t size) {
    if (size > this->size) {
        cerr << "Error: Tamaño solicitado (" << size 
             << " bytes) supera el tamaño disponible (" 
             << this->size << " bytes).\n";
        return nullptr;
    }
    return memoriaBase;
}

void BuddyAllocator::free(void* ptr) {
    // No liberamos porque el Buddy System maneja esto automáticamente.
}