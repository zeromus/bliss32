
#ifndef MEMORYBUS_H
#define MEMORYBUS_H

#include <iostream>
#include "Memory.h"

using namespace std;

/**
 * Emulates a memory bus which may be composed of memories of the same
 * size or narrower.
 *
 * @author Kyle Davis
 */
template <typename W>
class MemoryBus : public MemoryTemplate<W>
{

    public:
        MemoryBus();
        MemoryBus(UINT32 size);
        ~MemoryBus();
        void init(UINT32 size);
        void reset();
        void setUnmappedPeek(W);
        void addMemory(MemoryTemplate<W>* m);
        void removeMemory(MemoryTemplate<W>* m);
        UINT32 getLocation();
        W peek(UINT32 location);
        void poke(UINT32 location, W value);
        UINT32 getSize();

    private:
        MemoryTemplate<W>*     mappedMemories[50];
        INT32          mappedMemoryCount;
        MemoryTemplate<W>**    memory;
        UINT32         size;
        W              unmappedPeek;

};

template <typename W>
MemoryBus<W>::MemoryBus()
    : memory(NULL),
      mappedMemoryCount(0),
      unmappedPeek(0)
{
}

template <typename W>
MemoryBus<W>::MemoryBus(UINT32 size)
    : memory(NULL),
      mappedMemoryCount(0),
      unmappedPeek(0)
{
   init(size);
}

template <typename W>
void MemoryBus<W>::init(UINT32 size)
{
    this->size = size;
    memory = new MemoryTemplate<W>*[size];
    for (UINT32 i = 0; i < size; i++)
        memory[i] = NULL;
}

template <typename W>
MemoryBus<W>::~MemoryBus()
{
    if (memory)
        delete[] memory;
}

template <typename W>
void MemoryBus<W>::setUnmappedPeek(W p)
{
    this->unmappedPeek = p;
}

template <typename W>
void MemoryBus<W>::reset()
{
    for (INT32 i = 0; i < mappedMemoryCount; i++)
        mappedMemories[i]->reset();
}

template <typename W>
void MemoryBus<W>::addMemory(MemoryTemplate<W>* m)
{
    mappedMemories[mappedMemoryCount] = m;
    mappedMemoryCount++;

    UINT32 location = m->getLocation();
    UINT32 end = location + m->getSize();
    for (UINT32 i = location; i < end; i++)
        memory[i] = m;
}

template <typename W>
void MemoryBus<W>::removeMemory(MemoryTemplate<W>* m)
{
    for (INT32 i = 0; i < mappedMemoryCount; i++) {
        if (mappedMemories[i] == m) {
            INT32 j;
            for (j = i; j < (mappedMemoryCount-1); j++)
                mappedMemories[j] = mappedMemories[j+1];
            mappedMemoryCount--;

            UINT32 location = m->getLocation();
            UINT32 end = location + m->getSize();
            for (j = location; j < end; j++)
                memory[j] = NULL;
        }
    }

}

template <typename W>
UINT32 MemoryBus<W>::getLocation()
{
    return 0;
}

template <typename W>
W MemoryBus<W>::peek(UINT32 location)
{
    if (memory[location] == NULL)
        return unmappedPeek;

    return memory[location]->peek(location);
}

template <typename W>
void MemoryBus<W>::poke(UINT32 location, W value)
{
/*
if (location >= size)
cout << "memory poke out of bounds: " << location << "\n";
if (location >= 0x3000 && location <= 0x4927)
cout << "poked background: " << location << "\n";
*/
    if (memory[location] == NULL)
        return;

    memory[location]->poke(location, value);
}

template <typename W>
UINT32 MemoryBus<W>::getSize()
{
    return size;
}

typedef MemoryBus<UINT16> MemoryBus16Bit;
typedef MemoryBus<UINT8> MemoryBus8Bit;

#endif
