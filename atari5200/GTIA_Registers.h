
#ifndef GTIA_REGISTERS_H
#define GTIA_REGISTERS_H

#include "../core/Memory.h"

class GTIA_Registers : public Memory8Bit
{

    friend class GTIA;

    public:
        void reset();
        UINT32 getLocation();
        UINT32 getSize();
        void poke(UINT32 addr, UINT8 value);
        UINT8 peek(UINT32 addr);

    private:
        GTIA_Registers();
        GTIA_Registers(GTIA* gtia);
        void init(GTIA* gtia);

        GTIA* gtia;

};

#endif

