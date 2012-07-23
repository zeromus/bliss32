
#include <stdio.h>
#include "AY38914.h"
#include "AY38914_Registers.h"

AY38914_Registers::AY38914_Registers() {}

void AY38914_Registers::init(AY38914* ay38914, UINT32 location)
{
    this->ay38914 = ay38914;
    RAM16Bit::init(0x0010, location, 8);
}

void AY38914_Registers::poke(UINT32 location, UINT16 value)
{
    switch(location-this->location) {
        case 0x00:
            value = value & 0x00FF;
            ay38914->channel0.period = (ay38914->channel0.period & 0x0F00) |
                    value;
            ay38914->channel0.periodValue = (ay38914->channel0.period
                    ? ay38914->channel0.period : 0x1000);
            break;

        case 0x01:
            value = value & 0x00FF;
            ay38914->channel1.period = (ay38914->channel1.period & 0x0F00) |
                    value;
            ay38914->channel1.periodValue = (ay38914->channel1.period
                    ? ay38914->channel1.period : 0x1000);
            break;

        case 0x02:
            value = value & 0x00FF;
            ay38914->channel2.period = (ay38914->channel2.period & 0x0F00) |
                    value;
            ay38914->channel2.periodValue = (ay38914->channel2.period
                    ? ay38914->channel2.period : 0x1000);
            break;

        case 0x03:
            value = value & 0x00FF;
            ay38914->envelopePeriod = (ay38914->envelopePeriod & 0xFF00) |
                    value;
            ay38914->envelopePeriodValue = (ay38914->envelopePeriod
                    ? (ay38914->envelopePeriod << 1) : 0x20000);
            break;

        case 0x04:
            value = value & 0x000F;
            ay38914->channel0.period = (ay38914->channel0.period & 0x00FF) |
                    (value<<8);
            ay38914->channel0.periodValue = (ay38914->channel0.period
                    ? ay38914->channel0.period : 0x1000);
            break;

        case 0x05:
            value = value & 0x000F;
            ay38914->channel1.period = (ay38914->channel1.period & 0x00FF) |
                    (value<<8);
            ay38914->channel1.periodValue = (ay38914->channel1.period
                    ? ay38914->channel1.period : 0x1000);
            break;

        case 0x06:
            value = value & 0x000F;
            ay38914->channel2.period = (ay38914->channel2.period & 0x00FF) |
                    (value<<8);
            ay38914->channel2.periodValue = (ay38914->channel2.period
                    ? ay38914->channel2.period : 0x1000);
            break;

        case 0x07:
            value = value & 0x00FF;
            ay38914->envelopePeriod = (ay38914->envelopePeriod & 0x00FF) |
                    (value<<8);
            ay38914->envelopePeriodValue = (ay38914->envelopePeriod
                    ? (ay38914->envelopePeriod << 1) : 0x20000);
            break;

        case 0x08:
            value = value & 0x00FF;
            ay38914->channel0.toneDisabled = !!(value & 0x0001);
            ay38914->channel1.toneDisabled = !!(value & 0x0002);
            ay38914->channel2.toneDisabled = !!(value & 0x0004);
            ay38914->channel0.noiseDisabled = !!(value & 0x0008);
            ay38914->channel1.noiseDisabled = !!(value & 0x0010);
            ay38914->channel2.noiseDisabled = !!(value & 0x0020);
            ay38914->channel0.isDirty = TRUE;
            ay38914->channel1.isDirty = TRUE;
            ay38914->channel2.isDirty = TRUE;
            ay38914->noiseIdle = ay38914->channel0.noiseDisabled &
                    ay38914->channel1.noiseDisabled &
                    ay38914->channel2.noiseDisabled;
            break;

        case 0x09:
            value = value & 0x001F;
            ay38914->noisePeriod = value;
            ay38914->noisePeriodValue = (ay38914->noisePeriod
                    ? (ay38914->noisePeriod << 1) : 0x0040);
            break;

        case 0x0A:
            value = value & 0x000F;
            ay38914->envelopeHold = !!(value & 0x0001);
            ay38914->envelopeAltr = !!(value & 0x0002);
            ay38914->envelopeAtak = !!(value & 0x0004);
            ay38914->envelopeCont = !!(value & 0x0008);
            ay38914->envelopeVolume = (ay38914->envelopeAtak ? 0 : 15);
            ay38914->envelopeCounter = ay38914->envelopePeriodValue;
            ay38914->envelopeIdle = FALSE;
            break;

        case 0x0B:
            value = value & 0x003F;
            ay38914->channel0.envelope = !!(value & 0x0010);
            ay38914->channel0.volume = (value & 0x000F);
            ay38914->channel0.isDirty = TRUE;
            break;

        case 0x0C:
            value = value & 0x003F;
            ay38914->channel1.envelope = !!(value & 0x0010);
            ay38914->channel1.volume = (value & 0x000F);
            ay38914->channel1.isDirty = TRUE;
            break;

        case 0x0D:
            value = value & 0x003F;
            ay38914->channel2.envelope = !!(value & 0x0010);
            ay38914->channel2.volume = (value & 0x000F);
            ay38914->channel2.isDirty = TRUE;
            break;

        case 0x0E:
            ay38914->psgIO1->setOutputValue(value);
            break;

        case 0x0F:
            ay38914->psgIO0->setOutputValue(value);
            break;
    }
    RAM16Bit::poke(location, value);
}

UINT16 AY38914_Registers::peek(UINT32 location)
{
    switch(location-this->location) {
        case 0x0E:
            return ay38914->psgIO1->getInputValue();
        case 0x0F:
            return ay38914->psgIO0->getInputValue();
        default:
            return RAM16Bit::peek(location);
    }
}

