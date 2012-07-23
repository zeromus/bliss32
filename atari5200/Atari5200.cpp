
#include "Atari5200.h"

Atari5200::Atari5200()
{
    currentCart = NULL;
    init();
}

Atari5200::Atari5200(UINT8* biosImage)
{
    currentCart = NULL;
    init();
    setBIOSImage(biosImage);
}

void Atari5200::setBIOSImage(UINT8* biosImage)
{
    biosROM.loadImage(biosImage);
}

void Atari5200::insertCartridge(ROM8Bit* rom)
{
    if (currentCart)
        removeCartridge();

    currentCart = rom;
    if (currentCart)
        memoryBus.addMemory(rom);
}

void Atari5200::removeCartridge()
{
    if (!currentCart)
        return;

    memoryBus.removeMemory(currentCart);
    currentCart = NULL;
}

void Atari5200::init()
{
    //add the main CPU
    cpu.init(&memoryBus, &readyLine, &haltLine, &nmiLine);
    processorBus.addProcessor(&cpu);

    //add the Antic
    antic.init(&memoryBus, &gtia, &readyLine, &haltLine, &nmiLine, &cpu);
    processorBus.addProcessor(&antic);

    //init the memory bus
    memoryBus.init(0x10000);

    //add the 16K of 8-bit RAM
    ram.init(0x4000, 0x0000);
    memoryBus.addMemory(&ram);

    //add the GTIA register aliases
    Memory8Bit* gtiaRegisters = gtia.getRegisters();
    UINT16 alias;
    for (alias = 0xC000; alias < 0xC100; alias += 0x0020) {
        UINT8 nextAlias = (alias & 0x00E0) >> 5;
        gtiaRegisterAliases[nextAlias].init(gtiaRegisters, alias);
        memoryBus.addMemory(&gtiaRegisterAliases[nextAlias]);
    }

    //add the Antic register aliases
    Memory8Bit* anticRegisters = antic.getRegisters();
    for (alias = 0xD400; alias < 0xD600; alias += 0x0010) {
        UINT8 nextAlias = (alias & 0x01F0) >> 4;
        anticRegisterAliases[nextAlias].init(anticRegisters, alias);
        memoryBus.addMemory(&anticRegisterAliases[nextAlias]);
    }

    //add the *fake* (for now) Pokey registers
    pokeyRegisters.init(0x0100, 0xE800);
    memoryBus.addMemory(&pokeyRegisters);
    
    //add the BIOS ROM
    biosROM.init(0x0800, 0xF800);
    memoryBus.addMemory(&biosROM);
}

void Atari5200::reset()
{
    haltLine.isHigh = FALSE;
    readyLine.isHigh = TRUE;
    nmiLine.isHigh = FALSE;
    Emulator::reset();
}

