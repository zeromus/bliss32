#ifndef SDLINPUTDEVICE_H
#define SDLINPUTDEVICE_H

#include "../core/osd/InputDevice.h"
#include "SDLInputSignal.h"
#include <SDL.h>

#define MAX_JOYSTICKS  8
#define MAX_BUTTONS   24
#define MAX_AXES       4

class SDLInputDevice : public InputDevice
{
    public:
        SDLInputDevice();
        ~SDLInputDevice();

        const CHAR* getErrorDescription(ERRCODE errorCode);

        ERRCODE init();
        void release();

        //input remapping
        inline BOOL isRemappable() { return FALSE; }
        inline BOOL remapControl(INT32 inputID) { return FALSE; }
        inline const CHAR* getControlMapping(INT32 inputID) { return NULL; }
        inline void setControlMapping(INT32 inputID, const CHAR* mapping) { }

        // Input reading....
        void poll();
//        bool altPressed();
        float getControlValue(INT32 inputID);

    private:
        float getJoystickSignalValue(INT32 joystickNum, INT32 inputCode);

        SDLInputSignal** m_pMappedSignals;
        SDL_Joystick*    joy[MAX_JOYSTICKS];
        INT32            m_iNumJoysticks;
        UINT8*           m_KeyStates;
};


#endif
