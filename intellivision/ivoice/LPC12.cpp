
#include "LPC12.h"
#include "MicroSequencer.h"

const INT32 LPC12::qtbl[256] = {
    0,      511,    510,    509,    508,    507,    506,    505,
    504,    503,    502,    501,    500,    499,    498,    497,
    496,    495,    494,    493,    492,    491,    490,    489,
    488,    487,    486,    485,    484,    483,    482,    481,
    479,    477,    475,    473,    471,    469,    467,    465,
    463,    461,    459,    457,    455,    453,    451,    449,
    447,    445,    443,    441,    439,    437,    435,    433,
    431,    429,    427,    425,    421,    417,    413,    409,
    405,    401,    397,    393,    389,    385,    381,    377,
    373,    369,    365,    361,    357,    353,    349,    345,
    341,    337,    333,    329,    325,    321,    317,    313,
    309,    305,    301,    297,    289,    281,    273,    265,
    257,    249,    241,    233,    225,    217,    209,    201,
    193,    185,    177,    169,    161,    153,    145,    137,
    129,    121,    113,    105,    97,     89,     81,     73,
    65,     57,     49,     41,     33,     25,     12,     9,
    0,     -9,     -12,    -25,    -33,    -41,    -49,    -57,
   -65,    -73,    -81,    -89,    -97,    -105,   -113,   -121,
   -129,   -137,   -145,   -153,   -161,   -169,   -177,   -185,
   -193,   -201,   -209,   -217,   -225,   -233,   -241,   -249,
   -257,   -265,   -273,   -281,   -289,   -297,   -301,   -305,
   -309,   -313,   -317,   -321,   -325,   -329,   -333,   -337,
   -341,   -345,   -349,   -353,   -357,   -361,   -365,   -369,
   -373,   -377,   -381,   -385,   -389,   -393,   -397,   -401,
   -405,   -409,   -413,   -417,   -421,   -425,   -427,   -429,
   -431,   -433,   -435,   -437,   -439,   -441,   -443,   -445,
   -447,   -449,   -451,   -453,   -455,   -457,   -459,   -461,
   -463,   -465,   -467,   -469,   -471,   -473,   -475,   -477,
   -479,   -481,   -482,   -483,   -484,   -485,   -486,   -487,
   -488,   -489,   -490,   -491,   -492,   -493,   -494,   -495,
   -496,   -497,   -498,   -499,   -500,   -501,   -502,   -503,
   -504,   -505,   -506,   -507,   -508,   -509,   -510,   -511
};

LPC12::LPC12() {}

LPC12::LPC12(AudioOutputLine* aol) {
    init(aol);
}

void LPC12::init(AudioOutputLine* aol) {
    this->aol = aol;
}

INT32 LPC12::getClockSpeed() {
    return 10000;
}

void LPC12::reset() {
    amplitude = 0;
    period = 0;
    periodCounter = 0x1;
    isIdle = TRUE;
    random = 1;
    for (INT32 i = 0; i < 6; i++) {
        y[i][0] = 0;
        y[i][1] = 0;
    }
}

INT32 LPC12::tick()
{
    INT32 sample = 0;

    if (period == 0) {
        if (periodCounter == 0) {
            periodCounter = 64;
            repeat--;
            for (INT32 j = 0; j < 6; j++)
                y[j][0] = y[j][1] = 0;
        }
        else
            periodCounter--;

        sample = ((amplitude & 0x1F) << ((amplitude & 0xE0) >> 5));
        BOOL noise = ((random & 1) != 0);
        random = (random >> 1) ^ (noise ? 0x14000 : 0);
        if (!noise)
            sample = -sample;
    }
    else {
        if (periodCounter == 0) {
            periodCounter = period;
            repeat--;
            sample = ((amplitude & 0x1F) << ((amplitude & 0xE0) >> 5));
            for (INT32 j = 0; j < 6; j++)
                y[j][0] = y[j][1] = 0;

        }
        else
            periodCounter--;
    }

    period = ((period | 0x10000) + periodInterpolation) & 0xFFFF;
    amplitude = ((amplitude | 0x10000) + amplitudeInterpolation) & 0xFFFF;

    if (repeat == 0) {
        isIdle = true;
        microSequencer->isIdle = false;
    }

    for (INT32 i = 0; i < 6; i++) {
        sample += ((qtbl[0x80+b[i]]*y[i][1]) >> 9);
        sample += ((qtbl[0x80+f[i]]*y[i][0]) >> 8);
        y[i][1] = y[i][0];
        y[i][0] = sample;
    }

    if (sample > 2047) sample = 2047;
    if (sample < -2048) sample = -2048;

    aol->playSample(sample << 4);

    return 1;
}

/*
void save(SaveOutputStream sos) throws IOException {
    sos.writeInt(repeat);
    sos.writeInt(period);
    sos.writeInt(periodCounter);
    sos.writeInt(amplitude);
    for (INT32 i = 0; i < b.length; i++)
        sos.writeInt(b[i]);
    for (INT32 i = 0; i < f.length; i++)
        sos.writeInt(f[i]);
    for (INT32 i = 0; i < y.length; i++)
        for (INT32 j = 0; j < y[0].length; j++)
            sos.writeInt(y[i][j]);
    sos.writeInt(periodInterpolation);
    sos.writeInt(amplitudeInterpolation);
}

void load(LoadInputStream lis) throws IOException {
    repeat = lis.readInt(0, 0x3F);
    period = lis.readInt(0, 0xFFFF);
    periodCounter = lis.readInt(0, 0xFFFF);
    amplitude = lis.readInt(0, 0xFFFF);
    for (INT32 i = 0; i < b.length; i++)
        b[i] = (INT8)lis.readInt(-128, 127);
    for (INT32 i = 0; i < f.length; i++)
        f[i] = (INT8)lis.readInt(-128, 127);
    for (INT32 i = 0; i < y.length; i++)
        for (INT32 j = 0; j < y[0].length; j++)
            y[i][j] = (INT8)lis.readInt(-128, 127);
    periodInterpolation = (INT8)lis.readInt(-128, 127);
    amplitudeInterpolation = (INT8)lis.readInt(-128, 127);
}
*/
