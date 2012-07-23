
#ifndef ECSKEYBOARD_H
#define ECSKEYBOARD_H

#include "../PSGController.h"
#include "../../core/Controller.h"
#include "../../core/osd/InputDevice.h"

class EmulationDirector;

class ECSKeyboard : public PSGController
{

    friend class EmulationDirector;

    public:
        ECSKeyboard();
        void reset();
        void update(InputDevice* inputDevice);

        INT32 getControlCount();
        INT32 getControlDeviceID(INT32 num);
        const CHAR* getControlDescription(INT32 controlNum);

        INT32 getInputValue();
        void setOutputValue(INT32 value);

    private:
        INT32        rowsToScan;
        INT32        rowInputValues[8];
        InputDevice* inputDevice;

        const static INT32 mappings[48];
        const static INT32 CONTROL_IDS[48];

};

#endif

