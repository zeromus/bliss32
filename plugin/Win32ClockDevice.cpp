
#include "Win32ClockDevice.h"

INT64 Win32ClockDevice::getTickFrequency()
{
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    return freq.QuadPart;
} 

INT64 Win32ClockDevice::getTick()
{
    LARGE_INTEGER freq;
    QueryPerformanceCounter(&freq);
    return freq.QuadPart;
} 

