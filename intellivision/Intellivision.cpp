
#include <iostream>
#include "Intellivision.h"

using namespace std;

Intellivision::Intellivision()
{
    intellivoiceInUse = FALSE;
    ecsInUse = FALSE;
    playersSwitched = FALSE;
	currentCartridge = NULL;
    init();
}

Intellivision::~Intellivision()
{
    //delete the audio output lines
    audioMixer.releaseLine(&psgAudioLine);
    audioMixer.releaseLine(&ecsAudioLine);
    audioMixer.releaseLine(&ivoiceAudioLine);

    delete player1Controller;
    delete player2Controller;
}

void Intellivision::setNetworkGame(BOOL b)
{
    //emuDirector.setNetworkGame(b);
}

void Intellivision::setPlayersSwitched(BOOL b)
{
    if (playersSwitched == b)
        return;

    playersSwitched = b;
    HandController* hold = player1Controller;
    player1Controller = player2Controller;
    player2Controller = hold;
    psg.setPSGController0(player1Controller);
    psg.setPSGController1(player2Controller);
}

BOOL Intellivision::arePlayersSwitched()
{
    return playersSwitched;
}

void Intellivision::setPlayerOneRemote(BOOL b)
{
    //emuDirector.setPlayerOneRemote(b);
}

void Intellivision::setPlayerTwoRemote(BOOL b)
{
    //emuDirector.setPlayerTwoRemote(b);
}

/**
 * Initializes all of the basic hardware included in the Intellivision
 * Master Component as well as the ECS and Intellivoice peripherals.
 * This method is called only once when the Intellivision is constructed.
 */
void Intellivision::init()
{
    //init the memory bus
    memoryBus.init(0x10000);
    memoryBus.setUnmappedPeek(0xFFFF);

    //create the hand controllers
    player1Controller = new HandController(PLAYER_ONE);
    controllerBus.addController(player1Controller);
    player2Controller = new HandController(PLAYER_TWO);
    controllerBus.addController(player2Controller);

    //8 bit RAM used by EXEC and cartridge
    RAM8bit.init(0x00F0, 0x0100, 8);
    memoryBus.addMemory(&RAM8bit);

    //the executive (EXEC) rom
    execROM.init(0x1000, 0x1000);
    memoryBus.addMemory(&execROM);

    //16 bit RAM used for...
    //    02F0       EXEC Object GRAM Allocation Table Pointer
    //    02F1-0318  1610 Stack
    //    0319-031C  16 bit RAM for EXEC
    //    031D-035C  Moving Object RAM Data Bases
    //    035D-035F  16 bit RAM for EXEC
    RAM16bit.init(0x0070, 0x02F0, 16);
    memoryBus.addMemory(&RAM16bit);

    //init the CPU
    cpu.init(&memoryBus, &SR1toINTRMLine, &SR2toBUSRQLine,
            &BUSAKtoSSTLine, 0x1000, 0x1004);
    processorBus.addProcessor(&cpu);

    //init the STIC
    stic.init(&cpu, &SR1toINTRMLine, &SR2toBUSRQLine,
            &BUSAKtoSSTLine);
    setVideoProducer(&stic);
    processorBus.addProcessor(&stic);
    INT32 memoryCount = stic.getMemoryCount();
    for (INT32 i = 0; i < memoryCount; i++)
        memoryBus.addMemory(stic.getMemory(i));

    //init the sound chip
    audioMixer.createLine(&psgAudioLine);
    psg.init(psgAudioLine, 0x01F0, player1Controller, player2Controller);
    psgAudioLine->acquire();
    processorBus.addProcessor(&psg);
    memoryBus.addMemory(&psg.registers);

    //init the ECS for ECS cartridge support
    audioMixer.createLine(&ecsAudioLine);
    this->ecs.init(ecsAudioLine);

    //init the Intellivoice for Intellivoice cartridge support
    audioMixer.createLine(&ivoiceAudioLine);
    this->intellivoice.init(ivoiceAudioLine);
}

/**
 * Pushes the reset button on the Intellivision.
 */
void Intellivision::reset()
{
    Emulator::reset();
    memoryBus.reset();
}

/**
 * Inserts a cartridge into the Intellivision cartridge slot.
 *
 * @param cartridge the cartridge to insert
 */
void Intellivision::insertCartridge(IntellivisionCartridge* cartridge)
{
    //remove the old cartridge
    IntellivisionCartridge* oldCart = currentCartridge;
    if (currentCartridge != NULL)
        removeCartridge();
    this->currentCartridge = cartridge;

    //add the ECS if this cartridge needs it
    if (currentCartridge->requiresECS()) {
        processorBus.addProcessor(&ecs.psg2);
        memoryBus.addMemory(&ecs.psg2.registers);
        memoryBus.addMemory(&ecs.bank0);
        memoryBus.addMemory(&ecs.bank1);
        memoryBus.addMemory(&ecs.bank2);
        memoryBus.addMemory(&ecs.ramBank);
        memoryBus.addMemory(&ecs.uart);
        ecsAudioLine->acquire();
        ecsInUse = TRUE;
    }

    //add the Intellivoice if this cartridge can use it and we
    //have the ROM necessary for it
    if (currentCartridge->usesIntellivoice() &&
            intellivoice.hasIntellivoiceImage())
    {
        processorBus.addProcessor(&intellivoice.microSequencer);
        processorBus.addProcessor(&intellivoice.lpc12);
        memoryBus.addMemory(&intellivoice.microSequencer.registers);
        ivoiceAudioLine->acquire();
        intellivoiceInUse = TRUE;
    }

    //add the cartridge
    INT32 memoryCount = cartridge->getMemoryCount();
    for (INT32 i = 0; i < memoryCount; i++)
        memoryBus.addMemory((Memory16Bit*)cartridge->getMemory(i));
}

IntellivisionCartridge* Intellivision::getCartridge()
{
    return currentCartridge;
}

/**
 * Removes the current cartridge from the Intellivision cartridge slot.
 */
void Intellivision::removeCartridge()
{
    if (currentCartridge == NULL)
        return;

    IntellivisionCartridge* oldCart = currentCartridge;
    //remove the cartridge
    INT32 memoryCount = currentCartridge->getMemoryCount();
    for (INT32 i = 0; i < memoryCount; i++)
        memoryBus.removeMemory((Memory16Bit*)currentCartridge->getMemory(i));

    //remove the Intellivoice
    if (intellivoiceInUse) {
        ivoiceAudioLine->release();
        memoryBus.removeMemory(&intellivoice.microSequencer.registers);
        processorBus.removeProcessor(&intellivoice.lpc12);
        processorBus.removeProcessor(&intellivoice.microSequencer);
        intellivoiceInUse = FALSE;
    }

    if (ecsInUse) {
        ecsAudioLine->release();
        memoryBus.removeMemory(&ecs.uart);
        memoryBus.removeMemory(&ecs.ramBank);
        memoryBus.removeMemory(&ecs.bank2);
        memoryBus.removeMemory(&ecs.bank1);
        memoryBus.removeMemory(&ecs.bank0);
        memoryBus.removeMemory(&ecs.psg2.registers);
        processorBus.removeProcessor(&ecs.psg2);
        ecsInUse = TRUE;
    }

    currentCartridge = NULL;
}

void Intellivision::setExecImage(UINT16* execImage) {
    execROM.loadImage(execImage);
}

void Intellivision::setGROMImage(UINT16* gromImage) {
    stic.setGROMImage(gromImage);
}

void Intellivision::enableIntellivoiceSupport(UINT16* image) {
    intellivoice.setIntellivoiceImage(image);
}

void Intellivision::enableECSSupport(UINT16* image) {
    ecs.setECSROMImage(image);
}


