
#ifndef CRC32_H
#define CRC32_H

#include "../osd/types.h"

class CRC32
{

    public:
        CRC32();
        void reset();
        void update(UINT8 data);
        void update(UINT8* data, UINT32 length);
        UINT32 getValue();
        static UINT32 getCrc(const CHAR* filename);

    private:
        UINT32 crc;
        UINT32 crc32_table[256];

};

#endif

