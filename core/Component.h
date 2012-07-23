
#ifndef COMPONENT_H
#define COMPONENT_H

#include "font.h"
#include "image.h"
#include "osd/types.h"
#include "osd/VideoOutputDevice.h"
//#include <list>
#include <vector>


#define PRESS_COMPONENT 0
#define NEXT_COMPONENT  1
#define PREV_COMPONENT  2
#define NEXT_PAGE       4
#define PREV_PAGE       8


class Component
{
public:
    virtual void processEvent(INT32 inputID) {}    // Not all controls process events
    virtual void render(VideoOutputDevice*, int xOffset, int yOffset) {} //= 0;
    virtual void render(VideoOutputDevice*) {m_x=0; m_y=0;} // = 0;

protected:
    int m_x, m_y;
};


// TextCtl:  "Title" type control.  This is only for text that
//           does not need to be selected!
class TextCtl : public Component
{
public:
    TextCtl::TextCtl(font* inFont, const char* inText,
                        int inX, int inY) {
        m_pFont = inFont;
        m_pText = inText;
        m_x     = inX;
        m_y     = inY;
    }
    TextCtl::~TextCtl() {}

    void render(VideoOutputDevice* pvod, int xOffset, int yOffset);

private:
    font* m_pFont;
    const char* m_pText;
};


// ImageCtl: This is a non-interactive image that can be used in the
//           foreground, or as the background for a menu.
class ImageCtl : public Component
{
public:
    ImageCtl::ImageCtl(image* inImage, const UINT8 inAlpha,
                        int inX, int inY) {
        m_pImage = inImage;
        m_pImage->setAlpha(inAlpha);
        m_x      = inX;
        m_y      = inY;
    }
    ImageCtl::~ImageCtl() {}

    void render(VideoOutputDevice* pvod, int OffsetX, int OffsetY);

private:
    image* m_pImage;
};


// ListItem: an individual list item
typedef struct tagListItem        // X/Y-Width/Height coordinate
{
    const char* description;
    void (*callback)(void *listItem);
    void* data;
} ListItem, *PListItem;

typedef vector<PListItem> LISTITEMS;
//typedef list<PListItem> LISTITEMS;

// ListCtl: The main workhorse
//.
class ListCtl : public Component
{
public:
    ListCtl::ListCtl(font* inNrmFont, font* inSelFont,
            int inX, int inY, int viewable) {
        m_pNrmFont       = inNrmFont;
        m_pSelFont       = inSelFont;
        m_x              = inX;
        m_y              = inY;
        m_nItemsViewable = viewable;

        m_nFirstViewable = 0;
        m_selection      = 0;

        m_nMaxChars_Norm = 0;
        m_nMaxChars_Sel  = 0;
    }
    ListCtl::~ListCtl() {}

    void addItem(PListItem, bool alphabetize);
    void render(VideoOutputDevice* pvod, int OffsetX, int OffsetY);
    void processEvent(INT32 inputID);

    void setMaxChars(int norm, int sel)
    {
        m_nMaxChars_Norm = norm;
        m_nMaxChars_Sel  = sel;        
    };

private:
    font* m_pNrmFont;
    font* m_pSelFont;
    int   m_selection;
    int   m_nItemsViewable;
    int   m_nFirstViewable;

    int   m_nMaxChars_Norm;
    int   m_nMaxChars_Sel;

    LISTITEMS m_liv;
};

#endif
