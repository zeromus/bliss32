
#include "LPC12.h"
#include "MicroSequencer.h"

const INT32 MicroSequencer::bitMasks[16] = {
        0x0001, 0x0003, 0x0007, 0x000F, 0x001F, 0x003F, 0x007F, 0x00FF,
        0x01FF, 0x03FF, 0x07FF, 0x0FFF, 0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF };

const INT32 MicroSequencer::FIFO_LOCATION = 0x1800;
const INT32 MicroSequencer::FIFO_MAX_SIZE = 64;
const INT32 MicroSequencer::FIFO_END      = FIFO_LOCATION + FIFO_MAX_SIZE;

MicroSequencer::MicroSequencer() {}

MicroSequencer::MicroSequencer(LPC12* lpc12) {
    init(lpc12);
}

void MicroSequencer::init(LPC12* lpc12) {
    this->lpc12 = lpc12;
    lpc12->microSequencer = this;
    registers.init(this);
    ivoiceROM.init(0x800, 0x1000);
}

INT32 MicroSequencer::getClockSpeed() {
    return 1600000;
}

void MicroSequencer::reset() {
    currentBits = 0;
    bitsLeft = 0;
    isIdle = TRUE;

    pc = 0;
    page = 1;
    stack = 0;
    mode = 0;
    repeatPrefix = 0;
    command = 0;
    lrqHigh = TRUE;
    fifoHead = 0;
    fifoSize = 0;
    speaking = FALSE;
}

INT32 MicroSequencer::tick() {
    if (!speaking) {
        speaking = TRUE;
        pc = 0x1000 | (command << 1);
        bitsLeft = 0;
        command = 0;
        lrqHigh = TRUE;
    }

    while (!isIdle) {
        INT32 repeatBefore = repeatPrefix;
        decode();
        if (repeatBefore != 0)
            repeatPrefix = 0;
    }

    return 1;
}

INT8 MicroSequencer::readDelta(INT32 numBits) {
    INT32 value = readBits(numBits);
    if ((value & (1 << (numBits - 1))) != 0)
        value |= -1 << numBits;
    return (INT8)value;
}

INT32 MicroSequencer::readBits(INT32 numBits) {
    return readBits(numBits, FALSE);
}

INT32 MicroSequencer::readBits(INT32 numBits, BOOL reverseOrder) {
    while (bitsLeft < numBits) {
        if (pc < 0x1800) {
            currentBits |= (ivoiceROM.peek(pc) << bitsLeft);
            bitsLeft += 8;
            pc = (pc+1) & 0xFFFF;
        }
        else if (pc == 0x1800 && fifoSize > 0) {
            currentBits |= (fifoBytes[fifoHead] << bitsLeft);
            fifoHead = (fifoHead+1) & 0x3F;
            fifoSize--;
            bitsLeft += 10;
        }
        else {
            currentBits |= (0x03FF << bitsLeft);
            bitsLeft += 10;
            pc = (pc+1) & 0xFFFF;
        }

    }

    INT32 output = currentBits & bitMasks[numBits-1];
    if (reverseOrder)
        output = flipEndian(output, numBits);
    currentBits = currentBits >> numBits;
    bitsLeft -= numBits;
    return output;
}

void MicroSequencer::RTS() {
    if (stack == 0) {
        if (!lrqHigh) {
            pc = 0x1000 | (command << 1);
            bitsLeft = 0;
            command = 0;
            lrqHigh = TRUE;
        }
        else {
            speaking = FALSE;
            isIdle = TRUE;
        }
    }
    else {
        pc = stack;
        stack = 0;
        bitsLeft = 0;
    }
}

void MicroSequencer::SETPAGE(INT32 immed4) {
    this->page = flipEndian(immed4, 4);
}

void MicroSequencer::LOADALL(INT32 immed4) {
    lpc12->repeat = (repeatPrefix << 4) | immed4;
    if (lpc12->repeat == 0)
        return;

    lpc12->amplitude = readBits(8);
    lpc12->period = readBits(8);
    //lpc12->periodCounter = (lpc12->period == 0 ? 0x100 : lpc12->period);
    lpc12->b[0] = (INT8)readBits(8);
    lpc12->f[0] = (INT8)readBits(8);
    lpc12->b[1] = (INT8)readBits(8);
    lpc12->f[1] = (INT8)readBits(8);
    lpc12->b[2] = (INT8)readBits(8);
    lpc12->f[2] = (INT8)readBits(8);
    lpc12->b[3] = (INT8)readBits(8);
    lpc12->f[3] = (INT8)readBits(8);
    lpc12->b[4] = (INT8)readBits(8);
    lpc12->f[4] = (INT8)readBits(8);
    lpc12->b[5] = (INT8)readBits(8);
    lpc12->f[5] = (INT8)readBits(8);
    if ((mode & 0x01) == 0) {
        lpc12->amplitudeInterpolation = 0;
        lpc12->periodInterpolation = 0;
    }
    else {
        lpc12->amplitudeInterpolation = (INT8)readBits(8);
        lpc12->periodInterpolation = (INT8)readBits(8);
    }

    lpc12->isIdle = FALSE;
    isIdle = TRUE;
}

void MicroSequencer::LOAD_2(INT32 immed4) {
    lpc12->repeat = (repeatPrefix << 4) | immed4;
    if (lpc12->repeat == 0)
        return;

    lpc12->amplitude = (readBits(6) << 2) | (lpc12->amplitude & 0x03);
    lpc12->period = readBits(8);
    //lpc12->periodCounter = (lpc12->period == 0 ? 0x100 : lpc12->period);
    switch (mode) {
        case 0x0:
            lpc12->b[0] = (INT8)((readBits(3) << 4) | (lpc12->b[0] & 0x0F));
            lpc12->f[0] = (INT8)((readBits(5) << 3) | (lpc12->f[0] & 0x07));
            lpc12->b[1] = (INT8)((readBits(3) << 4) | (lpc12->b[1] & 0x0F));
            lpc12->f[1] = (INT8)((readBits(5) << 3) | (lpc12->f[1] & 0x07));
            lpc12->b[2] = (INT8)((readBits(3) << 4) | (lpc12->b[2] & 0x0F));
            lpc12->f[2] = (INT8)((readBits(5) << 3) | (lpc12->f[2] & 0x07));
            lpc12->b[3] = (INT8)((readBits(4) << 3) | (lpc12->b[3] & 0x07));
            lpc12->f[3] = (INT8)((readBits(6) << 2) | (lpc12->f[3] & 0x03));
            lpc12->b[4] = (INT8)((readBits(7) << 1) | (lpc12->b[4] & 0x01));
            lpc12->f[4] = (INT8)((readBits(6) << 2) | (lpc12->f[4] & 0x03));
            lpc12->b[5] = 0;
            lpc12->f[5] = 0;
            break;
        case 0x1:
            lpc12->b[0] = (INT8)((readBits(3) << 4) | (lpc12->b[0] & 0x0F));
            lpc12->f[0] = (INT8)((readBits(5) << 3) | (lpc12->f[0] & 0x07));
            lpc12->b[1] = (INT8)((readBits(3) << 4) | (lpc12->b[1] & 0x0F));
            lpc12->f[1] = (INT8)((readBits(5) << 3) | (lpc12->f[1] & 0x07));
            lpc12->b[2] = (INT8)((readBits(3) << 4) | (lpc12->b[2] & 0x0F));
            lpc12->f[2] = (INT8)((readBits(5) << 3) | (lpc12->f[2] & 0x07));
            lpc12->b[3] = (INT8)((readBits(4) << 3) | (lpc12->b[3] & 0x07));
            lpc12->f[3] = (INT8)((readBits(6) << 2) | (lpc12->f[3] & 0x03));
            lpc12->b[4] = (INT8)((readBits(7) << 1) | (lpc12->b[4] & 0x01));
            lpc12->f[4] = (INT8)((readBits(6) << 2) | (lpc12->f[4] & 0x03));
            lpc12->b[5] = (INT8)readBits(8);
            lpc12->f[5] = (INT8)readBits(8);
            break;
        case 0x2:
            lpc12->b[0] = (INT8)((readBits(6) << 1) | (lpc12->b[0] & 0x01));
            lpc12->f[0] = (INT8)((readBits(6) << 2) | (lpc12->f[0] & 0x03));
            lpc12->b[1] = (INT8)((readBits(6) << 1) | (lpc12->b[1] & 0x01));
            lpc12->f[1] = (INT8)((readBits(6) << 2) | (lpc12->f[1] & 0x03));
            lpc12->b[2] = (INT8)((readBits(6) << 1) | (lpc12->b[2] & 0x01));
            lpc12->f[2] = (INT8)((readBits(6) << 2) | (lpc12->f[2] & 0x03));
            lpc12->b[3] = (INT8)((readBits(6) << 1) | (lpc12->b[3] & 0x01));
            lpc12->f[3] = (INT8)((readBits(7) << 1) | (lpc12->f[3] & 0x01));
            lpc12->b[4] = (INT8)readBits(8);
            lpc12->f[4] = (INT8)readBits(8);
            lpc12->b[5] = 0;
            lpc12->f[5] = 0;
            break;
        case 0x3:
            lpc12->b[0] = (INT8)((readBits(6) << 1) | (lpc12->b[0] & 0x01));
            lpc12->f[0] = (INT8)((readBits(6) << 2) | (lpc12->f[0] & 0x03));
            lpc12->b[1] = (INT8)((readBits(6) << 1) | (lpc12->b[1] & 0x01));
            lpc12->f[1] = (INT8)((readBits(6) << 2) | (lpc12->f[1] & 0x03));
            lpc12->b[2] = (INT8)((readBits(6) << 1) | (lpc12->b[2] & 0x01));
            lpc12->f[2] = (INT8)((readBits(6) << 2) | (lpc12->f[2] & 0x03));
            lpc12->b[3] = (INT8)((readBits(6) << 1) | (lpc12->b[3] & 0x01));
            lpc12->f[3] = (INT8)((readBits(7) << 1) | (lpc12->f[3] & 0x01));
            lpc12->b[4] = (INT8)readBits(8);
            lpc12->f[4] = (INT8)readBits(8);
            lpc12->b[5] = (INT8)readBits(8);
            lpc12->f[5] = (INT8)readBits(8);
            break;
    }

    lpc12->amplitudeInterpolation = (INT8)
            ((lpc12->amplitudeInterpolation & 0xE0) | (readBits(5)));
    lpc12->periodInterpolation = (INT8)
            ((lpc12->periodInterpolation & 0xE0) | (readBits(5)));

    lpc12->isIdle = FALSE;
    isIdle = TRUE;
}

void MicroSequencer::SETMSB_3(INT32 immed4) {
    lpc12->repeat = (repeatPrefix << 4) | immed4;
    if (lpc12->repeat == 0)
        return;

    lpc12->amplitude = (readBits(6) << 2) | (lpc12->amplitude & 0x03);
    if (mode == 0x00 || mode == 0x02) {
        lpc12->b[5] = 0;
        lpc12->f[5] = 0;
    }

    switch (mode) {
        case 0x0:
        case 0x1:
            lpc12->f[0] = (INT8)((readBits(5) << 3) | (lpc12->f[0] & 0x07));
            lpc12->f[1] = (INT8)((readBits(5) << 3) | (lpc12->f[1] & 0x07));
            lpc12->f[2] = (INT8)((readBits(5) << 3) | (lpc12->f[2] & 0x07));
            break;
        case 0x2:
        case 0x3:
            lpc12->f[0] = (INT8)((readBits(6) << 2) | (lpc12->f[0] & 0x03));
            lpc12->f[1] = (INT8)((readBits(6) << 2) | (lpc12->f[1] & 0x03));
            lpc12->f[2] = (INT8)((readBits(6) << 2) | (lpc12->f[2] & 0x03));
            break;
    }

    lpc12->amplitudeInterpolation = (INT8)
            ((lpc12->amplitudeInterpolation & 0xE0) | (readBits(5)));
    lpc12->periodInterpolation = (INT8)
            ((lpc12->periodInterpolation & 0xE0) | (readBits(5)));

    lpc12->isIdle = FALSE;
    isIdle = TRUE;
}

void MicroSequencer::LOAD_4(INT32 immed4) {
    lpc12->repeat = (repeatPrefix << 4) | immed4;
    if (lpc12->repeat == 0)
        return;

    lpc12->amplitude = (readBits(6) << 2) | (lpc12->amplitude & 0x03);
    lpc12->period = readBits(8);
    //lpc12->periodCounter = (lpc12->period == 0 ? 0x100 : lpc12->period);
    lpc12->b[0] = 0;
    lpc12->f[0] = 0;
    lpc12->b[1] = 0;
    lpc12->f[1] = 0;
    lpc12->b[2] = 0;
    lpc12->f[2] = 0;
    switch (mode) {
        case 0x0:
            lpc12->b[3] = (INT8)((readBits(4) << 3) | (lpc12->b[3] & 0x07));
            lpc12->f[3] = (INT8)((readBits(6) << 2) | (lpc12->f[3] & 0x03));
            lpc12->b[4] = (INT8)((readBits(7) << 1) | (lpc12->b[4] & 0x01));
            lpc12->f[4] = (INT8)((readBits(6) << 2) | (lpc12->f[4] & 0x03));
            lpc12->b[5] = 0;
            lpc12->f[5] = 0;
            break;
        case 0x1:
            lpc12->b[3] = (INT8)((readBits(4) << 3) | (lpc12->b[3] & 0x07));
            lpc12->f[3] = (INT8)((readBits(6) << 2) | (lpc12->f[3] & 0x03));
            lpc12->b[4] = (INT8)((readBits(7) << 1) | (lpc12->b[4] & 0x01));
            lpc12->f[4] = (INT8)((readBits(6) << 2) | (lpc12->f[4] & 0x03));
            lpc12->b[5] = (INT8)readBits(8);
            lpc12->f[5] = (INT8)readBits(8);
            break;
        case 0x2:
            lpc12->b[3] = (INT8)((readBits(6) << 1) | (lpc12->b[3] & 0x01));
            lpc12->f[3] = (INT8)((readBits(7) << 1) | (lpc12->f[3] & 0x01));
            lpc12->b[4] = (INT8)readBits(8);
            lpc12->f[4] = (INT8)readBits(8);
            lpc12->b[5] = 0;
            lpc12->f[5] = 0;
            break;
        case 0x3:
            lpc12->b[3] = (INT8)((readBits(6) << 1) | (lpc12->b[3] & 0x01));
            lpc12->f[3] = (INT8)((readBits(7) << 1) | (lpc12->f[3] & 0x01));
            lpc12->b[4] = (INT8)readBits(8);
            lpc12->f[4] = (INT8)readBits(8);
            lpc12->b[5] = (INT8)readBits(8);
            lpc12->f[5] = (INT8)readBits(8);
            break;
    }

    lpc12->amplitudeInterpolation = 0;
    lpc12->periodInterpolation = 0;

    lpc12->isIdle = FALSE;
    isIdle = TRUE;
}

void MicroSequencer::SETMSB_5(INT32 immed4) {
    lpc12->repeat = (repeatPrefix << 4) | immed4;
    if (lpc12->repeat == 0)
        return;

    lpc12->amplitude = (readBits(6) << 2) | (lpc12->amplitude & 0x03);
    lpc12->period = readBits(8);
    //lpc12->periodCounter = (lpc12->period == 0 ? 0x100 : lpc12->period);
    if (mode == 0x00 || mode == 0x02) {
        lpc12->b[5] = 0;
        lpc12->f[5] = 0;
    }

    switch (mode) {
        case 0x0:
        case 0x1:
            lpc12->f[0] = (INT8)((readBits(5) << 3) | (lpc12->f[0] & 0x07));
            lpc12->f[1] = (INT8)((readBits(5) << 3) | (lpc12->f[1] & 0x07));
            lpc12->f[2] = (INT8)((readBits(5) << 3) | (lpc12->f[2] & 0x07));
            break;
        case 0x2:
        case 0x3:
            lpc12->f[0] = (INT8)((readBits(6) << 2) | (lpc12->f[0] & 0x03));
            lpc12->f[1] = (INT8)((readBits(6) << 2) | (lpc12->f[1] & 0x03));
            lpc12->f[2] = (INT8)((readBits(6) << 2) | (lpc12->f[2] & 0x03));
            break;
    }

    lpc12->isIdle = FALSE;
    isIdle = TRUE;
}

void MicroSequencer::SETMSB_6(INT32 immed4) {
    lpc12->repeat = (repeatPrefix << 4) | immed4;
    if (lpc12->repeat == 0)
        return;

    lpc12->amplitude = (readBits(6) << 2) | (lpc12->amplitude & 0x03);
    if (mode == 0x00 || mode == 0x02) {
        lpc12->b[5] = 0;
        lpc12->f[5] = 0;
    }

    switch (mode) {
        case 0x0:
            lpc12->f[3] = (INT8)((readBits(6) << 2) | (lpc12->f[3] & 0x03));
            lpc12->f[4] = (INT8)((readBits(6) << 2) | (lpc12->f[4] & 0x03));
            lpc12->b[5] = 0;
            lpc12->f[5] = 0;
            break;
        case 0x1:
            lpc12->f[3] = (INT8)((readBits(6) << 2) | (lpc12->f[3] & 0x03));
            lpc12->f[4] = (INT8)((readBits(6) << 2) | (lpc12->f[4] & 0x03));
            lpc12->f[5] = (INT8)readBits(8);
            break;
        case 0x2:
            lpc12->f[3] = (INT8)((readBits(7) << 1) | (lpc12->f[3] & 0x01));
            lpc12->f[4] = (INT8)readBits(8);
            lpc12->b[5] = 0;
            lpc12->f[5] = 0;
            break;
        case 0x3:
            lpc12->f[3] = (INT8)((readBits(7) << 1) | (lpc12->f[3] & 0x01));
            lpc12->f[4] = (INT8)readBits(8);
            lpc12->f[5] = (INT8)readBits(8);
            break;
    }

    lpc12->isIdle = FALSE;
    isIdle = TRUE;
}

void MicroSequencer::JMP(INT32 immed4) {
    pc = (page << 12) | (flipEndian(immed4, 4) << 8) | readBits(8, TRUE);
    bitsLeft = 0;
}

void MicroSequencer::SETMODE(INT32 immed4) {
    immed4 = flipEndian(immed4, 4);
    mode = immed4 & 0x3;
    repeatPrefix = (immed4 & 0xC) >> 2;
}

void MicroSequencer::DELTA_9(INT32 immed4) {
    lpc12->repeat = (repeatPrefix << 4) | immed4;
    if (lpc12->repeat == 0)
        return;

    lpc12->amplitude = (lpc12->amplitude + 0x10000 + (readDelta(4) << 2))
            & 0xFFFF;
    lpc12->period = (lpc12->period + 0x10000 + readDelta(5)) & 0xFFFF;
    //lpc12->periodCounter = (lpc12->period == 0 ? 0x100 : lpc12->period);
    switch (mode) {
        case 0x0:
            lpc12->b[0] += readDelta(3) << 4;
            lpc12->f[0] += readDelta(3) << 3;
            lpc12->b[1] += readDelta(3) << 4;
            lpc12->f[1] += readDelta(3) << 3;
            lpc12->b[2] += readDelta(3) << 4;
            lpc12->f[2] += readDelta(3) << 3;
            lpc12->b[3] += readDelta(3) << 3;
            lpc12->f[3] += readDelta(4) << 2;
            lpc12->b[4] += readDelta(4) << 1;
            lpc12->f[4] += readDelta(4) << 2;
            break;
        case 0x1:
            lpc12->b[0] += readDelta(3) << 4;
            lpc12->f[0] += readDelta(3) << 3;
            lpc12->b[1] += readDelta(3) << 4;
            lpc12->f[1] += readDelta(3) << 3;
            lpc12->b[2] += readDelta(3) << 4;
            lpc12->f[2] += readDelta(3) << 3;
            lpc12->b[3] += readDelta(3) << 3;
            lpc12->f[3] += readDelta(4) << 2;
            lpc12->b[4] += readDelta(4) << 1;
            lpc12->f[4] += readDelta(4) << 2;
            lpc12->b[5] += readDelta(5);
            lpc12->f[5] += readDelta(5);
            break;
        case 0x2:
            lpc12->b[0] += readDelta(4) << 2;
            lpc12->f[0] += readDelta(4);
            lpc12->b[1] += readDelta(4) << 1;
            lpc12->f[1] += readDelta(4) << 2;
            lpc12->b[2] += readDelta(4) << 1;
            lpc12->f[2] += readDelta(4) << 2;
            lpc12->b[3] += readDelta(4) << 1;
            lpc12->f[3] += readDelta(5) << 2;
            lpc12->b[4] += readDelta(5) << 1;
            lpc12->f[4] += readDelta(5) << 1;
            break;
        case 0x3:
            lpc12->b[0] += readDelta(4) << 2;
            lpc12->f[0] += readDelta(4);
            lpc12->b[1] += readDelta(4) << 1;
            lpc12->f[1] += readDelta(4) << 2;
            lpc12->b[2] += readDelta(4) << 1;
            lpc12->f[2] += readDelta(4) << 2;
            lpc12->b[3] += readDelta(4) << 1;
            lpc12->f[3] += readDelta(5) << 2;
            lpc12->b[4] += readDelta(5) << 1;
            lpc12->f[4] += readDelta(5) << 1;
            lpc12->b[5] += readDelta(5);
            lpc12->f[5] += readDelta(5);
            break;
    }

    lpc12->isIdle = FALSE;
    isIdle = TRUE;
}

void MicroSequencer::SETMSB_A(INT32 immed4) {
    lpc12->repeat = (repeatPrefix << 4) | immed4;
    if (lpc12->repeat == 0)
        return;

    lpc12->amplitude = (readBits(6) << 2) | (lpc12->amplitude & 0x03);
    switch (mode) {
        case 0x0:
        case 0x1:
            lpc12->f[0] = (INT8)((readBits(5) << 3) | (lpc12->f[0] & 0x07));
            lpc12->f[1] = (INT8)((readBits(5) << 3) | (lpc12->f[1] & 0x07));
            lpc12->f[2] = (INT8)((readBits(5) << 3) | (lpc12->f[2] & 0x07));
            break;
        case 0x2:
        case 0x3:
            lpc12->f[0] = (INT8)((readBits(6) << 2) | (lpc12->f[0] & 0x03));
            lpc12->f[1] = (INT8)((readBits(6) << 2) | (lpc12->f[1] & 0x03));
            lpc12->f[2] = (INT8)((readBits(6) << 2) | (lpc12->f[2] & 0x03));
            break;
    }

    lpc12->isIdle = FALSE;
    isIdle = TRUE;
}

void MicroSequencer::JSR(INT32 immed4) {
    INT32 newpc = (page << 12) | (flipEndian(immed4, 4) << 8) | readBits(8, TRUE);
    stack = pc;
    pc = newpc;
    bitsLeft = 0;
}

void MicroSequencer::LOAD_C(INT32 immed4) {
    lpc12->repeat = (repeatPrefix << 4) | immed4;
    if (lpc12->repeat == 0)
        return;

    lpc12->amplitude = (readBits(6) << 2) | (lpc12->amplitude & 0x03);
    lpc12->period = readBits(8);
    //lpc12->periodCounter = (lpc12->period == 0 ? 0x100 : lpc12->period);
    switch (mode) {
        case 0x0:
            lpc12->b[0] = (INT8)((readBits(3) << 4) | (lpc12->b[0] & 0x0F));
            lpc12->f[0] = (INT8)((readBits(5) << 3) | (lpc12->f[0] & 0x07));
            lpc12->b[1] = (INT8)((readBits(3) << 4) | (lpc12->b[1] & 0x0F));
            lpc12->f[1] = (INT8)((readBits(5) << 3) | (lpc12->f[1] & 0x07));
            lpc12->b[2] = (INT8)((readBits(3) << 4) | (lpc12->b[2] & 0x0F));
            lpc12->f[2] = (INT8)((readBits(5) << 3) | (lpc12->f[2] & 0x07));
            lpc12->b[3] = (INT8)((readBits(4) << 3) | (lpc12->b[3] & 0x07));
            lpc12->f[3] = (INT8)((readBits(6) << 2) | (lpc12->f[3] & 0x03));
            lpc12->b[4] = (INT8)((readBits(7) << 1) | (lpc12->b[4] & 0x01));
            lpc12->f[4] = (INT8)((readBits(6) << 2) | (lpc12->f[4] & 0x03));
            lpc12->b[5] = 0;
            lpc12->f[5] = 0;
            break;
        case 0x1:
            lpc12->b[0] = (INT8)((readBits(3) << 4) | (lpc12->b[0] & 0x0F));
            lpc12->f[0] = (INT8)((readBits(5) << 3) | (lpc12->f[0] & 0x07));
            lpc12->b[1] = (INT8)((readBits(3) << 4) | (lpc12->b[1] & 0x0F));
            lpc12->f[1] = (INT8)((readBits(5) << 3) | (lpc12->f[1] & 0x07));
            lpc12->b[2] = (INT8)((readBits(3) << 4) | (lpc12->b[2] & 0x0F));
            lpc12->f[2] = (INT8)((readBits(5) << 3) | (lpc12->f[2] & 0x07));
            lpc12->b[3] = (INT8)((readBits(4) << 3) | (lpc12->b[3] & 0x07));
            lpc12->f[3] = (INT8)((readBits(6) << 2) | (lpc12->f[3] & 0x03));
            lpc12->b[4] = (INT8)((readBits(7) << 1) | (lpc12->b[4] & 0x01));
            lpc12->f[4] = (INT8)((readBits(6) << 2) | (lpc12->f[4] & 0x03));
            lpc12->b[5] = (INT8)readBits(8);
            lpc12->f[5] = (INT8)readBits(8);
            break;
        case 0x2:
            lpc12->b[0] = (INT8)((readBits(6) << 1) | (lpc12->b[0] & 0x01));
            lpc12->f[0] = (INT8)((readBits(6) << 2) | (lpc12->f[0] & 0x03));
            lpc12->b[1] = (INT8)((readBits(6) << 1) | (lpc12->b[1] & 0x01));
            lpc12->f[1] = (INT8)((readBits(6) << 2) | (lpc12->f[1] & 0x03));
            lpc12->b[2] = (INT8)((readBits(6) << 1) | (lpc12->b[2] & 0x01));
            lpc12->f[2] = (INT8)((readBits(6) << 2) | (lpc12->f[2] & 0x03));
            lpc12->b[3] = (INT8)((readBits(6) << 1) | (lpc12->b[3] & 0x01));
            lpc12->f[3] = (INT8)((readBits(7) << 1) | (lpc12->f[3] & 0x01));
            lpc12->b[4] = (INT8)readBits(8);
            lpc12->f[4] = (INT8)readBits(8);
            lpc12->b[5] = 0;
            lpc12->f[5] = 0;
            break;
        case 0x3:
            lpc12->b[0] = (INT8)((readBits(6) << 1) | (lpc12->b[0] & 0x01));
            lpc12->f[0] = (INT8)((readBits(6) << 2) | (lpc12->f[0] & 0x03));
            lpc12->b[1] = (INT8)((readBits(6) << 1) | (lpc12->b[1] & 0x01));
            lpc12->f[1] = (INT8)((readBits(6) << 2) | (lpc12->f[1] & 0x03));
            lpc12->b[2] = (INT8)((readBits(6) << 1) | (lpc12->b[2] & 0x01));
            lpc12->f[2] = (INT8)((readBits(6) << 2) | (lpc12->f[2] & 0x03));
            lpc12->b[3] = (INT8)((readBits(6) << 1) | (lpc12->b[3] & 0x01));
            lpc12->f[3] = (INT8)((readBits(7) << 1) | (lpc12->f[3] & 0x01));
            lpc12->b[4] = (INT8)readBits(8);
            lpc12->f[4] = (INT8)readBits(8);
            lpc12->b[5] = (INT8)readBits(8);
            lpc12->f[5] = (INT8)readBits(8);
            break;
    }

    lpc12->amplitudeInterpolation = 0;
    lpc12->periodInterpolation = 0;

    lpc12->isIdle = FALSE;
    isIdle = TRUE;
}

void MicroSequencer::DELTA_D(INT32 immed4) {
    lpc12->repeat = (repeatPrefix << 4) | immed4;
    if (lpc12->repeat == 0)
        return;

    lpc12->amplitude = (lpc12->amplitude + 0x10000 + (readDelta(4) << 2))
            & 0xFFFF;
    lpc12->period = (lpc12->period + 0x10000 + readDelta(5)) & 0xFFFF;
    //lpc12->periodCounter = (lpc12->period == 0 ? 0x100 : lpc12->period);
    switch (mode) {
        case 0x0:
            lpc12->b[3] += readDelta(3) << 3;
            lpc12->f[3] += readDelta(4) << 2;
            lpc12->b[4] += readDelta(4) << 1;
            lpc12->f[4] += readDelta(4) << 2;
            break;
        case 0x1:
            lpc12->b[3] += readDelta(3) << 3;
            lpc12->f[3] += readDelta(4) << 2;
            lpc12->b[4] += readDelta(4) << 1;
            lpc12->f[4] += readDelta(4) << 2;
            lpc12->b[5] += readDelta(5);
            lpc12->f[5] += readDelta(5);
            break;
        case 0x2:
            lpc12->b[3] += readDelta(4) << 1;
            lpc12->f[3] += readDelta(5) << 1;
            lpc12->b[4] += readDelta(5);
            lpc12->f[4] += readDelta(5);
            break;
        case 0x3:
            lpc12->b[3] += readDelta(4) << 1;
            lpc12->f[3] += readDelta(5) << 1;
            lpc12->b[4] += readDelta(5);
            lpc12->f[4] += readDelta(5);
            lpc12->b[5] += readDelta(5);
            lpc12->f[5] += readDelta(5);
            break;
    }

    lpc12->isIdle = FALSE;
    isIdle = TRUE;
}

void MicroSequencer::LOAD_E(INT32 immed4) {
    lpc12->repeat = (repeatPrefix << 4) | immed4;
    if (lpc12->repeat == 0)
        return;

    lpc12->amplitude = (readBits(6) << 2) | (lpc12->amplitude & 0x03);
    lpc12->period = readBits(8);
    //lpc12->periodCounter = (lpc12->period == 0 ? 0x100 : lpc12->period);
    
    lpc12->isIdle = FALSE;
    isIdle = TRUE;
}

void MicroSequencer::PAUSE(INT32 immed4) {
    lpc12->repeat = (repeatPrefix << 4) | immed4;
    if (lpc12->repeat == 0)
        return;

    //clear everything
    lpc12->amplitude = 0;
    lpc12->period = 0;
    for (INT32 j = 0; j < 6; j++) {
        lpc12->b[j] = 0;
        lpc12->f[j] = 0;
    }
    lpc12->amplitudeInterpolation = 0;
    lpc12->periodInterpolation = 0;

    lpc12->isIdle = FALSE;
    isIdle = TRUE;
}

void MicroSequencer::decode() {
    INT32 immed4 = readBits(4);
    INT32 nextInstruction = readBits(4, TRUE);
    switch (nextInstruction) {
        case 0x0:
            if (immed4 == 0)
                RTS();
            else
                SETPAGE(immed4);
            break;
        case 0x8:
            SETMODE(immed4);
            break;
        case 0x4:
            LOAD_4(immed4);
            break;
        case 0xC:
            LOAD_C(immed4);
            break;
        case 0x2:
            LOAD_2(immed4);
            break;
        case 0xA:
            SETMSB_A(immed4);
            break;
        case 0x6:
            SETMSB_6(immed4);
            break;
        case 0xE:
            LOAD_E(immed4);
            break;
        case 0x1:
            LOADALL(immed4);
            break;
        case 0x9:
            DELTA_9(immed4);
            break;
        case 0x5:
            SETMSB_5(immed4);
            break;
        case 0xD:
            DELTA_D(immed4);
            break;
        case 0x3:
            SETMSB_3(immed4);
            break;
        case 0xB:
            JSR(immed4);
            break;
        case 0x7:
            JMP(immed4);
            break;
        case 0xF:
            PAUSE(immed4);
            break;

/*
        case 0x0:
            if (immed4 == 0)
                RTS();
            else
                SETPAGE(immed4);
            break;
        case 0x1:
            SETMODE(immed4);
            break;
        case 0x2:
            LOAD_4(immed4);
            break;
        case 0x3:
            LOAD_C(immed4);
            break;
        case 0x4:
            LOAD_2(immed4);
            break;
        case 0x5:
            SETMSB_A(immed4);
            break;
        case 0x6:
            SETMSB_6(immed4);
            break;
        case 0x7:
            LOAD_E(immed4);
            break;
        case 0x8:
            LOADALL(immed4);
            break;
        case 0x9:
            DELTA_9(immed4);
            break;
        case 0xA:
            SETMSB_5(immed4);
            break;
        case 0xB:
            DELTA_D(immed4);
            break;
        case 0xC:
            SETMSB_3(immed4);
            break;
        case 0xD:
            JSR(immed4);
            break;
        case 0xE:
            JMP(immed4);
            break;
        case 0xF:
            PAUSE(immed4);
            break;
*/
    }
}

INT32 MicroSequencer::flipEndian(INT32 value, INT32 bits) {
    INT32 output = 0;
    INT32 bitMask = 1;
    for (INT32 i = 0; i < bits; i++) {
        INT32 offset = (bits-1)-(i<<1);
        if (offset > 0)
            output |= (value & bitMask) << offset;
        else
            output |= (value & bitMask) >> -offset;
        bitMask = bitMask << 1;
    }
    return output;
}

/*
void MicroSequencer::save(SaveOutputStream sos) throws IOException {
    sos.writeInt(bitsLeft);
    sos.writeInt(currentBits);
    sos.writeInt(pc);
    sos.writeInt(mode);
    sos.writeInt(repeatPrefix);
    sos.writeInt(page);
    sos.writeInt(command);

    sos.writeBoolean(lrqHigh);
    sos.writeBoolean(speaking);
    sos.writeInt(fifoHead);
    sos.writeInt(fifoSize);
    for (INT32 i = 0; i < fifoBytes.length; i++)
        sos.writeInt(fifoBytes[i]);
}

void MicroSequencer::load(LoadInputStream lis) throws IOException {
    bitsLeft = lis.readInt(0, 31);
    currentBits = lis.readInt(0, Integer.MAX_VALUE);
    pc = lis.readInt(0, 0xFFFF);
    mode = lis.readInt(0, 3);
    repeatPrefix = lis.readInt(0, 3);
    page = lis.readInt(0, 0xF);
    command = lis.readInt(0, 0xFF);

    lrqHigh = lis.readBoolean();
    speaking = lis.readBoolean();
    fifoHead = lis.readInt(0, 63);
    fifoSize = lis.readInt(0, 64);
    for (INT32 i = 0; i < fifoBytes.length; i++)
        fifoBytes[i] = lis.readInt(0, 0x3FF);
}
*/
