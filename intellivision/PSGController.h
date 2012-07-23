
#ifndef PSGINPUTCONSUMER_H
#define PSGINPUTCONSUMER_H

#include "../core/Controller.h"
#include "../core/osd/types.h"

class PSGController : public Controller
{

    public:
        virtual INT32 getInputValue() = 0;
        virtual void setOutputValue(INT32 value) = 0;

};

#endif
