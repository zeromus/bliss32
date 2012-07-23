
#ifndef CONTAINER_H
#define CONTAINER_H

#include <vector>
#include "Component.h"
#include "osd/VideoOutputDevice.h"
#include "osd/types.h"

using namespace std;


typedef vector<Component*> COMPONENTVECTOR;


class Container : public Component
{
public:

	Container::Container(int x, int y) {
        m_x = x;
        m_y = y;
        m_focus = NULL;
        m_parent = NULL;
	}
	Container::~Container() {
	}

    void addComponent(Component* c);
//  void removeComponent(Component* c);
    void processEvent(INT32 inputID);
    void render(VideoOutputDevice* pvod);

    Container* m_parent;

private:
    COMPONENTVECTOR components;
    Component* m_focus;
};

#endif
