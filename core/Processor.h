
#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "osd/types.h"

/**
 * An abstract class representing a processor, which is a hardware component
 * that requires a clock input.
 *
 * @author Kyle Davis
 */
class Processor
{

    public:
        Processor() : isIdle(FALSE) {};
        virtual void reset() = 0;
        virtual INT32 getClockSpeed() = 0;
        virtual INT32 tick() = 0;

        BOOL isIdle;

};

#endif
