// Font functions
//
// Based upon the SoFont SDL library, modified to abstract it from SDL.
//


#include "font.h"


// Load a font from disk
bool font::load(const char* file)
{
	m_file		= file;

    if(m_pSurface) {
        m_pVideo->deleteSurface(m_pSurface);
        m_pSurface = NULL;
    }

	m_pSurface	= m_pVideo->createSurfaceFromFile(m_file);

	if(m_pSurface)
	{
		m_height = m_pVideo->getSurfaceHeight(m_pSurface)-1;
		UINT16 width = m_pVideo->getSurfaceWidth(m_pSurface)-1;

		int x = 0, i = 0, s = 0, p = 0, CharPos[256], spacing[256];
		while(x <= width)
		{
			if(doStartNewChar(x)) {
				if(i)
					spacing[i-1]=1+x-s;

				p = x;
				while((x < width) && doStartNewChar(x))
					x++;

				s = x;
				CharPos[i++]=(p+x)>>1;
			}
			x++;
		}
		spacing[i]=x-p;
		CharPos[i++]=width+1;

		m_maxI = FONT_STARTCHAR+i-1;
		m_background = m_pVideo->getPixel(m_pSurface, 0, m_height);
		m_pVideo->setColorKey(m_pSurface, m_background);
		cleanFont();

		m_pCharPos = new int[i];
		m_pSpacing=new int[i];
		memcpy(m_pCharPos, CharPos, i*sizeof(int));
		memcpy(m_pSpacing, spacing, i*sizeof(int));

		// search for a smart space width:
		m_spacewidth = 0;
		if (!m_spacewidth)
			m_spacewidth = textWidth("a");
		if (!m_spacewidth)
			m_spacewidth = textWidth("A");
		if (!m_spacewidth)
			m_spacewidth = textWidth("0");
		if (!m_spacewidth)
			m_spacewidth = CharPos[1] - CharPos[0];

		// Search for a smart cursor position:
		int cursBegin=0;
		int cursWidth=0;
		if (m_background == m_pVideo->getPixel(m_pSurface,
				CharPos['|' - FONT_STARTCHAR], m_height/2))
		{
			// Up to the first | color
			for(cursBegin=0; cursBegin <= textWidth("|"); cursBegin++)
				if (m_background != m_pVideo->getPixel(m_pSurface,
						CharPos['|'-FONT_STARTCHAR]+cursBegin, m_height>>1))
					break;
			// Up to the end of the | color
			for(cursWidth=0; cursWidth <= textWidth("|"); cursWidth++)
				if (m_background == m_pVideo->getPixel(m_pSurface,
						CharPos['|'-FONT_STARTCHAR]+cursBegin+cursWidth,
						m_height>>1))
					break;
		}
		else
		{
			// Up to the end of the | color
			for(cursWidth=0; cursWidth <= textWidth("|"); cursWidth++)
				if (m_background == m_pVideo->getPixel(m_pSurface,
						CharPos['|'-FONT_STARTCHAR]+cursWidth, m_height>>1))
					break;
		}
		m_cursShift = cursBegin+1; //cursWidth could be used if image format changes.


		return true;
	}

	return false;
}


bool font::doStartNewChar(INT32 x)
{
	if(!m_pSurface)
		return false;

	return m_pVideo->getPixel(m_pSurface, x, 0) ==
		m_pVideo->mapRGB(m_pSurface, 0xFF, 0x00, 0xFF);
}

void font::cleanFont()
{
	int x=0;
	while(x < m_pVideo->getSurfaceWidth(m_pSurface))
	{
		int y = 0;
		while(y < m_pVideo->getSurfaceHeight(m_pSurface))
		{
			if(m_pVideo->getPixel(m_pSurface, x, y)==
					m_pVideo->mapRGB(m_pSurface, 0xFF, 0x00, 0xFF))
				m_pVideo->setPixel(m_pSurface, x, y, m_background);
			y++;
		}
		x++;
	}
}

int font::textWidth(const char* text, const int min, const int max)
{
	int ofs, x = 0, i = min;
	while ((text[i] != '\0') && (i < max))
	{
		if (text[i] == ' ')
		{
			x += m_spacewidth;
			i++;
		}
		else if ((text[i] >= FONT_STARTCHAR) && (text[i] <= m_maxI))
		{
			ofs = text[i] - FONT_STARTCHAR;
			x += m_pSpacing[ofs];
			i++;
		}
		else
			i++;
	}
	return x;
}

void font::write(int x, int y, const char *text)
{
	if ((!m_pSurface) || (!m_pVideo) || (!text))
		return;

	int ofs, i=0;
	WHRect srcrect, dstrect; 
	while (text[i]!='\0')
	{
		if (text[i]==' ')
		{
			x += m_spacewidth;
			i++;
		}
		else if ((text[i] >= FONT_STARTCHAR) && (text[i] <= m_maxI))
		{
			ofs = text[i] - FONT_STARTCHAR;
			srcrect.w = dstrect.w = m_pCharPos[ofs+1] - m_pCharPos[ofs];
			srcrect.h = dstrect.h = m_height;
			srcrect.x = m_pCharPos[ofs];
			srcrect.y = 1;
			dstrect.x = x;
			dstrect.y = y;
			x += m_pSpacing[ofs];

            while ( m_pVideo->blitSurface( m_pSurface, &srcrect, &dstrect) == -2 ) {
                load( m_file );
            }

			i++;
		}
		else
			i++;// other chars are ignored
	}
}
