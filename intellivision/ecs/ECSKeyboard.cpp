
#include "ECSKeyboard.h"

const INT32 ECSKeyboard::mappings[48] = {
     0,  1,  2,  3,  4,  5,
     8,  9, 10, 11, 12, 13,
    16, 17, 18, 19, 20, 21,
    24, 25, 26, 27, 28, 29,
    32, 33, 34, 35, 36, 37,
    40, 41, 42, 43, 44, 45,
    48, 49, 50, 51, 52, 53,
    57, 58, 59, 60, 61, 62
};

const INT32 ECSKeyboard::CONTROL_IDS[48] = {
    KEYBOARD_LEFT,
    KEYBOARD_COMMA,
    KEYBOARD_N,
    KEYBOARD_V,
    KEYBOARD_X,
    KEYBOARD_SPACE,
    KEYBOARD_PERIOD,
    KEYBOARD_M,
    KEYBOARD_B,
    KEYBOARD_C,
    KEYBOARD_Z,
    KEYBOARD_DOWN,
    KEYBOARD_SEMICOLON,
    KEYBOARD_K,
    KEYBOARD_H,
    KEYBOARD_F,
    KEYBOARD_S,
    KEYBOARD_UP,
    KEYBOARD_P,
    KEYBOARD_I,
    KEYBOARD_Y,
    KEYBOARD_R,
    KEYBOARD_W,
    KEYBOARD_Q,
    KEYBOARD_ESCAPE,
    KEYBOARD_9,
    KEYBOARD_7,
    KEYBOARD_5,
    KEYBOARD_3,
    KEYBOARD_1,
    KEYBOARD_0,
    KEYBOARD_8,
    KEYBOARD_6,        
    KEYBOARD_4,
    KEYBOARD_2,
    KEYBOARD_RIGHT,
    KEYBOARD_ENTER,    
    KEYBOARD_O,
    KEYBOARD_U,
    KEYBOARD_T,
    KEYBOARD_E,
    KEYBOARD_CONTROL,
    KEYBOARD_L,
    KEYBOARD_J,
    KEYBOARD_G,
    KEYBOARD_D,
    KEYBOARD_A,
    KEYBOARD_SHIFT,
};

ECSKeyboard::ECSKeyboard()
{
}

void ECSKeyboard::reset() {
    for (INT32 i = 0; i < 8; i++)
        rowInputValues[i] = 0;
}

void ECSKeyboard::update(InputDevice* inputDevice) {
    for (INT32 row = 0; row < 8; row++) {
        rowInputValues[row] = 0;
        if (inputDevice->getControlValue(CONTROL_IDS[row]) == SIGNAL_ON)
            rowInputValues[row] |= 0x01;
        if (inputDevice->getControlValue(CONTROL_IDS[row+8]) == SIGNAL_ON)
            rowInputValues[row] |= 0x02;
        if (inputDevice->getControlValue(CONTROL_IDS[row+16]) == SIGNAL_ON)
            rowInputValues[row] |= 0x04;
        if (inputDevice->getControlValue(CONTROL_IDS[row+24]) == SIGNAL_ON)
            rowInputValues[row] |= 0x08;
        if (inputDevice->getControlValue(CONTROL_IDS[row+32]) == SIGNAL_ON)
            rowInputValues[row] |= 0x10;
        if (inputDevice->getControlValue(CONTROL_IDS[row+40]) == SIGNAL_ON)
            rowInputValues[row] |= 0x20;
        if (inputDevice->getControlValue(CONTROL_IDS[row+48]) == SIGNAL_ON)
            rowInputValues[row] |= 0x40;
        if (inputDevice->getControlValue(CONTROL_IDS[row+56]) == SIGNAL_ON)
            rowInputValues[row] |= 0x80;
    }
}

INT32 ECSKeyboard::getInputValue() {
    INT32 inputValue = 0;
    INT32 rowMask = 1;
    for (INT32 row = 0; row < 8; row++)  {
        if ((rowsToScan & rowMask) != 0) {
            rowMask = rowMask << 1;
            continue;
        }
        inputValue |= rowInputValues[row];

        rowMask = rowMask << 1;
    }

    return (0xFF ^ inputValue);
}

void ECSKeyboard::setOutputValue(INT32 value) {
    this->rowsToScan = value;
}

INT32 ECSKeyboard::getControlCount() {
    return 48;
}

INT32 ECSKeyboard::getControlDeviceID(INT32 controlNum) {
    return CONTROL_IDS[controlNum];
}

const CHAR* ECSKeyboard::getControlDescription(INT32 controlNum) {
    //nothing for now
    return NULL;
}
