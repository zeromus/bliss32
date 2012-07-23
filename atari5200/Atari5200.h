
#ifndef ATARI_5200_H
#define ATARI_5200_H

#include "6502c.h"
#include "Antic.h"
#include "GTIA.h"
#include "../core/Emulator.h"
#include "../core/MemoryAlias.h"
#include "../core/MemoryBus.h"
#include "../core/RAM.h"
#include "../core/ROM.h"
#include "../core/SignalLine.h"

class Atari5200 : public Emulator
{

    public:
        Atari5200();
        Atari5200(UINT8* biosImage);
        void reset();
        void setBIOSImage(UINT8* biosImage);
        void insertCartridge(ROM8Bit*);
        void removeCartridge();

    private:
        void init();

        ROM8Bit*        currentCart;

        MemoryBus8Bit memoryBus;

        SignalLine  readyLine;
        SignalLine  haltLine;
        SignalLine  nmiLine;
        _6502c      cpu;
        Antic       antic;
        GTIA        gtia;

        RAM8Bit         ram;
        MemoryAlias8Bit anticRegisterAliases[32];
        MemoryAlias8Bit gtiaRegisterAliases[8];
        RAM8Bit         pokeyRegisters;
        ROM8Bit         biosROM;

};

#endif

