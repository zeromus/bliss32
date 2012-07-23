
#ifndef PROCESSORBUS_H
#define PROCESSORBUS_H

#include "osd/types.h"
#include "Processor.h"
#include "AudioMixer.h"

const INT32 MAX_PROCESSORS = 15;

class ProcessorBus
{

    public:
        ProcessorBus();
        void setAudioMixer(AudioMixer* am);
        void addProcessor(Processor* p);
        void removeProcessor(Processor* p);
        void reset();
        void tick();

    private:
        AudioMixer* audioMixer;
        Processor*  processors[MAX_PROCESSORS];
        INT64       processorTickFactors[MAX_PROCESSORS];
        INT64       processorTicks[MAX_PROCESSORS];
        BOOL        processorsIdle[MAX_PROCESSORS];
        INT32       processorCount;

};

UINT64 lcm(UINT64 a, UINT64 b);
UINT64 gcd(UINT64 m, UINT64 n);

#endif
