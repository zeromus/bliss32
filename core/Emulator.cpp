
#include <iostream>
#include "Emulator.h"

using namespace std;

Emulator::Emulator()
{
    videoProducer = NULL;
    processorBus.setAudioMixer(&audioMixer);
    processorBus.addProcessor(&audioMixer);
}

void Emulator::reset()
{
    processorBus.reset();
    controllerBus.reset();
    audioMixer.reset();
}

void Emulator::setOutputSampleRate(UINT16 sampleRate)
{
    audioMixer.setClockSpeed(sampleRate);
}

void Emulator::getOutputImageSize(UINT16* width, UINT16* height)
{
    videoProducer->getOutputImageSize(width, height);
}

void Emulator::getPalette(UINT32** palette, UINT8* numEntries)
{
    videoProducer->getPalette(palette, numEntries);
}

void Emulator::run()
{
    while (videoProducer->inVerticalBlank())
        processorBus.tick();
    while (!videoProducer->inVerticalBlank())
        processorBus.tick();
}

void Emulator::updateControllers(InputDevice* inputDevice)
{
    controllerBus.updateControllers(inputDevice);
}

void Emulator::flushAudio(AudioOutputDevice* audioOutputDevice)
{
    audioMixer.flushAudio(audioOutputDevice);
}

void Emulator::render(VideoOutputDevice* videoOutputDevice)
{
    videoProducer->render(videoOutputDevice);
}

void Emulator::setVideoProducer(VideoProducer* vp)
{
    this->videoProducer = vp;
}

