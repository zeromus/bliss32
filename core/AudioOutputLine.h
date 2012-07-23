
#ifndef AUDIOOUTPUTLINE_H
#define AUDIOOUTPUTLINE_H

#include "osd/types.h"

class AudioOutputLine
{

    friend class AudioMixer;

    public:
        void playSample(UINT16 sample);
        void acquire();
        void release();

    private:
        AudioOutputLine(AudioMixer* mixer);
        AudioMixer* mixer;
        void reset();
        INT32             currentSample;
        INT64             lastWriteClockMark;
        INT64             sampleBuffer;

};

#endif


