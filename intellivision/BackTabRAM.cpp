
#include "BackTabRAM.h"

BackTabRAM::BackTabRAM()
    : image(NULL),
      dirtyBytes(NULL)
{
	size = 0;
}

void BackTabRAM::init(UINT32 size, UINT32 location, UINT16 bitWidth)
{
    this->size = size;
    this->location = location;
    this->bitWidth = bitWidth;
    trimmer = (1 << bitWidth) - 1;
    image = new UINT16[size];
    dirtyBytes = new BOOL[size];
}

BackTabRAM::~BackTabRAM() {
    if (image != NULL)
        delete[] image;
    if (dirtyBytes != NULL)
        delete[] dirtyBytes;
}

void BackTabRAM::reset() {
    dirtyRAM = TRUE;
    colorAdvanceBitsDirty = TRUE;
    for (UINT32 i = 0; i < size; i++) {
        image[i] = 0;
        dirtyBytes[i] = TRUE;
    }
}

UINT32 BackTabRAM::getLocation() {
    return location;
}

UINT16 BackTabRAM::peek(UINT32 location) {
    return image[location-this->location];
}

void BackTabRAM::poke(UINT32 location, UINT16 value) {
    value = (value & trimmer);
    if (image[location-this->location] != value) {
        if ((image[location-this->location] & 0x2000) != (value & 0x2000))
            colorAdvanceBitsDirty = TRUE;
        image[location-this->location] = value;
        dirtyBytes[location-this->location] = TRUE;
        dirtyRAM = TRUE;
    }
}

void BackTabRAM::markClean() {
    if (!dirtyRAM)
        return;

    for (UINT32 i = 0; i < size; i++)
        dirtyBytes[i] = FALSE;
    dirtyRAM = FALSE;
    colorAdvanceBitsDirty = FALSE;
}

BOOL BackTabRAM::areColorAdvanceBitsDirty() {
    return colorAdvanceBitsDirty;
}

BOOL BackTabRAM::isDirty() {
    return dirtyRAM;
}

BOOL BackTabRAM::isDirty(UINT32 location) {
    return dirtyBytes[location-this->location];
}

UINT32 BackTabRAM::getSize() {
    return size;
}

