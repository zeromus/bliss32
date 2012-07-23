
#ifndef CP1610_H
#define CP1610_H

#include "../core/MemoryBus.h"
#include "../core/Processor.h"
#include "../core/SignalLine.h"

class CP1610 : public Processor
{

    public:
        CP1610();
        void init(MemoryBus16Bit* m, SignalLine* intrmLine,
                SignalLine* busRqLine, SignalLine* busAkLine,
                INT32 resetAddress, INT32 interruptAddress);
        INT32 getClockSpeed();
        void reset();
        INT32 tick();
/*
        void save(SaveOutputStream os) throws IOException;
        void load(LoadInputStream is) throws IOException;
*/

    private:
        void setIndirect(INT32 register, INT32 value);
        INT32 getIndirect(INT32 register);
        void HLT();
        void SDBD();
        void EIS();
        void DIS();
        void TCI();
        void CLRC();
        void SETC();
        void J(INT32 target);
        void JSR(INT32 register, INT32 target);
        void JE(INT32 target);
        void JSRE(INT32 register, INT32 target);
        void JD(INT32 target);
        void JSRD(INT32 register, INT32 target);
        void INCR(INT32 register);
        void DECR(INT32 register);
        void COMR(INT32 register);
        void NEGR(INT32 register);
        void ADCR(INT32 register);
        void RSWD(INT32 register);
        void GSWD(INT32 register);
        void NOP(INT32 twoOption);
        void SIN(INT32 twoOption);
        void SWAP(INT32 shift, INT32 register);
        void SLL(INT32 shift, INT32 register);
        void RLC(INT32 shift, INT32 register);
        void SLLC(INT32 shift, INT32 register);
        void SLR(INT32 shift, INT32 register);
        void SAR(INT32 shift, INT32 register);
        void RRC(INT32 shift, INT32 register);
        void SARC(INT32 shift, INT32 register);
        void MOVR(INT32 sourceReg, INT32 destReg);
        void ADDR(INT32 sourceReg, INT32 destReg);
        void SUBR(INT32 sourceReg, INT32 destReg);
        void CMPR(INT32 sourceReg, INT32 destReg);
        void ANDR(INT32 sourceReg, INT32 destReg);
        void XORR(INT32 sourceReg, INT32 destReg);
        void BEXT(INT32 condition, INT32 displacement);
        void B(INT32 displacement);
        void NOPP(INT32 displacement);
        void BC(INT32 displacement);
        void BNC(INT32 displacement);
        void BOV(INT32 displacement);
        void BNOV(INT32 displacement);
        void BPL(INT32 displacement);
        void BMI(INT32 displacement);
        void BEQ(INT32 displacement);
        void BNEQ(INT32 displacement);
        void BLT(INT32 displacement);
        void BGE(INT32 displacement);
        void BLE(INT32 displacement);
        void BGT(INT32 displacement);
        void BUSC(INT32 displacement);
        void BESC(INT32 displacement);
        void MVO(INT32 register, INT32 address);
        void MVO_ind(INT32 registerWithAddress, INT32 registerToMove);
        void MVI(INT32 address, INT32 register);
        void MVI_ind(INT32 registerWithAddress, INT32 registerToReceive);
        void ADD(INT32 address, INT32 register);
        void ADD_ind(INT32 registerWithAddress, INT32 registerToReceive);
        void SUB(INT32 address, INT32 register);
        void SUB_ind(INT32 registerWithAddress, INT32 registerToReceive);
        void CMP(INT32 address, INT32 register);
        void CMP_ind(INT32 registerWithAddress, INT32 registerToReceive);
        void AND(INT32 address, INT32 register);
        void AND_ind(INT32 registerWithAddress, INT32 registerToReceive);
        void XOR(INT32 address, INT32 register);
        void XOR_ind(INT32 registerWithAddress, INT32 registerToReceive);
        void decode(INT32 op);

        //the mory bus
        MemoryBus16Bit* memoryBus;

        //signal lines
        SignalLine* intrmLine;
        SignalLine* busRqLine;
        SignalLine* busAkLine;
    
        //interrupt address
        INT32 interruptAddress;

        //reset address
        INT32 resetAddress;

        //the eight registers available in the CP1610
        INT32 r[8];

        //the six flags available in the CP1610
        BOOL S, Z, O, C, I, D;

        //indicates whether the last executed instruction is interruptible
        BOOL interruptible;

        //cycles used during each op
        INT32 usedCycles;

        //the four external lines
        INT8 ext;

};

#endif
