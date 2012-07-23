
#ifndef MICROSEQUENCER_H
#define MICROSEQUENCER_H

#include "../../core/osd/types.h"
#include "MicroSequencer_Registers.h"
#include "../../core/Processor.h"
#include "../../core/ROM.h"

class Intellivoice;
class LPC12;

class MicroSequencer : public Processor
{

    friend class Intellivoice;
    friend class MicroSequencer_Registers;

    public:
        INT32 getClockSpeed();
        void reset();
        INT32 tick();
/*
        void save(SaveOutputStream sos) throws IOException;
        void load(LoadInputStream lis) throws IOException;
*/

        MicroSequencer_Registers registers;

    private:
        MicroSequencer();
        MicroSequencer(LPC12* lpc12);
        void init(LPC12* lpc12);
        INT8 readDelta(INT32 numBits);
        INT32 readBits(INT32 numBits);
        INT32 readBits(INT32 numBits, BOOL reverseOrder);
        void RTS();
        void SETPAGE(INT32 immed4);
        void LOADALL(INT32 immed4);
        void LOAD_2(INT32 immed4);
        void SETMSB_3(INT32 immed4);
        void LOAD_4(INT32 immed4);
        void SETMSB_5(INT32 immed4);
        void SETMSB_6(INT32 immed4);
        void JMP(INT32 immed4);
        void SETMODE(INT32 immed4);
        void DELTA_9(INT32 immed4);
        void SETMSB_A(INT32 immed4);
        void JSR(INT32 immed4);
        void LOAD_C(INT32 immed4);
        void DELTA_D(INT32 immed4);
        void LOAD_E(INT32 immed4);
        void PAUSE(INT32 immed4);
        void decode();
        static INT32 flipEndian(INT32 value, INT32 bits);

        INT32 bitsLeft;
        INT32 currentBits;

        //registers
        INT32 pc;
        INT32 stack;
        INT32 mode;
        INT32 repeatPrefix;
        INT32 page;
        INT32 command;

        LPC12*                   lpc12;
        ROM16Bit           ivoiceROM;
        BOOL                     lrqHigh;
        BOOL                     speaking;
        INT32                    fifoBytes[64];
        INT32                    fifoHead;
        INT32                    fifoSize;

        static const INT32 bitMasks[16];
        static const INT32 FIFO_LOCATION;
        static const INT32 FIFO_MAX_SIZE;
        static const INT32 FIFO_END;

};

#endif
