
#ifndef AUDIOMIXER_H
#define AUDIOMIXER_H

#include "AudioOutputLine.h"
#include "Processor.h"
#include "osd/AudioOutputDevice.h"
#include "osd/types.h"

const INT32 MAX_LINES = 10;

class AudioMixer : public Processor
{

    friend class ProcessorBus;
    friend class AudioOutputLine;

    public:
        AudioMixer();
        void reset();
        void setClockSpeed(INT32);
        INT32 getClockSpeed();
        INT32 tick();
        void flushAudio(AudioOutputDevice* aod);
        void createLine(AudioOutputLine** line);
        void releaseLine(AudioOutputLine** line);

    private:
        UINT64             clock;

        BOOL               highQualityMixing;
        INT32              outputSampleRate;
        UINT16             sampleBuffer[4410];
        INT32              sampleCount;

        AudioOutputLine*   lines[MAX_LINES];
        INT32              lineCount;

};

#endif
