#ifndef SDLVIDEOOUTPUTDEVICE_H
#define SDLVIDEOOUTPUTDEVICE_H

#include "../core/osd/VideoOutputDevice.h"
#include <SDL.h>

class SDLVideoOutputDevice : public VideoOutputDevice
{
    public:
        SDLVideoOutputDevice();
        ~SDLVideoOutputDevice();

        const CHAR* getErrorDescription(ERRCODE errorCode);

        ERRCODE init();
        void    release();


        // Mode and resizing
        bool    enumModes(char* output);
		bool	windowed() {return !(m_Flags & SDL_FULLSCREEN);} 
		ERRCODE	toggleScreen(const int mode);

        void changeInputSize(const UINT16 width, const UINT16 height);
		ERRCODE	changeOutputSize(const UINT16 width, const UINT16 height);
		int		getOutputHeight() {	// current size, regardless of mode
			return m_OutputHeight;
		}
		int		getOutputWidth() {
			return m_OutputWidth;
		}


        // Drawing functions
        void setPalette(const UINT32* palette, const UINT8 numEntries);
        ERRCODE renderPalettedImage(UINT8* image);
		ERRCODE reset();
        void setGamma(float r, float g, float b);
        void present();
        void clear(UINT32 color);

        // Surface management functions
        PSURFACE createSurfaceFromFile(const char* file);
        void     deleteSurface(void*); ///PSURFACE surface);
        UINT16   getSurfaceHeight(PSURFACE surface);
        UINT16   getSurfaceWidth(PSURFACE surface);

        UINT32 getPixel(PSURFACE surface, INT32 x, INT32 y);
        void   setPixel(PSURFACE inSurface, INT32 x, INT32 y,
                UINT32 background);
        UINT32 mapRGB(PSURFACE inSurface, int r, int g, int b);
        void   setColorKey(PSURFACE inSurface, UINT32 background);
        int    blitSurface(PSURFACE inSurface, const WHRect* srcrect,
                const WHRect* dstrect);
        void  setAlpha(PSURFACE inSurface, UINT8 alpha);

        inline BOOL stopRequested() { return m_bStop; }

    private:
        // Buffer copy routines
        virtual void fillBackBuffer();
        virtual void fillBackBuffer_NoScaling();
        virtual void fillBackBuffer_Shrink();
        virtual void fillBackBuffer_Stretch();
        virtual void fillBackBuffer_ScaleXStretchY();

        // Surface change management routines
        void recalculatePalette();    
        void calculateStretchIndices();

        BOOL m_bStop;

        SDL_Surface* m_pSurface;

        UINT16  m_InputWidth;
        UINT16  m_InputHeight;

		UINT16	m_OutputWidth;		// Current Working Width
		UINT16	m_OutputHeight;		// Current Working Height

        // Scaling
        INT32 m_dy, m_dyinc1, m_dyinc2;
        INT32 m_dx, m_dxinc1, m_dxinc2;
        INT32 m_nScaleX;

        // Input image
        UINT8*    m_image;

        // fullscreen, etc...
        UINT32    m_Flags;

        //size of palette
        INT8    m_iNumEntries;
        UINT32* m_iEntries;

};

#endif

