
#include <iostream>
#include <string.h>
#include "CP1610.h"
#include "../core/osd/types.h"

#pragma warning(disable:4786)	// Suppress STL debug info > 255 chars messages
using namespace std;

CP1610::CP1610()
{
	resetAddress = 0;

	intrmLine = NULL;
    busRqLine = NULL;
    busAkLine = NULL;
    
	memset(r, 0, sizeof(r));

//	reset();
}

void CP1610::init(MemoryBus16Bit* m, SignalLine* intrmLine, SignalLine* busRqLine,
        SignalLine* busAkLine, INT32 resetAddress, INT32 interruptAddress)
{
    this->memoryBus = m;
    this->intrmLine = intrmLine;
    this->busRqLine = busRqLine;
    this->busAkLine = busAkLine;
    this->resetAddress = resetAddress;
    this->interruptAddress = interruptAddress;
}

INT32 CP1610::getClockSpeed() {
    return 3579545;
}

void CP1610::reset() {
    isIdle = FALSE;
	if(busAkLine)
		busAkLine->isHigh = TRUE;
    interruptible = FALSE;
    S = C = O = Z = I = D = FALSE;
    for (INT32 i = 0; i < 7; i++)
        r[i] = 0;
    r[7] = resetAddress;
}

/**
 * This method ticks the CPU and returns the number of cycles that were
 * used up, indicating to the main emulation loop when the CPU will need
 * to be ticked again.
 */
INT32 CP1610::tick()
{
    if (!busRqLine->isHigh && interruptible) {
        busAkLine->isHigh = FALSE;
        //sync up and run again with the STIC
        isIdle = TRUE;
        return 4;
    }
    else
        busAkLine->isHigh = TRUE;

    if (I && interruptible && !intrmLine->isHigh) {
        intrmLine->isHigh = TRUE;
        interruptible = FALSE;
        setIndirect(6, r[7]);
        r[7] = interruptAddress;
        return 28;
    }

    //do the next instruction
    interruptible = TRUE;
    BOOL oldD = D;
    usedCycles = 0;
//cout << r[7] << ": " << memoryBus->peek(r[7]) << "\n";
    decode(memoryBus->peek(r[7]));
    if (oldD && D)
        D = FALSE;

    return (usedCycles<<2);
}

void CP1610::setIndirect(INT32 registerNum, INT32 value) {
    memoryBus->poke(r[registerNum], value);

    //if the register number is 4-7, increment it
    if (registerNum & 0x04)
/*
    if (registerNum == 4 || registerNum == 5 || registerNum == 6 ||
            registerNum == 7)
*/
        r[registerNum] = (r[registerNum] + 1) & 0xFFFF;
}

INT32 CP1610::getIndirect(INT32 registerNum) {
    INT32 value;
    if (D) {
        if (registerNum == 6)
            r[6] = (r[6]-1) & 0xFFFF;
        value = memoryBus->peek(r[registerNum]) & 0xFF;
        if (registerNum == 4 || registerNum == 5 || registerNum == 7)
            r[registerNum] = (r[registerNum]+1) & 0xFFFF;
        if (registerNum == 6)
            r[6] = (r[6]-1) & 0xFFFF;
        value |= (memoryBus->peek(r[registerNum]) & 0xFF)
                << 8;
        if (registerNum == 4 || registerNum == 5 || registerNum == 7)
            r[registerNum] = (r[registerNum]+1) & 0xFFFF;
    }
    else {
        if (registerNum == 6)
            r[6] = (r[6]-1) & 0xFFFF;
        value = memoryBus->peek(r[registerNum]);
        if (registerNum == 4 || registerNum == 5 || registerNum == 7)
            r[registerNum] = (r[registerNum]+1) & 0xFFFF;
    }

    return value;
}

void CP1610::HLT() {
    //halted = TRUE;
    usedCycles += 1;
}

void CP1610::SDBD() {
    r[7] = (r[7]+1) & 0xFFFF;
    interruptible = FALSE;

    D = TRUE;

    usedCycles += 4;
}

void CP1610::EIS() {
    r[7] = (r[7]+1) & 0xFFFF;
    interruptible = FALSE;

    I = TRUE;

    usedCycles += 4;
}

void CP1610::DIS() {
    r[7] = (r[7]+1) & 0xFFFF;
    interruptible = FALSE;

    I = FALSE;

    usedCycles += 4;
}

void CP1610::TCI() {
    r[7] = (r[7]+1) & 0xFFFF;
    interruptible = FALSE;

    //TODO:  NOP for now?

    usedCycles += 4;
}

void CP1610::CLRC() {
    r[7] = (r[7]+1) & 0xFFFF;
    interruptible = FALSE;

    C = FALSE;

    usedCycles += 4;
}

void CP1610::SETC() {
    r[7] = (r[7]+1) & 0xFFFF;
    interruptible = FALSE;

    C = TRUE;

    usedCycles += 4;
}

void CP1610::J(INT32 target) {
    r[7] = target;

    usedCycles += 12;
}

void CP1610::JSR(INT32 registerNum, INT32 target) {
    r[7] = (r[7]+3) & 0xFFFF;

    r[registerNum] = r[7];
    r[7] = target;

    usedCycles += 12;
}

void CP1610::JE(INT32 target) {
    I = TRUE;
    r[7] = target;

    usedCycles += 12;
}

void CP1610::JSRE(INT32 registerNum, INT32 target) {
    r[7] = (r[7]+3) & 0xFFFF;

    I = TRUE;
    r[registerNum] = r[7];
    r[7] = target;

    usedCycles += 12;
}

void CP1610::JD(INT32 target) {
    I = FALSE;
    r[7] = target;

    usedCycles += 12;
}

void CP1610::JSRD(INT32 registerNum, INT32 target) {
    r[7] = (r[7]+3) & 0xFFFF;

    I = FALSE;
    r[registerNum] = r[7];
    r[7] = target;

    usedCycles += 12;
}

void CP1610::INCR(INT32 registerNum) {
    r[7] = (r[7]+1) & 0xFFFF;

    INT32 newValue = (r[registerNum]+1) & 0xFFFF;
    S = ((newValue & 0x8000) != 0);
    Z = (newValue == 0);
    r[registerNum] = newValue;

    usedCycles += 6;
}

void CP1610::DECR(INT32 registerNum) {
    r[7] = (r[7]+1) & 0xFFFF;

    INT32 newValue = (r[registerNum] + 0xFFFF) & 0xFFFF;
    S = ((newValue & 0x8000) != 0);
    Z = (newValue == 0);
    r[registerNum] = newValue;

    usedCycles += 6;
}

void CP1610::NEGR(INT32 registerNum) {
    r[7] = (r[7]+1) & 0xFFFF;

    INT32 op1 = r[registerNum];
    INT32 newValue = (op1 ^ 0xFFFF) + 1;
    C = ((newValue & 0x10000) != 0);
    O = ((newValue & op1) & 0x8000) != 0;
    newValue = (newValue & 0xFFFF);
    S = ((newValue & 0x8000) != 0);
    Z = (newValue == 0);
    r[registerNum] = newValue;

    usedCycles += 6;
}

void CP1610::ADCR(INT32 registerNum) {
    r[7] = (r[7]+1) & 0xFFFF;

    INT32 op1 = r[registerNum];
    INT32 op2 = (C ? 1 : 0);
    INT32 newValue = op1 + op2;
    C = ((newValue & 0x10000) != 0);
    O = ((op2 ^ newValue) & ~(op1 ^ op2) & 0x8000) != 0;
    newValue = (newValue & 0xFFFF);
    S = ((newValue & 0x8000) != 0);
    Z = (newValue == 0);
    r[registerNum] = newValue;

    usedCycles += 6;
}

void CP1610::RSWD(INT32 registerNum) {
    r[7] = (r[7]+1) & 0xFFFF;

    INT32 value = r[registerNum];
    S = ((value & 0x0080) != 0);
    Z = ((value & 0x0040) != 0);
    O = ((value & 0x0020) != 0);
    C = ((value & 0x0010) != 0);

    usedCycles += 6;
}

void CP1610::GSWD(INT32 registerNum) {
    r[7] = (r[7]+1) & 0xFFFF;

    INT32 value = (((S ? 1 : 0) << 7) |
            ((Z ? 1 : 0) << 6) | 
            ((O ? 1 : 0) << 5) |
            ((C ? 1 : 0) << 4)) & 0xF0;
    value = value | (value << 8);
    r[registerNum] = value;

    usedCycles += 6;
}

void CP1610::NOP(INT32 twoOption) {
    r[7] = (r[7]+1) & 0xFFFF;

    usedCycles += 6;
}

void CP1610::SIN(INT32 twoOption) {
    r[7] = (r[7]+1) & 0xFFFF;

    //TODO: does SIN need to do anything?!

    usedCycles += 6;
}

void CP1610::SWAP(INT32 shift, INT32 registerNum) {
    r[7] = (r[7]+1) & 0xFFFF;
    interruptible = FALSE;

    if (shift == 0) {
        INT32 value = r[registerNum];
        value = ((value & 0xFF00) >> 8) | ((value & 0xFF) << 8);
        S = ((value & 0x0080) != 0);
        Z = (value == 0);
        r[registerNum] = value;

        usedCycles += 6;
    }
    else {
        INT32 value = (r[registerNum] & 0xFF);
        value = value | (value << 8);
        S = ((value & 0x8000) != 0);
        Z = (value == 0);
        r[registerNum] = value;

        usedCycles += 8;
    }
}

void CP1610::COMR(INT32 registerNum) {
    r[7] = (r[7]+1) & 0xFFFF;

    INT32 newValue = r[registerNum] ^ 0xFFFF;
    S = ((newValue & 0x8000) != 0);
    Z = (newValue == 0);
    r[registerNum] = newValue;

    usedCycles += 6;
}

void CP1610::SLL(INT32 shift, INT32 registerNum) {
    r[7] = (r[7]+1) & 0xFFFF;
    interruptible = FALSE;

    if (shift == 0) {
        INT32 value = r[registerNum];
        value = (value << 1) & 0xFFFF;
        S = ((value & 0x8000) != 0);
        Z = (value == 0);
        r[registerNum] = value;

        usedCycles += 6;
    }
    else {
        INT32 value = r[registerNum];
        value = (value << 2) & 0xFFFF;
        S = ((value & 0x8000) != 0);
        Z = (value == 0);
        r[registerNum] = value;

        usedCycles += 8;
    }
}

void CP1610::RLC(INT32 shift, INT32 registerNum) {
    r[7] = (r[7]+1) & 0xFFFF;
    interruptible = FALSE;

    if (shift == 0) {
        INT32 value = r[registerNum];
        INT32 carry = (C ? 1 : 0);
        C = ((value & 0x8000) != 0);
        value = ((value << 1) & 0xFFFF) | carry;
        S = ((value & 0x8000) != 0);
        Z = (value == 0);
        r[registerNum] = value;

        usedCycles += 6;
    }
    else {
        INT32 value = r[registerNum];
        INT32 carry = (C ? 1 : 0);
        INT32 overflow = (O ? 1 : 0);
        C = ((value & 0x8000) != 0);
        O = ((value & 0x4000) != 0);
        value = ((value << 2) | (carry << 1) | overflow) & 0xFFFF;
        S = ((value & 0x8000) != 0);
        Z = (value == 0);
        r[registerNum] = value;

        usedCycles += 8;
    }
}

void CP1610::SLLC(INT32 shift, INT32 registerNum) {
    r[7] = (r[7]+1) & 0xFFFF;
    interruptible = FALSE;

    if (shift == 0) {
        INT32 value = r[registerNum];
        C = ((value & 0x8000) != 0);
        value = (value << 1) & 0xFFFF;
        S = ((value & 0x8000) != 0);
        Z = (value == 0);
        r[registerNum] = value;

        usedCycles += 6;
    }
    else {
        INT32 value = r[registerNum];
        C = ((value & 0x8000) != 0);
        O = ((value & 0x4000) != 0);
        value = (value << 2) & 0xFFFF;
        S = ((value & 0x8000) != 0);
        Z = (value == 0);
        r[registerNum] = value;

        usedCycles += 8;
    }
}

void CP1610::SLR(INT32 shift, INT32 registerNum) {
    r[7] = (r[7]+1) & 0xFFFF;
    interruptible = FALSE;

    if (shift == 0) {
        INT32 value = r[registerNum];
        value = (value >> 1) & 0x7FFF;
        S = ((value & 0x0080) != 0);
        Z = (value == 0);
        r[registerNum] = value;

        usedCycles += 6;
    }
    else {
        INT32 value = r[registerNum];
        value = (value >> 2) & 0x3FFF;
        S = ((value & 0x0080) != 0);
        Z = (value == 0);
        r[registerNum] = value;

        usedCycles += 8;
    }
}

void CP1610::SAR(INT32 shift, INT32 registerNum) {
    r[7] = (r[7]+1) & 0xFFFF;
    interruptible = FALSE;

    if (shift == 0) {
        INT32 value = r[registerNum];
        value = (value >> 1) | (value & 0x8000);
        S = ((value & 0x0080) != 0);
        Z = (value == 0);
        r[registerNum] = value;

        usedCycles += 6;
    }
    else {
        INT32 value = r[registerNum];
        INT32 s  = value & 0x8000;
        value = (value >> 2) | s | (s >> 1);
        S = ((value & 0x0080) != 0);
        Z = (value == 0);
        r[registerNum] = value;

        usedCycles += 8;
    }
}

void CP1610::RRC(INT32 shift, INT32 registerNum) {
    r[7] = (r[7]+1) & 0xFFFF;
    interruptible = FALSE;

    if (shift == 0) {
        INT32 value = r[registerNum];
        INT32 carry = (C ? 1 : 0);
        C = ((value & 0x0001) != 0);
        value = ((value >> 1) & 0x7FFF) | (carry << 15);
        S = ((value & 0x0080) != 0);
        Z = (value == 0);
        r[registerNum] = value;

        usedCycles += 6;
    }
    else {
        INT32 value = r[registerNum];
        INT32 carry = (C ? 1 : 0);
        INT32 overflow = (O ? 1 : 0);
        C = ((value & 0x0001) != 0);
        O = ((value & 0x0002) != 0);
        value = ((value >> 2) & 0x3FFF) | (carry << 14) | (overflow << 15);
        S = ((value & 0x0080) != 0);
        Z = (value == 0);
        r[registerNum] = value;

        usedCycles += 8;
    }
}

void CP1610::SARC(INT32 shift, INT32 registerNum) {
    r[7] = (r[7]+1) & 0xFFFF;
    interruptible = FALSE;

    if (shift == 0) {
        INT32 value = r[registerNum];
        C = ((value & 0x0001) != 0);
        value = ((value >> 1) & 0x7FFF) | (value & 0x8000);
        S = ((value & 0x0080) != 0);
        Z = (value == 0);
        r[registerNum] = value;

        usedCycles += 6;
    }
    else {
        INT32 value = r[registerNum];
            C = ((value & 0x0001) != 0);
    O = ((value & 0x0002) != 0);
        INT32 s = value & 0x8000;
        value = ((value >> 2) & 0x3FFF) | s | (s >> 1);
        S = ((value & 0x0080) != 0);
        Z = (value == 0);
        r[registerNum] = value;

        usedCycles += 8;
    }
}

void CP1610::MOVR(INT32 sourceReg, INT32 destReg) {
    r[7] = (r[7]+1) & 0xFFFF;

    INT32 value = r[sourceReg];
        r[destReg] = value;
    S = ((value & 0x8000) != 0);
    Z = (value == 0);

    usedCycles += (destReg >= 6 ? 7 : 6);
}

void CP1610::ADDR(INT32 sourceReg, INT32 destReg) {
    r[7] = (r[7]+1) & 0xFFFF;

    INT32 op1 = r[sourceReg];
    INT32 op2 = r[destReg];
    INT32 newValue = op1 + op2;
    C = ((newValue & 0x10000) != 0);
    O = ((op2 ^ newValue) & ~(op1 ^ op2) & 0x8000) != 0;
    newValue = (newValue & 0xFFFF);
    S = ((newValue & 0x8000) != 0);
    Z = (newValue == 0);
    r[destReg] = newValue;

    usedCycles += 6;
}

void CP1610::SUBR(INT32 sourceReg, INT32 destReg) {
    r[7] = (r[7]+1) & 0xFFFF;

    INT32 op1 = r[sourceReg];
    INT32 op2 = r[destReg];
    INT32 newValue = op2 + (0xFFFF ^ op1) + 1;
    C = ((newValue & 0x10000) != 0);
    O = ((op2 ^ newValue) & (op1 ^ op2) & 0x8000) != 0;
    newValue = (newValue & 0xFFFF);
    S = ((newValue & 0x8000) != 0);
    Z = (newValue == 0);
    r[destReg] = newValue;

    usedCycles += 6;
}

void CP1610::CMPR(INT32 sourceReg, INT32 destReg) {
    r[7] = (r[7]+1) & 0xFFFF;

    INT32 op1 = r[sourceReg];
    INT32 op2 = r[destReg];
    INT32 newValue = op2 + (0xFFFF ^ op1) + 1;
    C = ((newValue & 0x10000) != 0);
    O = ((op2 ^ newValue) & (op1 ^ op2) & 0x8000) != 0;
    newValue = (newValue & 0xFFFF);
    S = ((newValue & 0x8000) != 0);
    Z = (newValue == 0);

    usedCycles += 6;
}

void CP1610::ANDR(INT32 sourceReg, INT32 destReg) {
    r[7] = (r[7]+1) & 0xFFFF;

    INT32 newValue = r[destReg];
    newValue = newValue & r[sourceReg];
    S = ((newValue & 0x8000) != 0);
    Z = (newValue == 0);
    r[destReg] = newValue;

    usedCycles += 6;
}

void CP1610::XORR(INT32 sourceReg, INT32 destReg) {
    r[7] = (r[7]+1) & 0xFFFF;

    INT32 newValue = r[destReg] ^ r[sourceReg];
    S = ((newValue & 0x8000) != 0);
    Z = (newValue == 0);
    r[destReg] = newValue;

    usedCycles += 6;
}

void CP1610::BEXT(INT32 condition, INT32 displacement) {
    r[7] = (r[7]+2) & 0xFFFF;

    if (ext == condition) {
        r[7] = (r[7]+displacement) & 0xFFFF;
        usedCycles += 2;
    }

    usedCycles += 7;
}

void CP1610::B(INT32 displacement) {
    r[7] = (r[7]+2) & 0xFFFF;

    r[7] = (r[7]+displacement) & 0xFFFF;

    usedCycles += 9;
}

void CP1610::NOPP(INT32 displacement) {
    r[7] = (r[7]+2) & 0xFFFF;

    usedCycles += 7;
}

void CP1610::BC(INT32 displacement) {
    r[7] = (r[7]+2) & 0xFFFF;

    if (C) {
        r[7] = (r[7]+displacement) & 0xFFFF;
        usedCycles += 2;
    }

    usedCycles += 7;
}

void CP1610::BNC(INT32 displacement) {
    r[7] = (r[7]+2) & 0xFFFF;

    if (!C) {
        r[7] = (r[7]+displacement) & 0xFFFF;
        usedCycles += 2;
    }

    usedCycles += 7;
}

void CP1610::BOV(INT32 displacement) {
    r[7] = (r[7]+2) & 0xFFFF;

    if (O) {
        r[7] = (r[7]+displacement) & 0xFFFF;
        usedCycles += 2;
    }

    usedCycles += 7;
}

void CP1610::BNOV(INT32 displacement) {
    r[7] = (r[7]+2) & 0xFFFF;

    if (!O) {
        r[7] = (r[7]+displacement) & 0xFFFF;
        usedCycles += 2;
    }

    usedCycles += 7;
}

void CP1610::BPL(INT32 displacement) {
    r[7] = (r[7]+2) & 0xFFFF;

    if (!S) {
        r[7] = (r[7]+displacement) & 0xFFFF;
        usedCycles += 2;
    }

    usedCycles += 7;
}

void CP1610::BMI(INT32 displacement) {
    r[7] = (r[7]+2) & 0xFFFF;

    if (S) {
        r[7] = (r[7]+displacement) & 0xFFFF;
        usedCycles += 2;
    }

    usedCycles += 7;
}

void CP1610::BEQ(INT32 displacement) {
    r[7] = (r[7]+2) & 0xFFFF;

    if (Z) {
        r[7] = (r[7]+displacement) & 0xFFFF;
        usedCycles += 2;
    }

    usedCycles += 7;
}

void CP1610::BNEQ(INT32 displacement) {
    r[7] = (r[7]+2) & 0xFFFF;

    if (!Z) {
        r[7] = (r[7]+displacement) & 0xFFFF;
        usedCycles += 2;
    }

    usedCycles += 7;
}

void CP1610::BLT(INT32 displacement) {
    r[7] = (r[7]+2) & 0xFFFF;

    if (S != O) {
        r[7] = (r[7]+displacement) & 0xFFFF;
        usedCycles += 2;
    }

    usedCycles += 7;
}

void CP1610::BGE(INT32 displacement) {
    r[7] = (r[7]+2) & 0xFFFF;

    if (S == O) {
        r[7] = (r[7]+displacement) & 0xFFFF;
        usedCycles += 2;
    }

    usedCycles += 7;
}

void CP1610::BLE(INT32 displacement) {
    r[7] = (r[7]+2) & 0xFFFF;

    if (Z || (S != O)) {
        r[7] = (r[7]+displacement) & 0xFFFF;
        usedCycles += 2;
    }

    usedCycles += 7;
}

void CP1610::BGT(INT32 displacement) {
    r[7] = (r[7]+2) & 0xFFFF;

    if (!(Z || (S != O))) {
        r[7] = (r[7]+displacement) & 0xFFFF;
        usedCycles += 2;
    }

    usedCycles += 7;
}

void CP1610::BUSC(INT32 displacement) {
    r[7] = (r[7]+2) & 0xFFFF;

    if (C != S) {
        r[7] = (r[7]+displacement) & 0xFFFF;
        usedCycles += 2;
    }

    usedCycles += 7;
}

void CP1610::BESC(INT32 displacement) {
    r[7] = (r[7]+2) & 0xFFFF;

    if (C == S) {
        r[7] = (r[7]+displacement) & 0xFFFF;
        usedCycles += 2;
    }

    usedCycles += 7;
}

void CP1610::MVO(INT32 registerNum, INT32 address) {
    r[7] = (r[7]+2) & 0xFFFF;
    interruptible = FALSE;

    memoryBus->poke(address, r[registerNum]);

    usedCycles += 11;
}

void CP1610::MVO_ind(INT32 registerWithAddress, INT32 registerToMove) {
    r[7] = (r[7]+1) & 0xFFFF;
    interruptible = FALSE;

    setIndirect(registerWithAddress, r[registerToMove]);

    usedCycles += 9;
}

void CP1610::MVI(INT32 address, INT32 registerNum) {
    r[7] = (r[7]+2) & 0xFFFF;

    r[registerNum] = memoryBus->peek(address);

    usedCycles += 10;
}

void CP1610::MVI_ind(INT32 registerWithAddress, INT32 registerToReceive) {
    r[7] = (r[7]+1) & 0xFFFF;

    INT32 address = getIndirect(registerWithAddress);
    r[registerToReceive] = address;

    usedCycles += (D ? 10 : (registerWithAddress == 6 ? 11 : 8));
}

void CP1610::ADD(INT32 address, INT32 registerNum) {
    r[7] = (r[7]+2) & 0xFFFF;

    INT32 op1 = memoryBus->peek(address);
    INT32 op2 = r[registerNum];
    INT32 newValue = op1 + op2;
    C = ((newValue & 0x10000) != 0);
    O = ((op2 ^ newValue) & ~(op1 ^ op2) & 0x8000) != 0;
    newValue = (newValue & 0xFFFF);
    S = ((newValue & 0x8000) != 0);
    Z = (newValue == 0);
    r[registerNum] = newValue;

    usedCycles += 10;
}

void CP1610::ADD_ind(INT32 registerWithAddress, INT32 registerToReceive) {
    r[7] = (r[7]+1) & 0xFFFF;

    INT32 op1 = getIndirect(registerWithAddress);
    INT32 op2 = r[registerToReceive];
    INT32 newValue = op1 + op2;
    C = ((newValue & 0x10000) != 0);
    O = ((op2 ^ newValue) & ~(op1 ^ op2) & 0x8000) != 0;
    newValue = (newValue & 0xFFFF);
    S = ((newValue & 0x8000) != 0);
    Z = (newValue == 0);
    r[registerToReceive] = newValue;
    
    usedCycles += (D ? 10 : (registerWithAddress == 6 ? 11 : 8));
}

void CP1610::SUB(INT32 address, INT32 registerNum) {
    r[7] = (r[7]+2) & 0xFFFF;

    INT32 op1 = memoryBus->peek(address);
    INT32 op2 = r[registerNum];
    INT32 newValue = op2 + (0xFFFF ^ op1) + 1;
    C = ((newValue & 0x10000) != 0);
    O = ((op2 ^ newValue) & (op1 ^ op2) & 0x8000) != 0;
    newValue = (newValue & 0xFFFF);
    S = ((newValue & 0x8000) != 0);
    Z = (newValue == 0);
    r[registerNum] = newValue;

    usedCycles += 10;
}

void CP1610::SUB_ind(INT32 registerWithAddress, INT32 registerToReceive) {
    r[7] = (r[7]+1) & 0xFFFF;

    INT32 op1 = getIndirect(registerWithAddress);
    INT32 op2 = r[registerToReceive];
    INT32 newValue = op2 + (0xFFFF ^ op1) + 1;
    C = ((newValue & 0x10000) != 0);
    O = ((op2 ^ newValue) & (op1 ^ op2) & 0x8000) != 0;
    newValue = (newValue & 0xFFFF);
    S = ((newValue & 0x8000) != 0);
    Z = (newValue == 0);
    r[registerToReceive] = newValue;

    usedCycles += (D ? 10 : (registerWithAddress == 6 ? 11 : 8));
}

void CP1610::CMP(INT32 address, INT32 registerNum) {
    r[7] = (r[7]+2) & 0xFFFF;

    INT32 op1 = memoryBus->peek(address);
    INT32 op2 = r[registerNum];
    INT32 newValue = op2 + (0xFFFF ^ op1) + 1;
    C = ((newValue & 0x10000) != 0);
    O = ((op2 ^ newValue) & (op1 ^ op2) & 0x8000) != 0;
    newValue = (newValue & 0xFFFF);
    S = ((newValue & 0x8000) != 0);
    Z = (newValue == 0);

    usedCycles += 10;
}

void CP1610::CMP_ind(INT32 registerWithAddress, INT32 registerToReceive) {
    r[7] = (r[7]+1) & 0xFFFF;

    INT32 op1 = getIndirect(registerWithAddress);
    INT32 op2 = r[registerToReceive];
    INT32 newValue = op2 + (0xFFFF ^ op1) + 1;
    C = ((newValue & 0x10000) != 0);
    O = ((op2 ^ newValue) & (op1 ^ op2) & 0x8000) != 0;
    newValue = (newValue & 0xFFFF);
    S = ((newValue & 0x8000) != 0);
    Z = (newValue == 0);

    usedCycles += (D ? 10 : (registerWithAddress == 6 ? 11 : 8));
}

void CP1610::AND(INT32 address, INT32 registerNum) {
    r[7] = (r[7]+2) & 0xFFFF;

    INT32 value = memoryBus->peek(address);
    value = (value & r[registerNum]);
    S = ((value & 0x8000) != 0);
    Z = (value == 0);
    r[registerNum] = value;

    usedCycles += 10;
}

void CP1610::AND_ind(INT32 registerWithAddress, INT32 registerToReceive) {
    r[7] = (r[7]+1) & 0xFFFF;

    INT32 value = getIndirect(registerWithAddress);
    value = (value & r[registerToReceive]);
    S = ((value & 0x8000) != 0);
    Z = (value == 0);
    r[registerToReceive] = value;
    
    usedCycles += (D ? 10 : (registerWithAddress == 6 ? 11 : 8));
}

void CP1610::XOR(INT32 address, INT32 registerNum) {
    r[7] = (r[7]+2) & 0xFFFF;

    INT32 value = memoryBus->peek(address);
    value ^= r[registerNum];
    S = ((value & 0x8000) != 0);
    Z = (value == 0);
    r[registerNum] = value;

    usedCycles += 10;
}

void CP1610::XOR_ind(INT32 registerWithAddress, INT32 registerToReceive) {
    r[7] = (r[7]+1) & 0xFFFF;

    INT32 value = getIndirect(registerWithAddress);
    value ^= r[registerToReceive];
    S = ((value & 0x8000) != 0);
    Z = (value == 0);
    r[registerToReceive] = value;
    
    usedCycles += (D ? 10 : (registerWithAddress == 6 ? 11 : 8));
}

void CP1610::decode(INT32 op) {
    INT32 read;
    INT32 registerNum;
    INT32 interrupt;
    INT32 target;
    switch (op) {
        case 0x0000:
            HLT();
            break;
        case 0x0001:
            SDBD();
            break;
        case 0x0002:
            EIS();
            break;
        case 0x0003:
            DIS();
            break;
        case 0x0004:
            read = memoryBus->peek(r[7]+1);
            registerNum = ((read & 0x0300) >> 8);
            interrupt = (read & 0x0003);
            target = ((read & 0x00FC) << 8);
            read = memoryBus->peek(r[7]+2);
            target |= read & 0x03FF;
            if (registerNum == 3) {
                if (interrupt == 0)
                    J(target);
                else if (interrupt == 1)
                    JE(target);
                else if (interrupt == 2)
                    JD(target);
                else
                    HLT();  //invalid opcode
            }
            else {
                if (interrupt == 0)
                    JSR(registerNum+4, target);
                else if (interrupt == 1)
                    JSRE(registerNum+4, target);
                else if (interrupt == 2)
                    JSRD(registerNum+4, target);
                else
                    HLT();  //invalid opcode
            }
            break;
        case 0x0005:
            TCI();
            break;
        case 0x0006:
            CLRC();
            break;
        case 0x0007:
            SETC();
            break;
        case 0x0008:
            INCR(0);
            break;
        case 0x0009:
            INCR(1);
            break;
        case 0x000A:
            INCR(2);
            break;
        case 0x000B:
            INCR(3);
            break;
        case 0x000C:
            INCR(4);
            break;
        case 0x000D:
            INCR(5);
            break;
        case 0x000E:
            INCR(6);
            break;
        case 0x000F:
            INCR(7);
            break;
        case 0x0010:
            DECR(0);
            break;
        case 0x0011:
            DECR(1);
            break;
        case 0x0012:
            DECR(2);
            break;
        case 0x0013:
            DECR(3);
            break;
        case 0x0014:
            DECR(4);
            break;
        case 0x0015:
            DECR(5);
            break;
        case 0x0016:
            DECR(6);
            break;
        case 0x0017:
            DECR(7);
            break;
        case 0x0018:
            COMR(0);
            break;
        case 0x0019:
            COMR(1);
            break;
        case 0x001A:
            COMR(2);
            break;
        case 0x001B:
            COMR(3);
            break;
        case 0x001C:
            COMR(4);
            break;
        case 0x001D:
            COMR(5);
            break;
        case 0x001E:
            COMR(6);
            break;
        case 0x001F:
            COMR(7);
            break;
        case 0x0020:
            NEGR(0);
            break;
        case 0x0021:
            NEGR(1);
            break;
        case 0x0022:
            NEGR(2);
            break;
        case 0x0023:
            NEGR(3);
            break;
        case 0x0024:
            NEGR(4);
            break;
        case 0x0025:
            NEGR(5);
            break;
        case 0x0026:
            NEGR(6);
            break;
        case 0x0027:
            NEGR(7);
            break;
        case 0x0028:
            ADCR(0);
            break;
        case 0x0029:
            ADCR(1);
            break;
        case 0x002A:
            ADCR(2);
            break;
        case 0x002B:
            ADCR(3);
            break;
        case 0x002C:
            ADCR(4);
            break;
        case 0x002D:
            ADCR(5);
            break;
        case 0x002E:
            ADCR(6);
            break;
        case 0x002F:
            ADCR(7);
            break;
        case 0x0030:
            GSWD(0);
            break;
        case 0x0031:
            GSWD(1);
            break;
        case 0x0032:
            GSWD(2);
            break;
        case 0x0033:
            GSWD(3);
            break;
        case 0x0034:
            NOP(0);
            break;
        case 0x0035:
            NOP(1);
            break;
        case 0x0036:
            SIN(0);
            break;
        case 0x0037:
            SIN(1);
            break;
        case 0x0038:
            RSWD(0);
            break;
        case 0x0039:
            RSWD(1);
            break;
        case 0x003A:
            RSWD(2);
            break;
        case 0x003B:
            RSWD(3);
            break;
        case 0x003C:
            RSWD(4);
            break;
        case 0x003D:
            RSWD(5);
            break;
        case 0x003E:
            RSWD(6);
            break;
        case 0x003F:
            RSWD(7);
            break;
        case 0x0040:
            SWAP(0, 0);
            break;
        case 0x0041:
            SWAP(0, 1);
            break;
        case 0x0042:
            SWAP(0, 2);
            break;
        case 0x0043:
            SWAP(0, 3);
            break;
        case 0x0044:
            SWAP(1, 0);
            break;
        case 0x0045:
            SWAP(1, 1);
            break;
        case 0x0046:
            SWAP(1, 2);
            break;
        case 0x0047:
            SWAP(1, 3);
            break;
        case 0x0048:
            SLL(0, 0);
            break;
        case 0x0049:
            SLL(0, 1);
            break;
        case 0x004A:
            SLL(0, 2);
            break;
        case 0x004B:
            SLL(0, 3);
            break;
        case 0x004C:
            SLL(1, 0);
            break;
        case 0x004D:
            SLL(1, 1);
            break;
        case 0x004E:
            SLL(1, 2);
            break;
        case 0x004F:
            SLL(1, 3);
            break;
        case 0x0050:
            RLC(0, 0);
            break;
        case 0x0051:
            RLC(0, 1);
            break;
        case 0x0052:
            RLC(0, 2);
            break;
        case 0x0053:
            RLC(0, 3);
            break;
        case 0x0054:
            RLC(1, 0);
            break;
        case 0x0055:
            RLC(1, 1);
            break;
        case 0x0056:
            RLC(1, 2);
            break;
        case 0x0057:
            RLC(1, 3);
            break;
        case 0x0058:
            SLLC(0, 0);
            break;
        case 0x0059:
            SLLC(0, 1);
            break;
        case 0x005A:
            SLLC(0, 2);
            break;
        case 0x005B:
            SLLC(0, 3);
            break;
        case 0x005C:
            SLLC(1, 0);
            break;
        case 0x005D:
            SLLC(1, 1);
            break;
        case 0x005E:
            SLLC(1, 2);
            break;
        case 0x005F:
            SLLC(1, 3);
            break;
        case 0x0060:
            SLR(0, 0);
            break;
        case 0x0061:
            SLR(0, 1);
            break;
        case 0x0062:
            SLR(0, 2);
            break;
        case 0x0063:
            SLR(0, 3);
            break;
        case 0x0064:
            SLR(1, 0);
            break;
        case 0x0065:
            SLR(1, 1);
            break;
        case 0x0066:
            SLR(1, 2);
            break;
        case 0x0067:
            SLR(1, 3);
            break;
        case 0x0068:
            SAR(0, 0);
            break;
        case 0x0069:
            SAR(0, 1);
            break;
        case 0x006A:
            SAR(0, 2);
            break;
        case 0x006B:
            SAR(0, 3);
            break;
        case 0x006C:
            SAR(1, 0);
            break;
        case 0x006D:
            SAR(1, 1);
            break;
        case 0x006E:
            SAR(1, 2);
            break;
        case 0x006F:
            SAR(1, 3);
            break;
        case 0x0070:
            RRC(0, 0);
            break;
        case 0x0071:
            RRC(0, 1);
            break;
        case 0x0072:
            RRC(0, 2);
            break;
        case 0x0073:
            RRC(0, 3);
            break;
        case 0x0074:
            RRC(1, 0);
            break;
        case 0x0075:
            RRC(1, 1);
            break;
        case 0x0076:
            RRC(1, 2);
            break;
        case 0x0077:
            RRC(1, 3);
            break;
        case 0x0078:
            SARC(0, 0);
            break;
        case 0x0079:
            SARC(0, 1);
            break;
        case 0x007A:
            SARC(0, 2);
            break;
        case 0x007B:
            SARC(0, 3);
            break;
        case 0x007C:
            SARC(1, 0);
            break;
        case 0x007D:
            SARC(1, 1);
            break;
        case 0x007E:
            SARC(1, 2);
            break;
        case 0x007F:
            SARC(1, 3);
            break;
        case 0x0080:
            MOVR(0, 0);
            break;
        case 0x0081:
            MOVR(0, 1);
            break;
        case 0x0082:
            MOVR(0, 2);
            break;
        case 0x0083:
            MOVR(0, 3);
            break;
        case 0x0084:
            MOVR(0, 4);
            break;
        case 0x0085:
            MOVR(0, 5);
            break;
        case 0x0086:
            MOVR(0, 6);
            break;
        case 0x0087:
            MOVR(0, 7);
            break;
        case 0x0088:
            MOVR(1, 0);
            break;
        case 0x0089:
            MOVR(1, 1);
            break;
        case 0x008A:
            MOVR(1, 2);
            break;
        case 0x008B:
            MOVR(1, 3);
            break;
        case 0x008C:
            MOVR(1, 4);
            break;
        case 0x008D:
            MOVR(1, 5);
            break;
        case 0x008E:
            MOVR(1, 6);
            break;
        case 0x008F:
            MOVR(1, 7);
            break;
        case 0x0090:
            MOVR(2, 0);
            break;
        case 0x0091:
            MOVR(2, 1);
            break;
        case 0x0092:
            MOVR(2, 2);
            break;
        case 0x0093:
            MOVR(2, 3);
            break;
        case 0x0094:
            MOVR(2, 4);
            break;
        case 0x0095:
            MOVR(2, 5);
            break;
        case 0x0096:
            MOVR(2, 6);
            break;
        case 0x0097:
            MOVR(2, 7);
            break;
        case 0x0098:
            MOVR(3, 0);
            break;
        case 0x0099:
            MOVR(3, 1);
            break;
        case 0x009A:
            MOVR(3, 2);
            break;
        case 0x009B:
            MOVR(3, 3);
            break;
        case 0x009C:
            MOVR(3, 4);
            break;
        case 0x009D:
            MOVR(3, 5);
            break;
        case 0x009E:
            MOVR(3, 6);
            break;
        case 0x009F:
            MOVR(3, 7);
            break;
        case 0x00A0:
            MOVR(4, 0);
            break;
        case 0x00A1:
            MOVR(4, 1);
            break;
        case 0x00A2:
            MOVR(4, 2);
            break;
        case 0x00A3:
            MOVR(4, 3);
            break;
        case 0x00A4:
            MOVR(4, 4);
            break;
        case 0x00A5:
            MOVR(4, 5);
            break;
        case 0x00A6:
            MOVR(4, 6);
            break;
        case 0x00A7:
            MOVR(4, 7);
            break;
        case 0x00A8:
            MOVR(5, 0);
            break;
        case 0x00A9:
            MOVR(5, 1);
            break;
        case 0x00AA:
            MOVR(5, 2);
            break;
        case 0x00AB:
            MOVR(5, 3);
            break;
        case 0x00AC:
            MOVR(5, 4);
            break;
        case 0x00AD:
            MOVR(5, 5);
            break;
        case 0x00AE:
            MOVR(5, 6);
            break;
        case 0x00AF:
            MOVR(5, 7);
            break;
        case 0x00B0:
            MOVR(6, 0);
            break;
        case 0x00B1:
            MOVR(6, 1);
            break;
        case 0x00B2:
            MOVR(6, 2);
            break;
        case 0x00B3:
            MOVR(6, 3);
            break;
        case 0x00B4:
            MOVR(6, 4);
            break;
        case 0x00B5:
            MOVR(6, 5);
            break;
        case 0x00B6:
            MOVR(6, 6);
            break;
        case 0x00B7:
            MOVR(6, 7);
            break;
        case 0x00B8:
            MOVR(7, 0);
            break;
        case 0x00B9:
            MOVR(7, 1);
            break;
        case 0x00BA:
            MOVR(7, 2);
            break;
        case 0x00BB:
            MOVR(7, 3);
            break;
        case 0x00BC:
            MOVR(7, 4);
            break;
        case 0x00BD:
            MOVR(7, 5);
            break;
        case 0x00BE:
            MOVR(7, 6);
            break;
        case 0x00BF:
            MOVR(7, 7);
            break;
        case 0x00C0:
            ADDR(0, 0);
            break;
        case 0x00C1:
            ADDR(0, 1);
            break;
        case 0x00C2:
            ADDR(0, 2);
            break;
        case 0x00C3:
            ADDR(0, 3);
            break;
        case 0x00C4:
            ADDR(0, 4);
            break;
        case 0x00C5:
            ADDR(0, 5);
            break;
        case 0x00C6:
            ADDR(0, 6);
            break;
        case 0x00C7:
            ADDR(0, 7);
            break;
        case 0x00C8:
            ADDR(1, 0);
            break;
        case 0x00C9:
            ADDR(1, 1);
            break;
        case 0x00CA:
            ADDR(1, 2);
            break;
        case 0x00CB:
            ADDR(1, 3);
            break;
        case 0x00CC:
            ADDR(1, 4);
            break;
        case 0x00CD:
            ADDR(1, 5);
            break;
        case 0x00CE:
            ADDR(1, 6);
            break;
        case 0x00CF:
            ADDR(1, 7);
            break;
        case 0x00D0:
            ADDR(2, 0);
            break;
        case 0x00D1:
            ADDR(2, 1);
            break;
        case 0x00D2:
            ADDR(2, 2);
            break;
        case 0x00D3:
            ADDR(2, 3);
            break;
        case 0x00D4:
            ADDR(2, 4);
            break;
        case 0x00D5:
            ADDR(2, 5);
            break;
        case 0x00D6:
            ADDR(2, 6);
            break;
        case 0x00D7:
            ADDR(2, 7);
            break;
        case 0x00D8:
            ADDR(3, 0);
            break;
        case 0x00D9:
            ADDR(3, 1);
            break;
        case 0x00DA:
            ADDR(3, 2);
            break;
        case 0x00DB:
            ADDR(3, 3);
            break;
        case 0x00DC:
            ADDR(3, 4);
            break;
        case 0x00DD:
            ADDR(3, 5);
            break;
        case 0x00DE:
            ADDR(3, 6);
            break;
        case 0x00DF:
            ADDR(3, 7);
            break;
        case 0x00E0:
            ADDR(4, 0);
            break;
        case 0x00E1:
            ADDR(4, 1);
            break;
        case 0x00E2:
            ADDR(4, 2);
            break;
        case 0x00E3:
            ADDR(4, 3);
            break;
        case 0x00E4:
            ADDR(4, 4);
            break;
        case 0x00E5:
            ADDR(4, 5);
            break;
        case 0x00E6:
            ADDR(4, 6);
            break;
        case 0x00E7:
            ADDR(4, 7);
            break;
        case 0x00E8:
            ADDR(5, 0);
            break;
        case 0x00E9:
            ADDR(5, 1);
            break;
        case 0x00EA:
            ADDR(5, 2);
            break;
        case 0x00EB:
            ADDR(5, 3);
            break;
        case 0x00EC:
            ADDR(5, 4);
            break;
        case 0x00ED:
            ADDR(5, 5);
            break;
        case 0x00EE:
            ADDR(5, 6);
            break;
        case 0x00EF:
            ADDR(5, 7);
            break;
        case 0x00F0:
            ADDR(6, 0);
            break;
        case 0x00F1:
            ADDR(6, 1);
            break;
        case 0x00F2:
            ADDR(6, 2);
            break;
        case 0x00F3:
            ADDR(6, 3);
            break;
        case 0x00F4:
            ADDR(6, 4);
            break;
        case 0x00F5:
            ADDR(6, 5);
            break;
        case 0x00F6:
            ADDR(6, 6);
            break;
        case 0x00F7:
            ADDR(6, 7);
            break;
        case 0x00F8:
            ADDR(7, 0);
            break;
        case 0x00F9:
            ADDR(7, 1);
            break;
        case 0x00FA:
            ADDR(7, 2);
            break;
        case 0x00FB:
            ADDR(7, 3);
            break;
        case 0x00FC:
            ADDR(7, 4);
            break;
        case 0x00FD:
            ADDR(7, 5);
            break;
        case 0x00FE:
            ADDR(7, 6);
            break;
        case 0x00FF:
            ADDR(7, 7);
            break;
        case 0x0100:
            SUBR(0, 0);
            break;
        case 0x0101:
            SUBR(0, 1);
            break;
        case 0x0102:
            SUBR(0, 2);
            break;
        case 0x0103:
            SUBR(0, 3);
            break;
        case 0x0104:
            SUBR(0, 4);
            break;
        case 0x0105:
            SUBR(0, 5);
            break;
        case 0x0106:
            SUBR(0, 6);
            break;
        case 0x0107:
            SUBR(0, 7);
            break;
        case 0x0108:
            SUBR(1, 0);
            break;
        case 0x0109:
            SUBR(1, 1);
            break;
        case 0x010A:
            SUBR(1, 2);
            break;
        case 0x010B:
            SUBR(1, 3);
            break;
        case 0x010C:
            SUBR(1, 4);
            break;
        case 0x010D:
            SUBR(1, 5);
            break;
        case 0x010E:
            SUBR(1, 6);
            break;
        case 0x010F:
            SUBR(1, 7);
            break;
        case 0x0110:
            SUBR(2, 0);
            break;
        case 0x0111:
            SUBR(2, 1);
            break;
        case 0x0112:
            SUBR(2, 2);
            break;
        case 0x0113:
            SUBR(2, 3);
            break;
        case 0x0114:
            SUBR(2, 4);
            break;
        case 0x0115:
            SUBR(2, 5);
            break;
        case 0x0116:
            SUBR(2, 6);
            break;
        case 0x0117:
            SUBR(2, 7);
            break;
        case 0x0118:
            SUBR(3, 0);
            break;
        case 0x0119:
            SUBR(3, 1);
            break;
        case 0x011A:
            SUBR(3, 2);
            break;
        case 0x011B:
            SUBR(3, 3);
            break;
        case 0x011C:
            SUBR(3, 4);
            break;
        case 0x011D:
            SUBR(3, 5);
            break;
        case 0x011E:
            SUBR(3, 6);
            break;
        case 0x011F:
            SUBR(3, 7);
            break;
        case 0x0120:
            SUBR(4, 0);
            break;
        case 0x0121:
            SUBR(4, 1);
            break;
        case 0x0122:
            SUBR(4, 2);
            break;
        case 0x0123:
            SUBR(4, 3);
            break;
        case 0x0124:
            SUBR(4, 4);
            break;
        case 0x0125:
            SUBR(4, 5);
            break;
        case 0x0126:
            SUBR(4, 6);
            break;
        case 0x0127:
            SUBR(4, 7);
            break;
        case 0x0128:
            SUBR(5, 0);
            break;
        case 0x0129:
            SUBR(5, 1);
            break;
        case 0x012A:
            SUBR(5, 2);
            break;
        case 0x012B:
            SUBR(5, 3);
            break;
        case 0x012C:
            SUBR(5, 4);
            break;
        case 0x012D:
            SUBR(5, 5);
            break;
        case 0x012E:
            SUBR(5, 6);
            break;
        case 0x012F:
            SUBR(5, 7);
            break;
        case 0x0130:
            SUBR(6, 0);
            break;
        case 0x0131:
            SUBR(6, 1);
            break;
        case 0x0132:
            SUBR(6, 2);
            break;
        case 0x0133:
            SUBR(6, 3);
            break;
        case 0x0134:
            SUBR(6, 4);
            break;
        case 0x0135:
            SUBR(6, 5);
            break;
        case 0x0136:
            SUBR(6, 6);
            break;
        case 0x0137:
            SUBR(6, 7);
            break;
        case 0x0138:
            SUBR(7, 0);
            break;
        case 0x0139:
            SUBR(7, 1);
            break;
        case 0x013A:
            SUBR(7, 2);
            break;
        case 0x013B:
            SUBR(7, 3);
            break;
        case 0x013C:
            SUBR(7, 4);
            break;
        case 0x013D:
            SUBR(7, 5);
            break;
        case 0x013E:
            SUBR(7, 6);
            break;
        case 0x013F:
            SUBR(7, 7);
            break;
        case 0x0140:
            CMPR(0, 0);
            break;
        case 0x0141:
            CMPR(0, 1);
            break;
        case 0x0142:
            CMPR(0, 2);
            break;
        case 0x0143:
            CMPR(0, 3);
            break;
        case 0x0144:
            CMPR(0, 4);
            break;
        case 0x0145:
            CMPR(0, 5);
            break;
        case 0x0146:
            CMPR(0, 6);
            break;
        case 0x0147:
            CMPR(0, 7);
            break;
        case 0x0148:
            CMPR(1, 0);
            break;
        case 0x0149:
            CMPR(1, 1);
            break;
        case 0x014A:
            CMPR(1, 2);
            break;
        case 0x014B:
            CMPR(1, 3);
            break;
        case 0x014C:
            CMPR(1, 4);
            break;
        case 0x014D:
            CMPR(1, 5);
            break;
        case 0x014E:
            CMPR(1, 6);
            break;
        case 0x014F:
            CMPR(1, 7);
            break;
        case 0x0150:
            CMPR(2, 0);
            break;
        case 0x0151:
            CMPR(2, 1);
            break;
        case 0x0152:
            CMPR(2, 2);
            break;
        case 0x0153:
            CMPR(2, 3);
            break;
        case 0x0154:
            CMPR(2, 4);
            break;
        case 0x0155:
            CMPR(2, 5);
            break;
        case 0x0156:
            CMPR(2, 6);
            break;
        case 0x0157:
            CMPR(2, 7);
            break;
        case 0x0158:
            CMPR(3, 0);
            break;
        case 0x0159:
            CMPR(3, 1);
            break;
        case 0x015A:
            CMPR(3, 2);
            break;
        case 0x015B:
            CMPR(3, 3);
            break;
        case 0x015C:
            CMPR(3, 4);
            break;
        case 0x015D:
            CMPR(3, 5);
            break;
        case 0x015E:
            CMPR(3, 6);
            break;
        case 0x015F:
            CMPR(3, 7);
            break;
        case 0x0160:
            CMPR(4, 0);
            break;
        case 0x0161:
            CMPR(4, 1);
            break;
        case 0x0162:
            CMPR(4, 2);
            break;
        case 0x0163:
            CMPR(4, 3);
            break;
        case 0x0164:
            CMPR(4, 4);
            break;
        case 0x0165:
            CMPR(4, 5);
            break;
        case 0x0166:
            CMPR(4, 6);
            break;
        case 0x0167:
            CMPR(4, 7);
            break;
        case 0x0168:
            CMPR(5, 0);
            break;
        case 0x0169:
            CMPR(5, 1);
            break;
        case 0x016A:
            CMPR(5, 2);
            break;
        case 0x016B:
            CMPR(5, 3);
            break;
        case 0x016C:
            CMPR(5, 4);
            break;
        case 0x016D:
            CMPR(5, 5);
            break;
        case 0x016E:
            CMPR(5, 6);
            break;
        case 0x016F:
            CMPR(5, 7);
            break;
        case 0x0170:
            CMPR(6, 0);
            break;
        case 0x0171:
            CMPR(6, 1);
            break;
        case 0x0172:
            CMPR(6, 2);
            break;
        case 0x0173:
            CMPR(6, 3);
            break;
        case 0x0174:
            CMPR(6, 4);
            break;
        case 0x0175:
            CMPR(6, 5);
            break;
        case 0x0176:
            CMPR(6, 6);
            break;
        case 0x0177:
            CMPR(6, 7);
            break;
        case 0x0178:
            CMPR(7, 0);
            break;
        case 0x0179:
            CMPR(7, 1);
            break;
        case 0x017A:
            CMPR(7, 2);
            break;
        case 0x017B:
            CMPR(7, 3);
            break;
        case 0x017C:
            CMPR(7, 4);
            break;
        case 0x017D:
            CMPR(7, 5);
            break;
        case 0x017E:
            CMPR(7, 6);
            break;
        case 0x017F:
            CMPR(7, 7);
            break;
        case 0x0180:
            ANDR(0, 0);
            break;
        case 0x0181:
            ANDR(0, 1);
            break;
        case 0x0182:
            ANDR(0, 2);
            break;
        case 0x0183:
            ANDR(0, 3);
            break;
        case 0x0184:
            ANDR(0, 4);
            break;
        case 0x0185:
            ANDR(0, 5);
            break;
        case 0x0186:
            ANDR(0, 6);
            break;
        case 0x0187:
            ANDR(0, 7);
            break;
        case 0x0188:
            ANDR(1, 0);
            break;
        case 0x0189:
            ANDR(1, 1);
            break;
        case 0x018A:
            ANDR(1, 2);
            break;
        case 0x018B:
            ANDR(1, 3);
            break;
        case 0x018C:
            ANDR(1, 4);
            break;
        case 0x018D:
            ANDR(1, 5);
            break;
        case 0x018E:
            ANDR(1, 6);
            break;
        case 0x018F:
            ANDR(1, 7);
            break;
        case 0x0190:
            ANDR(2, 0);
            break;
        case 0x0191:
            ANDR(2, 1);
            break;
        case 0x0192:
            ANDR(2, 2);
            break;
        case 0x0193:
            ANDR(2, 3);
            break;
        case 0x0194:
            ANDR(2, 4);
            break;
        case 0x0195:
            ANDR(2, 5);
            break;
        case 0x0196:
            ANDR(2, 6);
            break;
        case 0x0197:
            ANDR(2, 7);
            break;
        case 0x0198:
            ANDR(3, 0);
            break;
        case 0x0199:
            ANDR(3, 1);
            break;
        case 0x019A:
            ANDR(3, 2);
            break;
        case 0x019B:
            ANDR(3, 3);
            break;
        case 0x019C:
            ANDR(3, 4);
            break;
        case 0x019D:
            ANDR(3, 5);
            break;
        case 0x019E:
            ANDR(3, 6);
            break;
        case 0x019F:
            ANDR(3, 7);
            break;
        case 0x01A0:
            ANDR(4, 0);
            break;
        case 0x01A1:
            ANDR(4, 1);
            break;
        case 0x01A2:
            ANDR(4, 2);
            break;
        case 0x01A3:
            ANDR(4, 3);
            break;
        case 0x01A4:
            ANDR(4, 4);
            break;
        case 0x01A5:
            ANDR(4, 5);
            break;
        case 0x01A6:
            ANDR(4, 6);
            break;
        case 0x01A7:
            ANDR(4, 7);
            break;
        case 0x01A8:
            ANDR(5, 0);
            break;
        case 0x01A9:
            ANDR(5, 1);
            break;
        case 0x01AA:
            ANDR(5, 2);
            break;
        case 0x01AB:
            ANDR(5, 3);
            break;
        case 0x01AC:
            ANDR(5, 4);
            break;
        case 0x01AD:
            ANDR(5, 5);
            break;
        case 0x01AE:
            ANDR(5, 6);
            break;
        case 0x01AF:
            ANDR(5, 7);
            break;
        case 0x01B0:
            ANDR(6, 0);
            break;
        case 0x01B1:
            ANDR(6, 1);
            break;
        case 0x01B2:
            ANDR(6, 2);
            break;
        case 0x01B3:
            ANDR(6, 3);
            break;
        case 0x01B4:
            ANDR(6, 4);
            break;
        case 0x01B5:
            ANDR(6, 5);
            break;
        case 0x01B6:
            ANDR(6, 6);
            break;
        case 0x01B7:
            ANDR(6, 7);
            break;
        case 0x01B8:
            ANDR(7, 0);
            break;
        case 0x01B9:
            ANDR(7, 1);
            break;
        case 0x01BA:
            ANDR(7, 2);
            break;
        case 0x01BB:
            ANDR(7, 3);
            break;
        case 0x01BC:
            ANDR(7, 4);
            break;
        case 0x01BD:
            ANDR(7, 5);
            break;
        case 0x01BE:
            ANDR(7, 6);
            break;
        case 0x01BF:
            ANDR(7, 7);
            break;
        case 0x01C0:
            XORR(0, 0);
            break;
        case 0x01C1:
            XORR(0, 1);
            break;
        case 0x01C2:
            XORR(0, 2);
            break;
        case 0x01C3:
            XORR(0, 3);
            break;
        case 0x01C4:
            XORR(0, 4);
            break;
        case 0x01C5:
            XORR(0, 5);
            break;
        case 0x01C6:
            XORR(0, 6);
            break;
        case 0x01C7:
            XORR(0, 7);
            break;
        case 0x01C8:
            XORR(1, 0);
            break;
        case 0x01C9:
            XORR(1, 1);
            break;
        case 0x01CA:
            XORR(1, 2);
            break;
        case 0x01CB:
            XORR(1, 3);
            break;
        case 0x01CC:
            XORR(1, 4);
            break;
        case 0x01CD:
            XORR(1, 5);
            break;
        case 0x01CE:
            XORR(1, 6);
            break;
        case 0x01CF:
            XORR(1, 7);
            break;
        case 0x01D0:
            XORR(2, 0);
            break;
        case 0x01D1:
            XORR(2, 1);
            break;
        case 0x01D2:
            XORR(2, 2);
            break;
        case 0x01D3:
            XORR(2, 3);
            break;
        case 0x01D4:
            XORR(2, 4);
            break;
        case 0x01D5:
            XORR(2, 5);
            break;
        case 0x01D6:
            XORR(2, 6);
            break;
        case 0x01D7:
            XORR(2, 7);
            break;
        case 0x01D8:
            XORR(3, 0);
            break;
        case 0x01D9:
            XORR(3, 1);
            break;
        case 0x01DA:
            XORR(3, 2);
            break;
        case 0x01DB:
            XORR(3, 3);
            break;
        case 0x01DC:
            XORR(3, 4);
            break;
        case 0x01DD:
            XORR(3, 5);
            break;
        case 0x01DE:
            XORR(3, 6);
            break;
        case 0x01DF:
            XORR(3, 7);
            break;
        case 0x01E0:
            XORR(4, 0);
            break;
        case 0x01E1:
            XORR(4, 1);
            break;
        case 0x01E2:
            XORR(4, 2);
            break;
        case 0x01E3:
            XORR(4, 3);
            break;
        case 0x01E4:
            XORR(4, 4);
            break;
        case 0x01E5:
            XORR(4, 5);
            break;
        case 0x01E6:
            XORR(4, 6);
            break;
        case 0x01E7:
            XORR(4, 7);
            break;
        case 0x01E8:
            XORR(5, 0);
            break;
        case 0x01E9:
            XORR(5, 1);
            break;
        case 0x01EA:
            XORR(5, 2);
            break;
        case 0x01EB:
            XORR(5, 3);
            break;
        case 0x01EC:
            XORR(5, 4);
            break;
        case 0x01ED:
            XORR(5, 5);
            break;
        case 0x01EE:
            XORR(5, 6);
            break;
        case 0x01EF:
            XORR(5, 7);
            break;
        case 0x01F0:
            XORR(6, 0);
            break;
        case 0x01F1:
            XORR(6, 1);
            break;
        case 0x01F2:
            XORR(6, 2);
            break;
        case 0x01F3:
            XORR(6, 3);
            break;
        case 0x01F4:
            XORR(6, 4);
            break;
        case 0x01F5:
            XORR(6, 5);
            break;
        case 0x01F6:
            XORR(6, 6);
            break;
        case 0x01F7:
            XORR(6, 7);
            break;
        case 0x01F8:
            XORR(7, 0);
            break;
        case 0x01F9:
            XORR(7, 1);
            break;
        case 0x01FA:
            XORR(7, 2);
            break;
        case 0x01FB:
            XORR(7, 3);
            break;
        case 0x01FC:
            XORR(7, 4);
            break;
        case 0x01FD:
            XORR(7, 5);
            break;
        case 0x01FE:
            XORR(7, 6);
            break;
        case 0x01FF:
            XORR(7, 7);
            break;
        case 0x0200:
            B(memoryBus->peek(r[7]+1));
            break;
        case 0x0201:
            BC(memoryBus->peek(r[7]+1));
            break;
        case 0x0202:
            BOV(memoryBus->peek(r[7]+1));
            break;
        case 0x0203:
            BPL(memoryBus->peek(r[7]+1));
            break;
        case 0x0204:
            BEQ(memoryBus->peek(r[7]+1));
            break;
        case 0x0205:
            BLT(memoryBus->peek(r[7]+1));
            break;
        case 0x0206:
            BLE(memoryBus->peek(r[7]+1));
            break;
        case 0x0207:
            BUSC(memoryBus->peek(r[7]+1));
            break;
        case 0x0208:
            NOPP(memoryBus->peek(r[7]+1));
            break;
        case 0x0209:
            BNC(memoryBus->peek(r[7]+1));
            break;
        case 0x020A:
            BNOV(memoryBus->peek(r[7]+1));
            break;
        case 0x020B:
            BMI(memoryBus->peek(r[7]+1));
            break;
        case 0x020C:
            BNEQ(memoryBus->peek(r[7]+1));
            break;
        case 0x020D:
            BGE(memoryBus->peek(r[7]+1));
            break;
        case 0x020E:
            BGT(memoryBus->peek(r[7]+1));
            break;
        case 0x020F:
            BESC(memoryBus->peek(r[7]+1));
            break;
        case 0x0210:
            BEXT(0, memoryBus->peek(r[7]+1));
            break;
        case 0x0211:
            BEXT(1, memoryBus->peek(r[7]+1));
            break;
        case 0x0212:
            BEXT(2, memoryBus->peek(r[7]+1));
            break;
        case 0x0213:
            BEXT(3, memoryBus->peek(r[7]+1));
            break;
        case 0x0214:
            BEXT(4, memoryBus->peek(r[7]+1));
            break;
        case 0x0215:
            BEXT(5, memoryBus->peek(r[7]+1));
            break;
        case 0x0216:
            BEXT(6, memoryBus->peek(r[7]+1));
            break;
        case 0x0217:
            BEXT(7, memoryBus->peek(r[7]+1));
            break;
        case 0x0218:
            BEXT(8, memoryBus->peek(r[7]+1));
            break;
        case 0x0219:
            BEXT(9, memoryBus->peek(r[7]+1));
            break;
        case 0x021A:
            BEXT(10, memoryBus->peek(r[7]+1));
            break;
        case 0x021B:
            BEXT(11, memoryBus->peek(r[7]+1));
            break;
        case 0x021C:
            BEXT(12, memoryBus->peek(r[7]+1));
            break;
        case 0x021D:
            BEXT(13, memoryBus->peek(r[7]+1));
            break;
        case 0x021E:
            BEXT(14, memoryBus->peek(r[7]+1));
            break;
        case 0x021F:
            BEXT(15, memoryBus->peek(r[7]+1));
            break;
        case 0x0220:
            B(-memoryBus->peek(r[7]+1)-1);
            break;
        case 0x0221:
            BC(-memoryBus->peek(r[7]+1)-1);
            break;
        case 0x0222:
            BOV(-memoryBus->peek(r[7]+1)-1);
            break;
        case 0x0223:
            BPL(-memoryBus->peek(r[7]+1)-1);
            break;
        case 0x0224:
            BEQ(-memoryBus->peek(r[7]+1)-1);
            break;
        case 0x0225:
            BLT(-memoryBus->peek(r[7]+1)-1);
            break;
        case 0x0226:
            BLE(-memoryBus->peek(r[7]+1)-1);
            break;
        case 0x0227:
            BUSC(-memoryBus->peek(r[7]+1)-1);
            break;
        case 0x0228:
            NOPP(-memoryBus->peek(r[7]+1)-1);
            break;
        case 0x0229:
            BNC(-memoryBus->peek(r[7]+1)-1);
            break;
        case 0x022A:
            BNOV(-memoryBus->peek(r[7]+1)-1);
            break;
        case 0x022B:
            BMI(-memoryBus->peek(r[7]+1)-1);
            break;
        case 0x022C:
            BNEQ(-memoryBus->peek(r[7]+1)-1);
            break;
        case 0x022D:
            BGE(-memoryBus->peek(r[7]+1)-1);
            break;
        case 0x022E:
            BGT(-memoryBus->peek(r[7]+1)-1);
            break;
        case 0x022F:
            BESC(-memoryBus->peek(r[7]+1)-1);
            break;
        case 0x0230:
            BEXT(0, -memoryBus->peek(r[7]+1)-1);
            break;
        case 0x0231:
            BEXT(1, -memoryBus->peek(r[7]+1)-1);
            break;
        case 0x0232:
            BEXT(2, -memoryBus->peek(r[7]+1)-1);
            break;
        case 0x0233:
            BEXT(3, -memoryBus->peek(r[7]+1)-1);
            break;
        case 0x0234:
            BEXT(4, -memoryBus->peek(r[7]+1)-1);
            break;
        case 0x0235:
            BEXT(5, -memoryBus->peek(r[7]+1)-1);
            break;
        case 0x0236:
            BEXT(6, -memoryBus->peek(r[7]+1)-1);
            break;
        case 0x0237:
            BEXT(7, -memoryBus->peek(r[7]+1)-1);
            break;
        case 0x0238:
            BEXT(8, -memoryBus->peek(r[7]+1)-1);
            break;
        case 0x0239:
            BEXT(9, -memoryBus->peek(r[7]+1)-1);
            break;
        case 0x023A:
            BEXT(10, -memoryBus->peek(r[7]+1)-1);
            break;
        case 0x023B:
            BEXT(11, -memoryBus->peek(r[7]+1)-1);
            break;
        case 0x023C:
            BEXT(12, -memoryBus->peek(r[7]+1)-1);
            break;
        case 0x023D:
            BEXT(13, -memoryBus->peek(r[7]+1)-1);
            break;
        case 0x023E:
            BEXT(14, -memoryBus->peek(r[7]+1)-1);
            break;
        case 0x023F:
            BEXT(15, -memoryBus->peek(r[7]+1)-1);
            break;
        case 0x0240:
            MVO(0, memoryBus->peek(r[7]+1));
            break;
        case 0x0241:
            MVO(1, memoryBus->peek(r[7]+1));
            break;
        case 0x0242:
            MVO(2, memoryBus->peek(r[7]+1));
            break;
        case 0x0243:
            MVO(3, memoryBus->peek(r[7]+1));
            break;
        case 0x0244:
            MVO(4, memoryBus->peek(r[7]+1));
            break;
        case 0x0245:
            MVO(5, memoryBus->peek(r[7]+1));
            break;
        case 0x0246:
            MVO(6, memoryBus->peek(r[7]+1));
            break;
        case 0x0247:
            MVO(7, memoryBus->peek(r[7]+1));
            break;
        case 0x0248:
            MVO_ind(1, 0);
            break;
        case 0x0249:
            MVO_ind(1, 1);
            break;
        case 0x024A:
            MVO_ind(1, 2);
            break;
        case 0x024B:
            MVO_ind(1, 3);
            break;
        case 0x024C:
            MVO_ind(1, 4);
            break;
        case 0x024D:
            MVO_ind(1, 5);
            break;
        case 0x024E:
            MVO_ind(1, 6);
            break;
        case 0x024F:
            MVO_ind(1, 7);
            break;
        case 0x0250:
            MVO_ind(2, 0);
            break;
        case 0x0251:
            MVO_ind(2, 1);
            break;
        case 0x0252:
            MVO_ind(2, 2);
            break;
        case 0x0253:
            MVO_ind(2, 3);
            break;
        case 0x0254:
            MVO_ind(2, 4);
            break;
        case 0x0255:
            MVO_ind(2, 5);
            break;
        case 0x0256:
            MVO_ind(2, 6);
            break;
        case 0x0257:
            MVO_ind(2, 7);
            break;
        case 0x0258:
            MVO_ind(3, 0);
            break;
        case 0x0259:
            MVO_ind(3, 1);
            break;
        case 0x025A:
            MVO_ind(3, 2);
            break;
        case 0x025B:
            MVO_ind(3, 3);
            break;
        case 0x025C:
            MVO_ind(3, 4);
            break;
        case 0x025D:
            MVO_ind(3, 5);
            break;
        case 0x025E:
            MVO_ind(3, 6);
            break;
        case 0x025F:
            MVO_ind(3, 7);
            break;
        case 0x0260:
            MVO_ind(4, 0);
            break;
        case 0x0261:
            MVO_ind(4, 1);
            break;
        case 0x0262:
            MVO_ind(4, 2);
            break;
        case 0x0263:
            MVO_ind(4, 3);
            break;
        case 0x0264:
            MVO_ind(4, 4);
            break;
        case 0x0265:
            MVO_ind(4, 5);
            break;
        case 0x0266:
            MVO_ind(4, 6);
            break;
        case 0x0267:
            MVO_ind(4, 7);
            break;
        case 0x0268:
            MVO_ind(5, 0);
            break;
        case 0x0269:
            MVO_ind(5, 1);
            break;
        case 0x026A:
            MVO_ind(5, 2);
            break;
        case 0x026B:
            MVO_ind(5, 3);
            break;
        case 0x026C:
            MVO_ind(5, 4);
            break;
        case 0x026D:
            MVO_ind(5, 5);
            break;
        case 0x026E:
            MVO_ind(5, 6);
            break;
        case 0x026F:
            MVO_ind(5, 7);
            break;
        case 0x0270:
            MVO_ind(6, 0);
            break;
        case 0x0271:
            MVO_ind(6, 1);
            break;
        case 0x0272:
            MVO_ind(6, 2);
            break;
        case 0x0273:
            MVO_ind(6, 3);
            break;
        case 0x0274:
            MVO_ind(6, 4);
            break;
        case 0x0275:
            MVO_ind(6, 5);
            break;
        case 0x0276:
            MVO_ind(6, 6);
            break;
        case 0x0277:
            MVO_ind(6, 7);
            break;
        case 0x0278:
            MVO_ind(7, 0);
            break;
        case 0x0279:
            MVO_ind(7, 1);
            break;
        case 0x027A:
            MVO_ind(7, 2);
            break;
        case 0x027B:
            MVO_ind(7, 3);
            break;
        case 0x027C:
            MVO_ind(7, 4);
            break;
        case 0x027D:
            MVO_ind(7, 5);
            break;
        case 0x027E:
            MVO_ind(7, 6);
            break;
        case 0x027F:
            MVO_ind(7, 7);
            break;
        case 0x0280:
            MVI(memoryBus->peek(r[7]+1), 0);
            break;
        case 0x0281:
            MVI(memoryBus->peek(r[7]+1), 1);
            break;
        case 0x0282:
            MVI(memoryBus->peek(r[7]+1), 2);
            break;
        case 0x0283:
            MVI(memoryBus->peek(r[7]+1), 3);
            break;
        case 0x0284:
            MVI(memoryBus->peek(r[7]+1), 4);
            break;
        case 0x0285:
            MVI(memoryBus->peek(r[7]+1), 5);
            break;
        case 0x0286:
            MVI(memoryBus->peek(r[7]+1), 6);
            break;
        case 0x0287:
            MVI(memoryBus->peek(r[7]+1), 7);
            break;
        case 0x0288:
            MVI_ind(1, 0);
            break;
        case 0x0289:
            MVI_ind(1, 1);
            break;
        case 0x028A:
            MVI_ind(1, 2);
            break;
        case 0x028B:
            MVI_ind(1, 3);
            break;
        case 0x028C:
            MVI_ind(1, 4);
            break;
        case 0x028D:
            MVI_ind(1, 5);
            break;
        case 0x028E:
            MVI_ind(1, 6);
            break;
        case 0x028F:
            MVI_ind(1, 7);
            break;
        case 0x0290:
            MVI_ind(2, 0);
            break;
        case 0x0291:
            MVI_ind(2, 1);
            break;
        case 0x0292:
            MVI_ind(2, 2);
            break;
        case 0x0293:
            MVI_ind(2, 3);
            break;
        case 0x0294:
            MVI_ind(2, 4);
            break;
        case 0x0295:
            MVI_ind(2, 5);
            break;
        case 0x0296:
            MVI_ind(2, 6);
            break;
        case 0x0297:
            MVI_ind(2, 7);
            break;
        case 0x0298:
            MVI_ind(3, 0);
            break;
        case 0x0299:
            MVI_ind(3, 1);
            break;
        case 0x029A:
            MVI_ind(3, 2);
            break;
        case 0x029B:
            MVI_ind(3, 3);
            break;
        case 0x029C:
            MVI_ind(3, 4);
            break;
        case 0x029D:
            MVI_ind(3, 5);
            break;
        case 0x029E:
            MVI_ind(3, 6);
            break;
        case 0x029F:
            MVI_ind(3, 7);
            break;
        case 0x02A0:
            MVI_ind(4, 0);
            break;
        case 0x02A1:
            MVI_ind(4, 1);
            break;
        case 0x02A2:
            MVI_ind(4, 2);
            break;
        case 0x02A3:
            MVI_ind(4, 3);
            break;
        case 0x02A4:
            MVI_ind(4, 4);
            break;
        case 0x02A5:
            MVI_ind(4, 5);
            break;
        case 0x02A6:
            MVI_ind(4, 6);
            break;
        case 0x02A7:
            MVI_ind(4, 7);
            break;
        case 0x02A8:
            MVI_ind(5, 0);
            break;
        case 0x02A9:
            MVI_ind(5, 1);
            break;
        case 0x02AA:
            MVI_ind(5, 2);
            break;
        case 0x02AB:
            MVI_ind(5, 3);
            break;
        case 0x02AC:
            MVI_ind(5, 4);
            break;
        case 0x02AD:
            MVI_ind(5, 5);
            break;
        case 0x02AE:
            MVI_ind(5, 6);
            break;
        case 0x02AF:
            MVI_ind(5, 7);
            break;
        case 0x02B0:
            MVI_ind(6, 0);
            break;
        case 0x02B1:
            MVI_ind(6, 1);
            break;
        case 0x02B2:
            MVI_ind(6, 2);
            break;
        case 0x02B3:
            MVI_ind(6, 3);
            break;
        case 0x02B4:
            MVI_ind(6, 4);
            break;
        case 0x02B5:
            MVI_ind(6, 5);
            break;
        case 0x02B6:
            MVI_ind(6, 6);
            break;
        case 0x02B7:
            MVI_ind(6, 7);
            break;
        case 0x02B8:
            MVI_ind(7, 0);
            break;
        case 0x02B9:
            MVI_ind(7, 1);
            break;
        case 0x02BA:
            MVI_ind(7, 2);
            break;
        case 0x02BB:
            MVI_ind(7, 3);
            break;
        case 0x02BC:
            MVI_ind(7, 4);
            break;
        case 0x02BD:
            MVI_ind(7, 5);
            break;
        case 0x02BE:
            MVI_ind(7, 6);
            break;
        case 0x02BF:
            MVI_ind(7, 7);
            break;
        case 0x02C0:
            ADD(memoryBus->peek(r[7]+1), 0);
            break;
        case 0x02C1:
            ADD(memoryBus->peek(r[7]+1), 1);
            break;
        case 0x02C2:
            ADD(memoryBus->peek(r[7]+1), 2);
            break;
        case 0x02C3:
            ADD(memoryBus->peek(r[7]+1), 3);
            break;
        case 0x02C4:
            ADD(memoryBus->peek(r[7]+1), 4);
            break;
        case 0x02C5:
            ADD(memoryBus->peek(r[7]+1), 5);
            break;
        case 0x02C6:
            ADD(memoryBus->peek(r[7]+1), 6);
            break;
        case 0x02C7:
            ADD(memoryBus->peek(r[7]+1), 7);
            break;
        case 0x02C8:
            ADD_ind(1, 0);
            break;
        case 0x02C9:
            ADD_ind(1, 1);
            break;
        case 0x02CA:
            ADD_ind(1, 2);
            break;
        case 0x02CB:
            ADD_ind(1, 3);
            break;
        case 0x02CC:
            ADD_ind(1, 4);
            break;
        case 0x02CD:
            ADD_ind(1, 5);
            break;
        case 0x02CE:
            ADD_ind(1, 6);
            break;
        case 0x02CF:
            ADD_ind(1, 7);
            break;
        case 0x02D0:
            ADD_ind(2, 0);
            break;
        case 0x02D1:
            ADD_ind(2, 1);
            break;
        case 0x02D2:
            ADD_ind(2, 2);
            break;
        case 0x02D3:
            ADD_ind(2, 3);
            break;
        case 0x02D4:
            ADD_ind(2, 4);
            break;
        case 0x02D5:
            ADD_ind(2, 5);
            break;
        case 0x02D6:
            ADD_ind(2, 6);
            break;
        case 0x02D7:
            ADD_ind(2, 7);
            break;
        case 0x02D8:
            ADD_ind(3, 0);
            break;
        case 0x02D9:
            ADD_ind(3, 1);
            break;
        case 0x02DA:
            ADD_ind(3, 2);
            break;
        case 0x02DB:
            ADD_ind(3, 3);
            break;
        case 0x02DC:
            ADD_ind(3, 4);
            break;
        case 0x02DD:
            ADD_ind(3, 5);
            break;
        case 0x02DE:
            ADD_ind(3, 6);
            break;
        case 0x02DF:
            ADD_ind(3, 7);
            break;
        case 0x02E0:
            ADD_ind(4, 0);
            break;
        case 0x02E1:
            ADD_ind(4, 1);
            break;
        case 0x02E2:
            ADD_ind(4, 2);
            break;
        case 0x02E3:
            ADD_ind(4, 3);
            break;
        case 0x02E4:
            ADD_ind(4, 4);
            break;
        case 0x02E5:
            ADD_ind(4, 5);
            break;
        case 0x02E6:
            ADD_ind(4, 6);
            break;
        case 0x02E7:
            ADD_ind(4, 7);
            break;
        case 0x02E8:
            ADD_ind(5, 0);
            break;
        case 0x02E9:
            ADD_ind(5, 1);
            break;
        case 0x02EA:
            ADD_ind(5, 2);
            break;
        case 0x02EB:
            ADD_ind(5, 3);
            break;
        case 0x02EC:
            ADD_ind(5, 4);
            break;
        case 0x02ED:
            ADD_ind(5, 5);
            break;
        case 0x02EE:
            ADD_ind(5, 6);
            break;
        case 0x02EF:
            ADD_ind(5, 7);
            break;
        case 0x02F0:
            ADD_ind(6, 0);
            break;
        case 0x02F1:
            ADD_ind(6, 1);
            break;
        case 0x02F2:
            ADD_ind(6, 2);
            break;
        case 0x02F3:
            ADD_ind(6, 3);
            break;
        case 0x02F4:
            ADD_ind(6, 4);
            break;
        case 0x02F5:
            ADD_ind(6, 5);
            break;
        case 0x02F6:
            ADD_ind(6, 6);
            break;
        case 0x02F7:
            ADD_ind(6, 7);
            break;
        case 0x02F8:
            ADD_ind(7, 0);
            break;
        case 0x02F9:
            ADD_ind(7, 1);
            break;
        case 0x02FA:
            ADD_ind(7, 2);
            break;
        case 0x02FB:
            ADD_ind(7, 3);
            break;
        case 0x02FC:
            ADD_ind(7, 4);
            break;
        case 0x02FD:
            ADD_ind(7, 5);
            break;
        case 0x02FE:
            ADD_ind(7, 6);
            break;
        case 0x02FF:
            ADD_ind(7, 7);
            break;
        case 0x0300:
            SUB(memoryBus->peek(r[7]+1), 0);
            break;
        case 0x0301:
            SUB(memoryBus->peek(r[7]+1), 1);
            break;
        case 0x0302:
            SUB(memoryBus->peek(r[7]+1), 2);
            break;
        case 0x0303:
            SUB(memoryBus->peek(r[7]+1), 3);
            break;
        case 0x0304:
            SUB(memoryBus->peek(r[7]+1), 4);
            break;
        case 0x0305:
            SUB(memoryBus->peek(r[7]+1), 5);
            break;
        case 0x0306:
            SUB(memoryBus->peek(r[7]+1), 6);
            break;
        case 0x0307:
            SUB(memoryBus->peek(r[7]+1), 7);
            break;
        case 0x0308:
            SUB_ind(1, 0);
            break;
        case 0x0309:
            SUB_ind(1, 1);
            break;
        case 0x030A:
            SUB_ind(1, 2);
            break;
        case 0x030B:
            SUB_ind(1, 3);
            break;
        case 0x030C:
            SUB_ind(1, 4);
            break;
        case 0x030D:
            SUB_ind(1, 5);
            break;
        case 0x030E:
            SUB_ind(1, 6);
            break;
        case 0x030F:
            SUB_ind(1, 7);
            break;
        case 0x0310:
            SUB_ind(2, 0);
            break;
        case 0x0311:
            SUB_ind(2, 1);
            break;
        case 0x0312:
            SUB_ind(2, 2);
            break;
        case 0x0313:
            SUB_ind(2, 3);
            break;
        case 0x0314:
            SUB_ind(2, 4);
            break;
        case 0x0315:
            SUB_ind(2, 5);
            break;
        case 0x0316:
            SUB_ind(2, 6);
            break;
        case 0x0317:
            SUB_ind(2, 7);
            break;
        case 0x0318:
            SUB_ind(3, 0);
            break;
        case 0x0319:
            SUB_ind(3, 1);
            break;
        case 0x031A:
            SUB_ind(3, 2);
            break;
        case 0x031B:
            SUB_ind(3, 3);
            break;
        case 0x031C:
            SUB_ind(3, 4);
            break;
        case 0x031D:
            SUB_ind(3, 5);
            break;
        case 0x031E:
            SUB_ind(3, 6);
            break;
        case 0x031F:
            SUB_ind(3, 7);
            break;
        case 0x0320:
            SUB_ind(4, 0);
            break;
        case 0x0321:
            SUB_ind(4, 1);
            break;
        case 0x0322:
            SUB_ind(4, 2);
            break;
        case 0x0323:
            SUB_ind(4, 3);
            break;
        case 0x0324:
            SUB_ind(4, 4);
            break;
        case 0x0325:
            SUB_ind(4, 5);
            break;
        case 0x0326:
            SUB_ind(4, 6);
            break;
        case 0x0327:
            SUB_ind(4, 7);
            break;
        case 0x0328:
            SUB_ind(5, 0);
            break;
        case 0x0329:
            SUB_ind(5, 1);
            break;
        case 0x032A:
            SUB_ind(5, 2);
            break;
        case 0x032B:
            SUB_ind(5, 3);
            break;
        case 0x032C:
            SUB_ind(5, 4);
            break;
        case 0x032D:
            SUB_ind(5, 5);
            break;
        case 0x032E:
            SUB_ind(5, 6);
            break;
        case 0x032F:
            SUB_ind(5, 7);
            break;
        case 0x0330:
            SUB_ind(6, 0);
            break;
        case 0x0331:
            SUB_ind(6, 1);
            break;
        case 0x0332:
            SUB_ind(6, 2);
            break;
        case 0x0333:
            SUB_ind(6, 3);
            break;
        case 0x0334:
            SUB_ind(6, 4);
            break;
        case 0x0335:
            SUB_ind(6, 5);
            break;
        case 0x0336:
            SUB_ind(6, 6);
            break;
        case 0x0337:
            SUB_ind(6, 7);
            break;
        case 0x0338:
            SUB_ind(7, 0);
            break;
        case 0x0339:
            SUB_ind(7, 1);
            break;
        case 0x033A:
            SUB_ind(7, 2);
            break;
        case 0x033B:
            SUB_ind(7, 3);
            break;
        case 0x033C:
            SUB_ind(7, 4);
            break;
        case 0x033D:
            SUB_ind(7, 5);
            break;
        case 0x033E:
            SUB_ind(7, 6);
            break;
        case 0x033F:
            SUB_ind(7, 7);
            break;
        case 0x0340:
            CMP(memoryBus->peek(r[7]+1), 0);
            break;
        case 0x0341:
            CMP(memoryBus->peek(r[7]+1), 1);
            break;
        case 0x0342:
            CMP(memoryBus->peek(r[7]+1), 2);
            break;
        case 0x0343:
            CMP(memoryBus->peek(r[7]+1), 3);
            break;
        case 0x0344:
            CMP(memoryBus->peek(r[7]+1), 4);
            break;
        case 0x0345:
            CMP(memoryBus->peek(r[7]+1), 5);
            break;
        case 0x0346:
            CMP(memoryBus->peek(r[7]+1), 6);
            break;
        case 0x0347:
            CMP(memoryBus->peek(r[7]+1), 7);
            break;
        case 0x0348:
            CMP_ind(1, 0);
            break;
        case 0x0349:
            CMP_ind(1, 1);
            break;
        case 0x034A:
            CMP_ind(1, 2);
            break;
        case 0x034B:
            CMP_ind(1, 3);
            break;
        case 0x034C:
            CMP_ind(1, 4);
            break;
        case 0x034D:
            CMP_ind(1, 5);
            break;
        case 0x034E:
            CMP_ind(1, 6);
            break;
        case 0x034F:
            CMP_ind(1, 7);
            break;
        case 0x0350:
            CMP_ind(2, 0);
            break;
        case 0x0351:
            CMP_ind(2, 1);
            break;
        case 0x0352:
            CMP_ind(2, 2);
            break;
        case 0x0353:
            CMP_ind(2, 3);
            break;
        case 0x0354:
            CMP_ind(2, 4);
            break;
        case 0x0355:
            CMP_ind(2, 5);
            break;
        case 0x0356:
            CMP_ind(2, 6);
            break;
        case 0x0357:
            CMP_ind(2, 7);
            break;
        case 0x0358:
            CMP_ind(3, 0);
            break;
        case 0x0359:
            CMP_ind(3, 1);
            break;
        case 0x035A:
            CMP_ind(3, 2);
            break;
        case 0x035B:
            CMP_ind(3, 3);
            break;
        case 0x035C:
            CMP_ind(3, 4);
            break;
        case 0x035D:
            CMP_ind(3, 5);
            break;
        case 0x035E:
            CMP_ind(3, 6);
            break;
        case 0x035F:
            CMP_ind(3, 7);
            break;
        case 0x0360:
            CMP_ind(4, 0);
            break;
        case 0x0361:
            CMP_ind(4, 1);
            break;
        case 0x0362:
            CMP_ind(4, 2);
            break;
        case 0x0363:
            CMP_ind(4, 3);
            break;
        case 0x0364:
            CMP_ind(4, 4);
            break;
        case 0x0365:
            CMP_ind(4, 5);
            break;
        case 0x0366:
            CMP_ind(4, 6);
            break;
        case 0x0367:
            CMP_ind(4, 7);
            break;
        case 0x0368:
            CMP_ind(5, 0);
            break;
        case 0x0369:
            CMP_ind(5, 1);
            break;
        case 0x036A:
            CMP_ind(5, 2);
            break;
        case 0x036B:
            CMP_ind(5, 3);
            break;
        case 0x036C:
            CMP_ind(5, 4);
            break;
        case 0x036D:
            CMP_ind(5, 5);
            break;
        case 0x036E:
            CMP_ind(5, 6);
            break;
        case 0x036F:
            CMP_ind(5, 7);
            break;
        case 0x0370:
            CMP_ind(6, 0);
            break;
        case 0x0371:
            CMP_ind(6, 1);
            break;
        case 0x0372:
            CMP_ind(6, 2);
            break;
        case 0x0373:
            CMP_ind(6, 3);
            break;
        case 0x0374:
            CMP_ind(6, 4);
            break;
        case 0x0375:
            CMP_ind(6, 5);
            break;
        case 0x0376:
            CMP_ind(6, 6);
            break;
        case 0x0377:
            CMP_ind(6, 7);
            break;
        case 0x0378:
            CMP_ind(7, 0);
            break;
        case 0x0379:
            CMP_ind(7, 1);
            break;
        case 0x037A:
            CMP_ind(7, 2);
            break;
        case 0x037B:
            CMP_ind(7, 3);
            break;
        case 0x037C:
            CMP_ind(7, 4);
            break;
        case 0x037D:
            CMP_ind(7, 5);
            break;
        case 0x037E:
            CMP_ind(7, 6);
            break;
        case 0x037F:
            CMP_ind(7, 7);
            break;
        case 0x0380:
            AND(memoryBus->peek(r[7]+1), 0);
            break;
        case 0x0381:
            AND(memoryBus->peek(r[7]+1), 1);
            break;
        case 0x0382:
            AND(memoryBus->peek(r[7]+1), 2);
            break;
        case 0x0383:
            AND(memoryBus->peek(r[7]+1), 3);
            break;
        case 0x0384:
            AND(memoryBus->peek(r[7]+1), 4);
            break;
        case 0x0385:
            AND(memoryBus->peek(r[7]+1), 5);
            break;
        case 0x0386:
            AND(memoryBus->peek(r[7]+1), 6);
            break;
        case 0x0387:
            AND(memoryBus->peek(r[7]+1), 7);
            break;
        case 0x0388:
            AND_ind(1, 0);
            break;
        case 0x0389:
            AND_ind(1, 1);
            break;
        case 0x038A:
            AND_ind(1, 2);
            break;
        case 0x038B:
            AND_ind(1, 3);
            break;
        case 0x038C:
            AND_ind(1, 4);
            break;
        case 0x038D:
            AND_ind(1, 5);
            break;
        case 0x038E:
            AND_ind(1, 6);
            break;
        case 0x038F:
            AND_ind(1, 7);
            break;
        case 0x0390:
            AND_ind(2, 0);
            break;
        case 0x0391:
            AND_ind(2, 1);
            break;
        case 0x0392:
            AND_ind(2, 2);
            break;
        case 0x0393:
            AND_ind(2, 3);
            break;
        case 0x0394:
            AND_ind(2, 4);
            break;
        case 0x0395:
            AND_ind(2, 5);
            break;
        case 0x0396:
            AND_ind(2, 6);
            break;
        case 0x0397:
            AND_ind(2, 7);
            break;
        case 0x0398:
            AND_ind(3, 0);
            break;
        case 0x0399:
            AND_ind(3, 1);
            break;
        case 0x039A:
            AND_ind(3, 2);
            break;
        case 0x039B:
            AND_ind(3, 3);
            break;
        case 0x039C:
            AND_ind(3, 4);
            break;
        case 0x039D:
            AND_ind(3, 5);
            break;
        case 0x039E:
            AND_ind(3, 6);
            break;
        case 0x039F:
            AND_ind(3, 7);
            break;
        case 0x03A0:
            AND_ind(4, 0);
            break;
        case 0x03A1:
            AND_ind(4, 1);
            break;
        case 0x03A2:
            AND_ind(4, 2);
            break;
        case 0x03A3:
            AND_ind(4, 3);
            break;
        case 0x03A4:
            AND_ind(4, 4);
            break;
        case 0x03A5:
            AND_ind(4, 5);
            break;
        case 0x03A6:
            AND_ind(4, 6);
            break;
        case 0x03A7:
            AND_ind(4, 7);
            break;
        case 0x03A8:
            AND_ind(5, 0);
            break;
        case 0x03A9:
            AND_ind(5, 1);
            break;
        case 0x03AA:
            AND_ind(5, 2);
            break;
        case 0x03AB:
            AND_ind(5, 3);
            break;
        case 0x03AC:
            AND_ind(5, 4);
            break;
        case 0x03AD:
            AND_ind(5, 5);
            break;
        case 0x03AE:
            AND_ind(5, 6);
            break;
        case 0x03AF:
            AND_ind(5, 7);
            break;
        case 0x03B0:
            AND_ind(6, 0);
            break;
        case 0x03B1:
            AND_ind(6, 1);
            break;
        case 0x03B2:
            AND_ind(6, 2);
            break;
        case 0x03B3:
            AND_ind(6, 3);
            break;
        case 0x03B4:
            AND_ind(6, 4);
            break;
        case 0x03B5:
            AND_ind(6, 5);
            break;
        case 0x03B6:
            AND_ind(6, 6);
            break;
        case 0x03B7:
            AND_ind(6, 7);
            break;
        case 0x03B8:
            AND_ind(7, 0);
            break;
        case 0x03B9:
            AND_ind(7, 1);
            break;
        case 0x03BA:
            AND_ind(7, 2);
            break;
        case 0x03BB:
            AND_ind(7, 3);
            break;
        case 0x03BC:
            AND_ind(7, 4);
            break;
        case 0x03BD:
            AND_ind(7, 5);
            break;
        case 0x03BE:
            AND_ind(7, 6);
            break;
        case 0x03BF:
            AND_ind(7, 7);
            break;
        case 0x03C0:
            XOR(memoryBus->peek(r[7]+1), 0);
            break;
        case 0x03C1:
            XOR(memoryBus->peek(r[7]+1), 1);
            break;
        case 0x03C2:
            XOR(memoryBus->peek(r[7]+1), 2);
            break;
        case 0x03C3:
            XOR(memoryBus->peek(r[7]+1), 3);
            break;
        case 0x03C4:
            XOR(memoryBus->peek(r[7]+1), 4);
            break;
        case 0x03C5:
            XOR(memoryBus->peek(r[7]+1), 5);
            break;
        case 0x03C6:
            XOR(memoryBus->peek(r[7]+1), 6);
            break;
        case 0x03C7:
            XOR(memoryBus->peek(r[7]+1), 7);
            break;
        case 0x03C8:
            XOR_ind(1, 0);
            break;
        case 0x03C9:
            XOR_ind(1, 1);
            break;
        case 0x03CA:
            XOR_ind(1, 2);
            break;
        case 0x03CB:
            XOR_ind(1, 3);
            break;
        case 0x03CC:
            XOR_ind(1, 4);
            break;
        case 0x03CD:
            XOR_ind(1, 5);
            break;
        case 0x03CE:
            XOR_ind(1, 6);
            break;
        case 0x03CF:
            XOR_ind(1, 7);
            break;
        case 0x03D0:
            XOR_ind(2, 0);
            break;
        case 0x03D1:
            XOR_ind(2, 1);
            break;
        case 0x03D2:
            XOR_ind(2, 2);
            break;
        case 0x03D3:
            XOR_ind(2, 3);
            break;
        case 0x03D4:
            XOR_ind(2, 4);
            break;
        case 0x03D5:
            XOR_ind(2, 5);
            break;
        case 0x03D6:
            XOR_ind(2, 6);
            break;
        case 0x03D7:
            XOR_ind(2, 7);
            break;
        case 0x03D8:
            XOR_ind(3, 0);
            break;
        case 0x03D9:
            XOR_ind(3, 1);
            break;
        case 0x03DA:
            XOR_ind(3, 2);
            break;
        case 0x03DB:
            XOR_ind(3, 3);
            break;
        case 0x03DC:
            XOR_ind(3, 4);
            break;
        case 0x03DD:
            XOR_ind(3, 5);
            break;
        case 0x03DE:
            XOR_ind(3, 6);
            break;
        case 0x03DF:
            XOR_ind(3, 7);
            break;
        case 0x03E0:
            XOR_ind(4, 0);
            break;
        case 0x03E1:
            XOR_ind(4, 1);
            break;
        case 0x03E2:
            XOR_ind(4, 2);
            break;
        case 0x03E3:
            XOR_ind(4, 3);
            break;
        case 0x03E4:
            XOR_ind(4, 4);
            break;
        case 0x03E5:
            XOR_ind(4, 5);
            break;
        case 0x03E6:
            XOR_ind(4, 6);
            break;
        case 0x03E7:
            XOR_ind(4, 7);
            break;
        case 0x03E8:
            XOR_ind(5, 0);
            break;
        case 0x03E9:
            XOR_ind(5, 1);
            break;
        case 0x03EA:
            XOR_ind(5, 2);
            break;
        case 0x03EB:
            XOR_ind(5, 3);
            break;
        case 0x03EC:
            XOR_ind(5, 4);
            break;
        case 0x03ED:
            XOR_ind(5, 5);
            break;
        case 0x03EE:
            XOR_ind(5, 6);
            break;
        case 0x03EF:
            XOR_ind(5, 7);
            break;
        case 0x03F0:
            XOR_ind(6, 0);
            break;
        case 0x03F1:
            XOR_ind(6, 1);
            break;
        case 0x03F2:
            XOR_ind(6, 2);
            break;
        case 0x03F3:
            XOR_ind(6, 3);
            break;
        case 0x03F4:
            XOR_ind(6, 4);
            break;
        case 0x03F5:
            XOR_ind(6, 5);
            break;
        case 0x03F6:
            XOR_ind(6, 6);
            break;
        case 0x03F7:
            XOR_ind(6, 7);
            break;
        case 0x03F8:
            XOR_ind(7, 0);
            break;
        case 0x03F9:
            XOR_ind(7, 1);
            break;
        case 0x03FA:
            XOR_ind(7, 2);
            break;
        case 0x03FB:
            XOR_ind(7, 3);
            break;
        case 0x03FC:
            XOR_ind(7, 4);
            break;
        case 0x03FD:
            XOR_ind(7, 5);
            break;
        case 0x03FE:
            XOR_ind(7, 6);
            break;
        case 0x03FF:
        default:
            XOR_ind(7, 7);
            break;
    }
}

