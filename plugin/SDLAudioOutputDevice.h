#ifndef SDLAUDIOOUTPUTDEVICE_H
#define SDLAUDIOOUTPUTDEVICE_H

#include "../core/osd/AudioOutputDevice.h"
#include <SDL.h>


class SDLAudioOutputDevice : public AudioOutputDevice
{
    public:
        SDLAudioOutputDevice();
        ~SDLAudioOutputDevice();
    
        const CHAR* getErrorDescription(ERRCODE errorCode);
    
        ERRCODE init();
        void    release();
    
        int     getSampleRate() { return 44100; }
        void    playSamples(UINT16* sampleBuffer, UINT16 sample);
    
    private:
        SDL_AudioSpec m_desired;
        SDL_AudioSpec m_obtained;
};


#endif
