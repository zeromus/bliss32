#ifndef FONT_H
#define FONT_H

#include "osd/types.h"
#include "osd/VideoOutputDevice.h"
#include <vector>

using namespace std ;

#define FONT_STARTCHAR 33


// An individual font instance
class font
{
public:
	font::font(VideoOutputDevice* video, const char* file) {
		m_pVideo = video;
        m_pSurface = NULL;

		m_maxI = 0;
		m_spacewidth = 0;
		m_background = 0;
		m_pSpacing = NULL;
		m_pCharPos = NULL;

		load(file);
	}
	font::~font() {
		if(m_pVideo && m_pSurface)
			m_pVideo->deleteSurface(m_pSurface);
	}

	bool load(const char* file);

	int		textWidth(const char* text, const int min=0, const int max=255);
    int     fontHeight()
    {
        return m_height;
    }

	void	write(int x, int y, const char *text); //, SDL_Rect *clip)

private:
	bool	doStartNewChar(INT32 x);
	void	cleanFont();

	VideoOutputDevice*	m_pVideo;
	PSURFACE			m_pSurface;
	const char*			m_file;
	
	int		m_height;		// Font height
	int		m_maxI;
	int		m_spacewidth;
	int		m_cursShift;
	UINT32	m_background;
	int*	m_pSpacing;		// Character spacing
	int*	m_pCharPos;		// Character start
};



#endif