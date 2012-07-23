
#ifndef ANTIC_H
#define ANTIC_H

#include "Antic_Registers.h"
#include "GTIA.h"
#include "6502c.h"
#include "../core/MemoryBus.h"
#include "../core/Processor.h"
#include "../core/SignalLine.h"
#include "../core/Palette.h"
#include "../core/osd/VideoOutputDevice.h"

class Antic : public Processor
{
    friend class Antic_Registers;

    public:
        Antic();
        Antic(MemoryBus8Bit* memoryBus, GTIA* gtia, SignalLine* rdyLine,
                SignalLine* haltLine, SignalLine* nmiLine, _6502c* cpu);
        ~Antic();
        void init(MemoryBus8Bit* memoryBus, GTIA* gtia, SignalLine* rdyLine,
                SignalLine* haltLine, SignalLine* nmiLine, _6502c* cpu);
        Antic_Registers* getRegisters();
        void reset();
        INT32 getClockSpeed();
        INT32 tick();

    private:
        void render_blank();
        void render_2();
        void render_3();
        void render_4();
        void render_5();
        void render_6();
        void render_7();
        void render_8();
        void render_9();
        void render_A();
        void render_B();
        void render_C();
        void render_D();
        void render_E();
        void render_F();

        VideoOutputDevice* vod;
        Antic_Registers registers;
        MemoryBus8Bit* memoryBus;
        GTIA* gtia;
        SignalLine* readyLine;
        SignalLine* haltLine;
        SignalLine* nmiLine;
        _6502c* cpu;

        UINT8 INST;
        UINT8 LCOUNT;
        UINT8 HCOUNT;
        UINT16 MEMSCAN;
        UINT8 MODE;
        UINT8 BYTEWIDTH;
        UINT8 BLOCKLENGTH;
        UINT8* imageBank;

        //registers
        UINT8  SHIFT[48];
        UINT8  DMACTL;
        UINT8  CHACTL;
        UINT16 DLIST;
        UINT8  HSCROL;
        UINT8  VSCROL;
        UINT8  PMBASE;
        UINT8  CHBASE;
        UINT16 VCOUNT;
        UINT8  NMIEN;
        UINT8  NMIST;

        const static UINT8 BLOCK_LENGTHS[14];
        const static UINT8 BYTE_WIDTHS[14][3];
        const static PALETTE palette;

};

#endif

