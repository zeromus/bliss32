// ****************************************************************************
// * Bliss:  Video Game System Emulator
// *           Created by Kyle Davis
// *
// *           Kyle Davis    - Lead Programmer, Design, etc.
// *           Jesse Litton  - Additional programming/debugging
// *
// ****************************************************************************

#include "Bliss.h"
#include <commctrl.h>
#include "cartridge/CRC32.h"
#include "ROM.h"
#include "std_file.h"

using namespace std;
#pragma warning(disable:4786)    // Suppress STL debug info > 255 chars messages


VideoOutputDevice* f_vod;


// Menu callbacks
Container*  currentMenu;
Container*  previousMenu;
void changeMenu(void *lItem)
{
    if(lItem) {
        previousMenu = currentMenu;
        currentMenu = (Container*)((PListItem)lItem)->data;
        currentMenu->m_parent = previousMenu;
    }
    else {
        currentMenu = NULL;
    }
}

void changeRez(void *data)
{
    char* cLine = (char*)((PListItem)data)->data;

    unsigned int x, y;
    int nParms = sscanf(cLine, "%ux%u", &x, &y);
    if(nParms == 2)
        f_vod->changeOutputSize(x, y);
}

void closeMenu(void *data)
{
    currentMenu = NULL;
}

bool emuClosed = false;
void closeEmu(void *data)
{
    emuClosed = true;
}


Bliss::Bliss()
{
    currentMenu = previousMenu = NULL;
    currentEmu = NULL;
    frameSkip = 1;
    frameCounter = 0;
    paused = FALSE;
    f_vod = NULL;
    aod = NULL;
    id = NULL;
    cd = NULL;
}

Bliss::~Bliss()
{
}

void Bliss::init(VideoOutputDevice* invod, AudioOutputDevice* aod,
        InputDevice* id, ClockDevice* cd)
{
//    this->vod = vod;
    f_vod = invod;

    this->aod = aod;
    this->id = id;
    this->cd = cd;
}

void Bliss::setEmulator(Emulator* emu)
{
    this->currentEmu = emu;
}

void Bliss::setDefaultMenu(Container* menu)
{
    this->defaultMenu = menu;
}

void Bliss::setFrameSkip(INT32 frameSkip)
{
    this->frameSkip = frameSkip;
}

INT32 Bliss::getFrameSkip()
{
    return frameSkip;
}

INT32 Bliss::run()
{
    UINT32* palette;
    UINT8 numEntries;
    currentEmu->getPalette(&palette, &numEntries);
    f_vod->setPalette(palette, numEntries);

    UINT16 width;
    UINT16 height;
    currentEmu->getOutputImageSize(&width, &height);
    f_vod->changeInputSize(width, height);

    currentEmu->setOutputSampleRate(aod->getSampleRate());

    currentEmu->reset();

	INT64 now = cd->getTick();
	INT64 inputSpeed = cd->getTickFrequency() * .2;	// 1/5 sec

    while (true) {
        id->poll();

        if (id->getControlValue(EMULATOR_PAUSE)) {
            paused = !paused;
        }
        else if (id->getControlValue(EMULATOR_RESET)) {
            currentEmu->reset();
        }

        if (id->toggleRequested()) {
			f_vod->toggleScreen();
            now = cd->getTick() + 5000;
        }
        else {
        
            // menu input
            if(cd->getTick() > (now + inputSpeed)) {
                if (id->getControlValue(MENU_DISPLAY)) {
                    if(currentMenu)
                        currentMenu = currentMenu->m_parent;    // Back up one
                    else
                        currentMenu = defaultMenu;              // Main menu

                    now = cd->getTick();
                }
                else if(currentMenu) {    // Not escape, but menu up
                    if(id->getControlValue(MENU_SELECT) ) { //&& !id->altPressed()) {
                        currentMenu->processEvent(PRESS_COMPONENT);
                        now = cd->getTick();
                    }
                    else if(id->getControlValue(MENU_DOWN)) {
                        currentMenu->processEvent(NEXT_COMPONENT);
                        now = cd->getTick();
                    }
                    else if(id->getControlValue(MENU_UP)) {
                        currentMenu->processEvent(PREV_COMPONENT);
                        now = cd->getTick();
                    }
                    else if(id->getControlValue(MENU_PGDOWN)) {
                        currentMenu->processEvent(NEXT_PAGE);
                        now = cd->getTick();
                    }
                    else if(id->getControlValue(MENU_PGUP)) {
                        currentMenu->processEvent(PREV_PAGE);
                        now = cd->getTick();
                    }
                }
            }

            if (f_vod->stopRequested() || emuClosed)
                break;
        }

        if (!paused) {
            if(!currentMenu)
                currentEmu->updateControllers(id);

            currentEmu->run();
            currentEmu->flushAudio(aod);
        }

        frameCounter--;
        if (frameCounter < 0) {
            frameCounter = frameSkip;

            currentEmu->render(f_vod);
            if(currentMenu)
                currentMenu->render(f_vod);

            f_vod->present();
        }

    }

    return 0;
}
