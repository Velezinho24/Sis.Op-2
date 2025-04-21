#ifndef BUDDY_ALLOCATOR_H
#define BUDDY_ALLOCATOR_H

#include <cstdlib>

class BuddyAllocator {
public:
    BuddyAllocator(size_t size);
    ~BuddyAllocator();

    void* alloc(size_t size);
    void free(void* ptr);

private:
    void* memoriaBase;
    size_t size;
};

#endif 