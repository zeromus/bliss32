
#include <stdio.h>
#include <memory.h>
#include "SDLAudioOutputDevice.h"


#define WORKING_BUFFER_SIZE 4410

UINT8    samples[WORKING_BUFFER_SIZE];
UINT32    playbackPosition    = 0;
int       queueStart    = 0;
int       queueFillSize = 0;


// Text for error messages
const CHAR* audioErrorMessages[2] = {
    "Unable to initialize audio",
    "Unable to open audio device",
};


void audioCallback(void *unused, Uint8 *stream, int len)
{
#ifdef _TRACE_AUDIO
    fprintf(stderr, "::audioCallBack()\n");
#endif

    // Don't stick around in this loop forever, because when the
    // program is ended cleanup will not begin until this callback
    // has returned - but it won't end without this check because
    // nothing new is being added to the queue.
    if (!queueFillSize) {
        //fprintf(stderr, "no queueFillSize, exiting SDLAudioOutputDevice::playSamples()\n");
        return;
    }

//fprintf(stderr, "1) queueStart/FillSize = %u/%u\n", queueStart, queueFillSize);

    if (queueStart+queueFillSize > WORKING_BUFFER_SIZE) {
        int endChunk = WORKING_BUFFER_SIZE-queueStart;
        memcpy(stream, samples+queueStart, endChunk);
        memcpy(stream+endChunk, samples, queueFillSize-endChunk);
    }
    else
        memcpy(stream, samples+queueStart, queueFillSize);

    queueStart = (queueStart+queueFillSize)%WORKING_BUFFER_SIZE;
    queueFillSize = 0;

//fprintf(stderr, "2) queueStart/FillSize = %u/%u\n", queueStart, queueFillSize);

#ifdef _TRACE_AUDIO
    fprintf(stderr, "exiting ::audioCallBack()\n");
#endif

}


SDLAudioOutputDevice::SDLAudioOutputDevice()
{
    // Init all variables
    m_desired.freq        = 44100;
    m_desired.format    = AUDIO_S16LSB;
    m_desired.samples    = 2205;
    m_desired.channels    = 1;
    m_desired.userdata    = NULL;
    m_desired.callback    = audioCallback;

    // send emptiness to buffer
//    memset(samples, 0, WORKING_BUFFER_SIZE << 1);
//    queuePosition = WORKING_BUFFER_SIZE << 1;
}

SDLAudioOutputDevice::~SDLAudioOutputDevice()
{
    // Cleanup before shutdown
    release();
}


const CHAR* SDLAudioOutputDevice::getErrorDescription(ERRCODE errorCode)
{
    return audioErrorMessages[errorCode-1];
}


// init: Activate audio
ERRCODE SDLAudioOutputDevice::init()
{
    if(SDL_Init(SDL_INIT_AUDIO))
        return ERR_INIT_AUDIO;

    SDL_AudioSpec* desired    = &m_desired;
    SDL_AudioSpec* obtained    = &m_obtained;
    if ( SDL_OpenAudio(desired, obtained) < 0 )
        return ERR_OPEN_AUDIO;

    // TODO: CHECK TO SEE IF OBTAINED = DESIRED, AND ACCOUNT FOR DIFFERENCES!!!!

    SDL_PauseAudio(0);
    return OK;
}

// release: Clean up
void SDLAudioOutputDevice::release()
{
    SDL_PauseAudio(true);
    SDL_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void SDLAudioOutputDevice::playSamples(UINT16* sampleBuffer, UINT16 sampleCount)
{
#ifdef _TRACE_AUDIO
    fprintf(stderr, "SDLAudioOutputDevice::playSamples()\n");
#endif

    //wait for the necessary space to become available in the buffer
    int neededSpace = (sampleCount<<1);

    int queuePosition = (queueStart + queueFillSize)%WORKING_BUFFER_SIZE;
    for (UINT16 i = 0; i < sampleCount; i++) {

        while (queueFillSize >= WORKING_BUFFER_SIZE)
            SDL_Delay(1);

        samples[queuePosition] = (UINT8)(sampleBuffer[i] & 0x00FF);
        samples[queuePosition+1] = (UINT8)((sampleBuffer[i] & 0xFF00) >> 8);
        queueFillSize += 2;

        queuePosition += 2;
        if (queuePosition >= WORKING_BUFFER_SIZE)
            queuePosition = 0;
    }

#ifdef _TRACE_AUDIO
    fprintf(stderr, "exiting SDLAudioOutputDevice::playSamples()\n");
#endif
}
