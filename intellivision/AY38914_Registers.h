
#ifndef AY38914_REGISTERS_H
#define AY38914_REGISTERS_H

#include "../core/RAM.h"

class AY38914;

class AY38914_Registers : public RAM16Bit
{

    friend class AY38914;

    private:
        AY38914_Registers();
        void init(AY38914* ay38914, UINT32 location);
        void poke(UINT32 location, UINT16 value);
        UINT16 peek(UINT32 location);

        AY38914* ay38914;

};

#endif

