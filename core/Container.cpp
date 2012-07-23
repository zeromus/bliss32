#include "Container.h"

void Container::addComponent(Component* c)
{
    components.push_back(c);
    m_focus = c;
}

void Container::render(VideoOutputDevice* pvod)
{
    int xpos = (pvod->getOutputWidth() - m_x) >> 1;
    int ypos = (pvod->getOutputHeight() - m_y) >> 1;

    INT32 size = components.size();
    for (INT32 i = 0; i < size; i++) {
        components[i]->render(pvod, xpos, ypos);
    }
}

void Container::processEvent(INT32 inputID)
{
    if(m_focus)
        m_focus->processEvent(inputID);
}
