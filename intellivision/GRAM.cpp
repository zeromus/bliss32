
#include "GRAM.h"

GRAM::GRAM()
    : //image(NULL),
      dirtyBytes(NULL)
{
	dirtyBytes = NULL;
	image = NULL;
	reset();
}

void GRAM::init(UINT32 size, UINT32 location, UINT16 bitWidth)
{
    this->size = size;
    this->location = location;
    this->bitWidth = bitWidth;
    trimmer = (1 << bitWidth) -1;
    image = new UINT16[size];
    dirtyBytes = new BOOL[size];
}

GRAM::~GRAM() {
    if (image != NULL)
        delete[] image;
    if (dirtyBytes != NULL)
        delete[] dirtyBytes;
}

void GRAM::reset() {
    dirtyRAM = TRUE;
	if(image && dirtyBytes)
		for (UINT32 i = 0; i < size; i++) {
			image[i] = 0;
			dirtyBytes[i] = TRUE;
		}
}

UINT32 GRAM::getLocation() {
    return location;
}

UINT16 GRAM::peek(UINT32 location) {
    return image[location-this->location];
}

void GRAM::poke(UINT32 location, UINT16 value) {
    value = (value & trimmer);
    if (image[location-this->location] != value) {
        image[location-this->location] = value;
        dirtyBytes[location-this->location] = TRUE;
        dirtyRAM = TRUE;
    }
}

void GRAM::markClean() {
    if (!dirtyRAM)
        return;

    for (UINT32 i = 0; i < size; i++)
        dirtyBytes[i] = FALSE;
    dirtyRAM = FALSE;
}

BOOL GRAM::isDirty() {
    return dirtyRAM;
}

BOOL GRAM::isDirty(UINT32 location) {
    return dirtyBytes[location-this->location];
}

UINT32 GRAM::getSize() {
    return size;
}

