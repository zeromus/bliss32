#pragma warning(disable:4786)	// Suppress STL debug info > 255 chars messages

#include "IntellivisionCartridgeConfiguration.h"


IntellivisionCartridgeConfiguration::IntellivisionCartridgeConfiguration(
        UINT32 crc, CHAR* name, CHAR* producer, CHAR* year,
        UINT32 size, BOOL requiresECS, BOOL usesIntellivoice,
        UINT32 spanStart1, UINT32 spanEnd1)
    : CartridgeConfiguration(crc, name, producer, year, size)
{
    this->ecs = requiresECS;
    this->ivoice = usesIntellivoice;
    this->memoryCount = 1;
    this->spans = new INT32*[1];
    this->spans[0] = new INT32[3];
    this->spans[0][0] = spanStart1;
    this->spans[0][1] = (spanEnd1 - spanStart1)+1;
    this->spans[0][2] = -1;
}

IntellivisionCartridgeConfiguration::IntellivisionCartridgeConfiguration(
        UINT32 crc, CHAR* name, CHAR* producer, CHAR* year,
        UINT32 size, BOOL requiresECS, BOOL usesIntellivoice,
        UINT32 spanStart1, UINT32 spanEnd1,
        UINT32 spanStart2, UINT32 spanEnd2)
    : CartridgeConfiguration(crc, name, producer, year, size)
{
    this->ecs = requiresECS;
    this->ivoice = usesIntellivoice;
    this->memoryCount = 2;
    this->spans = new INT32*[2];
    for (int i = 0; i < (int)memoryCount; i++)
        this->spans[i] = new INT32[3];
    this->spans[0][0] = spanStart1;
    this->spans[0][1] = (spanEnd1 - spanStart1)+1;
    this->spans[0][2] = -1;
    this->spans[1][0] = spanStart2;
    this->spans[1][1] = (spanEnd2 - spanStart2)+1;
    this->spans[1][2] = -1;
}

IntellivisionCartridgeConfiguration::IntellivisionCartridgeConfiguration(
        UINT32 crc, CHAR* name, CHAR* producer, CHAR* year,
        UINT32 size, BOOL requiresECS, BOOL usesIntellivoice,
        UINT32 spanStart1, UINT32 spanEnd1,
        UINT32 spanStart2, UINT32 spanEnd2,
        UINT32 spanStart3, UINT32 spanEnd3)
    : CartridgeConfiguration(crc, name, producer, year, size)
{
    this->ecs = requiresECS;
    this->ivoice = usesIntellivoice;
    this->memoryCount = 3;
    this->spans = new INT32*[3];
    for (int i = 0; i < (int)memoryCount; i++)
        this->spans[i] = new INT32[3];
    this->spans[0][0] = spanStart1;
    this->spans[0][1] = (spanEnd1 - spanStart1)+1;
    this->spans[0][2] = -1;
    this->spans[1][0] = spanStart2;
    this->spans[1][1] = (spanEnd2 - spanStart2)+1;
    this->spans[1][2] = -1;
    this->spans[2][0] = spanStart3;
    this->spans[2][1] = (spanEnd3 - spanStart3)+1;
    this->spans[2][2] = -1;
}

IntellivisionCartridgeConfiguration::IntellivisionCartridgeConfiguration(
        UINT32 crc, CHAR* name, CHAR* producer, CHAR* year,
        UINT32 size, BOOL requiresECS, BOOL usesIntellivoice,
        UINT32 spanStart1, UINT32 spanEnd1,
        UINT32 spanStart2, UINT32 spanEnd2,
        UINT32 spanStart3, UINT32 spanEnd3,
        UINT32 spanStart4, UINT32 spanEnd4)
    : CartridgeConfiguration(crc, name, producer, year, size)
{
    this->ecs = requiresECS;
    this->ivoice = usesIntellivoice;
    this->memoryCount = 4;
    this->spans = new INT32*[4];
    for (int i = 0; i < (int)memoryCount; i++)
        this->spans[i] = new INT32[3];
    this->spans[0][0] = spanStart1;
    this->spans[0][1] = (spanEnd1 - spanStart1)+1;
    this->spans[0][2] = -1;
    this->spans[1][0] = spanStart2;
    this->spans[1][1] = (spanEnd2 - spanStart2)+1;
    this->spans[1][2] = -1;
    this->spans[2][0] = spanStart3;
    this->spans[2][1] = (spanEnd3 - spanStart3)+1;
    this->spans[2][2] = -1;
    this->spans[3][0] = spanStart4;
    this->spans[3][1] = (spanEnd4 - spanStart4)+1;
    this->spans[3][2] = -1;
}

IntellivisionCartridgeConfiguration::~IntellivisionCartridgeConfiguration() {
    delete[] this->spans;
}

BOOL IntellivisionCartridgeConfiguration::requiresECS() {
    return ecs;
}

BOOL IntellivisionCartridgeConfiguration::usesIntellivoice() {
    return ivoice;
}

UINT32 IntellivisionCartridgeConfiguration::getMemoryCount() {
    return memoryCount;
}

UINT32 IntellivisionCartridgeConfiguration::getMemoryLocation(UINT32 i) {
    return spans[i][0];
}

UINT32 IntellivisionCartridgeConfiguration::getMemorySize(UINT32 i) {
    return spans[i][1];
}

BOOL IntellivisionCartridgeConfiguration::isMemoryBanked(UINT32 i) {
    return (spans[i][2] != -1);
}

UINT32 IntellivisionCartridgeConfiguration::getMemoryBank(UINT32 i) {
    return spans[i][1];
}

