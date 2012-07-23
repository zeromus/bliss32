#pragma warning(disable:4786)	// Suppress STL debug info > 255 chars messages

#ifndef INTELLIVISIONRECOGNITIONDATABASE_H
#define INTELLIVISIONRECOGNITIONDATABASE_H

#include <map>
#include "IntellivisionCartridgeConfiguration.h"
#include "../../core/osd/types.h"
#include "../../core/cartridge/RecognitionDatabase.h"

typedef std::map<UINT32, IntellivisionCartridgeConfiguration*> RECOGNITIONMAP;


class IntellivisionRecognitionDatabase : public RecognitionDatabase
{

    public:
        CartridgeConfiguration* getConfiguration(UINT32 crc);
        static IntellivisionRecognitionDatabase FACTORY;

    private:
        IntellivisionRecognitionDatabase();
        static RECOGNITIONMAP recognitionMap;
        static IntellivisionCartridgeConfiguration configurations[170];

};

#endif

