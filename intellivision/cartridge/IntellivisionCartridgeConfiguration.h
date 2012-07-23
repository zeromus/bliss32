
#ifndef INTELLIVISIONCARTRIDGECONFIGURATION_H
#define INTELLIVISIONCARTRIDGECONFIGURATION_H

#include "../../core/osd/types.h"
#include "../../core/cartridge/CartridgeConfiguration.h"

class IntellivisionCartridgeConfiguration : public CartridgeConfiguration
{

    public:
        IntellivisionCartridgeConfiguration(UINT32 crc, CHAR* name,
                CHAR* producer, CHAR* year,
                UINT32 size, BOOL requiresECS, BOOL usesIntellivoice,
                UINT32 spanStart1, UINT32 spanEnd1);
        IntellivisionCartridgeConfiguration(UINT32 crc, CHAR* name,
                CHAR* producer, CHAR* year,
                UINT32 size, BOOL requiresECS, BOOL usesIntellivoice,
                UINT32 spanStart1, UINT32 spanEnd1,
                UINT32 spanStart2, UINT32 spanEnd2);
        IntellivisionCartridgeConfiguration(UINT32 crc, CHAR* name,
                CHAR* producer, CHAR* year,
                UINT32 size, BOOL requiresECS, BOOL usesIntellivoice,
                UINT32 spanStart1, UINT32 spanEnd1,
                UINT32 spanStart2, UINT32 spanEnd2,
                UINT32 spanStart3, UINT32 spanEnd3);
        IntellivisionCartridgeConfiguration(UINT32 crc, CHAR* name,
                CHAR* producer, CHAR* year,
                UINT32 size, BOOL requiresECS, BOOL usesIntellivoice,
                UINT32 spanStart1, UINT32 spanEnd1,
                UINT32 spanStart2, UINT32 spanEnd2,
                UINT32 spanStart3, UINT32 spanEnd3,
                UINT32 spanStart4, UINT32 spanEnd4);
        ~IntellivisionCartridgeConfiguration();
        UINT32 getMemoryCount();
        UINT32 getMemoryLocation(UINT32 i);
        UINT32 getMemorySize(UINT32 i);
        BOOL isMemoryBanked(UINT32 i);
        UINT32 getMemoryBank(UINT32 i);
        BOOL requiresECS();
        BOOL usesIntellivoice();

    private:
        BOOL        ecs;
        BOOL        ivoice;
        UINT32      memoryCount;
        INT32**    spans;

};

#endif

