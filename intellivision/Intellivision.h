
#ifndef INTELLIVISION_H
#define INTELLIVISION_H

#include "CP1610.h"
#include "AY38900.h"
#include "AY38914.h"
#include "HandController.h"
#include "cartridge/IntellivisionCartridge.h"
#include "ecs/ECS.h"
#include "ivoice/Intellivoice.h"
#include "../core/Emulator.h"
#include "../core/MemoryBus.h"
#include "../core/RAM.h"
#include "../core/ROM.h"

class Intellivision : public Emulator
{

    public:
        Intellivision();
        ~Intellivision();
        void reset();
        void insertCartridge(IntellivisionCartridge* cartridge);
        IntellivisionCartridge* getCartridge();
        void removeCartridge();
        void setExecImage(UINT16* execImage);
        void setGROMImage(UINT16* gromImage);
        BOOL hasGROMImage();
        void enableECSSupport(UINT16* image);
        void enableIntellivoiceSupport(UINT16* image);
        void setNetworkGame(BOOL b);
        void setPlayersSwitched(BOOL b);
        BOOL arePlayersSwitched();
        void setPlayerOneRemote(BOOL b);
        void setPlayerTwoRemote(BOOL b);

    private:
        void init();

        //audio output lines
        AudioOutputLine*  psgAudioLine;
        AudioOutputLine*  ecsAudioLine;
        AudioOutputLine*  ivoiceAudioLine;

        BOOL              playersSwitched;

        //the memory bus (16-bit)
        MemoryBus16Bit    memoryBus;

        //core processors
        CP1610            cpu;
        AY38900           stic;
        AY38914           psg;
    
        //core memories
        RAM16Bit          RAM8bit;
        RAM16Bit          RAM16bit;
        ROM16Bit          execROM;

        //signal lines
        SignalLine        SR1toINTRMLine;
        SignalLine        SR2toBUSRQLine;
        SignalLine        BUSAKtoSSTLine;

        //hand controllers
        HandController*   player1Controller;
        HandController*   player2Controller;

        //current cartridge in the Intellivision
        IntellivisionCartridge*        currentCartridge;

        //the ECS peripheral
        ECS               ecs;
        BOOL              ecsInUse;
    
        //the Intellivoice peripheral
        Intellivoice      intellivoice;
        BOOL              intellivoiceInUse;

};

#endif
