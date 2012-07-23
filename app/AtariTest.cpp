
#include <windows.h>
#include <iostream>
#include "AtariTest.h"
#include "../atari5200/Atari5200.h"
#include "../core/cartridge/CRC32.h"
#include "../core/ROM.h"
#include "../core/std_file.h"

#pragma warning(disable:4786)	// Suppress STL debug info > 255 chars messages
using namespace std;

const UINT32 AtariTest::CRC_BIOS_ROM              = 0x4248D3E3;
const CHAR* AtariTest::FILENAME_BIOS_ROM          = "5200.rom";

/*
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
        LPSTR lpCmdLine, int nCmdShow)
*/
/*
int main(int argc, char** argv)
{
    if (argc != 2) {
        cout << "Usage:  Atari <cart_filename>\n";
        return -1;
    }

    AtariTest atariTest;
    //return atariTest.run(lpCmdLine);
    return atariTest.run((argc > 1 ? argv[1] : ""));
}
*/
AtariTest::AtariTest()
{
    biosImage = NULL;
}

INT32 AtariTest::run(CHAR* lpCmdLine) {
    CHAR currentDir[MAX_PATH];
    GetModuleFileName(NULL, currentDir, MAX_PATH);
    INT32 len = strlen(currentDir);
    while (currentDir[len] != '\\')
        len--;
    currentDir[len+1] = 0;
    CHAR nextFile[MAX_PATH];

    //form the BIOS rom filename
    strcpy(nextFile, currentDir);
    strcat(nextFile, "\\");
    strcat(nextFile, FILENAME_BIOS_ROM);

	//verify that the BIOS rom exists.
	if(!fileExists(nextFile)) {
        cout << "ERROR: BIOS ROM file '5200.rom' not found\n";
		return -1;
    }

    //verify the CRC of the BIOS rom file
    UINT32 crc = CRC32::getCrc(nextFile);
    if (crc != CRC_BIOS_ROM) {
        cout << "ERROR: File '5200.rom' does not contain the correct BIOS ROM\n";
        return -1;
    }

    //load the BIOS rom file
    UINT32 size;
    ROM8Bit::loadROMImage(&biosImage, &size, nextFile, FALSE);

    cout << "Successfully loaded Atari 5200 BIOS\n";

    Atari5200 atari5200;
    cout << "Created emulator\n";

    atari5200.setBIOSImage(biosImage);
    cout << "Set the BIOS image: " << size << "\n";

/*
    Win32PlugIn plugIn;
    atari5200.setPlugIn(&plugIn);
*/
    cout << "Set the plug-in\n";

    strcpy(nextFile, currentDir);
    strcat(nextFile, "\\");
    strcat(nextFile, lpCmdLine);
    if (!fileExists(nextFile)) {
        cout << "ERROR: File '" << lpCmdLine << "' not found\n";
        return -1;
    }

    UINT8* cartImage;
    ROM8Bit::loadROMImage(&cartImage, &size, nextFile, FALSE);
    if (size > 32768) {
        cout << "ERROR: Cartridge image is too big.  Must be less than 32K.\n";
        delete[] cartImage;
        return -1;
    }

    ROM8Bit cartROM(cartImage, size, 0xC000-size);
    atari5200.insertCartridge(&cartROM);
    cout << "Inserted the cartridge: " << size << "\n";

    //plugIn.init();
    cout << "Initialized the plug-in\n";

    atari5200.run();

    //delete the cartridge image
    delete[] cartImage;

    //plugIn.release();
    cout << "Released the plug-in\n";

    return 0;
}

AtariTest::~AtariTest() {
    //clean up
    if (biosImage != NULL)
        delete[] biosImage;
}
