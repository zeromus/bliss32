
#ifndef VIDEOPRODUCER_H
#define VIDEOPRODUCER_H

#include "osd/VideoOutputDevice.h"

class VideoProducer
{

    public:
        virtual void getOutputImageSize(UINT16* width, UINT16* height) = 0;
        virtual void getPalette(UINT32** palette, UINT8* numEntries) = 0;
        inline BOOL inVerticalBlank() { return inVBlank; }
        virtual void render(VideoOutputDevice*) = 0;

    protected:
        VideoProducer() : inVBlank(FALSE) {}
        BOOL inVBlank;

};

#endif
