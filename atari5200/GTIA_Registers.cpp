
#include <iostream>
#include "GTIA_Registers.h"
#include "GTIA.h"

using namespace std;

GTIA_Registers::GTIA_Registers()
    : gtia(NULL)
{}

GTIA_Registers::GTIA_Registers(GTIA* g)
    : gtia(g)
{}

void GTIA_Registers::init(GTIA* g)
{
    gtia = g;
}

void GTIA_Registers::reset()
{}

UINT32 GTIA_Registers::getLocation()
{
    return 0;
}

UINT32 GTIA_Registers::getSize()
{
    return 0x20;
}

void GTIA_Registers::poke(UINT32 addr, UINT8 value)
{
    switch (addr) {
        case 0x00:
            gtia->HPOSP0 = value;
            break;
        case 0x01:
            gtia->HPOSP1 = value;
            break;
        case 0x02:
            gtia->HPOSP2 = value;
            break;
        case 0x03:
            gtia->HPOSP3 = value;
            break;
        case 0x04:
            gtia->HPOSM0 = value;
            break;
        case 0x05:
            gtia->HPOSM1 = value;
            break;
        case 0x06:
            gtia->HPOSM2 = value;
            break;
        case 0x07:
            gtia->HPOSM3 = value;
            break;
        case 0x08:
            gtia->SIZEP0 = value;
            break;
        case 0x09:
            gtia->SIZEP1 = value;
            break;
        case 0x0A:
            gtia->SIZEP2 = value;
            break;
        case 0x0B:
            gtia->SIZEP3 = value;
            break;
        case 0x0C:
            gtia->SIZEM = value;
            break;
        case 0x0D:
            gtia->GRAFP0 = value;
            break;
        case 0x0E:
            gtia->GRAFP1 = value;
            break;
        case 0x0F:
            gtia->GRAFP2 = value;
            break;
        case 0x10:
            gtia->GRAFP3 = value;
            break;
        case 0x11:
            gtia->GRAFM = value;
            break;
        case 0x12:
            gtia->COLPM0 = value;
            break;
        case 0x13:
            gtia->COLPM1 = value;
            break;
        case 0x14:
            gtia->COLPM2 = value;
            break;
        case 0x15:
            gtia->COLPM3 = value;
            break;
        case 0x16:
            gtia->COLPF0 = value;
            break;
        case 0x17:
            gtia->COLPF1 = value;
            break;
        case 0x18:
            gtia->COLPF2 = value;
            break;
        case 0x19:
            gtia->COLPF3 = value;
            break;
        case 0x1A:
            gtia->COLBK = value;
            break;
        case 0x1B:
            gtia->PRIOR = value;
            break;
        case 0x1C:
            gtia->VDELAY = value;
            break;
        case 0x1D:
            gtia->GRACTL = value;
            break;
        case 0x1E:
            //TODO: clear the collision registers
            break;
        case 0x1F:
            gtia->CONSOL = value;
            break;
    }
}

UINT8 GTIA_Registers::peek(UINT32 addr)
{
    switch (addr) {
		case 0:				// Suppress 'default only' compiler warning
        default:
            return 0xFF;
    }
}

