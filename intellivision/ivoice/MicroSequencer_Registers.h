
#ifndef MICROSEQUENCER_REGISTERS_H
#define MICROSEQUENCER_REGISTERS_H

#include "../../core/osd/types.h"
#include "../../core/Memory.h"

class MicroSequencer;

class MicroSequencer_Registers : public Memory16Bit
{

    friend class MicroSequencer;

    public:
        UINT32 getSize();
        UINT32 getLocation();
        void reset();
        void poke(UINT32 location, UINT16 value);
        UINT16 peek(UINT32 location);

    private:
        MicroSequencer_Registers();
        MicroSequencer_Registers(MicroSequencer* ms);
        void init(MicroSequencer* ms);
        MicroSequencer*  ms;

};

#endif
