
#include <iostream>
#include "ECS.h"

ECS::ECS() {}

ECS::ECS(AudioOutputLine* aol)
{
    init(aol);
}

void ECS::init(AudioOutputLine* aol) {
    this->aol = aol;
    hasECSROM = FALSE;
    init();
}

ECS::~ECS() {
    //delete[] memorySegments;
}

void ECS::setECSROMImage(UINT16* ecsImage) {
    bank0.loadImage(ecsImage);
    bank1.loadImage(&ecsImage[0x1000]);
    bank2.loadImage(&ecsImage[0x2000]);
    hasECSROM = TRUE;
}

BOOL ECS::hasECSROMImage()  {
    return hasECSROM;
}

void ECS::setPSGClockDivisor(INT32 divisor) {
    psg2.setClockDivisor(divisor);
}

void ECS::init() {
    psg2.init(aol, 0x00F0, &keyboard, &keyboard);

    bank0.init(0x1000, 1, 0x2000);
    bank1.init(0x1000, 0, 0x7000);
    bank2.init(0x1000, 1, 0xE000);

    ramBank.init(0x0800, 0x4000, 8);

    uart.init(4, 0xE0, 8);
}
