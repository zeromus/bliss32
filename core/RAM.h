
#ifndef RAM_H
#define RAM_H

#include "Memory.h"

template <typename W>
class RAM : public MemoryTemplate<W>
{

    public:
        RAM();
        RAM(UINT32 size, UINT32 location);
        RAM(UINT32 size, UINT32 location, W bitWidth);
        void init(UINT32 size, UINT32 location);
        void init(UINT32 size, UINT32 location, W bitWidth);
        ~RAM();
        void reset();
        UINT32 getLocation();
        virtual W peek(UINT32 location);
        virtual void poke(UINT32 location, W value);
        UINT32 getSize();

    protected:
        UINT32  size;
        UINT32  location;

    private:
        W  bitWidth;
        W  trimmer;
        W* image;

};

template <typename W>
RAM<W>::RAM()
    : size(0),
      location(0),
      bitWidth(0),
      trimmer(0),
      image(NULL)
{}

template <typename W>
RAM<W>::RAM(UINT32 size, UINT32 location)
{
    init(size, location);
}

template <typename W>
RAM<W>::RAM(UINT32 size, UINT32 location, W bitWidth)
{
    init(size, location, bitWidth);
}

template <typename W>
void RAM<W>::init(UINT32 size, UINT32 location)
{
    init(size, location, sizeof(W)<<3);
}

template <typename W>
void RAM<W>::init(UINT32 size, UINT32 location, W bitWidth)
{
    this->size = size;
    this->location = location;
    this->bitWidth = bitWidth;
    this->trimmer = (W)(1 << bitWidth) -1;
    image = new W[size];
}

template <typename W>
RAM<W>::~RAM()
{
    if (image)
        delete[] image;
}

template <typename W>
void RAM<W>::reset()
{
    for (W i = 0; i < size; i++)
        image[i] = 0;
}

template <typename W>
UINT32 RAM<W>::getLocation()
{
    return location;
}

template <typename W>
W RAM<W>::peek(UINT32 location)
{
    return image[location-this->location];
}

template <typename W>
void RAM<W>::poke(UINT32 location, W value)
{
    image[location-this->location] = (value & trimmer);
}

template <typename W>
UINT32 RAM<W>::getSize()
{
    return size;
}

typedef RAM<UINT16> RAM16Bit;
typedef RAM<UINT8> RAM8Bit;

#endif
