
#ifndef ECS_H
#define ECS_H

#include "BankSwitchedROM.h"
#include "ECSKeyboard.h"
#include "../AY38914.h"
#include "../../core/osd/types.h"
#include "../../core/AudioOutputLine.h"
#include "../../core/RAM.h"

class Intellivision;

class ECS
{

    friend class Intellivision;

    public:
        ECS();
        ECS(AudioOutputLine* aol);
        ~ECS();
        void init(AudioOutputLine* aol);

    private:
        void init();
        void setECSROMImage(UINT16* ecsImage);
        BOOL hasECSROMImage();
        void setPSGClockDivisor(INT32 divisor);

        AudioOutputLine*    aol;
        ECSKeyboard         keyboard;
        BOOL                hasECSROM;

    public:
        BankSwitchedROM     bank0;
        BankSwitchedROM     bank1;
        BankSwitchedROM     bank2;
        RAM16Bit            ramBank;
        RAM16Bit            uart;
        AY38914             psg2;

};

#endif
