#pragma warning(disable:4786)	// Suppress STL debug info > 255 chars messages

// ****************************************************************************
// * Bliss32:  Video Game System Emulator
// *           Created by Kyle Davis
// *
// *           Kyle Davis    - Lead Programmer, Design, etc.
// *           Jesse Litton  - Additional programming/debugging
// *
// ****************************************************************************

#include <time.h>

#include "Bliss32.h"
#include "SDL.h"
#include "config.h"
#include "../core/Bliss.h"
#include "../core/colors.h"
#include "../core/container.h"
#include "../core/font.h"
#include "../core/image.h"
#include "../core/std_file.h"
#include "../core/cartridge/CRC32.h"
#include "../intellivision/cartridge/IntellivisionCartridge.h"
#include "../intellivision/cartridge/IntellivisionCartridgeConfiguration.h"
#include "../intellivision/cartridge/IntellivisionRecognitionDatabase.h"

const UINT32 Bliss32::CRC_EXECUTIVE_ROM = 0xCBCE86F7;
const UINT32 Bliss32::CRC_GROM          = 0x683A4158;
const UINT32 Bliss32::CRC_ECS_ROM       = 0xEA790A06;
const UINT32 Bliss32::CRC_IVOICE_ROM    = 0x0DE7579D;
const CHAR* Bliss32::FILENAME_EXECUTIVE_ROM = "exec.bin";
const CHAR* Bliss32::FILENAME_GROM          = "grom.bin";
const CHAR* Bliss32::FILENAME_ECS_ROM       = "ecs.bin";
const CHAR* Bliss32::FILENAME_IVOICE_ROM    = "ivoice.bin";

Bliss bliss;

SDLVideoOutputDevice vod;
SDLAudioOutputDevice aod;
SDLInputDevice id;
Win32ClockDevice cd;

vector<IntellivisionCartridge*> cartridgeFileList;
IntellivisionRecognitionDatabase* pIrdb = &IntellivisionRecognitionDatabase::FACTORY;
char intvCartPath[MAX_GAMES][MAX_PATH];

IntellivisionCartridge* cart;
Intellivision inty;

char lastcart[MAX_PATH];


int main(int argc, char* argv[])
{
    Bliss32 bliss32;
    return bliss32.run(argc,argv);
}


void changeGame(void *lItem)
{
    inty.removeCartridge();
    delete cart;

    cart = IntellivisionCartridge::loadCartridge((char*)((PListItem)lItem)->data);
    inty.insertCartridge(cart);
    inty.reset();

    strcpy(lastcart, (char*)((PListItem)lItem)->data);

    changeMenu(NULL);
}


Bliss32::Bliss32()
{
    execImage = NULL;
    gromImage = NULL;
    ecsImage = NULL;
    ivoiceImage = NULL;
}

Bliss32::~Bliss32()
{
    if (execImage)
        delete[] execImage;
    if (gromImage)
        delete[] gromImage;
    if (ecsImage)
        delete[] ecsImage;
    if (ivoiceImage)
        delete[] ivoiceImage;
}


// This function just shows the title screen in a simple timed loop
void Bliss32::showTitleScreen(VideoOutputDevice* pvod, Win32ClockDevice* pcd,
										unsigned int time)
{  
	font neonFont(pvod, "NeonFont.png");
	image logoImage(pvod, "bliss.jpg", false);

    INT64 oneSec  = pcd->getTickFrequency();
    INT64 start   = pcd->getTick();
    INT64 finish  = start + (oneSec*time);

    UINT8  fade;
    INT64 fadein  = start;
    INT64 fadeout = finish;

    if(time > 2) {
        fadein += oneSec;
        fadeout -= oneSec;
    }


    while(INT64 time=pcd->getTick() < finish)
    {
        if(time > fadein && time < fadeout) {
            fade = float(time-start)/oneSec*255.0f;
        }
        else if(time > fadeout) {
            fade = float(time-fadeout)/oneSec*255.0f;
        }
        else {
            fade = 255;
        }

        pvod->setGamma(fade, fade, fade);
	    logoImage.draw(0, 0);
	    neonFont.write(555, 445, "v1.9");
	    pvod->present();

        id.poll();
        if(id.getControlValue(KEYBOARD_SPACE) ||
            id.getControlValue(KEYBOARD_ENTER) ||
            id.getControlValue(KEYBOARD_ESCAPE))
            break;
    }  
}

INT32 Bliss32::run(int argc, char **argv)
{
    Bliss bliss;

	// --------------------- Load title screen/mode preferences
	Config ini("bliss.ini");
	char cLine[32];
	unsigned int mode=0;

	//zero 23-jul-2012 - dont feel like wrestling with this now. get fullscreen out of the way
#ifdef ALLOW_FULLSCREEN
	if(ini.get("global", "mode", &cLine[0], 9)) {
		int nParms = sscanf(cLine, "%u", &mode);
		if(nParms == 1)
			vod.toggleScreen(mode);
	}
    else
    	vod.toggleScreen(MODE_FULLSCREEN);
#endif

	unsigned int splash=6;
	if(ini.get("global", "splashtime", &cLine[0], 9)) {
		int nParms = sscanf(cLine, "%u", &splash);
	}

    // Set size for title screen, this has to be done before
    // initializing the video, because SDL won't re-center yet.
    vod.setFullScreenSize(640, 480);
    vod.setWindowSize(640, 480);

    //init the video device
    INT32 result = vod.init();
    if (result)
        return result;

		//zero 23-jul-2012 - this is stupid
    //showTitleScreen(&vod, &cd, splash);  // Show title


	// --------------------- Now, load/set the preferred size settings
   	unsigned int x=320, y=200;  // Default fullscreen rez
	if(ini.get("global", "fullscreen", &cLine[0], 9)) {
		sscanf(cLine, "%ux%u", &x, &y);
	}
	vod.setFullScreenSize(x, y);
    if(!mode)
        vod.changeOutputSize(x, y);

    x=640; y=384;           // Default windowed rez
	if(ini.get("global", "windowed", &cLine[0], 9)) {
		sscanf(cLine, "%ux%u", &x, &y);
	}
	vod.setWindowSize(x, y);
    if(mode)
        vod.changeOutputSize(x, y);


	// --------------------- Load the ROM path
	if(!ini.get("intellivision", "rom_dir", romdir_intv, _MAX_DIR)) {
        strcpy(romdir_intv, "roms");
    }
	if(!ini.get("5200", "rom_dir", romdir_5200, _MAX_DIR)) {
        strcpy(romdir_5200, "roms");
    }


    // Menu Test......................................................
    font        fontGray(&vod, "SmallGray.png");
    font        fontSmallNeon(&vod, "SmallNeon.png");
    font        fontSmall(&vod, "smallstone.png");
    font        fontBlue14(&vod, "14P_Copperplate_LBlue.png");
    font        fontYellow14(&vod, "14P_Copperplate_Yellow.png");
    image       imageBack(&vod, "background2.png", true);
    ImageCtl    imgBack(&imageBack, 200, 0, 0);

    Container   menuMain(220, 180), menuLoad(220, 180), menuIntv(220, 180),
                menu5200(220, 180), menuVideo(220, 180), menuCredits(220, 180);

    // -------------- Main menu construction
    menuMain.addComponent(&imgBack);
    TextCtl  txtMainTitle(&fontGray, "Main Menu", 35, 4);
    menuMain.addComponent(&txtMainTitle);

    ListCtl  lstMain(&fontBlue14, &fontYellow14, 15, 44, 8);
    ListItem loadGame, videoSettings, credits, exitBliss;

    loadGame.callback = &changeMenu;
//    loadGame.data = &menuLoad;
    loadGame.data = &menuIntv;
    loadGame.description = "New Game";
    lstMain.addItem(&loadGame, false);

    videoSettings.callback = changeMenu;
    videoSettings.data = &menuVideo;
    videoSettings.description = "Video Settings";
    lstMain.addItem(&videoSettings, false);

    credits.callback = changeMenu;
    credits.data = &menuCredits;
    credits.description = "Credits";
    lstMain.addItem(&credits, false);
    
    exitBliss.callback = &closeEmu;
    exitBliss.description = "Exit Bliss32";
    lstMain.addItem(&exitBliss, false);

    menuMain.addComponent(&lstMain);

    // -------------- System menu construction
    menuLoad.addComponent(&imgBack);
    TextCtl  txtLoadTitle(&fontGray, "Load Game", 35, 4);
    menuLoad.addComponent(&txtLoadTitle);

    ListCtl  lstSystems(&fontBlue14, &fontYellow14, 15, 44, 6);
    ListItem itm5200, itmIntv;

    itm5200.callback    = &changeMenu;
    itm5200.data        = &menu5200;
    itm5200.description = "Atari 5200";
    lstSystems.addItem(&itm5200, false);

    itmIntv.callback = &changeMenu;
    itmIntv.data = &menuIntv;
    itmIntv.description = "Intellivision";
    lstSystems.addItem(&itmIntv, false);

    menuLoad.addComponent(&lstSystems);

    // -------------- Atari 5200 menu construction
    menu5200.addComponent(&imgBack);
    TextCtl  txt5200Title(&fontGray, "5200 Games", 29, 4);
    menu5200.addComponent(&txt5200Title);
    

    // -------------- Intellivision menu construction
    menuIntv.addComponent(&imgBack);
    TextCtl  txtIntvTitle(&fontGray, "Intellivision", 27, 4);
    menuIntv.addComponent(&txtIntvTitle);

    ListCtl  lstGames(&fontSmall, &fontSmallNeon, 15, 44, 8);
    lstGames.setMaxChars(21, 18);

    fontYellow14.write(2, 2, "Scanning ROMs...");
    vod.present();

    scanROMDirectory_Intv(&lstGames);

    menuIntv.addComponent(&lstGames);


    // -------------- Video menu construction
    menuVideo.addComponent(&imgBack);
    TextCtl   txtVideoTitle(&fontGray, "Video", 65, 4);
    menuVideo.addComponent(&txtVideoTitle);
 
    ListCtl  lstVideo(&fontBlue14, &fontYellow14, 15, 44, 6);
    ListItem itmVidRez[64];

    char cmode[64][16];
    unsigned int modeCount = 0, ex = 0, ey = 0;
    while(vod.enumModes(cmode[modeCount]) && modeCount < 64)
    {
		INT16 nParms = sscanf(cmode[modeCount], "%ux%u", &ex, &ey);
        if(nParms == 2 && ( ex <= 1024 && ey <= 768))
        {
            itmVidRez[modeCount].description = cmode[modeCount];
            itmVidRez[modeCount].callback = &changeRez;
            itmVidRez[modeCount].data = &cmode[modeCount][0];
            lstVideo.addItem(&itmVidRez[modeCount], true);
            modeCount++;
        }
    }
    menuVideo.addComponent(&lstVideo);

    // -------------- Credits menu construction
    menuCredits.addComponent(&imgBack);
    TextCtl   txtCreditsTitle(&fontGray, "Credits", 60, 4);
    menuCredits.addComponent(&txtCreditsTitle);
    TextCtl   txtCredits1(&fontBlue14, "Kyle Davis", 5, 34);
    TextCtl   txtCredits1a(&fontSmall, "(core program & design)", 15, 54);
    TextCtl   txtCredits2(&fontBlue14, "Jesse Litton", 5, 64);
    TextCtl   txtCredits2a(&fontSmall, "(programming/debugging)", 15, 84);
    TextCtl   txtCredits3(&fontBlue14, "Joseph Zbiciak", 5, 94);
    TextCtl   txtCredits3a(&fontSmall, "(arcane knowledge)", 15, 114);
    menuCredits.addComponent(&txtCredits1);
    menuCredits.addComponent(&txtCredits1a);
    menuCredits.addComponent(&txtCredits2);
    menuCredits.addComponent(&txtCredits2a);
    menuCredits.addComponent(&txtCredits3);
    menuCredits.addComponent(&txtCredits3a);
    
    bliss.setDefaultMenu(&menuMain);


    if(!loadSystemROMs()) {
			MessageBox(NULL,"put exec.bin and grom.bin in the bliss output directory","",MB_OK);
		return 1;
	}
    inty.setExecImage(execImage);
    inty.setGROMImage(gromImage);
    if (ecsImage) {
        inty.enableECSSupport(ecsImage);
    }
    if (ivoiceImage) {
        inty.enableIntellivoiceSupport(ivoiceImage);
    }


    // Load Cartridge
	unsigned int randomcart = 0;
	if(ini.get("global", "random_cart", &cLine[0], 2)) {
		int nParms = sscanf(cLine, "%u", &randomcart);
	}

	if((!ini.get("global", "cartridge", lastcart, _MAX_FNAME)) || randomcart) {
        srand( cd.getTick() );
        int nCart = (cartridgeFileList.size()-1)*((float)rand()/RAND_MAX);
        strcpy(lastcart, intvCartPath[nCart]);
    }

	//zero 23-jul-2012 - support loading roms from commandline
	if(argc==2)
		strcpy(lastcart,argv[1]);

    cart = IntellivisionCartridge::loadCartridge(lastcart);
    inty.insertCartridge(cart);

    char fskip[2] = "1";
	if(ini.get("global", "frameskip", fskip, 2)) {
        bliss.setFrameSkip(atoi(fskip));
	}

    //init the audio device
    result = aod.init();
    if (result) {
        vod.release();
        return result;
    }

    //init the input device
    result = id.init();
    if (result) {
        aod.release();
        vod.release();
        return result;
    }

    bliss.init(&vod, &aod, &id, &cd);
    bliss.setEmulator(&inty);
    bliss.run();

    //release input, audio and video devices
    id.release();
    aod.release();
    vod.release();

    delete cart;

	// ---------------------------- Update INI file
	UINT16 xf = 320, yf = 200, xw = 640, yw = 400;
	vod.getOutputSize(xf, yf, xw, yw);
	char line[32];
	sprintf(line, "%ux%u\0", xf, yf);
	ini.put("global", "fullscreen", line);
	sprintf(line, "%ux%u\0", xw, yw);
	ini.put("global", "windowed", line);
	sprintf(line, "%u", vod.windowed());
	ini.put("global", "mode", line);
	sprintf(line, "%u", splash);
	ini.put("global", "splashtime", line);

    ini.put("global", "cartridge", lastcart);
	sprintf(line, "%u", randomcart);
	ini.put("global", "random_cart", line);

    ini.put("global", "frameskip", fskip);

    ini.put("intellivision", "rom_dir", romdir_intv);
    ini.put("5200", "rom_dir", romdir_5200);

    return 0;
}

INT32 Bliss32::loadSystemROMs()
{
    CHAR currentDir[MAX_PATH], nextFile[MAX_PATH];
    
    // Get program directory
    if(!getProgramDir(currentDir, MAX_PATH)) {
        vod.toggleScreen(MODE_WINDOWED);
        MessageBox(NULL,
                "Could not identify the program directory.",
                "Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    // Verify that the executive rom file exists
    strcpy(nextFile, currentDir);
    strcat(nextFile, "\\");
    strcat(nextFile, FILENAME_EXECUTIVE_ROM);
    if(fileExists(nextFile)) {
        //verify the CRC of the executive rom file
        UINT32 crc = CRC32::getCrc(nextFile);
        if (crc != CRC_EXECUTIVE_ROM) {
            vod.toggleScreen(MODE_WINDOWED);
            MessageBox(NULL,
                    "File 'exec.bin' does not contain the correct Executive ROM.",
                    "Error", MB_OK | MB_ICONERROR);
            return 0;
        }
    }
    else {
        vod.toggleScreen(MODE_WINDOWED);
        MessageBox(NULL,
                "File 'exec.bin' does not exist in the program directory. Read docs\\readme.doc for information about this file.",
                "Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    // Load the executive rom file
    UINT32 size;
    ROM16Bit::loadROMImage(&execImage, &size, nextFile, TRUE);


    // Verify the grom file exists.
    //
    strcpy(nextFile, currentDir);
    strcat(nextFile, "\\");
    strcat(nextFile, FILENAME_GROM);
    if(fileExists(nextFile)) {
        // Verify the CRC of the grom rom file
        UINT32 crc = CRC32::getCrc(nextFile);
        if (crc != CRC_GROM) {
            vod.toggleScreen(MODE_WINDOWED);
            MessageBox(NULL,
                    "File 'grom.bin' does not contain the correct GROM.",
                    "Error", MB_OK | MB_ICONERROR);
            delete[] execImage;
            return 0;
        }
    }
    else {
        vod.toggleScreen(MODE_WINDOWED);
        MessageBox(NULL,
                "File 'grom.bin' does not exist in the program directory.   Read docs\\readme.doc for information about this file.",
                "Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    //load the grom rom file
    ROM16Bit::loadROMImage(&gromImage, &size, nextFile, FALSE);

    strcpy(nextFile, currentDir);
    strcat(nextFile, "\\");
    strcat(nextFile, FILENAME_ECS_ROM);
    if(fileExists(nextFile)) {
        // verify the CRC of the ECS ROM
        UINT32 crc = CRC32::getCrc(nextFile);
        if (crc == CRC_ECS_ROM) {
            //load the ECS rom file
            ROM16Bit::loadROMImage(&ecsImage, &size, nextFile, TRUE);
        }
    }
    else {
        //No error, can still run non-ECS carts
    }

    strcpy(nextFile, currentDir);
    strcat(nextFile, "\\");
    strcat(nextFile, FILENAME_IVOICE_ROM);
    if(fileExists(nextFile)) {
        //verify the CRC of the Intellivoice rom file
        UINT32 crc = CRC32::getCrc(nextFile);
        if (crc == CRC_IVOICE_ROM) {
            //load the intellivoice ROM file
            ROM16Bit::loadROMImage(&ivoiceImage, &size, nextFile, FALSE);
        }
    }
    else {
        //No error, can still run Intellivoice carts without voice
    }

    return 1;
}

// getProgramDir:  Retrieve current program directory.
//                 The return value is the length.
//
INT32 Bliss32::getProgramDir(CHAR* sDir, UINT32 maxLen)
{
    if(GetModuleFileName(NULL, sDir, maxLen)) {
        INT32 len = strlen(sDir);
        while (sDir[len] != '\\')
            len--;
        sDir[len+1] = 0;
        return len;
    }

    return 0;
}





void Bliss32::scanROMDirectory_Intv(ListCtl* ctl)
{
    clearCartList_Intv();

    //scan for the new list
    WIN32_FIND_DATA nextFile;
    char findpath[MAX_PATH];
    sprintf(findpath, "%s\\*", romdir_intv);
    HANDLE findHandle = FindFirstFile(findpath, &nextFile);
    if (findHandle == INVALID_HANDLE_VALUE)
	{
		// No ROMs found - need to force user to set ROM directory
		//MessageBox(NULL,"create a roms subdirectory of the bliss output directory and drop roms in there","",MB_OK);
		return;
	}

    int i = 0;
    char nextPath[MAX_PATH];
    do {
        sprintf(nextPath, "%s\\%s", romdir_intv, nextFile.cFileName);
        UINT32 crc = CRC32::getCrc(nextPath);
        IntellivisionCartridgeConfiguration* nextCartType = 
            (IntellivisionCartridgeConfiguration*)
            pIrdb->getConfiguration(crc);
        if (nextCartType != NULL) {
            strcpy(intvCartPath[i], nextPath);
            IntellivisionCartridge* nextCartFile = new IntellivisionCartridge(
                nextCartType->getName(),
                NULL,
                nextCartType->getMemoryCount(),
                nextCartType->requiresECS(),
                nextCartType->usesIntellivoice());
            cartridgeFileList.push_back(nextCartFile);

            itmIntvGame[i].data = intvCartPath[i];
            itmIntvGame[i].description = (*cartridgeFileList[i]).getName();
            itmIntvGame[i].callback = &changeGame;
            ctl->addItem(&itmIntvGame[i], true);
            i++;
        }
    } while (FindNextFile(findHandle, &nextFile));
    FindClose(findHandle);

}


void Bliss32::clearCartList_Intv() {
    //delete the current list
    INT32 size = cartridgeFileList.size();
    for (INT32 i = 0; i < size; i++) {
        intvCartPath[i][0] = '\0';
        delete cartridgeFileList[i];
    }
    cartridgeFileList.erase(cartridgeFileList.begin(),
		cartridgeFileList.end());
}
