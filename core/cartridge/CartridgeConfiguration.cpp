
#include "CartridgeConfiguration.h"

CartridgeConfiguration::CartridgeConfiguration(UINT32 crc, const CHAR* name,
        const CHAR* producer, const CHAR* year, UINT32 size)
{
    this->crc = crc;
    this->name = name;
    this->producer = producer;
    this->year = year;
    this->size = size;
}

UINT32 CartridgeConfiguration::getCrc() {
    return crc;
}

const CHAR* CartridgeConfiguration::getName() {
    return name;
}

const CHAR* CartridgeConfiguration::getProducer() {
    return producer;
}

const CHAR* CartridgeConfiguration::getYear() {
    return year;
}

UINT32 CartridgeConfiguration::getSize() {
    return size;
}

