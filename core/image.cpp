#include "image.h"


// Load an image from disk
bool image::load(const char* file)
{
	m_file		= file;

    if(m_pSurface) {
        m_pVideo->deleteSurface(m_pSurface);
        m_pSurface = NULL;
    }

	m_pSurface	= m_pVideo->createSurfaceFromFile(m_file);

	if(m_pSurface)
	{
        if(m_colorKeyed) {
    		m_background = m_pVideo->getPixel(m_pSurface, 0, 0);
   		    m_pVideo->setColorKey(m_pSurface, m_background);
        }
		return true;
	}

	return false;
}



void image::draw(int x, int y) //, SDL_Rect *clip)
{
	if ((!m_pSurface) || (!m_pVideo))
		return;

	WHRect srcrect, dstrect; 
	srcrect.x = 0;
	srcrect.y = 0;
	srcrect.w = m_pVideo->getSurfaceWidth(m_pSurface);// - 1;
	srcrect.h = m_pVideo->getSurfaceHeight(m_pSurface);// - 1;
	dstrect.x = x;
	dstrect.y = y;
	dstrect.w = m_pVideo->getSurfaceWidth(m_pSurface) + x;
	dstrect.h = m_pVideo->getSurfaceHeight(m_pSurface) + y;

    while ( m_pVideo->blitSurface( m_pSurface, &srcrect, &dstrect) == -2 ) {
        load( m_file );
    }
}

/*
// This does not work yet!  SDL does not support stretching on the
// blit surface.
void image::drawStretched(int x, int y, int w, int h)
{
	if ((!m_pSurface) || (!m_pVideo))
		return;

	WHRect srcrect, dstrect; 
	srcrect.x = 0;
	srcrect.y = 0;
	srcrect.w = m_pVideo->getSurfaceWidth(m_pSurface) - 1;
	srcrect.h = m_pVideo->getSurfaceHeight(m_pSurface) - 1;
	dstrect.x = x;
	dstrect.y = y;
	dstrect.w = w;
	dstrect.h = h;
	m_pVideo->blitSurface( m_pSurface, &srcrect, &dstrect); 
}
*/

void image::setAlpha(UINT8 alpha)
{
    m_pVideo->setAlpha(m_pSurface, alpha);
}