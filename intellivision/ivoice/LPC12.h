
#ifndef LPC12_H
#define LPC12_H

#include "../../core/osd/types.h"
#include "../../core/AudioOutputLine.h"
#include "../../core/Processor.h"

class Intellivoice;
class MicroSequencer;

class LPC12 : public Processor
{

    friend class Intellivoice;
    friend class MicroSequencer;

    public:
        INT32 getClockSpeed();
        void reset();
        INT32 tick();
/*
        void save(SaveOutputStream sos) throws IOException;
        void load(LoadInputStream lis) throws IOException;
*/

    private:
        LPC12();
        LPC12(AudioOutputLine* aol);
        void init(AudioOutputLine* aol);
        AudioOutputLine* aol;
        MicroSequencer*  microSequencer;
        INT32              random;

        //registers
        INT32 repeat;
        INT32 period;
        INT32 periodCounter;
        INT32 amplitude;
        INT8  b[6];
        INT8  f[6];
        INT32 y[6][2];
        INT8  periodInterpolation;
        INT8  amplitudeInterpolation;

        static const INT32 qtbl[256];

};

#endif
