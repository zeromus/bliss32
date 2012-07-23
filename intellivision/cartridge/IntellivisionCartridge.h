
#ifndef INTELLIVISIONCARTRIDGE_H
#define INTELLIVISIONCARTRIDGE_H

#include "../../core/Memory.h"
#include "../../core/cartridge/Cartridge.h"

class IntellivisionCartridge : public Cartridge
{

    public:
        IntellivisionCartridge(const CHAR* name, Memory16Bit** memories,
                INT32 memCount);
        IntellivisionCartridge(const CHAR* name, Memory16Bit** memories,
                INT32 memCount, BOOL requiresECS, BOOL usesIntellivoice);
        ~IntellivisionCartridge();

        UINT32 getMemoryCount();
        Memory* getMemory(UINT32 i);
        BOOL requiresECS();
        BOOL usesIntellivoice();
        static IntellivisionCartridge* loadCartridge(const CHAR* filename);

    private:
        Memory16Bit**  memories;
        UINT32         memCount;
        BOOL           ecs;
        BOOL           ivoice;

};

#endif
