
#ifndef EMULATOR_H
#define EMULATOR_H

#include "AudioMixer.h"
#include "ControllerBus.h"
#include "ProcessorBus.h"
#include "VideoProducer.h"
#include "osd/InputDevice.h"
#include "osd/VideoOutputDevice.h"
#include "osd/AudioOutputDevice.h"

class Emulator
{
    public:
        void setOutputSampleRate(UINT16 sampleRate);
        void getOutputImageSize(UINT16* width, UINT16* height);
        void getPalette(UINT32** palette, UINT8* numEntries);
        void reset();
        void updateControllers(InputDevice* id);
        void run();
        void flushAudio(AudioOutputDevice* aod);
        void render(VideoOutputDevice* vod);

    protected:
        Emulator();
        void setVideoProducer(VideoProducer* vp);

        AudioMixer         audioMixer;
        ControllerBus      controllerBus;
        ProcessorBus       processorBus;

    private:
        VideoProducer*     videoProducer;

};

#endif
