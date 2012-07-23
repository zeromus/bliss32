
#ifndef INPUTBUS_H
#define INPUTBUS_H

#include "Controller.h"
#include "osd/InputDevice.h"

class Emulator;

const INT32 MAX_INPUT_CONSUMERS = 10;

class ControllerBus
{

    public:
        ControllerBus();
        void addController(Controller* ic);
        void removeController(Controller* ic);
        void reset();
        void updateControllers(InputDevice* id);

    private:
        Controller*     controllers[MAX_INPUT_CONSUMERS];
        INT32              controllerCount;

};

#endif
