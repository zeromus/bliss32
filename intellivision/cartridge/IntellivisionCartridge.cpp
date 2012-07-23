
#include <iostream>
#include "IntellivisionCartridge.h"
#include "IntellivisionRecognitionDatabase.h"
#include "../../core/cartridge/CRC32.h"
#include "../../core/ROM.h"

#pragma warning(disable:4786) //Suppress STL debug info > 255 chars messages

using namespace std;

IntellivisionCartridge::IntellivisionCartridge(const CHAR* name,
        Memory16Bit** memories, INT32 memCount)
    : Cartridge(name, NULL, NULL, 0)
{
    this->memories = memories;
    this->memCount = memCount;
    this->ecs = FALSE;
    this->ivoice = FALSE;
}

IntellivisionCartridge::IntellivisionCartridge(const CHAR* name,
        Memory16Bit** memories, INT32 memCount, BOOL requiresECS,
        BOOL usesIntellivoice)
    : Cartridge(name, NULL, NULL, 0)
{
    this->memories = memories;
    this->memCount = memCount;
    this->ecs = requiresECS;
    this->ivoice = usesIntellivoice;
}

IntellivisionCartridge::~IntellivisionCartridge() {
    delete[] memories;
}

UINT32 IntellivisionCartridge::getMemoryCount() {
    return memCount;
}

Memory* IntellivisionCartridge::getMemory(UINT32 i) {
    return memories[i];
}

BOOL IntellivisionCartridge::requiresECS() {
    return ecs;
}

BOOL IntellivisionCartridge::usesIntellivoice() {
    return ivoice;
}

IntellivisionCartridge* IntellivisionCartridge::loadCartridge(const CHAR* filename) {
    UINT16* image;
    UINT32 size;
    ROM16Bit::loadROMImage(&image, &size, filename, TRUE);

    UINT32 crc = CRC32::getCrc(filename);
    IntellivisionCartridgeConfiguration* type =
            (IntellivisionCartridgeConfiguration*)
            IntellivisionRecognitionDatabase::FACTORY.getConfiguration(crc);

    IntellivisionCartridge* cart;
    ROM16Bit* rom;
    if (type == NULL) {
        Memory16Bit** memories = new Memory16Bit*[1];
        rom = new ROM16Bit((size>>1), 0x5000);
        rom->loadImage(image);
        delete[] image;
        memories[0] = rom;
        cart = new IntellivisionCartridge("Unknown", memories, 1);
    }
    else if (type->getMemoryCount() == 1) {
        Memory16Bit** memories = new Memory16Bit*[1];
        rom = new ROM16Bit(size, type->getMemoryLocation(0));
        rom->loadImage(image);
        delete[] image;
        memories[0] = rom;
        cart = new IntellivisionCartridge(type->getName(), memories, 1,
                type->requiresECS(), type->usesIntellivoice());
    }
    else {
        //split the image into multiple smaller memory images
        INT32 memoryCount = type->getMemoryCount();
        Memory16Bit** memories = new Memory16Bit*[memoryCount];
        INT32 nextMemStart = 0;
        for (INT32 i = 0; i < memoryCount; i++) {
            INT32 nextSize = type->getMemorySize(i);
            rom = new ROM16Bit(nextSize, type->getMemoryLocation(i));
            rom->loadImage(image+nextMemStart);
            memories[i] = rom;
            nextMemStart += nextSize;
        }
        delete[] image;
        cart = new IntellivisionCartridge(type->getName(), memories, memoryCount,
                type->requiresECS(), type->usesIntellivoice());
    }
    
    return cart;
}

