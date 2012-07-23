
#include "AudioOutputLine.h"
#include "AudioMixer.h"

AudioOutputLine::AudioOutputLine(AudioMixer* mixer)
{
    this->mixer = mixer;
    reset();
}

void AudioOutputLine::reset()
{
    sampleBuffer = 0;
    currentSample = 0;
    lastWriteClockMark = 0;
}

void AudioOutputLine::acquire()
{
    mixer->lines[mixer->lineCount] = this;
    mixer->lineCount++;
}

void AudioOutputLine::release()
{
    for (INT32 i = 0; i < mixer->lineCount; i++) {
        if (mixer->lines[i] == this) {
            for (INT32 j = i+1; j < mixer->lineCount-1; j++)
                mixer->lines[j] = mixer->lines[j+1];
            mixer->lineCount--;
            return;
        }
    }
}

void AudioOutputLine::playSample(UINT16 sample)
{
    if (mixer->highQualityMixing) {
        sampleBuffer += ((INT64)currentSample) *
                (INT64)(mixer->clock - lastWriteClockMark);
        lastWriteClockMark = mixer->clock;
    }
    currentSample = sample;
}



