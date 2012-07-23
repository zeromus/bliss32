
#ifndef POKEY_H
#define POKEY_H

#include "../core/Processor.h"

class Pokey : public Processor
{

    private:
        UINT8 AUDCTL;
        UINT8 AUDFX0;
        UINT8 AUDFX1;
        UINT8 AUDFX2;
        UINT8 AUDFX3;
        UINT8 AUDCX0;
        UINT8 AUDCX1;
        UINT8 AUDCX2;
        UINT8 AUDCX3;

};

#endif
