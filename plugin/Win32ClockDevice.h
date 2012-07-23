
#ifndef WIN32CLOCKDEVICE_H
#define WIN32CLOCKDEVICE_H

#include <windows.h>
#include "../core/osd/ClockDevice.h"

class Win32ClockDevice : public ClockDevice
{

    public:
        INT64 getTickFrequency();
        INT64 getTick();

};

#endif

