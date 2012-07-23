
#ifndef AY38900_H
#define AY38900_H

#include "CP1610.h"
#include "BackTabRAM.h"
#include "GRAM.h"
#include "MOB.h"
#include "AY38900_Registers.h"
#include "../core/MemoryAlias.h"
#include "../core/Processor.h"
#include "../core/ROM.h"
#include "../core/VideoProducer.h"
#include "../core/SignalLine.h"
#include "../core/osd/types.h"
#include "../core/osd/VideoOutputDevice.h"

class AY38900 : public Processor, public VideoProducer
{

    friend class Intellivision;
    friend class AY38900_Registers;

    public:
        AY38900();
        void getOutputImageSize(UINT16* width, UINT16* height);
        void getPalette(UINT32** palette, UINT8* numEntries);
        void init(CP1610* cpu, SignalLine* SR1,
                SignalLine* SR2, SignalLine* SST);
        void render(VideoOutputDevice*);
        INT32 getClockSpeed();
        void reset();
        INT32 tick();
        void setGROMImage(UINT16* gromImage);
        INT32 getMemoryCount();
        Memory16Bit* getMemory(INT32 i);

        static const INT32 TICK_LENGTH_SCANLINE;
        static const INT32 TICK_LENGTH_FRAME;
        static const INT32 TICK_LENGTH_VBLANK;
        static const INT32 TICK_LENGTH_START_ACTIVE_DISPLAY;
        static const INT32 TICK_LENGTH_IDLE_ACTIVE_DISPLAY;
        static const INT32 TICK_LENGTH_FETCH_ROW;
        static const INT32 TICK_LENGTH_RENDER_ROW;
        static const INT32 LOCATION_BACKTAB;
        static const INT32 LOCATION_GROM;
        static const INT32 LOCATION_GRAM;
        static const INT32 LOCATION_COLORSTACK;
        static const INT32 FOREGROUND_BIT;

    private:
        void setGraphicsBusVisible(BOOL visible);
        void renderFrame();
        BOOL somethingChanged();
        void markClean();
        void renderBorders();
        void renderMOBs();
        void renderBackground();
        void copyBackgroundBufferToStagingArea();
        void renderForegroundBackgroundMode();
        void renderColorStackMode();
        void copyMOBsToStagingArea();
        void renderLine(INT32 nextByte, INT32 x, INT32 y,
                UINT8 fgcolor, UINT8 bgcolor);
        void renderColoredSquares(INT32 x, INT32 y, UINT8 color0, UINT8 color1,
                UINT8 color2, UINT8 color3);
        void renderMessage();
        void renderCharacter(char c, INT32 x, INT32 y);
        void determineMOBCollisions();
        BOOL mobsCollide(INT32 mobNum0, INT32 mobNum1);

        CP1610*                cpu;
        SignalLine*            SR1;
        SignalLine*            SR2;
        SignalLine*            SST;
        MemoryAlias16Bit       memorySegments[10];
        BackTabRAM             backtab;
        ROM16Bit               grom;
        GRAM                   gram;
        AY38900_Registers      registers;
        UINT8                  stagingBankData[320*192];
        UINT8                  backgroundDataBuffer[192][320];
        MOB                    mobs[8];

        //state info
        INT32           mode;
        BOOL            previousDisplayEnabled;
        BOOL            displayEnabled;
        BOOL            colorStackMode;
        BOOL            colorModeChanged;
        BOOL            bordersChanged;
        BOOL            colorStackChanged;
        BOOL            offsetsChanged;
        BOOL            imageBufferChanged;

        //register info
        UINT8   borderColor;
        BOOL    blockLeft;
        BOOL    blockTop;
        INT32   horizontalOffset;
        INT32   verticalOffset;

        //mob info
        BOOL     mobBuffers[8][16][128];

        //palette
        const static UINT32 palette[32];

};

#endif
