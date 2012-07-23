#include "Component.h"

void TextCtl::render(VideoOutputDevice* pvod, int xOffset, int yOffset)
{
    m_pFont->write(m_x+xOffset, m_y+yOffset, m_pText);
}


void ImageCtl::render(VideoOutputDevice* pvod, int xOffset, int yOffset)
{
    m_pImage->draw(m_x+xOffset, m_y+yOffset);
}


void ListCtl::addItem(PListItem li, bool alphabetize)
                      //const char *description, void* calback())
{
    INT32 size = m_liv.size();
    if(size && alphabetize) {
        for (INT32 i = 0; i < size; i++) {
            if(strcmp(m_liv[i]->description, li->description) > 0)
            {
                m_liv.insert(m_liv.begin()+i, li);
                return;
            }
        }
        m_liv.push_back(li);
    }
    else {
        m_liv.push_back(li);        
    }
 
}

void ListCtl::render(VideoOutputDevice* pvod, int xOffset, int yOffset)
{
    INT32 size = m_liv.size();
    for (INT32 i = m_nFirstViewable;
            i < size && i < m_nFirstViewable+m_nItemsViewable;
            i++)
    {

        font* pFont;
        int   maxchars = 0;
        if(i == m_selection) {
            maxchars = m_nMaxChars_Sel;
            pFont = m_pSelFont;
        }
        else {
            maxchars = m_nMaxChars_Norm;
            pFont = m_pNrmFont;
        }

        if(!maxchars)
            pFont->write(m_x+xOffset, m_y+yOffset, m_liv[i]->description);
        else {
            char temp[256];
            strncpy(temp, m_liv[i]->description, maxchars);
            temp[maxchars] = '\0';
            pFont->write(m_x+xOffset, m_y+yOffset, temp);
        }

        yOffset += pFont->fontHeight() + 1;
    }
}

void ListCtl::processEvent(INT32 inputID)
{
    switch(inputID) {

    case PRESS_COMPONENT:
        if(m_liv[m_selection]->callback)
            m_liv[m_selection]->callback(m_liv[m_selection]);
        break;

    case PREV_COMPONENT:
        m_selection > 0 ? m_selection-- : m_selection = m_liv.size()-1;
        break;
    case NEXT_COMPONENT:
        m_selection < (m_liv.size()-1) ? m_selection++ : m_selection = 0;
        break;

    case PREV_PAGE:
        m_selection > 0 ? m_selection -= m_nItemsViewable : m_selection = m_liv.size() - 1;
        if(m_selection < 0)
            m_selection = 0;
        break;
    case NEXT_PAGE:
        m_selection < (m_liv.size()-1) ? m_selection += m_nItemsViewable : m_selection = 0;
        if(m_selection > (m_liv.size()-1))
            m_selection = m_liv.size() - 1;
        break;
    }

    if(m_selection < m_nFirstViewable) {
        m_nFirstViewable = m_selection;
    }
    else if(m_selection > m_nFirstViewable + m_nItemsViewable - 1) {
        m_nFirstViewable = m_selection - m_nItemsViewable + 1;
    }
}