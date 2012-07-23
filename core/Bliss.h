
#ifndef BLISS_H
#define BLISS_H

#include <windows.h>
#include <vector>
#include "container.h"
#include "Emulator.h"
#include "osd/VideoOutputDevice.h"
#include "osd/AudioOutputDevice.h"
#include "osd/InputDevice.h"
#include "osd/ClockDevice.h"

using namespace std;


// Menu callbacks
//void changeGame(void *data);
void changeMenu(void *data);
void changeRez(void *data);
void closeMenu(void *userdata);
void closeEmu(void *userdata);

class Bliss
{

    public:
        Bliss();
        ~Bliss();
        void init(VideoOutputDevice*, AudioOutputDevice*, InputDevice*,
                ClockDevice*);
        void setEmulator(Emulator* emu);
        INT32 run();

        void setDefaultMenu(Container* menu);
        void setFrameSkip(INT32 frameSkip);
        INT32 getFrameSkip();

    private:
        void doAction(INT32 actionCode);

        Emulator*   currentEmu;
        Container*  defaultMenu;
        BOOL paused;
        INT32       frameSkip;
        INT32       frameCounter;

        AudioOutputDevice* aod;
        InputDevice* id;
        ClockDevice* cd;

};

#endif
