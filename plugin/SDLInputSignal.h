#ifndef SDLINPUTSIGNAL_H
#define SDLINPUTSIGNAL_H

#include <SDL.h>
#include "../core/osd/types.h"

class SDLInputSignal
{
    friend class SDLInputDevice;

    public:
        const CHAR* getDescription();
        const CHAR* getConfigKey();

    private:
        SDLInputSignal(UINT64, SDLKey, const CHAR*);
        SDLInputSignal(const SDLInputSignal&);
        ~SDLInputSignal();
    
        CHAR*  description;
        CHAR   configKey[256];
        UINT64 deviceID;
        SDLKey controlID;
};

#endif
