
#ifndef MEMORYALIAS_H
#define MEMORYALIAS_H

#include <iostream>
#include "Memory.h"

using namespace std;

template <typename W>
class MemoryAlias : public MemoryTemplate<W>
{
    public:
        MemoryAlias();
        void init(MemoryTemplate<W>* m, UINT32 location);
        void reset();
        void setVisible(BOOL visible);
        BOOL isVisible();
        UINT32 getLocation();
        W peek(UINT32 location);
        void poke(UINT32 location, W value);
        UINT32 getSize();

    private:
        MemoryTemplate<W>* aliasedMemory;
        UINT32             location;
        UINT32             offset;
        BOOL               visible;
    
};

template <typename W>
MemoryAlias<W>::MemoryAlias()
{}

template <typename W>
void MemoryAlias<W>::init(MemoryTemplate<W>* m, UINT32 location)
{
    this->aliasedMemory = m;
    this->location = location;
    this->offset = location - m->getLocation();
}

template <typename W>
void MemoryAlias<W>::reset()
{
    visible = TRUE;
}

template <typename W>
void MemoryAlias<W>::setVisible(BOOL visible)
{
    this->visible = visible;
}

template <typename W>
BOOL MemoryAlias<W>::isVisible()
{
    return visible;
}

template <typename W>
UINT32 MemoryAlias<W>::getLocation()
{
    return location;
}

template <typename W>
W MemoryAlias<W>::peek(UINT32 location)
{
    return (visible ? aliasedMemory->peek(location-offset) : location);
}

template <typename W>
void MemoryAlias<W>::poke(UINT32 location, W value)
{
    aliasedMemory->poke(location-offset, value);
}

template <typename W>
UINT32 MemoryAlias<W>::getSize() {
    return aliasedMemory->getSize();
}

typedef MemoryAlias<UINT16> MemoryAlias16Bit;
typedef MemoryAlias<UINT8> MemoryAlias8Bit;

#endif

