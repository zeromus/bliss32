
#include <string.h>
#include "ProcessorBus.h"
#include "AudioMixer.h"

ProcessorBus::ProcessorBus()
{
    processorCount     = 0;
    memset(processors, 0, sizeof(processors));
    memset(processorTickFactors, 0, sizeof(processorTickFactors));
    memset(processorTicks, 0, sizeof(processorTicks));
    memset(processorsIdle, 0, sizeof(processorsIdle));
}

void ProcessorBus::setAudioMixer(AudioMixer* am)
{
    this->audioMixer = am;
}

void ProcessorBus::reset()
{
    INT32 i;
    UINT64 totalClockSpeed = (UINT64)processors[0]->getClockSpeed();
    for (i = 1; i < processorCount; i++) {
        totalClockSpeed = lcm(totalClockSpeed,
                ((UINT64)processors[i]->getClockSpeed()));
    }

    for (i = 0; i < processorCount; i++) {
        processors[i]->reset();
        processorsIdle[i] = FALSE;
        processorTicks[i] = 0;
        processorTickFactors[i] = (totalClockSpeed /
                ((UINT64)processors[i]->getClockSpeed()));
    }
}

void ProcessorBus::addProcessor(Processor* p)
{
    processors[processorCount] = p;
    processorCount++;
}

void ProcessorBus::removeProcessor(Processor* p)
{
    for (INT32 i = 0; i < processorCount; i++) {
        if (processors[i] == p) {
            for (INT32 j = i; j < (processorCount-1); j++)
                processors[j] = processors[j+1];
            processorCount--;
        }
    }
}

void ProcessorBus::tick()
{
    //determine the next tick delta
#ifdef _MSC_VER
	UINT64 tickDelta = 0xFFFFFFFFFFFFFFFF;
#else
	UINT64 tickDelta = 0xFFFFFFFFFFFFFFFFll;
#endif

    INT32 i;
    for (i = 0; i < processorCount; i++) {
        //wake up any processors that want to wake up
        processorsIdle[i] = (processorsIdle[i] && processors[i]->isIdle);

        //if the processor is not idle by this point, use it
        //to calculate the tick delta
        if (!processorsIdle[i] && (UINT64)processorTicks[i] < tickDelta)
            tickDelta = processorTicks[i];
    }

    //move the audio mixer clock forward by the tick delta amount
    audioMixer->clock += tickDelta;

    for (i = 0; i < processorCount; i++) {
        //skip this processor if it has been idled
        if (!processorsIdle[i]) {
            processorTicks[i] -= tickDelta;
 
            //if the clock just caught up to the processor, and
            //if the processor is not about to go idle, then run it
            if (processorTicks[i] == 0) {
                if (processors[i]->isIdle)
                    processorsIdle[i] = TRUE;
                else
{
                    processorTicks[i] = (((UINT64)processors[i]->tick()) *
                                processorTickFactors[i]);
//cout << "ticked: " << i << "   now at: " << ((INT32)processorTicks[i]) << "\n";
}
            }
        }
    }
}

UINT64 lcm(UINT64 a, UINT64 b) {
    //This is an implementation of Euclid's Algorithm for determining
    //the greatest common denominator of two numbers.
    UINT64 m = a;
    UINT64 n = b;
    UINT64 r = m % n;
    while (r != 0) {
        m = n;
        n = r;
        r = m % n;
    }
    UINT64 gcd = n;

    //lcm is determined from gcd through the following equation
    return (a/gcd)*b;
}
