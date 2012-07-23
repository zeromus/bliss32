
#ifndef AUDIOOUTPUTDEVICE_H
#define AUDIOOUTPUTDEVICE_H

#include "Device.h"

#define ERR_INIT_AUDIO	1
#define ERR_OPEN_AUDIO	2

class AudioOutputDevice : public Device
{

    public:
        virtual int getSampleRate() = 0;
        virtual void playSamples(UINT16* sampleBuffer, UINT16 sampleCount) = 0;

};

#endif
