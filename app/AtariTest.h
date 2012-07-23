
#ifndef ATARITEST_H
#define ATARITEST_H

#include <windows.h>
#include "../core/osd/types.h"

class AtariTest
{

    public:
        AtariTest();
        ~AtariTest();
        INT32 run(CHAR* lpCmdLine);

    private:
        UINT8*     biosImage;

        static const UINT32 CRC_BIOS_ROM;
        static const CHAR* FILENAME_BIOS_ROM;

};

#endif
