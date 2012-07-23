
#ifndef POKEY_REGISTERS_H
#define POKEY_REGISTERS_H

#include "../core/Memory.h"

class Pokey;

class Pokey_Registers : Memory8Bit
{

    public:
        Pokey_Registers();
        Pokey_Registers(Pokey* p);
        void init(Pokey* p);
        void reset();
        UINT32 getLocation();
        UINT32 getSize();
        void poke(UINT32 addr, UINT8 value);
        UINT8 peek(UINT32 addr);

    private:
        Pokey* pokey;
};

#endif
