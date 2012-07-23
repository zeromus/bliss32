
#include "SDLVideoOutputDevice.h"
#include <SDL_Image.h>
#include <string.h>

// Text for error messages
const CHAR* videoErrorMessages[2] = {
    "Unable to initialize video",
    "Unable to set the requested mode/resolution",
};


// Fill Routine
void (SDLVideoOutputDevice::*fnFillFunction)();


SDLVideoOutputDevice::SDLVideoOutputDevice()
{
    m_bStop = FALSE;

    // Init all variables
    m_InputWidth        = 0;
    m_InputHeight       = 0;
	m_OutputWidthFS		= 640;		// Default fullscreen width (overridden in Bliss32)
	m_OutputHeightFS	= 480;		// Default fullscreen height
	m_OutputWidthWin	= 640;		// Default window width
	m_OutputHeightWin	= 480;		// Default window height
	m_OutputDepth		= 16;		// Default to 16 bit color
    m_Flags             = SDL_SWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE;

    m_dy                = 0;
    m_dyinc1            = 0;
    m_dyinc2            = 0;
    m_dx                = 0;
    m_dxinc1            = 0;
    m_dxinc2            = 0;
    m_nScaleX           = 0;        // Scaling factor

    m_pSurface          = NULL;
//    m_ImageLength       = 0;
    fnFillFunction      = &SDLVideoOutputDevice::fillBackBuffer;

    m_iEntries          = NULL;
    m_iNumEntries       = 0;

}

SDLVideoOutputDevice::~SDLVideoOutputDevice()
{
    SDL_ShowCursor(true);

    // Cleanup before shutdown
    release();
}

const CHAR* SDLVideoOutputDevice::getErrorDescription(ERRCODE errorCode) {
    return videoErrorMessages[errorCode-1];
}

// init:  Activate video
ERRCODE SDLVideoOutputDevice::init()
{
#ifdef _TRACE_VIDEO
    fprintf(stderr, "SDLVideoOutputDevice::init()\n");
#endif

    SDL_ShowCursor(false);

    m_bStop = FALSE;

    if(SDL_Init(SDL_INIT_VIDEO))
        return ERR_INIT_VIDEO;

		//zero 23-jul-2012 - who needs an icon. makes it sloppier to build
    // Set window icon and caption
    //SDL_Surface* m_icon = SDL_LoadBMP("B32.bmp");
    //SDL_SetColorKey(m_icon, SDL_SRCCOLORKEY,
    //    SDL_MapRGB(m_icon->format, 255, 0, 0));
    //SDL_WM_SetIcon(m_icon, NULL);
    //SDL_WM_SetCaption("Bliss32 v2.0", "Bliss");

	// Create Output Window or Screen
	ERRCODE ec;

    if(windowed())
		ec = changeOutputSize(m_OutputWidthWin, m_OutputHeightWin);
	else
		ec = changeOutputSize(m_OutputWidthFS, m_OutputHeightFS);
	if(ec)
		return ec;

    if (!m_pSurface) {
        SDL_Quit();
        return ERR_INIT_VIDEO;
    }

    return OK;
}

void SDLVideoOutputDevice::release()
{
#ifdef _TRACE_VIDEO
    fprintf(stderr, "SDLVideoOutputDevice::release()\n");
#endif

    SDL_Quit();
}


// Build stretch matrix for fast scaling and rendering
void SDLVideoOutputDevice::calculateStretchIndices()
{
#ifdef _TRACE_VIDEO
  fprintf(stderr, "   SDLVideoOutputDevice::calculateStretchIndices()\n");
  fprintf(stderr, "      iw = %d, ih = %d\n",
      m_InputWidth, m_InputHeight);
  fprintf(stderr, "      ow = %d, oh = %d, od = %d, flags = %u\n",
      m_OutputWidth, m_OutputHeight, m_OutputDepth, m_Flags);
#endif

    // Scaling prep work
    if (m_InputHeight > m_OutputHeight) {
        m_dyinc1 = m_OutputHeight << 1;
        m_dy = m_dyinc1 - (m_InputHeight << 1);
        m_dyinc2 = (m_OutputHeight - m_InputHeight) << 1;
    }
    else {
        m_dyinc1 = m_InputHeight << 1;
        m_dy = m_dyinc1 - (m_OutputHeight << 1);
        m_dyinc2 = (m_InputHeight - m_OutputHeight) << 1;
    }

    if (m_InputWidth > m_OutputWidth) {
        m_dxinc1 = m_OutputWidth << 1;
        m_dx = m_dxinc1 - (m_InputWidth << 1);
        m_dxinc2 = (m_OutputWidth - m_InputWidth) << 1;
    }
    else {
        m_dxinc1 = m_InputWidth << 1;
        m_dx = m_dxinc1 - (m_OutputWidth << 1);
        m_dxinc2 = (m_InputWidth - m_OutputWidth) << 1;
    }

    if (m_OutputWidth && m_InputWidth && (m_OutputWidth % m_InputWidth == 0))
    {
        m_nScaleX = m_OutputWidth / m_InputWidth;
    }
    else {
        m_nScaleX = 0;
    }


    // Set Fill function
    if (m_InputHeight == m_OutputHeight && m_InputWidth == m_OutputWidth)
    {
        fnFillFunction = &SDLVideoOutputDevice::fillBackBuffer_NoScaling;
    }
    else if (m_InputHeight < m_OutputHeight && m_nScaleX) {
        fnFillFunction = &SDLVideoOutputDevice::fillBackBuffer_ScaleXStretchY;
    }
    else if (m_InputHeight > m_OutputHeight && m_InputWidth > m_OutputWidth) {
        fnFillFunction = &SDLVideoOutputDevice::fillBackBuffer_Shrink;
    }
    else if (m_InputHeight < m_OutputHeight && m_InputWidth < m_OutputWidth) {
        fnFillFunction = &SDLVideoOutputDevice::fillBackBuffer_Stretch;
    }
    else {
        fnFillFunction = &SDLVideoOutputDevice::fillBackBuffer;
    }
}


// Set source image size
void SDLVideoOutputDevice::changeInputSize(const UINT16 width, const UINT16 height)
{
#ifdef _TRACE_VIDEO
    fprintf(stderr, "SDLVideoOutputDevice::setInputSize(%d, %d)\n", width, height);
#endif

    m_InputWidth  = width;
    m_InputHeight = height;

    calculateStretchIndices();
}


// Set index -> RGB palette
void SDLVideoOutputDevice::setPalette(const UINT32* p, const UINT8 numEntries)
{
#ifdef _TRACE_VIDEO
  fprintf(stderr, "SDLVideoOutputDevice::setPalette()\n");
#endif

    if (m_iEntries)
        delete[] m_iEntries;

    m_iEntries = new UINT32[numEntries];
    m_iNumEntries = numEntries;
    for (UINT16 i = 0; i < m_iNumEntries; i++)
        m_iEntries[i] = p[i];

    if (m_pSurface)
        recalculatePalette();
}

// Create working palette that matches the current video surface format
void SDLVideoOutputDevice::recalculatePalette()
{
#ifdef _TRACE_VIDEO
  fprintf(stderr, "   SDLVideoOutputDevice::convertPalette()\n");
#endif

    for (int i = 0; i < m_iNumEntries; i++) {
        m_WorkingPalette[i] = SDL_MapRGB(m_pSurface->format,
                (m_iEntries[i] & 0xFF0000) >> 16,
                (m_iEntries[i] & 0x00FF00) >> 8,
                m_iEntries[i] & 0x0000FF);
    }
}


// Draw an emulator frame
ERRCODE SDLVideoOutputDevice::renderPalettedImage(UINT8* image)
{
#ifdef _TRACE_VIDEO
  fprintf(stderr, "SDLVideoOutputDevice::updateImage()\n");
#endif

    m_image = image;     // Set image for render routine.

    if(!SDL_LockSurface(m_pSurface)) {
//        fillBackBuffer();
        (this->*fnFillFunction)();
        SDL_UnlockSurface(m_pSurface);
    }

	return false;
}

// Update visible window
void SDLVideoOutputDevice::present()
{
#ifdef _TRACE_VIDEO
  fprintf(stderr, "SDLVideoOutputDevice::present()...");
#endif

    SDL_Flip(m_pSurface);

    //flush the event queue
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                m_bStop = TRUE;
                break;
            case SDL_VIDEORESIZE:
				changeOutputSize(event.resize.w, event.resize.h);
                break;
        }
    };

#ifdef _TRACE_VIDEO
  fprintf(stderr, "  ok\n");
#endif
}


UINT32 SDLVideoOutputDevice::getPixel(PSURFACE inSurface, INT32 x, INT32 y)
{
    UINT8  *bits;
    UINT32 Bpp;

    SDL_Surface* surface = (SDL_Surface*)inSurface;

    Bpp = surface->format->BytesPerPixel;

    bits = ((Uint8 *)surface->pixels)+y * surface->pitch+x * Bpp;

    switch(Bpp)
    {
        case 1:
            return *((Uint8 *)surface->pixels + y * surface->pitch + x);
        break;
            case 2:
            return *((Uint16 *)surface->pixels + y * (surface->pitch>>1) + x);
        break;
            case 3:
            // Format/endian independent
            Uint8 r, g, b;
            r = *((bits)+(surface->format->Rshift>>3));
            g = *((bits)+(surface->format->Gshift>>3));
            b = *((bits)+(surface->format->Bshift>>3));
            return SDL_MapRGB(surface->format, r, g, b);
        break;
        case 4:
            return *((Uint32 *)surface->pixels + y * (surface->pitch>>2) + x);
        break;
    }

    return (UINT32)-1;
}

void SDLVideoOutputDevice::setPixel(PSURFACE inSurface, INT32 x, INT32 y, UINT32 c)
{
    UINT8  *bits;
    UINT32 Bpp;

    SDL_Surface* surface = (SDL_Surface*)inSurface;    
    if ((!surface)||(x<0)||(x>=surface->w))
        return;
    
    Bpp = surface->format->BytesPerPixel;
    
    bits = ((UINT8 *)surface->pixels)+y * surface->pitch+x * Bpp;
    
    switch(Bpp)
    {
        case 1:
            *((UINT8 *)surface->pixels + y * surface->pitch + x) = (UINT8)c;
        break;
            case 2:
            *((UINT16 *)surface->pixels + y * (surface->pitch>>1) + x) = (UINT16)c;
        break;
        case 3:
            // Format/endian independent
            Uint8 r, g, b;
            SDL_GetRGB(c, surface->format, &r, &g, &b);
            *((bits)+(surface->format->Rshift>>3))=r;
            *((bits)+(surface->format->Gshift>>3))=g;
            *((bits)+(surface->format->Bshift>>3))=b;
        break;
        case 4:
            *((UINT32*)surface->pixels + y * (surface->pitch>>2) + x) = c;
        break;
    }
}

UINT32 SDLVideoOutputDevice::mapRGB(PSURFACE inSurface, int r, int g, int b)
{
    SDL_Surface* surface = (SDL_Surface*) inSurface;
    return SDL_MapRGB(surface->format, r, g, b);
}

void SDLVideoOutputDevice::setColorKey(PSURFACE inSurface, UINT32 background)
{
    SDL_Surface* surface = (SDL_Surface*) inSurface;
    SDL_SetColorKey(surface, SDL_SRCCOLORKEY, background);
}

int SDLVideoOutputDevice::blitSurface(PSURFACE inSurface,
                           const WHRect* srcrect, const WHRect* dstrect)
{
    SDL_Surface* surface = (SDL_Surface*) inSurface;

    SDL_Rect src, dst;
    src.x = (int)srcrect->x;
    src.y = (int)srcrect->y;
    src.w = (int)srcrect->w;
    src.h = (int)srcrect->h;
    dst.x = (int)dstrect->x;
    dst.y = (int)dstrect->y;
    dst.w = (int)dstrect->w;
    dst.h = (int)dstrect->h;

    return SDL_BlitSurface(surface, &src, m_pSurface, &dst);
}


// Switch between windowed and fullscreen mode
ERRCODE SDLVideoOutputDevice::toggleScreen(const int mode)
{
#ifdef _TRACE_VIDEO
	fprintf(stderr, "SDLVideo::toggleScreen()\n");
#endif

    switch(mode)
	{
	case MODE_SWITCH:
		if(m_Flags & SDL_FULLSCREEN) {			// if currently fullscreen
			m_Flags &= ~SDL_FULLSCREEN;
			m_OutputWidth = m_OutputWidthWin;
			m_OutputHeight = m_OutputHeightWin;
		}
		else {
			m_Flags |= SDL_FULLSCREEN;
			m_OutputWidth = m_OutputWidthFS;
			m_OutputHeight = m_OutputHeightFS;
		}
		break;

	case MODE_WINDOWED:
		m_Flags &= ~SDL_FULLSCREEN;
		m_OutputWidth = m_OutputWidthWin;
		m_OutputHeight = m_OutputHeightWin;
		break;

	case MODE_FULLSCREEN:
		m_Flags |= SDL_FULLSCREEN;
		m_OutputWidth = m_OutputWidthFS;
		m_OutputHeight = m_OutputHeightFS;
		break;
	}

	return reset();
}


ERRCODE SDLVideoOutputDevice::reset()
{
#ifdef _TRACE_VIDEO
  fprintf(stderr, "   SDLVideo::reset()\n");
  fprintf(stderr, "      ow = %d, oh = %d, od = %d, flags = %u\n",
	  m_OutputWidth, m_OutputHeight, m_OutputDepth, m_Flags);
#endif

	if(m_pSurface = SDL_SetVideoMode(m_OutputWidth, m_OutputHeight,
						m_OutputDepth, m_Flags))
	{
		calculateStretchIndices();
		return OK;
	}

	return ERR_SET_MODE;	// Surface not acquired
}

// Change the current OutputSize
ERRCODE SDLVideoOutputDevice::changeOutputSize(const UINT16 width, const UINT16 height)
{
#ifdef _TRACE_VIDEO
  fprintf(stderr, "SDLVideo::changeOutputSize(%d, %d)\n", width, height);
  fprintf(stderr, "   windowed() = %d\n", windowed());
#endif

	m_OutputWidth	= width;	// Set working resolution
	m_OutputHeight	= height;

	// Preserve screen size in case of a toggle
	if(windowed()) {
		m_OutputWidthWin = m_OutputWidth;
		m_OutputHeightWin = m_OutputHeight;
	}
	else {
		m_OutputWidthFS = m_OutputWidth;
		m_OutputHeightFS = m_OutputHeight;
	}

	return reset();
}

void SDLVideoOutputDevice::setGamma(float r, float g, float b)
{
    SDL_SetGamma(r, g, b);
}

void SDLVideoOutputDevice::setAlpha(PSURFACE inSurface, UINT8 alpha)
{

    SDL_Surface* surface = (SDL_Surface*)inSurface;

    SDL_SetAlpha(surface, SDL_SRCALPHA|SDL_RLEACCEL, alpha);
}


bool SDLVideoOutputDevice::enumModes(char* output)
{
    /* Get available fullscreen/hardware modes */
    static SDL_Rect** modes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_SWSURFACE);
    static int modeNum = 0;

    if(modes[modeNum]) {
        sprintf(output, "%dx%d", modes[modeNum]->w, modes[modeNum]->h);
        modeNum++;
    }
    else {
        output = NULL;
        modeNum = 0;
        return false;
    }

    return true;
}


// Copy the emulated video ram to the backbuffer
//
// This version of fillbuffer can scale X and Y in opposite directions.
//
void SDLVideoOutputDevice::fillBackBuffer()
{
#ifdef _TRACE_VIDEO
    fprintf(stderr, "   SDLVideoOutputDevice::fillBackBuffer()\n");
#endif

    // The buffer cannot be set ahead of time, because it may
    //    return null if the surface is not locked.
    PUINT16 backBuffer = (PUINT16) m_pSurface->pixels;
    if (!backBuffer)
        return;

    INT32 pitch = m_pSurface->pitch >> 1;

    INT32 x = 0, y = 0;
    for (y = m_OutputHeight; y > 0; y--) {
        PUINT16 currentPixel;

        currentPixel = backBuffer;
        if (m_nScaleX) {
            if (m_nScaleX == 1) {
                for (x = m_OutputWidth >> 1; x > 0; x--) {
                    unsigned long a;
                    a = m_WorkingPalette[*m_image++];
                    a |= m_WorkingPalette[*m_image++] << 16;
                    *((unsigned long *) currentPixel) = a;
                    currentPixel += 2;
                }
                if (m_OutputWidth & 1) {
                    *currentPixel++ = m_WorkingPalette[*m_image++];
                }
            }
            else if (m_nScaleX == 2) {
                for (x = m_OutputWidth >> 1; x > 0; x--) {
                    unsigned long a;
                    a = m_WorkingPalette[*m_image++];
                    a |= a << 16;
                    *((unsigned long *) currentPixel) = a;
                    currentPixel += 2;
                }
            }
            else {
                for (x = m_OutputWidth; x > 0; x--) {
                    unsigned long a;
                    int scale = m_nScaleX;
                    a = m_WorkingPalette[*m_image++];
                    a |= a << 16;
                    while (scale > 2) {
                        *((unsigned long *) currentPixel) = a;
                        currentPixel += 2;
                        scale -= 2;
                    }
                    if (scale == 1) {
                        *currentPixel++ = m_WorkingPalette[*m_image++];
                    }
                }
            }
        }
        else if (m_InputWidth > m_OutputWidth) {
            for (x = m_OutputWidth; x > 0; x--) {
                *currentPixel++ = m_WorkingPalette[*m_image++];

                while (m_dx < 0) {
                    m_dx += m_dxinc1;
                    m_image++;
                }
                m_dx += m_dxinc2;
            }
        }
        else {
            for (x = m_OutputWidth; x > 0; x--) {
                *currentPixel++ = m_WorkingPalette[*m_image];
    
                if (m_dx < 0) {
                    m_dx += m_dxinc1;
                }
                else {
                    m_dx += m_dxinc2;
                    m_image++;
                }
            }
        }

        backBuffer += pitch;

        if (m_InputHeight > m_OutputHeight) {
            while (m_dy < 0) {
                m_dy += m_dyinc1;
                m_image += m_InputWidth;
            }
            m_dy += m_dyinc2;
        }
        else {
            while (m_dy < 0) {
                m_dy += m_dyinc1;
                memcpy(backBuffer,backBuffer-pitch,m_OutputWidth<<1);
                backBuffer += pitch;
                y--;
            }
            m_dy += m_dyinc2;
        }
    }
}

// Shortcut optimization for when input size equals the output size
void SDLVideoOutputDevice::fillBackBuffer_NoScaling()
{
    PUINT16 backBuffer = (PUINT16) m_pSurface->pixels;
    if (!backBuffer)
        return;

    INT32 pitch = m_pSurface->pitch >> 1;

    for (INT32 y = m_OutputHeight; y > 0; y--) {
        PUINT16 currentPixel;
        currentPixel = backBuffer;
        for (INT32 x = m_OutputWidth >> 1; x > 0; x--) {
            unsigned long a;
            a = m_WorkingPalette[*m_image++];
            a |= m_WorkingPalette[*m_image++] << 16;
            *((unsigned long *) currentPixel) = a;
            currentPixel += 2;
        }
        if (m_OutputWidth & 1) {
            *currentPixel++ = m_WorkingPalette[*m_image++];
        }
        backBuffer += pitch;
    }

    return;
}

// Copy the emulated video ram to the backbuffer
void SDLVideoOutputDevice::fillBackBuffer_Shrink()
{
#ifdef _TRACE_VIDEO
    fprintf(stderr, "   SDLVideoOutputDevice::fillBackBuffer()\n");
#endif

    // The buffer cannot be set ahead of time, because it may
    //    return null if the surface is not locked.
    PUINT16 backBuffer = (PUINT16) m_pSurface->pixels;
    if (!backBuffer)
        return;

    INT32 pitch = m_pSurface->pitch >> 1;

    INT32 x = 0, y = 0;
    for (y = m_OutputHeight; y > 0; y--) {
        PUINT16 currentPixel;

        currentPixel = backBuffer;
        for (x = m_OutputWidth; x > 0; x--) {
            *currentPixel++ = m_WorkingPalette[*m_image++];

            while (m_dx < 0) {
                m_dx += m_dxinc1;
                m_image++;
            }
            m_dx += m_dxinc2;
        }

        backBuffer += pitch;

        while (m_dy < 0) {
            m_dy += m_dyinc1;
            m_image += m_InputWidth;
        }
        m_dy += m_dyinc2;
    }
}


void SDLVideoOutputDevice::fillBackBuffer_Stretch()
{
#ifdef _TRACE_VIDEO
    fprintf(stderr, "   SDLVideoOutputDevice::fillBackBuffer()\n");
#endif

    // The buffer cannot be set ahead of time, because it may
    //    return null if the surface is not locked.
    PUINT16 backBuffer = (PUINT16) m_pSurface->pixels;
    if (!backBuffer)
        return;

    INT32 pitch = m_pSurface->pitch >> 1;

    INT32 x = 0, y = 0;
    for (y = m_OutputHeight; y > 0; y--) {
        PUINT16 currentPixel;

        currentPixel = backBuffer;
        for (x = m_OutputWidth; x > 0; x--) {
            *currentPixel++ = m_WorkingPalette[*m_image];

            if (m_dx < 0) {
                m_dx += m_dxinc1;
            }
            else {
                m_dx += m_dxinc2;
                m_image++;
            }
        }

        backBuffer += pitch;

        while (m_dy < 0) {
            m_dy += m_dyinc1;
            memcpy(backBuffer,backBuffer-pitch,m_OutputWidth<<1);
            backBuffer += pitch;
            y--;
        }
        m_dy += m_dyinc2;
    }
}

void SDLVideoOutputDevice::fillBackBuffer_ScaleXStretchY()
{
#ifdef _TRACE_VIDEO
    fprintf(stderr, "   SDLVideoOutputDevice::fillBackBuffer()\n");
#endif

    // The buffer cannot be set ahead of time, because it may
    //    return null if the surface is not locked.
    PUINT16 backBuffer = (PUINT16) m_pSurface->pixels;
    if (!backBuffer)
        return;

    INT32 pitch = m_pSurface->pitch >> 1;

    INT32 x = 0, y = 0;
    for (y = m_OutputHeight; y > 0; y--) {
        PUINT16 currentPixel;

        currentPixel = backBuffer;

        if (m_nScaleX == 1) {
            for (x = m_OutputWidth >> 1; x > 0; x--) {
                unsigned long a;
                a = m_WorkingPalette[*m_image++];
                a |= m_WorkingPalette[*m_image++] << 16;
                *((unsigned long *) currentPixel) = a;
                currentPixel += 2;
            }
            if (m_OutputWidth & 1) {
                *currentPixel++ = m_WorkingPalette[*m_image++];
            }
        }
        else if (m_nScaleX == 2) {
            for (x = m_OutputWidth >> 1; x > 0; x--) {
                unsigned long a;
                a = m_WorkingPalette[*m_image++];
                a |= a << 16;
                *((unsigned long *) currentPixel) = a;
                currentPixel += 2;
            }
        }
        else {
            for (x = m_OutputWidth; x > 0; x--) {
                unsigned long a;
                int scale = m_nScaleX;
                a = m_WorkingPalette[*m_image++];
                a |= a << 16;
                while (scale > 2) {
                    *((unsigned long *) currentPixel) = a;
                    currentPixel += 2;
                    scale -= 2;
                }
                if (scale == 1) {
                    *currentPixel++ = m_WorkingPalette[*m_image++];
                }
            }
        }

        backBuffer += pitch;

        while (m_dy < 0) {
            m_dy += m_dyinc1;
            memcpy(backBuffer,backBuffer-pitch,m_OutputWidth<<1);
            backBuffer += pitch;
            y--;
        }
        m_dy += m_dyinc2;
    }
}


PSURFACE SDLVideoOutputDevice::createSurfaceFromFile(const char* file)
{
    SDL_Surface* surface = NULL;

    if(file)
        surface = IMG_Load(file);

    return (PSURFACE)surface;
}

void SDLVideoOutputDevice::deleteSurface(void* surface) //PSURFACE surface)
{
    if(surface) {
        SDL_FreeSurface((SDL_Surface*)surface);
//        IMG_free((SDL_Surface*)surface);
        surface = NULL;
    }
}

UINT16 SDLVideoOutputDevice::getSurfaceHeight(PSURFACE inSurface)
{
    SDL_Surface* surface = (SDL_Surface*)inSurface;
    return surface->h;
}

UINT16 SDLVideoOutputDevice::getSurfaceWidth(PSURFACE inSurface)
{
    SDL_Surface* surface = (SDL_Surface*)inSurface;
    return surface->w;
}


void SDLVideoOutputDevice::clear(UINT32 color)
{
    SDL_FillRect(m_pSurface, NULL, color);
}
