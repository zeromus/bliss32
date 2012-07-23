
#ifndef ROM_H
#define ROM_H

#include <string.h>
#include <stdio.h>
#include "Memory.h"
#include "osd/types.h"

#pragma warning(disable:4786)	// Suppress STL debug info > 255 chars messages


template <typename W>
class ROM : public MemoryTemplate<W>
{

    public:
        ROM();
        ROM(UINT32 size, UINT32 location);
        ROM(W* image, UINT32 size, UINT32 location);
        void init(UINT32 size, UINT32 location);
        void init(W* image, UINT32 size, UINT32 location);
        ~ROM();
        void loadImage(W* image);
        void reset();
        UINT32 getLocation();
        W peek(UINT32 location);
        void poke(UINT32 location, W value);
        UINT32 getSize();
        static void loadROMImage(W** romImage, UINT32* size,
                const CHAR* resourceName, BOOL is16bit);
        static void loadROM(ROM** rom, UINT32 location, const CHAR* resourceName,
                BOOL is16bit);

    private:
        UINT32   size;
        UINT32   location;
        W*  image;

};

template <typename W>
ROM<W>::ROM()
    : image(NULL)
{}

template <typename W>
ROM<W>::ROM(UINT32 size, UINT32 location)
{
    init(size, location);
}

template <typename W>
ROM<W>::ROM(W* image, UINT32 size, UINT32 location)
{
    init(image, size, location);
}

template <typename W>
void ROM<W>::init(UINT32 size, UINT32 location)
{
    this->image = new W[size];
    this->size = size;
    this->location = location;
}

template <typename W>
void ROM<W>::init(W* image, UINT32 size, UINT32 location)
{
    this->image = new W[size];
    this->size = size;
    this->location = location;
    loadImage(image);
}

template <typename W>
void ROM<W>::loadImage(W* image)
{
    memcpy(this->image, image, size*sizeof(W));
}

template <typename W>
ROM<W>::~ROM()
{
    if (image != NULL)
        delete[] image;
}

template <typename W>
void ROM<W>::reset()
{
    //ROM can't be changed, so there's nothing to reset
}

template <typename W>
UINT32 ROM<W>::getLocation() {
    return location;
}

template <typename W>
W ROM<W>::peek(UINT32 location) {
    return image[location-this->location];
}

template <typename W>
void ROM<W>::poke(UINT32 location, W value)
{
    //can't change ROM
}

template <typename W>
UINT32 ROM<W>::getSize()
{
    return size;
}

template <typename W>
void ROM<W>::loadROMImage(W** romImage, UINT32* size, const CHAR* fileName,
        BOOL is16bit)
{
    UINT32 currentSize = 4096;
    W* image = new W[currentSize];
    UINT32 totalSize = 0;
    INT32 nextRead;

    FILE* file = fopen(fileName, "rb");
    nextRead = fgetc(file);
    while (nextRead != EOF) {
        if (totalSize >= currentSize) {
            W* newImage = new W[currentSize<<1];
            memcpy(newImage, image, currentSize*sizeof(W));
            currentSize <<= 1;
            delete[] image;
            image = newImage;
        }

        if (is16bit)
            image[totalSize] = (nextRead << 8) | fgetc(file);
        else
            image[totalSize] = nextRead;

        totalSize++;
        nextRead = fgetc(file);
    }

    W* finalImage = new W[totalSize];
    memcpy(finalImage, image, totalSize*sizeof(W));
    delete[] image;
    (*romImage) = finalImage;
    (*size) = totalSize;
}

template <typename W>
void ROM<W>::loadROM(ROM** rom, UINT32 location, const CHAR* fileName,
        BOOL is16bit)
{
    W* romImage;
    UINT32 size;
    loadROMImage(&romImage, &size, fileName, is16bit);
    (*rom) = new ROM(size, location);
    (*rom)->loadImage(romImage);
    delete[] romImage;
}

typedef ROM<UINT16> ROM16Bit;
typedef ROM<UINT8> ROM8Bit;

#endif
