#ifndef IMAGE_H
#define IMAGE_H

#include "osd/types.h"
#include "osd/VideoOutputDevice.h"


// An individual font instance
class image
{
public:
	image::image(VideoOutputDevice* video, const char* file, bool keyed) {
		m_pVideo = video;
        m_pSurface = NULL;

        m_colorKeyed = keyed;
		load(file);
	}
	image::~image() {
		if(m_pVideo && m_pSurface)
			m_pVideo->deleteSurface(m_pSurface);
	}

	bool load(const char* file);

	void draw(int x, int y);
//	void drawStretched(int x, int y, int w, int h);

    void setAlpha(UINT8 alpha);

private:
	VideoOutputDevice*	m_pVideo;
	PSURFACE			m_pSurface;
	const char*			m_file;
    bool                m_colorKeyed;
	
	UINT32	m_background;
};


#endif