
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "osd/types.h"
#include "osd/InputDevice.h"

/**
 * A Controller is a gaming input device, such as the hand controllers for
 * the Intellivision and Atari 5200 or the joysticks and paddles for the
 * Atari2600.
 */
class Controller
{

    public:
        /**
         * Reset the controller.
         */
        virtual void reset() = 0;
        virtual void update(InputDevice*) = 0;
        virtual INT32 getControlCount() = 0;
        virtual INT32 getControlDeviceID(INT32 controlNum) = 0;
        virtual const CHAR* getControlDescription(INT32 controlNum) = 0;

};

#endif
