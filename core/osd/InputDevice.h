
#ifndef INPUTDEVICE_H
#define INPUTDEVICE_H

#include "Device.h"


#define ERR_INIT_INPUT	1

// System messages
#define MSG_QUIT		1
#define MSG_TOGGLE		2	// Toggle fullscreen / windowed


/**
 * An InputDevice ties together all inputs attached to a computer, presenting
 * them through a single interface in a way that is meaningful to an emulator.
 */
class InputDevice : public Device
{

    public:
        /**
         * Poll any data inputs attached to this device that require polling.
         * Input devices that do not require polling may simply provide a
         * "do-nothing" implementation of this function.
         *
         * Note: This function will be invoked by the emulator at the start of
         * every emulated vertical blank.
         */
        virtual void poll() = 0;

        /**
         * Obtains a value representing the state of the input mapped to the
         * specified control.
         *
         * @param  controlID  the ID of the desired control
         * @return SIGNAL_ON if the input mapped to the specified control
         *         is pressed; SIGNAL_OFF if the input is not pressed; may
         *         return a value between 0.0 and 1.0 (inclusive) if the input
         *         which is mapped to the specified control is an analog input
         */
        virtual float getControlValue(INT32 controlID) = 0;

        /**
         * Determines if the input controls for this input device can be
         * remapped.  If it can be remapped, then the other 3 remapping
         * functions should be implemented to fully support remapping.
         *
         * @return TRUE if the input device can be remapped; FALSE otherwise
         */
        virtual BOOL isRemappable() = 0;

        /**
         * Polls the input device to determine if an input is in use (such as
         * a key pressed on the keyboard or a button pressed on a gamepad) and
         * if so, maps the selected input to the designated control.
         *
         * @param  controlID  the ID of the control to be remapped
         * @return TRUE if the control was remapped because an input was
         *         sensed; FALSE otherwise
         */
		virtual BOOL remapControl(INT32 controlID) = 0;

        /**
         * Obtains a string representing the input that is mapped to the
         * specified control.  The string should be suitable for storage
         * in a human-readable file, such as a config or '.ini' file.
         *
         * @param  controlID  the ID of the desired control
         * @return the string representing the input that is mapped to
         *         the specified control
         */
        virtual const CHAR* getControlMapping(INT32 controlID) = 0;

        /**
         * Maps a control to an input based on a string that was previously
         * returned by the getControlMapping() function.
         *
         * @param  controlID  the ID of the desired control
         * @return the string representing the input to be mapped to the
         *         specified control
         */
        virtual void setControlMapping(INT32 controlID, const CHAR* map) = 0;


		virtual bool toggleRequested()
		{
			bool rc = !!(m_requestFlags & MSG_TOGGLE);
			if(rc)
				m_requestFlags &= ~MSG_TOGGLE;
			return rc;
		}

//        virtual bool altPressed() = 0;  // Bleh... workaround, temp.
		
	protected:
		UINT32 m_requestFlags;	// Multi-key all emu requests
};


#define SIGNAL_OFF 0.0f
#define SIGNAL_ON  1.0f

typedef enum {
    PLAYER_ONE_ID_START,
    PLAYER_ONE_NORTH          = PLAYER_ONE_ID_START,
    PLAYER_ONE_NORTHEAST      = PLAYER_ONE_ID_START + 1,
    PLAYER_ONE_EAST           = PLAYER_ONE_ID_START + 2,
    PLAYER_ONE_SOUTHEAST      = PLAYER_ONE_ID_START + 3,
    PLAYER_ONE_SOUTH          = PLAYER_ONE_ID_START + 4,
    PLAYER_ONE_SOUTHWEST      = PLAYER_ONE_ID_START + 5,
    PLAYER_ONE_WEST           = PLAYER_ONE_ID_START + 6,
    PLAYER_ONE_NORTHWEST      = PLAYER_ONE_ID_START + 7,
    PLAYER_ONE_BUTTON_ONE     = PLAYER_ONE_ID_START + 8,
    PLAYER_ONE_BUTTON_TWO     = PLAYER_ONE_ID_START + 9,
    PLAYER_ONE_BUTTON_THREE   = PLAYER_ONE_ID_START + 10,
    PLAYER_ONE_BUTTON_FOUR    = PLAYER_ONE_ID_START + 11,
    PLAYER_ONE_BUTTON_FIVE    = PLAYER_ONE_ID_START + 12,
    PLAYER_ONE_BUTTON_SIX     = PLAYER_ONE_ID_START + 13,
    PLAYER_ONE_KEYPAD_ONE     = PLAYER_ONE_ID_START + 14,
    PLAYER_ONE_KEYPAD_TWO     = PLAYER_ONE_ID_START + 15,
    PLAYER_ONE_KEYPAD_THREE   = PLAYER_ONE_ID_START + 16,
    PLAYER_ONE_KEYPAD_FOUR    = PLAYER_ONE_ID_START + 17,
    PLAYER_ONE_KEYPAD_FIVE    = PLAYER_ONE_ID_START + 18,
    PLAYER_ONE_KEYPAD_SIX     = PLAYER_ONE_ID_START + 19,
    PLAYER_ONE_KEYPAD_SEVEN   = PLAYER_ONE_ID_START + 20,
    PLAYER_ONE_KEYPAD_EIGHT   = PLAYER_ONE_ID_START + 21,
    PLAYER_ONE_KEYPAD_NINE    = PLAYER_ONE_ID_START + 22,
    PLAYER_ONE_KEYPAD_ZERO    = PLAYER_ONE_ID_START + 23,
    PLAYER_ONE_KEYPAD_DELETE  = PLAYER_ONE_ID_START + 24,
    PLAYER_ONE_KEYPAD_ENTER   = PLAYER_ONE_ID_START + 25,
    PLAYER_ONE_KEYPAD_ASTERIX = PLAYER_ONE_ID_START + 26,
    PLAYER_ONE_KEYPAD_POUND   = PLAYER_ONE_ID_START + 27,
    PLAYER_ONE_ID_END         = PLAYER_ONE_KEYPAD_POUND,

    PLAYER_TWO_ID_START,
    PLAYER_TWO_NORTH          = PLAYER_TWO_ID_START + 0,
    PLAYER_TWO_NORTHEAST      = PLAYER_TWO_ID_START + 1,
    PLAYER_TWO_EAST           = PLAYER_TWO_ID_START + 2,
    PLAYER_TWO_SOUTHEAST      = PLAYER_TWO_ID_START + 3,
    PLAYER_TWO_SOUTH          = PLAYER_TWO_ID_START + 4,
    PLAYER_TWO_SOUTHWEST      = PLAYER_TWO_ID_START + 5,
    PLAYER_TWO_WEST           = PLAYER_TWO_ID_START + 6,
    PLAYER_TWO_NORTHWEST      = PLAYER_TWO_ID_START + 7,
    PLAYER_TWO_BUTTON_ONE     = PLAYER_TWO_ID_START + 8,
    PLAYER_TWO_BUTTON_TWO     = PLAYER_TWO_ID_START + 9,
    PLAYER_TWO_BUTTON_THREE   = PLAYER_TWO_ID_START + 10,
    PLAYER_TWO_BUTTON_FOUR    = PLAYER_TWO_ID_START + 11,
    PLAYER_TWO_BUTTON_FIVE    = PLAYER_TWO_ID_START + 12,
    PLAYER_TWO_BUTTON_SIX     = PLAYER_TWO_ID_START + 13,
    PLAYER_TWO_KEYPAD_ONE     = PLAYER_TWO_ID_START + 14,
    PLAYER_TWO_KEYPAD_TWO     = PLAYER_TWO_ID_START + 15,
    PLAYER_TWO_KEYPAD_THREE   = PLAYER_TWO_ID_START + 16,
    PLAYER_TWO_KEYPAD_FOUR    = PLAYER_TWO_ID_START + 17,
    PLAYER_TWO_KEYPAD_FIVE    = PLAYER_TWO_ID_START + 18,
    PLAYER_TWO_KEYPAD_SIX     = PLAYER_TWO_ID_START + 19,
    PLAYER_TWO_KEYPAD_SEVEN   = PLAYER_TWO_ID_START + 20,
    PLAYER_TWO_KEYPAD_EIGHT   = PLAYER_TWO_ID_START + 21,
    PLAYER_TWO_KEYPAD_NINE    = PLAYER_TWO_ID_START + 22,
    PLAYER_TWO_KEYPAD_ZERO    = PLAYER_TWO_ID_START + 23,
    PLAYER_TWO_KEYPAD_DELETE  = PLAYER_TWO_ID_START + 24,
    PLAYER_TWO_KEYPAD_ENTER   = PLAYER_TWO_ID_START + 25,
    PLAYER_TWO_KEYPAD_ASTERIX = PLAYER_TWO_ID_START + 26,
    PLAYER_TWO_KEYPAD_POUND   = PLAYER_TWO_ID_START + 27,
    PLAYER_TWO_ID_END         = PLAYER_TWO_KEYPAD_POUND,

    KEYBOARD_ID_START,
    KEYBOARD_A = KEYBOARD_ID_START,
    KEYBOARD_B = KEYBOARD_ID_START + 1,
    KEYBOARD_C = KEYBOARD_ID_START + 2,
    KEYBOARD_D = KEYBOARD_ID_START + 3,
    KEYBOARD_E = KEYBOARD_ID_START + 4,
    KEYBOARD_F = KEYBOARD_ID_START + 5,
    KEYBOARD_G = KEYBOARD_ID_START + 6,
    KEYBOARD_H = KEYBOARD_ID_START + 7,
    KEYBOARD_I = KEYBOARD_ID_START + 8,
    KEYBOARD_J = KEYBOARD_ID_START + 9,
    KEYBOARD_K = KEYBOARD_ID_START + 10,
    KEYBOARD_L = KEYBOARD_ID_START + 11,
    KEYBOARD_M = KEYBOARD_ID_START + 12,
    KEYBOARD_N = KEYBOARD_ID_START + 13,
    KEYBOARD_O = KEYBOARD_ID_START + 14,
    KEYBOARD_P = KEYBOARD_ID_START + 15,
    KEYBOARD_Q = KEYBOARD_ID_START + 16,
    KEYBOARD_R = KEYBOARD_ID_START + 17,
    KEYBOARD_S = KEYBOARD_ID_START + 18,
    KEYBOARD_T = KEYBOARD_ID_START + 19,
    KEYBOARD_U = KEYBOARD_ID_START + 20,
    KEYBOARD_V = KEYBOARD_ID_START + 21,
    KEYBOARD_W = KEYBOARD_ID_START + 22,
    KEYBOARD_X = KEYBOARD_ID_START + 23,
    KEYBOARD_Y = KEYBOARD_ID_START + 24,
    KEYBOARD_Z = KEYBOARD_ID_START + 25,

    KEYBOARD_0 = KEYBOARD_ID_START + 26,
    KEYBOARD_1 = KEYBOARD_ID_START + 27,
    KEYBOARD_2 = KEYBOARD_ID_START + 28,
    KEYBOARD_3 = KEYBOARD_ID_START + 29,
    KEYBOARD_4 = KEYBOARD_ID_START + 30,
    KEYBOARD_5 = KEYBOARD_ID_START + 31,
    KEYBOARD_6 = KEYBOARD_ID_START + 32,
    KEYBOARD_7 = KEYBOARD_ID_START + 33,
    KEYBOARD_8 = KEYBOARD_ID_START + 34,
    KEYBOARD_9 = KEYBOARD_ID_START + 35,

    KEYBOARD_UP        = KEYBOARD_ID_START + 36,
    KEYBOARD_DOWN      = KEYBOARD_ID_START + 37,
    KEYBOARD_LEFT      = KEYBOARD_ID_START + 38,
    KEYBOARD_RIGHT     = KEYBOARD_ID_START + 39,

    KEYBOARD_COMMA     = KEYBOARD_ID_START + 40,
    KEYBOARD_PERIOD    = KEYBOARD_ID_START + 41,
    KEYBOARD_SPACE     = KEYBOARD_ID_START + 42,
    KEYBOARD_SEMICOLON = KEYBOARD_ID_START + 43,
    KEYBOARD_COLON     = KEYBOARD_ID_START + 44,
    KEYBOARD_ESCAPE    = KEYBOARD_ID_START + 45,
    KEYBOARD_ENTER     = KEYBOARD_ID_START + 46,
    KEYBOARD_CONTROL   = KEYBOARD_ID_START + 47,
    KEYBOARD_SHIFT     = KEYBOARD_ID_START + 48,
    KEYBOARD_ALT       = KEYBOARD_ID_START + 49,
    KEYBOARD_ID_END    = KEYBOARD_ALT,

    MENU_ID_START,
    MENU_DISPLAY  = MENU_ID_START,
    MENU_UP       = MENU_ID_START + 1,
    MENU_DOWN     = MENU_ID_START + 2,
    MENU_PGUP     = MENU_ID_START + 3,
    MENU_PGDOWN   = MENU_ID_START + 4,
    MENU_SELECT   = MENU_ID_START + 5,
    MENU_ID_END   = MENU_SELECT,

    EMULATOR_ID_START,
    EMULATOR_PAUSE      = EMULATOR_ID_START,
    EMULATOR_SAVE       = EMULATOR_ID_START + 1,
    EMULATOR_LOAD       = EMULATOR_ID_START + 2,
    EMULATOR_RESET      = EMULATOR_ID_START + 3,
    EMULATOR_SCREENSHOT = EMULATOR_ID_START + 4,
    EMULATOR_ID_END     = EMULATOR_SCREENSHOT,

    TOTAL_CONTROL_COUNT

} ControlID;

#endif

