
#ifndef AY38914_H
#define AY38914_H

#include "AY38914_Registers.h"
#include "AY38914_Channel.h"
#include "PSGController.h"
#include "../core/AudioOutputLine.h"
#include "../core/MemoryAlias.h"
#include "../core/Processor.h"
#include "../core/osd/types.h"

class Intellivision;

/**
 * The AY-3-8914 chip in the Intellivision, also know as the Programmable
 * Sound Generator (PSG).
 *
 * @author Kyle Davis
 */
class AY38914 : public Processor
{

    friend class AY38914_Registers;
    friend class Intellivision;

    public:
        AY38914();
        void init(AudioOutputLine* aol, INT32 location, PSGController* io0,
                PSGController* io1);
        INT32 getClockSpeed();
        void reset();
        INT32 tick();

        void setClockDivisor(INT32 clockDivisor);
        INT32 getClockDivisor();
        void setPSGController0(PSGController* io0);
        void setPSGController1(PSGController* io1);

    private:
        AudioOutputLine*    aol;
        PSGController*   psgIO0;
        PSGController*   psgIO1;

        //divisor for slowing down the clock speed for the AY38914
        INT32 clockDivisor;

        //registers and channels
        AY38914_Registers      registers;
        AY38914_Channel        channel0;
        AY38914_Channel        channel1;
        AY38914_Channel        channel2;

        //cached total output sample
        BOOL  cachedTotalOutputIsDirty;
        INT32 cachedTotalOutput;

        //envelope data
        BOOL  envelopeIdle;
        INT32 envelopePeriod;
        INT32 envelopePeriodValue;
        INT32 envelopeCounter;
        INT32 envelopeVolume;
        BOOL  envelopeHold;
        BOOL  envelopeAltr;
        BOOL  envelopeAtak;
        BOOL  envelopeCont;

        //noise data
        BOOL  noiseIdle;
        INT32 noisePeriod;
        INT32 noisePeriodValue;
        INT32 noiseCounter;

        //data for random number generator, used for white noise accuracy
        INT32 random;
        BOOL  noise;

        //output amplitudes for a single channel
        static const INT32 amplitudes16Bit[16];

};

#endif

