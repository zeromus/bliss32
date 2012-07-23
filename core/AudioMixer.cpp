
#include <string.h>
#include "AudioMixer.h"
#include "AudioOutputLine.h"

AudioMixer::AudioMixer()
{
    memset(lines, 0, sizeof(lines));
    lineCount = 0;
    outputSampleRate = 0;
    highQualityMixing = TRUE;

    memset(sampleBuffer, 0, sizeof(sampleBuffer));
    sampleCount = 0;

    clock = 1;
}

void AudioMixer::createLine(AudioOutputLine** line) {
    (*line) = new AudioOutputLine(this);
}

void AudioMixer::releaseLine(AudioOutputLine** line) {
    delete (*line);
}

void AudioMixer::reset()
{
    memset(sampleBuffer, 0, sizeof(sampleBuffer));
    sampleCount = 0;

    clock = 1;
}

void AudioMixer::setClockSpeed(INT32 cs)
{
    outputSampleRate = cs;
}

INT32 AudioMixer::getClockSpeed() {
    return outputSampleRate;
}

INT32 AudioMixer::tick() {
    INT32 totalSample = 0;

    //mix and flush the sample buffers
    for (INT32 i = 0; i < lineCount; i++) {
        AudioOutputLine* nextLine = lines[i];

        //calculate the sample for this line
        if (highQualityMixing) {
            nextLine->sampleBuffer += ((INT64)nextLine->currentSample) *
                    (INT64)(clock - nextLine->lastWriteClockMark);
            totalSample += (INT32)(nextLine->sampleBuffer / clock);
        }
        else
            totalSample += nextLine->currentSample;

        //clear the sample buffer for this line
        nextLine->sampleBuffer = 0;
        nextLine->lastWriteClockMark = 0;
    }
    clock = 1;

    totalSample = totalSample / lineCount;

    //apply the saturation clipping to correctly model the
    //cross-channel modulation that occurs on a real Intellivision
    totalSample <<= 1;
    if (totalSample > 0x6000)
        totalSample = 0x6000 + ((totalSample - 0x6000)/6);

    sampleBuffer[sampleCount] = totalSample;
    sampleCount++;

    return 1;
}

void AudioMixer::flushAudio(AudioOutputDevice* aod)
{
    aod->playSamples(sampleBuffer, sampleCount);
    sampleCount = 0;
}

