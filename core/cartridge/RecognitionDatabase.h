
#ifndef RECOGNITIONDATABASE_H
#define RECOGNITIONDATABASE_H

#include <map>
#include "../osd/types.h"
#include "CartridgeConfiguration.h"

class RecognitionDatabase
{

    public:
        /**
         * Obtains the cartridge configuration for the cartridge matching
         * the specified CRC, or NULL if the CRC does not match a known
         * cartridge.
         *
         * @param  crc the CRC of the desired cartridge configuration
         * @return the cartridge configuration of the cartridge matching
         *         the specified CRC if the CRC is recognized.
         *         NULL otherwise
         */
        virtual CartridgeConfiguration* getConfiguration(UINT32 cartCrc) = 0;

};

#endif

