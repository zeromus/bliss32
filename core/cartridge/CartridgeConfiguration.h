
#ifndef CARTRIDGECONFIGURATION_H
#define CARTRIDGECONFIGURATION_H

#include "../osd/types.h"

class CartridgeConfiguration
{

    public:
        CartridgeConfiguration(UINT32 crc, const CHAR* name,
                const CHAR* producer, const CHAR* year, UINT32 size);
        UINT32 getCrc();
        const CHAR* getName();
        const CHAR* getProducer();
        const CHAR* getYear();
        UINT32 getSize();

        virtual UINT32 getMemoryCount() = 0;
        virtual UINT32 getMemoryLocation(UINT32 i) = 0;
        virtual UINT32 getMemorySize(UINT32 i) = 0;
        virtual BOOL isMemoryBanked(UINT32 i) = 0;
        virtual UINT32 getMemoryBank(UINT32 i) = 0;

    private:
        UINT32      crc;
        const CHAR* name;
        const CHAR* producer;
        const CHAR* year;
        UINT32      size;

};

#endif

