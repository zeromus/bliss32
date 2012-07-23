
#ifndef ANTIC_REGISTERS_H
#define ANTIC_REGISTERS_H

#include "../core/Memory.h"

class Antic_Registers : public Memory8Bit
{

    friend class Antic;

    public:
        void reset();
        UINT32 getLocation();
        UINT32 getSize();
        void poke(UINT32 addr, UINT8 value);
        UINT8 peek(UINT32 addr);

    private:
        Antic_Registers();
        Antic_Registers(Antic* antic);
        void init(Antic* antic);

        Antic* antic;

};

#endif

