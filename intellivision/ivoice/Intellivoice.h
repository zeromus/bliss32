
#ifndef INTELLIVOICE_H
#define INTELLIVOICE_H

#include "MicroSequencer.h"
#include "LPC12.h"
#include "../../core/AudioOutputLine.h"
#include "../../core/Memory.h"
#include "../../core/Processor.h"
#include "../../core/osd/types.h"

class Intellivoice
{

    public:
        Intellivoice();
        Intellivoice(AudioOutputLine* aol);
        void init(AudioOutputLine* aol);
        INT32 getProcessorCount();
        void getProcessor(INT32 i, Processor** p);
        INT32 getMemoryCount();
        void getMemory(INT32 i, Memory16Bit** m);
        void setIntellivoiceImage(UINT16* image);
        BOOL hasIntellivoiceImage();

    //private:
        MicroSequencer microSequencer;
        LPC12          lpc12;
        BOOL           hasIvoiceImage;

};

#endif
