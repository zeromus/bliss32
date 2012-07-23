
#ifndef MEMORY_H
#define MEMORY_H

#include "osd/types.h"

class Memory
{
    public:
        virtual void reset() = 0;
        virtual UINT32 getLocation() = 0;
        virtual UINT32 getSize() = 0;
};

template <typename W> class MemoryTemplate : public Memory
{

    public:
        virtual W peek(UINT32 location) = 0;
        virtual void poke(UINT32 location, W value) = 0;

};

typedef MemoryTemplate<UINT16> Memory16Bit;

typedef MemoryTemplate<UINT8> Memory8Bit;

#endif
