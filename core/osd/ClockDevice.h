
#ifndef CLOCKDEVICE_H
#define CLOCKDEVICE_H

#include "types.h"
#include "Device.h"

class ClockDevice : public Device
{

    public:
        virtual INT64 getTickFrequency() = 0;
        virtual INT64 getTick() = 0;

};

#endif
