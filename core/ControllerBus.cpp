
#include <iostream>
#include "ControllerBus.h"

using namespace std;

ControllerBus::ControllerBus()
{
    controllerCount = 0;
}

void ControllerBus::addController(Controller* p)
{
    controllers[controllerCount] = p;
    controllerCount++;
}

void ControllerBus::removeController(Controller* p)
{
    for (INT32 i = 0; i < controllerCount; i++) {
        if (controllers[i] == p) {
            for (INT32 j = i; j < (controllerCount-1); j++)
                controllers[j] = controllers[j+1];
            controllerCount--;
        }
    }
}

void ControllerBus::reset()
{
    //reset each controller
    for (INT32 i = 0; i < controllerCount; i++)
        controllers[i]->reset();
}

void ControllerBus::updateControllers(InputDevice* id)
{
    //tell each of the controllers that they may now pull their data from
    //the input device
    for (INT32 i = 0; i < controllerCount; i++)
        controllers[i]->update(id);
}
