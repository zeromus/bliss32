
#include <string.h>
#include "SDLInputDevice.h"


// Text for error messages
const CHAR* inputErrorMessages[1] = {
    "Unable to initialize joystick",
};

char* g_lpszPOVDescriptions[4] = {
    "Up", "Right", "Down", "Left"
};

SDLKey DEFAULT_KEYS_PLAYER_ONE[28] = {
    SDLK_UP, (SDLKey)0, SDLK_RIGHT, (SDLKey)0, SDLK_DOWN, (SDLKey)0, SDLK_LEFT,
    (SDLKey)0,
    SDLK_LCTRL, SDLK_z, SDLK_x, (SDLKey)0, (SDLKey)0, (SDLKey)0,
    SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_5, SDLK_6, SDLK_7, SDLK_8, SDLK_9,
    SDLK_0, SDLK_DELETE, SDLK_RETURN, SDLK_ASTERISK, SDLK_PLUS
};

SDLKey DEFAULT_KEYS_PLAYER_TWO[28] = {
    SDLK_LEFTBRACKET, (SDLKey)0, SDLK_RIGHTBRACKET, (SDLKey)0, SDLK_BACKSLASH,
    (SDLKey)0,
    SDLK_QUOTE, (SDLKey)0, SDLK_RCTRL, SDLK_PERIOD, SDLK_BACKSLASH, (SDLKey)0,
    (SDLKey)0,
    (SDLKey)0,
    SDLK_q, SDLK_w, SDLK_e, SDLK_r, SDLK_t, SDLK_y, SDLK_u, SDLK_i, SDLK_o,
    SDLK_p, SDLK_k, SDLK_l, SDLK_COMMA, SDLK_m,
};

SDLKey DEFAULT_KEYS_KEYBOARD[49] = {
    SDLK_a, SDLK_b, SDLK_c, SDLK_d, SDLK_e, SDLK_f, SDLK_g, SDLK_h, SDLK_i,
    SDLK_j, SDLK_k, SDLK_l, SDLK_m, SDLK_n, SDLK_o, SDLK_p, SDLK_q, SDLK_r,
    SDLK_s, SDLK_t, SDLK_u, SDLK_v, SDLK_w, SDLK_x, SDLK_y, SDLK_z, SDLK_0,
    SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_5, SDLK_6, SDLK_7, SDLK_8, SDLK_9,
    SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT,
    SDLK_COMMA, SDLK_PERIOD, SDLK_SPACE, SDLK_SEMICOLON, SDLK_COLON,
    SDLK_ESCAPE, SDLK_RETURN, SDLK_LCTRL, SDLK_LSHIFT,
};

SDLKey DEFAULT_KEYS_EMULATOR[5] = {
    SDLK_F1, SDLK_F5, SDLK_F6, SDLK_F9, SDLK_F12
};

SDLKey DEFAULT_KEYS_MENU[6] = {
    SDLK_ESCAPE, SDLK_UP, SDLK_DOWN, SDLK_PAGEUP, SDLK_PAGEDOWN, SDLK_RETURN
};

SDLInputDevice::SDLInputDevice()
{
	m_requestFlags = 0;

    m_pMappedSignals = new SDLInputSignal*[TOTAL_CONTROL_COUNT];
//    memset(m_pMappedSignals, 0, sizeof(m_pMappedSignals));	// sizeof = 4
	memset(m_pMappedSignals, 0, sizeof(SDLInputSignal*)*TOTAL_CONTROL_COUNT);

    //initialize the player one input signals
    INT32 size = (PLAYER_ONE_ID_END - PLAYER_ONE_ID_START)+1;
    INT32 i;
    for (i = 0; i < size; i++) {
        if (!DEFAULT_KEYS_PLAYER_ONE[i])
            continue;

        m_pMappedSignals[PLAYER_ONE_ID_START+i] = new SDLInputSignal(0,
                DEFAULT_KEYS_PLAYER_ONE[i],
                SDL_GetKeyName(DEFAULT_KEYS_PLAYER_ONE[i]));
    }

    //initialize the player two input signals
    size = (PLAYER_TWO_ID_END - PLAYER_TWO_ID_START)+1;
    for (i = 0; i < size; i++) {
        if (!DEFAULT_KEYS_PLAYER_TWO[i])
            continue;

        m_pMappedSignals[PLAYER_TWO_ID_START+i] = new SDLInputSignal(0,
                DEFAULT_KEYS_PLAYER_TWO[i],
                SDL_GetKeyName(DEFAULT_KEYS_PLAYER_TWO[i]));
    }

    //initialize the ecs keyboard input signals
    size = (KEYBOARD_ID_END - KEYBOARD_ID_START)+1;
    for (i = 0; i < size; i++) {
        if (!DEFAULT_KEYS_KEYBOARD[i])
            continue;

        m_pMappedSignals[KEYBOARD_ID_START+i] = new SDLInputSignal(0,
                DEFAULT_KEYS_KEYBOARD[i],
                SDL_GetKeyName(DEFAULT_KEYS_KEYBOARD[i]));
    }

    //initialize the emulator control input signals
    size = (EMULATOR_ID_END - EMULATOR_ID_START)+1;
    for (i = 0; i < size; i++) {
        if (!DEFAULT_KEYS_EMULATOR[i])
            continue;

        m_pMappedSignals[EMULATOR_ID_START+i] = new SDLInputSignal(0,
                DEFAULT_KEYS_EMULATOR[i],
                SDL_GetKeyName(DEFAULT_KEYS_EMULATOR[i]));
    }

    //initialize the menu control input signals
    size = (MENU_ID_END - MENU_ID_START)+1;
    for (i = 0; i < size; i++) {
        if (!DEFAULT_KEYS_MENU[i])
            continue;

        m_pMappedSignals[MENU_ID_START+i] = new SDLInputSignal(0,
                DEFAULT_KEYS_MENU[i],
                SDL_GetKeyName(DEFAULT_KEYS_MENU[i]));
    }

}

SDLInputDevice::~SDLInputDevice()
{
    // Cleanup before shutdown
    release();

    delete[] m_pMappedSignals;
}


const CHAR* SDLInputDevice::getErrorDescription(ERRCODE errorCode) {
    return inputErrorMessages[errorCode-1];
}

// init: Activate input
ERRCODE SDLInputDevice::init()
{
#ifdef _TRACE_INPUT
  fprintf(stderr, "SDLInputDevice::init()\n");
#endif

    if(SDL_Init(SDL_INIT_JOYSTICK))
        return ERR_INIT_INPUT;

    m_iNumJoysticks = SDL_NumJoysticks(); 

    INT32 x;
    for(x = 0; x < m_iNumJoysticks; x++)
        joy[x] = SDL_JoystickOpen(x);

    return OK;
}

// release: Cleanup
void SDLInputDevice::release()
{
#ifdef _TRACE_INPUT
  fprintf(stderr, "SDLInputDevice::release()\n");
#endif

    for(int x = 0; x < m_iNumJoysticks; x++) {
        if(SDL_JoystickOpened(x))
            SDL_JoystickClose(joy[x]);
    }
    m_iNumJoysticks = 0;

    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
}


// poll: Get input events
void SDLInputDevice::poll()
{
#ifdef _TRACE_INPUT
  fprintf(stderr, "SDLInputDevice::poll()... ");
#endif

    //SDL_PumpEvents();
//    SDL_Event event;
//    while (SDL_PollEvent(&event));			// Going to lose non-keyboard events?

    m_KeyStates = SDL_GetKeyState(NULL);
    SDL_PumpEvents();

	if(m_KeyStates[SDLK_RETURN]) {
		if(SDL_GetModState() & KMOD_ALT) {
			m_requestFlags	|= MSG_TOGGLE;
            m_KeyStates[SDLK_RETURN] = 0;
		}
	}
}
/*
bool SDLInputDevice::altPressed()
{
    if(SDL_GetModState() & KMOD_ALT)
        return true;

    return false;
}
*/


float SDLInputDevice::getJoystickSignalValue(INT32 joystickNum, INT32 inputCode)
{
    if (joystickNum >= SDL_NumJoysticks())
        return 0.0f;

    if (inputCode < 6) {
        long inputValue = 0;
        long maxValue = 32768;
        switch (inputCode) {
            case 0:
                inputValue = -(SDL_JoystickGetAxis(joy[joystickNum], 0));
                break;
            case 1:
                inputValue = -(SDL_JoystickGetAxis(joy[joystickNum], 1));
                break;
            case 2:
                inputValue = -(SDL_JoystickGetAxis(joy[joystickNum], 2));
                break;
            case 3:
                inputValue = SDL_JoystickGetAxis(joy[joystickNum], 0);
                break;
            case 4:
                inputValue = SDL_JoystickGetAxis(joy[joystickNum], 1);
                break;
            case 5:
                inputValue = SDL_JoystickGetAxis(joy[joystickNum], 2);
                break;

/*                inputValue = -(m_JoystickStates[joystickNum].lX -
                        m_JoystickAxes[joystickNum].lCenterX);
                maxValue = m_JoystickAxes[joystickNum].lMaxDeltaX;
                break;
            case 1:
                inputValue = -(m_JoystickStates[joystickNum].lY -
                        m_JoystickAxes[joystickNum].lCenterY);
                maxValue = m_JoystickAxes[joystickNum].lMaxDeltaY;
                break;
            case 2:
                inputValue = -(m_JoystickStates[joystickNum].lZ -
                        m_JoystickAxes[joystickNum].lCenterZ);
                maxValue = m_JoystickAxes[joystickNum].lMaxDeltaZ;
                break;
            case 3:
                inputValue = m_JoystickStates[joystickNum].lX -
                        m_JoystickAxes[joystickNum].lCenterX;
                maxValue = m_JoystickAxes[joystickNum].lMaxDeltaX;
                break;
            case 4:
                inputValue = m_JoystickStates[joystickNum].lY -
                        m_JoystickAxes[joystickNum].lCenterY;
                maxValue = m_JoystickAxes[joystickNum].lMaxDeltaY;
                break;
            case 5:
                inputValue = m_JoystickStates[joystickNum].lZ -
                        m_JoystickAxes[joystickNum].lCenterZ;
                maxValue = m_JoystickAxes[joystickNum].lMaxDeltaZ;
                break; */
        }

        if (inputValue <= 0)
            return 0.0f;

        return (float)((float)inputValue)/((float)maxValue);
    }
    else if (inputCode < 262) {
        return SDL_JoystickGetButton(joy[joystickNum], inputCode-6);
//        m_JoystickStates[joystickNum].
//                rgbButtons[inputCode-6] & 0x80 ? 1.0f : 0.0f);
    }
/*    else if (inputCode < 278) {
        INT32 directionToCheck = (inputCode-262) & 0x03;
        DWORD povValue = m_
//            m_JoystickStates[joystickNum].
//                rgdwPOV[((inputCode-262) & 0x0C) >> 2];
        if (LOWORD(povValue) == 0xFFFF)
            return 0.0f;

        INT32 direction = ((povValue + 4500) / 9000) & 0x03;
        return (direction == directionToCheck ? 1.0f : 0.0f);
    } */


    return 0.0f;
}

float SDLInputDevice::getControlValue(INT32 inputID)
{
    SDLInputSignal* inputSignal = m_pMappedSignals[inputID];
    if (inputSignal) {
        if (inputSignal->deviceID == 0) {
            return (m_KeyStates[inputSignal->controlID] ? SIGNAL_ON
                    : SIGNAL_OFF);
        }
        else
            return getJoystickSignalValue((int)inputSignal->deviceID-1,
                    (int)inputSignal->controlID);
    }

    return 0.0f;
}
