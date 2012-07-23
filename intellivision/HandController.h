
#ifndef HANDCONTROLLER_H
#define HANDCONTROLLER_H

#include <math.h>
#include "PSGController.h"
#include "../core/Controller.h"

typedef enum _PLAYER_NUM {
    PLAYER_ONE = 0,
    PLAYER_TWO = 1,
} PLAYER_NUM;

class HandController : public PSGController
{

    public:
        HandController(PLAYER_NUM p);
        void reset();
        void update(InputDevice* inputDevice);

        INT32 getControlCount();
        const CHAR* getControlDescription(INT32 controlNum);
        INT32 getControlDeviceID(INT32 controlNum);

        void setOutputValue(INT32 value);
        INT32 getInputValue();

    private:
        PLAYER_NUM       playerNum;
        INT32            inputValue;
        InputDevice*     inputDevice;

        static const INT32 CONTROL_IDS[2][23];
        static const INT32 BUTTON_OUTPUT_VALUES[15];
        static const INT32 DIRECTION_OUTPUT_VALUES[16];
        static const float vectorParse;

};

#endif
