#include <string.h>
#include <stdlib.h>
#include "SDLInputSignal.h"

SDLInputSignal::SDLInputSignal(UINT64 did, SDLKey cid, const CHAR* desc)
{
    deviceID = did;
    controlID = cid;
    description = new CHAR[strlen(desc)];
    strcpy(description, desc);
    configKey[0] = 0;
    CHAR buffer[20];
    itoa((int)deviceID, buffer, 10);
    strcpy(configKey, buffer);
    strcat(configKey, ":");
    itoa((int)controlID, buffer, 10);
    strcat(configKey, buffer);
    strcat(configKey, ":");
    strcat(configKey, description);
}

SDLInputSignal::SDLInputSignal(const SDLInputSignal& dis)
{
    deviceID = dis.deviceID;
    controlID = dis.controlID;
    description = new CHAR[strlen(dis.description)];
    strcpy(description, dis.description);
    strcpy(configKey, dis.configKey);
}

SDLInputSignal::~SDLInputSignal()
{
    delete[] description;
}

const CHAR* SDLInputSignal::getDescription()
{
    return description;
}

const CHAR* SDLInputSignal::getConfigKey()
{
    return configKey;
}
