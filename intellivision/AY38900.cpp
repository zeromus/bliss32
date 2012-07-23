
#include <iostream>
#include <string.h>
#include "AY38900.h"
#include "MOBRect.h"

using namespace std;

const INT32 AY38900::TICK_LENGTH_SCANLINE             = 228;
const INT32 AY38900::TICK_LENGTH_FRAME                = 59736;
const INT32 AY38900::TICK_LENGTH_VBLANK               = 15164;
const INT32 AY38900::TICK_LENGTH_START_ACTIVE_DISPLAY = 112;
const INT32 AY38900::TICK_LENGTH_IDLE_ACTIVE_DISPLAY  = 456;
const INT32 AY38900::TICK_LENGTH_FETCH_ROW            = 456;
const INT32 AY38900::TICK_LENGTH_RENDER_ROW           = 3192;

const INT32 AY38900::LOCATION_BACKTAB    = 0x0200;
const INT32 AY38900::LOCATION_GROM       = 0x3000;
const INT32 AY38900::LOCATION_GRAM       = 0x3800;
const INT32 AY38900::LOCATION_COLORSTACK = 0x0028;
const INT32 AY38900::FOREGROUND_BIT = 0x0010;

const UINT32 AY38900::palette[32] = {
    0x000000, 0x002DFF, 0xFF3D10, 0xC9CFAB,
    0x386B3F, 0x00A756, 0xFAEA50, 0xFFFCFF,
    0xBDACC8, 0x24B8FF, 0xFFB41F, 0x546E00,
    0xFF4E57, 0xA496FF, 0x75CC80, 0xB51A58,
    0x000000, 0x002DFF, 0xFF3D10, 0xC9CFAB,
    0x386B3F, 0x00A756, 0xFAEA50, 0xFFFCFF,
    0xBDACC8, 0x24B8FF, 0xFFB41F, 0x546E00,
    0xFF4E57, 0xA496FF, 0x75CC80, 0xB51A58,
};

AY38900::AY38900()
{
	horizontalOffset	= 0;
	verticalOffset		= 0;
	blockTop			= 0;
	blockLeft			= 0;
	mode				= 0;

	SR1 = NULL;
	SR2 = NULL;
	cpu = NULL;

	reset();
}

void AY38900::init(CP1610* cpu, SignalLine* SR1,
        SignalLine* SR2, SignalLine* SST)
{
    this->cpu = cpu;
    this->SR1 = SR1;
    this->SR2 = SR2;
    this->SST = SST;

    //16 bit RAM used for The Background table (BACKTAB)
    backtab.init(0x00F0, 0, 16);
    memorySegments[0].init(&backtab, 0x0200);

    //GROM
    grom.init(0x800, 0);
    memorySegments[1].init(&grom, 0x3000);

    //GRAM
    gram.init(0x0200, 0, 8);
    memorySegments[2].init(&gram, 0x3800);
    memorySegments[3].init(&gram, 0x3A00);
    memorySegments[4].init(&gram, 0x3C00);
    memorySegments[5].init(&gram, 0x3E00);

    registers.init(this);
    memorySegments[6].init(&registers, 0x0000);
    memorySegments[7].init(&registers, 0x4000);
    memorySegments[8].init(&registers, 0x8000);
    memorySegments[9].init(&registers, 0xC000);
}

INT32 AY38900::getMemoryCount()
{
    return 10;
}

Memory16Bit* AY38900::getMemory(INT32 i)
{
    return &memorySegments[i];
}

INT32 AY38900::getClockSpeed() {
    return 3579545;
}

void AY38900::reset() {
    inVBlank = FALSE;

    //reset the registers first
    registers.reset();

    //switch to bus copy mode
    setGraphicsBusVisible(TRUE);

    //reset the memory aliases
    INT32 i;
    for (i = 0; i < 10; i++)
        memorySegments[i].reset();

    //reset the mobs
    for (i = 0; i < 8; i++)
        mobs[i].reset();

    //reset the state variables
    mode = -1;
	if(SR1)
		SR1->isHigh = TRUE;
	if(SR2)
		SR2->isHigh = TRUE;
	if(cpu)
		cpu->isIdle = FALSE;
    backtab.reset();
    gram.reset();
    grom.reset();
    previousDisplayEnabled = TRUE;
    displayEnabled         = FALSE;
    colorStackMode         = FALSE;
    colorModeChanged       = TRUE;
    bordersChanged         = TRUE;
    colorStackChanged      = TRUE;
    offsetsChanged         = TRUE;
    imageBufferChanged     = TRUE;

    //local register data
    borderColor = 0;
    blockLeft = blockTop = FALSE;
    horizontalOffset = verticalOffset = 0;

    //blank the offscreen image
    for (INT32 x = 0; x < (320*192); x++)
        stagingBankData[x] = 0;

/*
	if(clock) {
		startTime = clock->getTick();
		missedFrameBuffer = clock->getTickFrequency()/4;
	}
    frameCount = 0;

    //let go of the message
    if (message != NULL) {
        message = NULL;
        messageChanged = TRUE;
    }
*/
}

void AY38900::setGraphicsBusVisible(BOOL visible)
{
    for (INT32 i = 1; i < 10; i++)
        memorySegments[i].setVisible(visible);
}

INT32 AY38900::tick()
{
    //move to the next mode
    mode++;

    switch (mode) {
        case 0:
            renderFrame();
            inVBlank = TRUE;
            displayEnabled = FALSE;
            setGraphicsBusVisible(TRUE);

            //release SR2, allowing the CPU to run
            SR2->isHigh = TRUE;
            cpu->isIdle = FALSE;

            //kick the irq line
            SR1->isHigh = FALSE;

            return TICK_LENGTH_VBLANK;

        case 1:
            inVBlank = FALSE;

            //irq line comes back up
            SR1->isHigh = TRUE;

            //if the display is not enabled, skip the rest of the modes
            if (!displayEnabled) {
                if (previousDisplayEnabled) {
                    //render a blank screen
                    for (INT32 x = 0; x < 61440; x++)
                        stagingBankData[x] = borderColor;
                }
                mode = -1;
                return (TICK_LENGTH_FRAME - TICK_LENGTH_VBLANK);
            }
            else {
                previousDisplayEnabled = TRUE;
                SR2->isHigh = FALSE;
                return TICK_LENGTH_START_ACTIVE_DISPLAY;
            }

        case 2:
            //switch to bus isolation mode
            if (!SST->isHigh)
                setGraphicsBusVisible(FALSE);

            //release SR2
            SR2->isHigh = TRUE;
            cpu->isIdle = FALSE;

            return TICK_LENGTH_IDLE_ACTIVE_DISPLAY +
                    (2*verticalOffset*TICK_LENGTH_SCANLINE);

        case 3:
        case 5:
        case 7:
        case 9:
        case 11:
        case 13:
        case 15:
        case 17:
        case 19:
        case 21:
        case 23:
        case 25:
            SR2->isHigh = FALSE;
            return TICK_LENGTH_FETCH_ROW;

        case 4:
        case 6:
        case 8:
        case 10:
        case 12:
        case 14:
        case 16:
        case 18:
        case 20:
        case 22:
        case 24:
            SR2->isHigh = TRUE;
            cpu->isIdle = FALSE;
            return TICK_LENGTH_RENDER_ROW;

        case 26:
            SR2->isHigh = TRUE;
            cpu->isIdle = FALSE;

            //this mode could be cut off in tick length if the vertical
            //offset is greater than 1
            switch (verticalOffset) {
                case 0:
                    return TICK_LENGTH_RENDER_ROW;
                case 1:
                    mode = -1;
                    return TICK_LENGTH_RENDER_ROW - TICK_LENGTH_SCANLINE;
                default:
                    mode = -1;
                    return (TICK_LENGTH_RENDER_ROW - TICK_LENGTH_SCANLINE -
                            (2*(verticalOffset-1)*TICK_LENGTH_SCANLINE));
            }

        case 27:
        default:
            mode = -1;
            SR2->isHigh = FALSE;
            return TICK_LENGTH_SCANLINE;
    }
}

void AY38900::renderFrame()
{
    //render the next frame
    if (somethingChanged()) {
        renderBorders();
        renderBackground();
        renderMOBs();
        for (INT32 i = 0; i < 8; i++)
            mobs[i].collisionRegister = 0;
        copyMOBsToStagingArea();
        determineMOBCollisions();
        markClean();
    }
    for (INT32 i = 0; i < 8; i++)
        registers.memory[0x18+i] |= mobs[i].collisionRegister;
}

void AY38900::render(VideoOutputDevice* videoOutputDevice)
{
    videoOutputDevice->renderPalettedImage(stagingBankData);
}

BOOL AY38900::somethingChanged() {
    return (offsetsChanged || bordersChanged || colorStackChanged ||
        colorModeChanged || backtab.isDirty() || gram.isDirty() ||
        mobs[0].changed || mobs[1].changed ||
        mobs[2].changed || mobs[3].changed ||
        mobs[4].changed || mobs[5].changed ||
        mobs[6].changed || mobs[7].changed);
}

void AY38900::markClean() {
    //everything has been rendered and is now clean
    offsetsChanged = FALSE;
    bordersChanged = FALSE;
    colorStackChanged = FALSE;
    colorModeChanged = FALSE;
    backtab.markClean();
    gram.markClean();
    for (INT32 i = 0; i < 8; i++)
        mobs[i].markClean();
}

void AY38900::renderBorders() {
    if (!bordersChanged && (!offsetsChanged || (blockLeft && blockTop)))
        return;

    //draw the borders, if necessary
    if (blockTop) {
        for (INT32 x = 0; x < 320; x++) {
            stagingBankData[x] = borderColor;
            stagingBankData[x+320] = borderColor;
            stagingBankData[x+(2*320)] = borderColor;
            stagingBankData[x+(3*320)] = borderColor;
            stagingBankData[x+(4*320)] = borderColor;
            stagingBankData[x+(5*320)] = borderColor;
            stagingBankData[x+(6*320)] = borderColor;
            stagingBankData[x+(7*320)] = borderColor;
            stagingBankData[x+(184*320)] = borderColor;
            stagingBankData[x+(185*320)] = borderColor;
            stagingBankData[x+(186*320)] = borderColor;
            stagingBankData[x+(187*320)] = borderColor;
            stagingBankData[x+(188*320)] = borderColor;
            stagingBankData[x+(189*320)] = borderColor;
            stagingBankData[x+(190*320)] = borderColor;
            stagingBankData[x+(191*320)] = borderColor;
        }
    }
    else if (verticalOffset > 0) {
        for (INT32 y = 0; y < verticalOffset; y++) {
            INT32 offsetY = y * 640;
            for (INT32 x = 0; x < 320; x++) {
                stagingBankData[x+offsetY] = borderColor;
                stagingBankData[x+offsetY+320] = borderColor;
            }
        }
    }

    if (blockLeft) {
        for (INT32 y = 0; y < 192; y++) {
            INT32 offsetY = y * 320;
            stagingBankData[offsetY] = borderColor;
            stagingBankData[offsetY+1] = borderColor;
            stagingBankData[offsetY+2] = borderColor;
            stagingBankData[offsetY+3] = borderColor;
            stagingBankData[offsetY+4] = borderColor;
            stagingBankData[offsetY+5] = borderColor;
            stagingBankData[offsetY+6] = borderColor;
            stagingBankData[offsetY+7] = borderColor;
            stagingBankData[offsetY+312] = borderColor;
            stagingBankData[offsetY+313] = borderColor;
            stagingBankData[offsetY+314] = borderColor;
            stagingBankData[offsetY+315] = borderColor;
            stagingBankData[offsetY+316] = borderColor;
            stagingBankData[offsetY+317] = borderColor;
            stagingBankData[offsetY+318] = borderColor;
            stagingBankData[offsetY+319] = borderColor;
        }
    }
    else if (horizontalOffset > 0) {
        for (INT32 y = 0; y < 192; y++) {
            INT32 offsetY = y * 320;
            for (INT32 x = 0; x < horizontalOffset; x++) {
                stagingBankData[x+offsetY] = borderColor;
            }
        }
    }

}

void AY38900::renderMOBs() {
    MOBRect* r;
    INT32 cardNumber;
    INT32 cardMemoryLocation;
    INT32 pixelSize;
    INT32 mobPixelHeight;
    BOOL doubleX;
    INT32 nextMemoryLocation;
    INT32 nextData;
    INT32 nextX;
    INT32 nextY;
    INT32 xInc;

    for (INT32 i = 0; i < 8; i++) {
        if (!mobs[i].changed && mobs[i].isGrom)
            continue;

        cardNumber = mobs[i].cardNumber;
        if (!mobs[i].isGrom)
            cardNumber = (cardNumber & 0x003F);
        cardMemoryLocation = (cardNumber << 3);

        r = mobs[i].getBounds();
        pixelSize = (mobs[i].quadHeight ? 4 : 1) *
                (mobs[i].doubleHeight ? 2 : 1);
        mobPixelHeight = 2 * r->height;
        doubleX = mobs[i].doubleWidth;

        for (INT32 j = 0; j < mobPixelHeight; j++) {
            nextMemoryLocation = (cardMemoryLocation + (j/pixelSize));
            if (!mobs[i].changed && !gram.isDirty(nextMemoryLocation))
                continue;

            nextData = (mobs[i].isGrom
                    ? (nextMemoryLocation >= (INT32)grom.getSize()
                        ? 0xFFFF : grom.peek(nextMemoryLocation))
                    : (nextMemoryLocation >= (INT32)gram.getSize()
                        ? 0xFFFF: gram.peek(nextMemoryLocation)));
            nextX = (mobs[i].horizontalMirror ? (doubleX ? 15 : 7) : 0);
            nextY = (mobs[i].verticalMirror
                    ? (mobPixelHeight - j - 1) : j);
            xInc = (mobs[i].horizontalMirror ? -1: 1);
            mobBuffers[i][nextX][nextY] = ((nextData & 0x0080) != 0);
            mobBuffers[i][nextX + xInc][nextY] = (doubleX
                    ? ((nextData & 0x0080) != 0)
                    : ((nextData & 0x0040) != 0));
            mobBuffers[i][nextX + (2*xInc)][nextY] = (doubleX
                    ? ((nextData & 0x0040) != 0)
                    : ((nextData & 0x0020) != 0));
            mobBuffers[i][nextX + (3*xInc)][nextY] = (doubleX
                    ? ((nextData & 0x0040) != 0)
                    : ((nextData & 0x0010) != 0));
            mobBuffers[i][nextX + (4*xInc)][nextY] = (doubleX
                    ? ((nextData & 0x0020) != 0)
                    : ((nextData & 0x0008) != 0));
            mobBuffers[i][nextX + (5*xInc)][nextY] = (doubleX
                    ? ((nextData & 0x0020) != 0)
                    : ((nextData & 0x0004) != 0));
            mobBuffers[i][nextX + (6*xInc)][nextY] = (doubleX
                    ? ((nextData & 0x0010) != 0)
                    : ((nextData & 0x0002) != 0));
            mobBuffers[i][nextX + (7*xInc)][nextY] = (doubleX
                    ? ((nextData & 0x0010) != 0)
                    : ((nextData & 0x0001) != 0));
            if (!doubleX)
                continue;

            mobBuffers[i][nextX + (8*xInc)][nextY] =
                    ((nextData & 0x0008) != 0);
            mobBuffers[i][nextX + (9*xInc)][nextY] =
                    ((nextData & 0x0008) != 0);
            mobBuffers[i][nextX + (10*xInc)][nextY] =
                    ((nextData & 0x0004) != 0);
            mobBuffers[i][nextX + (11*xInc)][nextY] =
                    ((nextData & 0x0004) != 0);
            mobBuffers[i][nextX + (12*xInc)][nextY] =
                    ((nextData & 0x0002) != 0);
            mobBuffers[i][nextX + (13*xInc)][nextY] =
                    ((nextData & 0x0002) != 0);
            mobBuffers[i][nextX + (14*xInc)][nextY] =
                    ((nextData & 0x0001) != 0);
            mobBuffers[i][nextX + (15*xInc)][nextY] =
                    ((nextData & 0x0001) != 0);
        }

    }
}

void AY38900::renderBackground() {
    if (backtab.isDirty() || gram.isDirty() || colorStackChanged ||
            colorModeChanged)
    {
        if (colorStackMode)
            renderColorStackMode();
        else
            renderForegroundBackgroundMode();
    }
    copyBackgroundBufferToStagingArea();
}

void AY38900::renderForegroundBackgroundMode() {
    for (INT32 i = 0; i < 240; i++) {
        INT32 nextCard = backtab.peek(i);
        BOOL isGrom = ((nextCard & 0x0800) == 0);
        BOOL renderAll = backtab.isDirty(i) || colorModeChanged;
        if (!renderAll && isGrom)
            continue;

        INT32 memoryLocation = (nextCard & 0x01F8);
        INT32 nextx = ((i%20)*16);
        INT32 nexty = ((i/20)*16);
        UINT8 fgcolor = (UINT8)((nextCard & 0x0007) | FOREGROUND_BIT);
        UINT8 bgcolor = (UINT8)(((nextCard & 0x2000) >> 11) |
                ((nextCard & 0x1600) >> 9));

        Memory16Bit* memory = (isGrom ? ((Memory16Bit*)&grom)
                : ((Memory16Bit*)&gram));
        for (INT32 j = 0; j < 8; j++) {
            if (renderAll || (!isGrom && gram.isDirty(memoryLocation+j))) {
                 renderLine(memory->peek(memoryLocation+j), nextx, nexty,
                         fgcolor, bgcolor);
            }
            nexty += 2;
        }
    }
}

void AY38900::renderColorStackMode() {
    INT32 colorStackPointer = 0;
    //if there are any dirty color advance bits in the backtab, or if
    //the color stack or the color mode has changed, the whole scene
    //must be rendered
    BOOL renderAll = backtab.areColorAdvanceBitsDirty() ||
            colorStackChanged || colorModeChanged;
    for (INT32 h = 0; h < 240; h++) {
        INT32 nextCard = backtab.peek(h);
        INT32 nextx = ((h%20)*16);
        INT32 nexty = ((h/20)*16);
        if ((nextCard & 0x1800) == 0x1000) {
            //colored squares mode
            UINT8 colorStackColor = (UINT8)(registers.peek(LOCATION_COLORSTACK +
                    colorStackPointer) & 0x000F);
            UINT8 color0 = (UINT8)(nextCard & 0x0007);
            UINT8 color1 = (UINT8)((nextCard & 0x0038) >> 3);
            UINT8 color2 = (UINT8)((nextCard & 0x01C0) >> 6);
            UINT8 color3 = (UINT8)(((nextCard & 0x2000) >> 11) |
                        ((nextCard & 0x0600) >> 9));
            renderColoredSquares(nextx, nexty,
                    (color0 == 7 ? colorStackColor : (UINT8)(color0 |
                        FOREGROUND_BIT)),
                    (color1 == 7 ? colorStackColor : (UINT8)(color1 |
                        FOREGROUND_BIT)),
                    (color2 == 7 ? colorStackColor : (UINT8)(color2 |
                        FOREGROUND_BIT)),
                    (color3 == 7 ? colorStackColor : (UINT8)(color3 |
                        FOREGROUND_BIT)));
        }
        else {
            //color stack mode
            BOOL isGrom = ((nextCard & 0x0800) == 0);
            INT32 memoryLocation = (isGrom ? (nextCard & 0x07F8)
                        : (nextCard & 0x01F8));

            UINT8 fgcolor = (UINT8)(((nextCard & 0x1000) >> 9) |
                    (nextCard & 0x0007) | FOREGROUND_BIT);

            if ((nextCard & 0x2000) != 0)
                colorStackPointer = (colorStackPointer+1) & 0x03;
            UINT8 bgcolor = (UINT8)(registers.peek(LOCATION_COLORSTACK +
                    colorStackPointer) & 0x000F);

            BOOL backtabIsDirty = backtab.isDirty(h);
            Memory16Bit* memory = (isGrom ? ((Memory16Bit*)&grom)
                    : ((Memory16Bit*)&gram));
            for (INT32 j = 0; j < 8; j++) {
                if (renderAll || backtabIsDirty ||
                        (!isGrom && gram.isDirty(memoryLocation+j)))
                {
                    renderLine(memory->peek(memoryLocation+j), nextx, nexty,
                            fgcolor, bgcolor);
                }
                nexty += 2;
            }
        }
    }
}

void AY38900::copyBackgroundBufferToStagingArea() {
    INT32 sourceStartX = 2*(blockLeft ? (8 - horizontalOffset) : 0);
    INT32 sourceWidthX = 2*(blockLeft ? 152 : (160 - horizontalOffset));
    INT32 sourceStartY = 2*(blockTop ? (8 - verticalOffset) : 0 );
    INT32 sourceEndY = sourceStartY + (2*(blockTop ? 88 :
            (96 - verticalOffset)));

    UINT32 j;
    for (INT32 y = sourceStartY; y < sourceEndY; y+=2) {
        INT32 nextLocation = sourceStartX +
                (2 * (horizontalOffset - (blockLeft ? 4 : 0))) +
                ((y + (2 * (verticalOffset - (blockTop ? 4 : 0)))) * 320);
/*
        memcpy(stagingBankData+nextLocation,
                (&backgroundDataBuffer[y])+sourceStartX,
                sourceWidthX*sizeof(UINT8));
        memcpy(stagingBankData+nextLocation+320,
                (&backgroundDataBuffer[y+1])+sourceStartX,
                sourceWidthX*sizeof(UINT8));
*/
        for (j = 0; (INT32)j < sourceWidthX; j++)
            stagingBankData[nextLocation+j] =
                    backgroundDataBuffer[y][sourceStartX+j];
        for (j = 0; (INT32)j < sourceWidthX; j++)
            stagingBankData[nextLocation+j+320] =
                    backgroundDataBuffer[y+1][sourceStartX+j];
    }
}

//copy the offscreen mob buffers to the staging area
void AY38900::copyMOBsToStagingArea() {
    for (INT32 i = 7; i >= 0; i--) {
        BOOL isVisible = mobs[i].isVisible;
        if (mobs[i].xLocation == 0 ||
                (!mobs[i].flagCollisions && !isVisible))
            continue;

        BOOL borderCollision = FALSE;
        BOOL foregroundCollision = FALSE;

        MOBRect* r;
        r = mobs[i].getBounds();
        INT32 mobPixelHeight = 2 * r->height;
        UINT8 fgcolor = (UINT8)mobs[i].foregroundColor;
        INT32 leftX = ((r->x + horizontalOffset) * 2);
        INT32 topY = ((r->y + verticalOffset) * 2);
        INT32 topLeftPixelIndex = leftX + (blockLeft ? -8 : 0) +
                (320 * (topY + (blockTop ? -8 : 0)));

        for (INT32 y = 0; y < mobPixelHeight; y++) {
            INT32 nextStagingY = topY + y;
            for (INT32 x = 0; x < (INT32)r->width; x++) {
                //if this mob pixel is not on, then don't paINT32 it
                if (!mobBuffers[i][x][y])
                    continue;
    
            INT32 nextStagingX = leftX + (2*x);
                INT32 nextPixelIndex = topLeftPixelIndex + (320*y) + (2*x);

                //if the next pixel location is on the border, then we
                //have a border collision and we can ignore painting it
                if (nextStagingX < (blockLeft ? 16 : 0) ||
                        nextStagingX > 317 ||
                        nextStagingY < (blockTop ? 16 : 0) ||
                        nextStagingY > 191)
                {
                    borderCollision = TRUE;
                    continue;
                }

                INT32 currentPixel = stagingBankData[nextPixelIndex];

                //if the foreground bit is set, then we have a foreground
                //collision
                if ((currentPixel & FOREGROUND_BIT) != 0) {
                    foregroundCollision = TRUE;

                    //if this mob is behind the foreground, then ignore
                    //this pixel
                    if (mobs[i].behindForeground)
                        continue;
                }

                if (isVisible) {
                    stagingBankData[nextPixelIndex] = (UINT8)(fgcolor |
                            (currentPixel & FOREGROUND_BIT));
                    stagingBankData[nextPixelIndex+1] =
                            stagingBankData[nextPixelIndex];
                }
            }
            nextStagingY++;
        }

        //update the collision bits
        if (mobs[i].flagCollisions) {
            if (foregroundCollision)
                mobs[i].collisionRegister |= 0x0100;
            if (borderCollision)
                mobs[i].collisionRegister |= 0x0200;
        }
    }
}

void AY38900::renderLine(INT32 nextByte, INT32 x, INT32 y,
        UINT8 fgcolor, UINT8 bgcolor)
{
    INT32 topPixel = y;
    INT32 bottomPixel = y+1;
    UINT8 nextPixel = (((nextByte & 0x80) == 0) ? bgcolor : fgcolor);
    backgroundDataBuffer[topPixel][x] =  nextPixel;
    backgroundDataBuffer[bottomPixel][x] =  nextPixel;
    backgroundDataBuffer[topPixel][x+1] =  nextPixel;
    backgroundDataBuffer[bottomPixel][x+1] =  nextPixel;
    nextPixel = (((nextByte & 0x40) == 0) ? bgcolor : fgcolor);
    backgroundDataBuffer[topPixel][x+2] =  nextPixel;
    backgroundDataBuffer[bottomPixel][x+2] =  nextPixel;
    backgroundDataBuffer[topPixel][x+3] =  nextPixel;
    backgroundDataBuffer[bottomPixel][x+3] =  nextPixel;
    nextPixel = (((nextByte & 0x20) == 0) ? bgcolor : fgcolor);
    backgroundDataBuffer[topPixel][x+4] =  nextPixel;
    backgroundDataBuffer[bottomPixel][x+4] =  nextPixel;
    backgroundDataBuffer[topPixel][x+5] =  nextPixel;
    backgroundDataBuffer[bottomPixel][x+5] =  nextPixel;
    nextPixel = (((nextByte & 0x10) == 0) ? bgcolor : fgcolor);
    backgroundDataBuffer[topPixel][x+6] =  nextPixel;
    backgroundDataBuffer[bottomPixel][x+6] =  nextPixel;
    backgroundDataBuffer[topPixel][x+7] =  nextPixel;
    backgroundDataBuffer[bottomPixel][x+7] =  nextPixel;
    nextPixel = (((nextByte & 0x08) == 0) ? bgcolor : fgcolor);
    backgroundDataBuffer[topPixel][x+8] =  nextPixel;
    backgroundDataBuffer[bottomPixel][x+8] =  nextPixel;
    backgroundDataBuffer[topPixel][x+9] =  nextPixel;
    backgroundDataBuffer[bottomPixel][x+9] =  nextPixel;
    nextPixel = (((nextByte & 0x04) == 0) ? bgcolor : fgcolor);
    backgroundDataBuffer[topPixel][x+10] =  nextPixel;
    backgroundDataBuffer[bottomPixel][x+10] =  nextPixel;
    backgroundDataBuffer[topPixel][x+11] =  nextPixel;
    backgroundDataBuffer[bottomPixel][x+11] =  nextPixel;
    nextPixel = (((nextByte & 0x02) == 0) ? bgcolor : fgcolor);
    backgroundDataBuffer[topPixel][x+12] =  nextPixel;
    backgroundDataBuffer[bottomPixel][x+12] =  nextPixel;
    backgroundDataBuffer[topPixel][x+13] =  nextPixel;
    backgroundDataBuffer[bottomPixel][x+13] =  nextPixel;
    nextPixel = (((nextByte & 0x01) == 0) ? bgcolor : fgcolor);
    backgroundDataBuffer[topPixel][x+14] =  nextPixel;
    backgroundDataBuffer[bottomPixel][x+14] =  nextPixel;
    backgroundDataBuffer[topPixel][x+15] =  nextPixel;
    backgroundDataBuffer[bottomPixel][x+15] =  nextPixel;
}

void AY38900::renderColoredSquares(INT32 x, INT32 y, UINT8 color0, UINT8 color1,
        UINT8 color2, UINT8 color3)
{
    INT32 topPixel = y;
    INT32 bottomPixel = y+1;
    for (INT32 i = 0; i < 8; i+=2) {
        backgroundDataBuffer[topPixel][x] = color0;
        backgroundDataBuffer[topPixel][x+1] = color0;
        backgroundDataBuffer[topPixel][x+2] = color0;
        backgroundDataBuffer[topPixel][x+3] = color0;
        backgroundDataBuffer[topPixel][x+4] = color0;
        backgroundDataBuffer[topPixel][x+5] = color0;
        backgroundDataBuffer[topPixel][x+6] = color0;
        backgroundDataBuffer[topPixel][x+7] = color0;
        backgroundDataBuffer[bottomPixel][x] = color0;
        backgroundDataBuffer[bottomPixel][x+1] = color0;
        backgroundDataBuffer[bottomPixel][x+2] = color0;
        backgroundDataBuffer[bottomPixel][x+3] = color0;
        backgroundDataBuffer[bottomPixel][x+4] = color0;
        backgroundDataBuffer[bottomPixel][x+5] = color0;
        backgroundDataBuffer[bottomPixel][x+6] = color0;
        backgroundDataBuffer[bottomPixel][x+7] = color0;

        backgroundDataBuffer[topPixel][x+8] = color1;
        backgroundDataBuffer[topPixel][x+9] = color1;
        backgroundDataBuffer[topPixel][x+10] = color1;
        backgroundDataBuffer[topPixel][x+11] = color1;
        backgroundDataBuffer[topPixel][x+12] = color1;
        backgroundDataBuffer[topPixel][x+13] = color1;
        backgroundDataBuffer[topPixel][x+14] = color1;
        backgroundDataBuffer[topPixel][x+15] = color1;
        backgroundDataBuffer[bottomPixel][x+8] = color1;
        backgroundDataBuffer[bottomPixel][x+9] = color1;
        backgroundDataBuffer[bottomPixel][x+10] = color1;
        backgroundDataBuffer[bottomPixel][x+11] = color1;
        backgroundDataBuffer[bottomPixel][x+12] = color1;
        backgroundDataBuffer[bottomPixel][x+13] = color1;
        backgroundDataBuffer[bottomPixel][x+14] = color1;
        backgroundDataBuffer[bottomPixel][x+15] = color1;

        backgroundDataBuffer[topPixel+8][x] = color2;
        backgroundDataBuffer[topPixel+8][x+1] = color2;
        backgroundDataBuffer[topPixel+8][x+2] = color2;
        backgroundDataBuffer[topPixel+8][x+3] = color2;
        backgroundDataBuffer[topPixel+8][x+4] = color2;
        backgroundDataBuffer[topPixel+8][x+5] = color2;
        backgroundDataBuffer[topPixel+8][x+6] = color2;
        backgroundDataBuffer[topPixel+8][x+7] = color2;
        backgroundDataBuffer[bottomPixel+8][x] = color2;
        backgroundDataBuffer[bottomPixel+8][x+1] = color2;
        backgroundDataBuffer[bottomPixel+8][x+2] = color2;
        backgroundDataBuffer[bottomPixel+8][x+3] = color2;
        backgroundDataBuffer[bottomPixel+8][x+4] = color2;
        backgroundDataBuffer[bottomPixel+8][x+5] = color2;
        backgroundDataBuffer[bottomPixel+8][x+6] = color2;
        backgroundDataBuffer[bottomPixel+8][x+7] = color2;

        backgroundDataBuffer[topPixel+8][x+8] = color3;
        backgroundDataBuffer[topPixel+8][x+9] = color3;
        backgroundDataBuffer[topPixel+8][x+10] = color3;
        backgroundDataBuffer[topPixel+8][x+11] = color3;
        backgroundDataBuffer[topPixel+8][x+12] = color3;
        backgroundDataBuffer[topPixel+8][x+13] = color3;
        backgroundDataBuffer[topPixel+8][x+14] = color3;
        backgroundDataBuffer[topPixel+8][x+15] = color3;
        backgroundDataBuffer[bottomPixel+8][x+8] = color3;
        backgroundDataBuffer[bottomPixel+8][x+9] = color3;
        backgroundDataBuffer[bottomPixel+8][x+10] = color3;
        backgroundDataBuffer[bottomPixel+8][x+11] = color3;
        backgroundDataBuffer[bottomPixel+8][x+12] = color3;
        backgroundDataBuffer[bottomPixel+8][x+13] = color3;
        backgroundDataBuffer[bottomPixel+8][x+14] = color3;
        backgroundDataBuffer[bottomPixel+8][x+15] = color3;
        topPixel += 2;
        bottomPixel += 2;
    }
}

/*
void AY38900::renderMessage() {
    if (message != NULL) {
        INT32 length = strlen(message);
        INT32 x = (320 - (length * 16))/2;
        INT32 y = 88;
        //black out a square around the message
        INT32 width = (length*8) + 4;
        INT32 i;
        for (i = 0; i < width; i++) {
            for (INT32 j = 0; j < 11; j++) {
                INT32 topPixel = ((y-3+(j*2))*320)+x-4+(i*2);
                INT32 bottomPixel = topPixel+320;
                stagingBankData[topPixel] = 0;
                stagingBankData[bottomPixel] = 0;
                stagingBankData[topPixel+1] = 0;
                stagingBankData[bottomPixel+1] = 0;
            }
        }
        for (i = 0; i < length; i++) {
            renderCharacter(message[i], x, y);
            x += 16;
        }
    }
    messageChanged = FALSE;
}

void AY38900::renderCharacter(CHAR c, INT32 x, INT32 y) {
    INT32 gromAddress = 0x0108 + ((c - 'A') * 8);
    for (INT32 i = 0; i < 8; i++) {
        INT32 nextByte = grom.peek(gromAddress+i);
        INT32 topPixel = ((y+(i*2))*320)+x;
        INT32 bottomPixel = topPixel+320;
        UINT8 fgcolor = 0x07;
        if ((nextByte & 0x80) != 0) {
            stagingBankData[topPixel] = fgcolor;
            stagingBankData[bottomPixel] = fgcolor;
            stagingBankData[topPixel+1] = fgcolor;
            stagingBankData[bottomPixel+1] = fgcolor;
        }
        if ((nextByte & 0x40) != 0) {
            stagingBankData[topPixel+2] = fgcolor;
            stagingBankData[bottomPixel+2] = fgcolor;
            stagingBankData[topPixel+3] = fgcolor;
            stagingBankData[bottomPixel+3] = fgcolor;
        }
        if ((nextByte & 0x20) != 0) {
            stagingBankData[topPixel+4] = fgcolor;
            stagingBankData[bottomPixel+4] = fgcolor;
            stagingBankData[topPixel+5] = fgcolor;
            stagingBankData[bottomPixel+5] = fgcolor;
        }
        if ((nextByte & 0x10) != 0) {
            stagingBankData[topPixel+6] = fgcolor;
            stagingBankData[bottomPixel+6] = fgcolor;
            stagingBankData[topPixel+7] = fgcolor;
            stagingBankData[bottomPixel+7] = fgcolor;
        }
        if ((nextByte & 0x08) != 0) {
            stagingBankData[topPixel+8] = fgcolor;
            stagingBankData[bottomPixel+8] = fgcolor;
            stagingBankData[topPixel+9] = fgcolor;
            stagingBankData[bottomPixel+9] = fgcolor;
        }
        if ((nextByte & 0x04) != 0) {
            stagingBankData[topPixel+10] = fgcolor;
            stagingBankData[bottomPixel+10] = fgcolor;
            stagingBankData[topPixel+11] = fgcolor;
            stagingBankData[bottomPixel+11] = fgcolor;
        }
        if ((nextByte & 0x02) != 0) {
            stagingBankData[topPixel+12] = fgcolor;
            stagingBankData[bottomPixel+12] = fgcolor;
            stagingBankData[topPixel+13] = fgcolor;
            stagingBankData[bottomPixel+13] = fgcolor;
        }
        if ((nextByte & 0x01) != 0) {
            stagingBankData[topPixel+14] = fgcolor;
            stagingBankData[bottomPixel+14] = fgcolor;
            stagingBankData[topPixel+15] = fgcolor;
            stagingBankData[bottomPixel+15] = fgcolor;
        }
    }
}
*/

void AY38900::determineMOBCollisions()
{
    //check mob to mob collisions
    for (int i = 0; i < 7; i++) {
        for (int j = i+1; j < 8; j++) {
            if (!mobs[i].flagCollisions && !mobs[j].flagCollisions)
                continue;

            if (!mobsCollide(i, j))
                continue;

            if (mobs[i].flagCollisions)
                mobs[i].collisionRegister |= (1 << j);

            if (mobs[j].flagCollisions)
                mobs[j].collisionRegister |= (1 << i);
        }
    }
}

#define MIN(v1, v2) (v1 < v2 ? v1 : v2)
#define MAX(v1, v2) (v1 > v2 ? v1 : v2)

BOOL AY38900::mobsCollide(INT32 mobNum0, INT32 mobNum1) {
    MOBRect* r0;
    r0 = mobs[mobNum0].getBounds();
    MOBRect* r1;
    r1 = mobs[mobNum1].getBounds();
    if (!r0->intersects(r1))
        return FALSE;

    //iterate over the intersecting bits to see if any touch
    INT32 x0 = MAX(r0->x, r1->x);
    INT32 y0 = MAX(r0->y, r1->y);
    INT32 r0y = 2*(y0-r0->y);
    INT32 r1y = 2*(y0-r1->y);
    INT32 width = MIN(r0->x+r0->width, r1->x+r1->width) - x0;
    INT32 height = (MIN(r0->y+r0->height, r1->y+r1->height) - y0) * 2;
    for (INT32 x = 0; x < width; x++) {
        for (INT32 y = 0; y < height; y++) {
            if (mobBuffers[mobNum0][x0-r0->x+x][r0y+y] &&
                    mobBuffers[mobNum1][x0-r1->x+x][r1y+y])
                return TRUE;
        }
    }

    return FALSE;
}

void AY38900::setGROMImage(UINT16* gromImage) {
    grom.loadImage(gromImage);
}

void AY38900::getOutputImageSize(UINT16* width, UINT16* height)
{
    (*width) = 320;
    (*height) = 192;
}

void AY38900::getPalette(UINT32** p, UINT8* numEntries)
{
    (*p) = (UINT32*)&palette[0];
    (*numEntries) = 32;
}
