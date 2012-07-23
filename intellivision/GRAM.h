
#ifndef GRAM_H
#define GRAM_H

#include "../core/Memory.h"

class GRAM : public Memory16Bit
{

    public:
        GRAM();
        void init(UINT32 size, UINT32 location, UINT16 bitWidth);
        ~GRAM();
        void reset();
        UINT32 getLocation();
        UINT16 peek(UINT32 location);
        void poke(UINT32 location, UINT16 value);
        void markClean();
        BOOL isDirty();
        BOOL isDirty(UINT32 location);
        UINT32 getSize();
/*
        void save(SaveOutputStream os) throws IOException;
        void load(LoadInputStream is) throws IOException;
*/

    private:
        UINT32       size;
        UINT32       location;
        UINT16       bitWidth;
        UINT16       trimmer;
        UINT16*      image;
        BOOL*     dirtyBytes;
        BOOL      dirtyRAM;

};

#endif

