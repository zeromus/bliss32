
#include "MicroSequencer.h"
#include "MicroSequencer_Registers.h"

MicroSequencer_Registers::MicroSequencer_Registers() {}

MicroSequencer_Registers::MicroSequencer_Registers(MicroSequencer* ms) {
    init(ms);
}

void MicroSequencer_Registers::init(MicroSequencer* ms) {
    this->ms = ms;
}

UINT32 MicroSequencer_Registers::getSize() {
    return 2;
}

UINT32 MicroSequencer_Registers::getLocation() {
    return 0x0080;
}

void MicroSequencer_Registers::reset() {}

void MicroSequencer_Registers::poke(UINT32 location, UINT16 value) {
    switch(location) {
        case 0x0080:
            if (ms->lrqHigh) {
                ms->command = value & 0xFF;
                ms->lrqHigh = FALSE;
                if (!ms->speaking)
                    ms->isIdle = FALSE;
            }
            break;
        case 0x0081:
            if ((value & 0x0400) != 0)
                ms->reset();
            else if (ms->fifoSize < MicroSequencer::FIFO_MAX_SIZE) {
                ms->fifoBytes[(ms->fifoHead+ms->fifoSize) & 0x3F] = value;
                ms->fifoSize++;
            }
            break;
    }
}

UINT16 MicroSequencer_Registers::peek(UINT32 location) {
    switch(location) {
        case 0x0080:
            return (ms->lrqHigh ? 0x8000 : 0);
        case 0x0081:
        default:
            return (ms->fifoSize == MicroSequencer::FIFO_MAX_SIZE ? 0x8000 : 0);
    }
}

/*
void MicroSequencer_Registers::save(SaveOutputStream sos) {}

void MicroSequencer_Registers::load(LoadInputStream lis) {}
*/
