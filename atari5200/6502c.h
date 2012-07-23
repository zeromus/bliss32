
#ifndef _6502C_H_
#define _6502C_H_

#include "../core/Processor.h"
#include "../core/MemoryBus.h"
#include "../core/SignalLine.h"

class _6502c : public Processor
{

    public:
        _6502c();
        void init(MemoryBus8Bit* m, SignalLine* readyLine, SignalLine* haltLine,
                SignalLine* nmiLine);
        INT32 getClockSpeed();
        void reset();
        UINT16 getProgramCounter();
        void   getCurrentOpcode(CHAR op[30]);
        void   getRegisters(CHAR op[40]);
        INT32 tick();

    private:
        SignalLine* readyLine;
        SignalLine* haltLine;
        SignalLine* nmiLine;
        UINT32 resetVector;
        UINT32 irqVector;
        UINT32 nmiVector;

        //status bits
        BOOL   N, V, B, D, I, Z, C;

        //registers
        UINT8  AC, XR, YR, SP;

        //program counter
        UINT16 PC;

        //reset address
        UINT32 RESET;

        //the 64K 8-bit memory bus
        MemoryBus8Bit* memoryBus;

};

void itoa16(char* c, int i, int digits);

#endif
