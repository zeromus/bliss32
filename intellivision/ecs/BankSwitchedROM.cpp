
#include <iostream>
#include "BankSwitchedROM.h"

BankSwitchedROM::BankSwitchedROM()
    : image(NULL)
{}

BankSwitchedROM::BankSwitchedROM(UINT32 size, UINT32 page,
        UINT32 location)
    : image(NULL)
{
    init(size, page, location);
}

BankSwitchedROM::~BankSwitchedROM()
{
    if (image != NULL)
        delete[] image;
}

void BankSwitchedROM::init(UINT32 size, UINT32 page, UINT32 location)
{
    this->image = new UINT16[size];
    this->size = size;
    this->imagePage = page;
    this->location = location;
    this->currentPage = 0;
    this->pageChangeValue = (location & 0xF000) | 0x0A50;
}

void BankSwitchedROM::loadImage(UINT16* image)
{
    memcpy(this->image, image, size*sizeof(UINT16));
}

void BankSwitchedROM::reset() {
    currentPage = 0;
}

UINT32 BankSwitchedROM::getSize() {
    return size;
}

UINT32 BankSwitchedROM::getLocation() {
    return location;
}

void BankSwitchedROM::poke(UINT32 location, UINT16 value) {
    if (((location - this->location) == 0x0FFF) &&
            ((value & 0xFFF0) == pageChangeValue))
        currentPage = (value & 0x000F);
}

UINT16 BankSwitchedROM::peek(UINT32 location) {
    return (this->currentPage == this->imagePage
            ? this->image[location - this->location] : 0xFFFF );
}

/*
void BankSwitchedROM::save(SaveOutputStream sos) throws IOException {
    sos.writeInt(currentPage);
}

void BankSwitchedROM::load(LoadInputStream lis) throws IOException {
    currentPage = lis.readInt(0, 15);
}
*/

