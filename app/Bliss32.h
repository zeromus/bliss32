
#ifndef BLISS32_H
#define BLISS32_H

#include <windows.h>
#include "../core/container.h"
#include "../plugin/SDLVideoOutputDevice.h"
#include "../plugin/SDLAudioOutputDevice.h"
#include "../plugin/SDLInputDevice.h"
#include "../plugin/Win32ClockDevice.h"
#include "../intellivision/Intellivision.h"

#define MAX_GAMES 512

using namespace std;

class Bliss32
{

    public:
        Bliss32();
        ~Bliss32();

        INT32 run(int argc, char** argv);
        INT32 loadSystemROMs();
        INT32 getProgramDir(CHAR* sdir, UINT32 maxLen);

        void showTitleScreen(VideoOutputDevice* pvod, Win32ClockDevice* pcd,
					unsigned int time);

        void scanROMDirectory_Intv(ListCtl* ctl);
        void clearCartList_Intv();

    private:
        UINT16*     execImage;
        UINT16*     gromImage;
        UINT16*     ecsImage;
        UINT16*     ivoiceImage;

        static const UINT32 CRC_EXECUTIVE_ROM;
        static const UINT32 CRC_GROM;
        static const UINT32 CRC_ECS_ROM;
        static const UINT32 CRC_IVOICE_ROM;
        static const CHAR* FILENAME_EXECUTIVE_ROM;
        static const CHAR* FILENAME_GROM;
        static const CHAR* FILENAME_ECS_ROM;
        static const CHAR* FILENAME_IVOICE_ROM;

        char romdir_intv[_MAX_DIR];
        char romdir_5200[_MAX_DIR];

        ListItem itmIntvGame[MAX_GAMES];
};

#endif
