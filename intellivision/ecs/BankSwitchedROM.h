
#ifndef BANKSWITCHEDROM_H
#define BANKSWITCHEDROM_H

#include "../../core/osd/types.h"
#include "../../core/Memory.h"

class BankSwitchedROM : public Memory16Bit
{

    public:
        BankSwitchedROM();
        BankSwitchedROM(UINT32 size, UINT32 page, UINT32 location);
        ~BankSwitchedROM();
        void init(UINT32 size, UINT32 page, UINT32 location);
        void reset();
        void loadImage(UINT16* image);
        UINT32 getSize();
        UINT32 getLocation();
        void poke(UINT32 location, UINT16 value);
        UINT16 peek(UINT32 location);
/*
        void save(SaveOutputStream sos) throws IOException;
        void load(LoadInputStream lis) throws IOException;
*/

    private:
        UINT32  currentPage;
        UINT16* image;
        UINT32  size;
        UINT32  imagePage;
        UINT32  location;
        UINT16  pageChangeValue;

};

#endif

