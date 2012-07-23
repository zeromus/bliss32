
#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include "../Memory.h"
#include "../osd/types.h"

class Cartridge
{

    public:
        Cartridge(const CHAR* name, const CHAR* producer, const CHAR* year,
                UINT32 size);
        virtual const CHAR* getName();
        virtual const CHAR* getProducer();
        virtual const CHAR* getYear();
        virtual UINT32 getSize();

        virtual UINT32 getMemoryCount() = 0;
        virtual Memory* getMemory(UINT32 i) = 0;

    private:
        const CHAR* name;
        const CHAR* producer;
        const CHAR* year;
        UINT32 size;

};

#endif
