
#include <stdio.h>
#include <iostream>
#include "HandController.h"

using namespace std;

const INT32 HandController::CONTROL_IDS[2][23] = {
    {
        PLAYER_ONE_KEYPAD_ONE,
        PLAYER_ONE_KEYPAD_TWO,
        PLAYER_ONE_KEYPAD_THREE,
        PLAYER_ONE_KEYPAD_FOUR,
        PLAYER_ONE_KEYPAD_FIVE,
        PLAYER_ONE_KEYPAD_SIX,
        PLAYER_ONE_KEYPAD_SEVEN,
        PLAYER_ONE_KEYPAD_EIGHT,
        PLAYER_ONE_KEYPAD_NINE,
        PLAYER_ONE_KEYPAD_DELETE,
        PLAYER_ONE_KEYPAD_ZERO,
        PLAYER_ONE_KEYPAD_ENTER,
        PLAYER_ONE_BUTTON_ONE,
        PLAYER_ONE_BUTTON_TWO,
        PLAYER_ONE_BUTTON_THREE,
        PLAYER_ONE_NORTH,
        PLAYER_ONE_NORTHEAST,
        PLAYER_ONE_EAST,
        PLAYER_ONE_SOUTHEAST,
        PLAYER_ONE_SOUTH,
        PLAYER_ONE_SOUTHWEST,
        PLAYER_ONE_WEST,
        PLAYER_ONE_NORTHWEST,
    },
    {
        PLAYER_TWO_KEYPAD_ONE,
        PLAYER_TWO_KEYPAD_TWO,
        PLAYER_TWO_KEYPAD_THREE,
        PLAYER_TWO_KEYPAD_FOUR,
        PLAYER_TWO_KEYPAD_FIVE,
        PLAYER_TWO_KEYPAD_SIX,
        PLAYER_TWO_KEYPAD_SEVEN,
        PLAYER_TWO_KEYPAD_EIGHT,
        PLAYER_TWO_KEYPAD_NINE,
        PLAYER_TWO_KEYPAD_DELETE,
        PLAYER_TWO_KEYPAD_ZERO,
        PLAYER_TWO_KEYPAD_ENTER,
        PLAYER_TWO_BUTTON_ONE,
        PLAYER_TWO_BUTTON_TWO,
        PLAYER_TWO_BUTTON_THREE,
        PLAYER_TWO_NORTH,
        PLAYER_TWO_NORTHEAST,
        PLAYER_TWO_EAST,
        PLAYER_TWO_SOUTHEAST,
        PLAYER_TWO_SOUTH,
        PLAYER_TWO_SOUTHWEST,
        PLAYER_TWO_WEST,
        PLAYER_TWO_NORTHWEST,
    },
};

const INT32 HandController::BUTTON_OUTPUT_VALUES[15] = {
    0x81, //OUTPUT_KEYPAD_ONE
    0x41, //OUTPUT_KEYPAD_TWO
    0x21, //OUTPUT_KEYPAD_THREE
    0x82, //OUTPUT_KEYPAD_FOUR
    0x42, //OUTPUT_KEYPAD_FIVE
    0x22, //OUTPUT_KEYPAD_SIX
    0x84, //OUTPUT_KEYPAD_SEVEN
    0x44, //OUTPUT_KEYPAD_EIGHT
    0x24, //OUTPUT_KEYPAD_NINE
    0x88, //OUTPUT_KEYPAD_CLEAR
    0x48, //OUTPUT_KEYPAD_ZERO
    0x28, //OUTPUT_KEYPAD_ENTER
    0xA0, //OUTPUT_ACTION_BUTTON_TOP
    0x60, //OUTPUT_ACTION_BUTTON_BOTTOM_LEFT
    0xC0  //OUTPUT_ACTION_BUTTON_BOTTOM_RIGHT
};

const INT32 HandController::DIRECTION_OUTPUT_VALUES[16] = {
    0x04, //OUTPUT_DISC_NORTH
    0x14, //OUTPUT_DISC_NORTH_NORTH_EAST
    0x16, //OUTPUT_DISC_NORTH_EAST
    0x06, //OUTPUT_DISC_EAST_NORTH_EAST
    0x02, //OUTPUT_DISC_EAST
    0x12, //OUTPUT_DISC_EAST_SOUTH_EAST
    0x13, //OUTPUT_DISC_SOUTH_EAST
    0x03, //OUTPUT_DISC_SOUTH_SOUTH_EAST
    0x01, //OUTPUT_DISC_SOUTH
    0x11, //OUTPUT_DISC_SOUTH_SOUTH_WEST
    0x19, //OUTPUT_DISC_SOUTH_WEST
    0x09, //OUTPUT_DISC_WEST_SOUTH_WEST
    0x08, //OUTPUT_DISC_WEST
    0x18, //OUTPUT_DISC_WEST_NORTH_WEST
    0x1C, //OUTPUT_DISC_NORTH_WEST
    0x0C  //OUTPUT_DISC_NORTH_NORTH_WEST
};

const double PI = 3.14159265358979323846;

const float HandController::vectorParse = (float)sin(PI/4.0);

HandController::HandController(PLAYER_NUM p)
{
	inputDevice = NULL;
    this->playerNum = p;
}

void HandController::reset()
{
    inputValue = 0xFF;
}

INT32 HandController::getControlCount()
{
    return 23;
}

const CHAR* HandController::getControlDescription(INT32 controlNum)
{
    //nothing for now
    return NULL;
}

INT32 HandController::getControlDeviceID(INT32 controlNum)
{
    return CONTROL_IDS[playerNum][controlNum];
}

void HandController::setOutputValue(INT32 value)
{}

INT32 HandController::getInputValue()
{
    return inputValue;
}

void HandController::update(InputDevice* inputDevice)
{
    inputValue = 0;

    for (INT32 i = 0; i < 16; i++) {
        if (inputDevice->getControlValue(CONTROL_IDS[playerNum][i]) ==
                SIGNAL_ON)
            inputValue |= BUTTON_OUTPUT_VALUES[i];
    }

    //evaluate the disc
    float neswVector =
            (inputDevice->getControlValue(CONTROL_IDS[playerNum][16]) -
            inputDevice->getControlValue(CONTROL_IDS[playerNum][20])) *
            vectorParse;
    float nwseVector =
            (inputDevice->getControlValue(CONTROL_IDS[playerNum][22]) -
            inputDevice->getControlValue(CONTROL_IDS[playerNum][18])) *
            vectorParse;
    float yPos = inputDevice->getControlValue(CONTROL_IDS[playerNum][15]) -
            inputDevice->getControlValue(CONTROL_IDS[playerNum][19]) +
            nwseVector + neswVector;
    float xPos = inputDevice->getControlValue(CONTROL_IDS[playerNum][17]) -
            inputDevice->getControlValue(CONTROL_IDS[playerNum][21]) -
            nwseVector + neswVector;

    if (xPos != 0 || yPos != 0) {
        double positionInRadians = (atan2(-xPos, -yPos)+PI);
        double offset = (2.0 * PI)/16.0;
        INT32 directionValue = DIRECTION_OUTPUT_VALUES[(INT32)(
                (positionInRadians+(offset/2.0))/offset) & 0x0F];
        inputValue |= directionValue;
    }

    inputValue = (0xFF ^ inputValue);
}

/*
private final static String[] descriptionKeys = {
        "InputKeyPadOne",
        "InputKeyPadTwo",
        "InputKeyPadThree",
        "InputKeyPadFour",
        "InputKeyPadFive",
        "InputKeyPadSix",
        "InputKeyPadSeven",
        "InputKeyPadEight",
        "InputKeyPadNine",
        "InputKeyPadClear",
        "InputKeyPadZero",
        "InputKeyPadEnter",
        "InputActionButtonsTop",
        "InputActionButtonBottomLeft",
        "InputActionButtonBottomRight",
        "InputDiscNorth",
        "InputDiscNorthEast",
        "InputDiscEast",
        "InputDiscSouthEast",
        "InputDiscSouth",
        "InputDiscSouthWest",
        "InputDiscWest",
        "InputDiscNorthWest"
    };
*/


