
#ifndef VIDEOOUTPUTDEVICE_H
#define VIDEOOUTPUTDEVICE_H

#include "types.h"
#include "Device.h"


#define MAX_PALETTEENTRIES 256

// Error message return codes
#define ERR_INIT_VIDEO	1
#define ERR_SET_MODE	2
#define ERR_NO_LOCK		4

// Toggle switches
#define MODE_FULLSCREEN	0
#define MODE_WINDOWED	1
#define MODE_SWITCH		2


class VideoOutputDevice : public Device
{

    public:
		// Set attributes of emulated video memory
        virtual void changeInputSize(const UINT16 width, const UINT16 height) = 0;
		// Set size attributes of emulated video memory
        virtual ERRCODE changeOutputSize(const UINT16 width, const UINT16 height) = 0;
		// Get size attributes of emulated video memory
        virtual void getOutputSize(UINT16& fullwidth, UINT16& fullheight,
						UINT16& winwidth, UINT16& winheight)
		{
			fullwidth	= m_OutputWidthFS;
			fullheight	= m_OutputHeightFS;
			winwidth	= m_OutputWidthWin;
			winheight	= m_OutputHeightWin;
		}
		// Return the current size
		virtual int getOutputHeight() = 0;
		virtual int getOutputWidth() = 0;
		// Set default size variables - THIS DOES NOT CHANGE THE CURRENT MODE/SIZE
		virtual void setFullScreenSize(const UINT16 x, const UINT16 y)
		{
			m_OutputWidthFS		= x;
			m_OutputHeightFS	= y;
		}
		// Set default size variables - THIS DOES NOT CHANGE THE CURRENT MODE/SIZE
		virtual void setWindowSize(const UINT16 x, const UINT16 y)
		{
			m_OutputWidthWin	= x;
			m_OutputHeightWin	= y;
		}
		// Toggle FullScreen / Windowed mode
		virtual ERRCODE toggleScreen(const int mode = 2) = 0;   
		// Determine FullScreen / Windowed mode
		// Set RGB conversion palette
        virtual void setPalette(const UINT32* entries, const UINT8 numEntries) = 0;
		// Update the emulated video
        virtual ERRCODE renderPalettedImage(UINT8* image) = 0;
        // Set output game.  WARNING: NOT SUPPORTED EVERYWHERE
        virtual void setGamma(float r, float g, float b) = 0;
        // Output the emulated video
		virtual void present() = 0;
        // Erase screen
        virtual void clear(UINT32 color) = 0;

		// Surface calls - used by fonts and images
		virtual PSURFACE	createSurfaceFromFile(const char* file) = 0;
		virtual void		deleteSurface(void* surface) = 0; //PSURFACE surface) = 0;
		virtual UINT16		getSurfaceHeight(PSURFACE surface) = 0;
		virtual UINT16		getSurfaceWidth(PSURFACE surface) = 0;
		virtual UINT32		getPixel(PSURFACE surface, INT32 x, INT32 y) = 0;
		virtual void		setPixel(PSURFACE inSurface, INT32 x, INT32 y,
								UINT32 background) = 0;
		virtual UINT32		mapRGB(PSURFACE inSurface, int r, int g, int b) = 0;
		virtual void		setColorKey(PSURFACE inSurface, UINT32 background) = 0;
		virtual int 		blitSurface(PSURFACE inSurface,	const WHRect* srcrect,
								const WHRect* dstrect) = 0; 
        virtual void        setAlpha(PSURFACE inSurface, UINT8 alpha) = 0;


		virtual BOOL stopRequested() = 0;

	protected:
		UINT16	m_InputWidth;		// width of emulated video memory
		UINT16	m_InputHeight;		// height of emulated video memory
		UINT16	m_OutputWidthFS;	// width of output screen
		UINT16	m_OutputHeightFS;	// height of output screen
		UINT16	m_OutputWidthWin;	// width of output window
		UINT16	m_OutputHeightWin;	// height of output window
		UINT16	m_OutputDepth;		// Bits per pixel of output surface

		UINT32	m_Palette[MAX_PALETTEENTRIES];
		UINT32	m_WorkingPalette[MAX_PALETTEENTRIES];
};

#endif
