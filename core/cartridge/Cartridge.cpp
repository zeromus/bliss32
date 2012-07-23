
#include "Cartridge.h"

Cartridge::Cartridge(const CHAR* name, const CHAR* producer, const CHAR* year,
        UINT32 size)
{
    this->name = name;
    this->producer = producer;
    this->year = year;
    this->size = size;
}

const CHAR* Cartridge::getName()
{
    return name;
}

const CHAR* Cartridge::getProducer()
{
    return producer;
}

const CHAR* Cartridge::getYear()
{
    return year;
}

UINT32 Cartridge::getSize()
{
    return size;
}
