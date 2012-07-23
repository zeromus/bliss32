
#include <iostream>
#include "Pokey_Registers.h"
#include "Pokey.h"

using namespace std;

Pokey_Registers::Pokey_Registers()
    : pokey(NULL)
{}

Pokey_Registers::Pokey_Registers(Pokey* p)
    : pokey(p)
{}

void Pokey_Registers::init(Pokey* p)
{
    pokey = p;
}

void Pokey_Registers::reset()
{}

UINT32 Pokey_Registers::getLocation()
{
    return 0;
}

UINT32 Pokey_Registers::getSize()
{
    return 0x20;
}

void Pokey_Registers::poke(UINT32 addr, UINT8 value)
{
    switch (addr) {
        case 0x0:  //AUDF1
            break;
        case 0x1:  //AUDC1
            break;
        case 0x2:  //AUDF2
            break;
        case 0x3:  //AUDC2
            break;
        case 0x4:  //AUDF3
            break;
        case 0x5:  //AUDC3
            break;
        case 0x6:  //AUDF4
            break;
        case 0x7:  //AUDC4
            break;
        case 0x8:  //AUDCTL
            break;
        case 0x9:  //STIMER
            break;
        case 0xA:  //SKRES
            break;
        case 0xB:  //POTGO
            break;
        case 0xD:  //SEROUT
            break;
        case 0xE:  //IRQEN
            break;
        case 0xF:  //SKCTLS
            break;
    }
}

UINT8 Pokey_Registers::peek(UINT32 addr)
{
    switch (addr) {
        case 0x0:  //POT0
            break;
        case 0x1:  //POT1
            break;
        case 0x2:  //POT2
            break;
        case 0x3:  //POT3
            break;
        case 0x4:  //POT4
            break;
        case 0x5:  //POT5
            break;
        case 0x6:  //POT6
            break;
        case 0x7:  //POT7
            break;
        case 0x8:  //ALLPOT
            break;
        case 0x9:  //KBCODE
            break;
        case 0xA:  //RANDOM
            break;
        case 0xD:  //SERIN
            break;
        case 0xE:  //IRQST
            break;
        case 0xF:  //SKSTAT
            break;
    }
    //unimplemented or write-only registers
    return 0xFF;
}

