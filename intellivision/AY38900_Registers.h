
#ifndef AY38900_REGISTERS_H
#define AY38900_REGISTERS_H

#include "../core/Memory.h"

class AY38900;

class AY38900_Registers : Memory16Bit
{
    friend class AY38900;

    public:
        void reset();
        UINT32 getSize();
        UINT32 getLocation();
        void poke(UINT32 location, UINT16 value);
        UINT16 peek(UINT32 location);

    private:
        AY38900_Registers();
        void init(AY38900* ay38900);

        AY38900* ay38900;
        UINT16   memory[0x40];

};

#endif
