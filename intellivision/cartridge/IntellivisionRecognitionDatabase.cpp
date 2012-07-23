#pragma warning(disable:4786)	// Suppress STL debug info > 255 chars messages

#include "IntellivisionRecognitionDatabase.h"



const int configurationCount = 170;
RECOGNITIONMAP IntellivisionRecognitionDatabase::recognitionMap;

IntellivisionRecognitionDatabase::IntellivisionRecognitionDatabase() {
    for (int i = 0; i < configurationCount; i++) {
        recognitionMap.insert(RECOGNITIONMAP::value_type(
                configurations[i].getCrc(), &configurations[i]));
    }
}

CartridgeConfiguration* IntellivisionRecognitionDatabase::getConfiguration(UINT32 crc) {
    RECOGNITIONMAP::iterator it;
    it = recognitionMap.find(crc);
    if (it == recognitionMap.end())
        return NULL;
    else
        return (*it).second;
}

IntellivisionCartridgeConfiguration IntellivisionRecognitionDatabase::configurations[configurationCount] = {
    IntellivisionCartridgeConfiguration(
            0xD7C78754, "4-TRIS (GPL)", "Joseph Zbiciak",
            "2000", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0xA60E25FC, "ABPA Backgammon", "Mattel",
            "1978", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0xF8B1F2B7, "Advanced Dungeons and Dragons", "Mattel",
            "1982", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x16C3B62F, "Advanced Dungeons and Dragons - Treasure of Tarmin", "Mattel",
            "1982", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x11C3BCFA, "Adventure (AD&D - Cloudy Mountain)", "Mattel",
            "1982", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x2C668249, "Air Strike", "Mattel",
            "1982", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0xB45633CF, "All-Stars Major League Baseball", "Mattel",
            "1980", 24576, FALSE, FALSE,
            0x5000, 0x6FFF, 0xD000, 0xDFFF),
    IntellivisionCartridgeConfiguration(
            0x6F91FBC1, "Armor Battle", "Mattel",
            "1978", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0xBE8BBA, "Astrosmash - Meteor", "Mattel",
            "1981", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0xFAB2992C, "Astrosmash", "Mattel",
            "1981", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0x13FF363C, "Atlantis", "Imagic",
            "1981", 16384, FALSE, FALSE,
            0x4800, 0x67FF),
    IntellivisionCartridgeConfiguration(
            0xB35C1101, "Auto Racing", "Mattel",
            "1979", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0x8AD19AB3, "B-17 Bomber", "Mattel",
            "1981", 24576, FALSE, TRUE,
            0x5000, 0x6FFF, 0xD000, 0xDFFF),
    IntellivisionCartridgeConfiguration(
            0xDAB36628, "Baseball", "Mattel",
            "1978", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0xEAF650CC, "BeamRider", "Activision",
            "1983", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0xC047D487, "Beauty and the Beast", "Imagic",
            "1982", 16384, FALSE, FALSE,
            0x4800, 0x67FF),
    IntellivisionCartridgeConfiguration(
            0xB03F739B, "Blockade Runner", "Interphase",
            "1983", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x515E1D7E, "Body Slam Super Pro Wrestling", "Mattel",
            "1988", 32768, FALSE, FALSE,
            0x5000, 0x6FFF, 0x9000, 0xAFFF),
    IntellivisionCartridgeConfiguration(
            0x32697B72, "Bomb Squad", "Mattel",
            "1982", 24576, FALSE, TRUE,
            0x5000, 0x6FFF, 0xD000, 0xDFFF),
    IntellivisionCartridgeConfiguration(
            0x18E08520, "Bouncing Pixels (GPL)", "JRMZ Electronics",
            "1999", 1028, FALSE, FALSE,
            0x5000, 0x5201),
    IntellivisionCartridgeConfiguration(
            0xAB87C16F, "Boxing", "Mattel",
            "1980", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0x9F85015B, "Brickout!", "Mattel",
            "1981", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0x999CCEED, "Bump 'N' Jump", "Mattel",
            "1982-83", 32768, FALSE, FALSE,
            0x5000, 0x6FFF, 0xD000, 0xDFFF, 0xF000, 0xFFFF),
    IntellivisionCartridgeConfiguration(
            0x43806375, "BurgerTime!", "Mattel",
            "1982", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0xFA492BBD, "Buzz Bombers", "Mattel",
            "1982", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x43870908, "Carnival", "Coleco",
            "1982", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0xD5363B8C, "Centipede", "Atarisoft",
            "1983", 16384, FALSE, FALSE,
            0x6000, 0x7FFF),
    IntellivisionCartridgeConfiguration(
            0x4CC46A04, "Championship Tennis", "Nice Ideas",
            "1985", 32768, FALSE, FALSE,
            0x5000, 0x6FFF, 0xD000, 0xEFFF),
    IntellivisionCartridgeConfiguration(
            0x36E1D858, "Checkers", "Mattel",
            "1979", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0xBF464C6, "Chip Shot Super Pro Golf", "Mattel",
            "1987", 32768, FALSE, FALSE,
            0x5000, 0x6FFF, 0x9000, 0xAFFF),
    IntellivisionCartridgeConfiguration(
            0x3289C8BA, "Commando", "INTV",
            "1987", 32768, FALSE, FALSE,
            0x5000, 0x6FFF, 0x9000, 0xAFFF),
    IntellivisionCartridgeConfiguration(
            0x4B23A757, "Congo Bongo", "Sega",
            "1983", 24576, FALSE, FALSE,
            0x5000, 0x7FFF),
    IntellivisionCartridgeConfiguration(
            0xE1EE408F, "Crazy Clones", "Mattel",
            "1981", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0xCDC14ED8, "Deadly Dogs!", "Unknown",
            "1987", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0x6802B191, "Deep Pockets - Super Pro Pool and Billiards", "Realtime",
            "1990", 32768, FALSE, FALSE,
            0x5000, 0x6FFF, 0x9000, 0xAFFF),
    IntellivisionCartridgeConfiguration(
            0xD8F99AA2, "Defender", "Atarisoft",
            "1983", 24576, FALSE, FALSE,
            0x5000, 0x7FFF),
    IntellivisionCartridgeConfiguration(
            0x5E6A8CD8, "Demon Attack", "Imagic",
            "1982", 16384, FALSE, FALSE,
            0x4800, 0x67FF),
    IntellivisionCartridgeConfiguration(
            0x159AF7F7, "Dig Dug", "INTV",
            "1987", 32768, FALSE, FALSE,
            0x5000, 0x7FFF, 0x9000, 0x9FFF),
    IntellivisionCartridgeConfiguration(
            0x13EE56F1, "Diner", "INTV",
            "1987", 32768, FALSE, FALSE,
            0x5000, 0x6FFF, 0x9000, 0xAFFF),
    IntellivisionCartridgeConfiguration(
            0xC30F61C0, "Donkey Kong", "Coleco",
            "1982", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0x6DF61A9F, "Donkey Kong Jr", "Coleco",
            "1982", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x84BEDCC1, "Dracula", "Imagic",
            "1982", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0xAF8718A1, "Dragonfire", "Imagic",
            "1982", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0x3B99B889, "Dreadnaught Factor, The", "Activision",
            "1983", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x20ACE89D, "Easter Eggs", "Mattel",
            "1981", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0x54A3FC11, "Electric Company - Math Fun", "CTW",
            "1978", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0xC9EAACAB, "Electric Company - Word Fun", "CTW",
            "1980", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0x4221EDE7, "Fathom", "Imagic",
            "1983", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x37222762, "Frog Bog", "Mattel",
            "1982", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0xD27495E9, "Frogger", "Parker Bros",
            "1983", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0xDBCA82C5, "Go For The Gold", "Mattel",
            "1981", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x291AC826, "Grid Shock", "Mattel",
            "1982", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0xE573863A, "Groovy! (GPL)", "JRMZ Electronics",
            "1999", 15504, FALSE, FALSE,
            0x5000, 0x6E47),
    IntellivisionCartridgeConfiguration(
            0x4B8C5932, "Happy Trails", "Activision",
            "1983", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0xB6A3D4DE, "Hard Hat", "Mattel",
            "1979", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0xB5C7F25D, "Horse Racing", "Mattel",
            "1980", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0xFF83FF80, "Hover Force", "Mattel",
            "1986", 49152, FALSE, FALSE,
            0x5000, 0x6FFF, 0x9000, 0xBFFF, 0xD000, 0xDFFF),
    IntellivisionCartridgeConfiguration(
            0xA3147630, "Hypnotic Lights", "Mattel",
            "1981", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0x4F3E3F69, "Ice Trek", "Imagic",
            "1983", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x2919024, "Intellivision - Intelligent Television Demo", "Mattel",
            "1978", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0xC83EEA4C, "Intellivision Test Cartridge and Baseball", "Mattel",
            "1978", 16384, FALSE, FALSE,
            0x5000, 0x5FFF, 0x7000, 0x7FFF),
    IntellivisionCartridgeConfiguration(
            0xEE5F1BE2, "Jetsons, The - Ways With Words", "Mattel",
            "1983", 24576, TRUE, FALSE,
            0x5000, 0x6FFF, 0xD000, 0xDFFF),
    IntellivisionCartridgeConfiguration(
            0x4422868E, "King of the Mountain", "Mattel",
            "1982", 32768, FALSE, FALSE,
            0x5000, 0x6FFF, 0xD000, 0xEFFF),
    IntellivisionCartridgeConfiguration(
            0x8C9819A2, "Kool-Aid Man", "Mattel",
            "1983", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0xA6840736, "Lady Bug", "Coleco",
            "1983", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x3825C25B, "Land Battle", "Mattel",
            "1982", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x604611C0, "Las Vegas Blackjack and Poker", "Mattel",
            "1979", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0x48D74D3C, "Las Vegas Roulette", "Mattel",
            "1979", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0x2C5FD5FA, "Learning Fun I - Math Master Factor Fun", "INTV",
            "1987", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x632F6ADF, "Learning Fun II - Word Wizard Memory Fun", "INTV",
            "1987", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0xE00D1399, "Lock 'N' Chase", "Mattel",
            "1982", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x6B6E80EE, "Loco-Motion", "Mattel",
            "1982", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x573B9B6D, "Masters of the Universe - The Power of He-Man!", "Mattel",
            "1983", 32768, FALSE, FALSE,
            0x5000, 0x6FFF, 0xD000, 0xDFFF, 0xF000, 0xFFFF),
    IntellivisionCartridgeConfiguration(
            0x7D0F8162, "Maze Demo #1 (GPL)", "JRMZ Electronics",
            "2000", 732, FALSE, FALSE,
            0x5000, 0x516D),
    IntellivisionCartridgeConfiguration(
            0x2138DAD4, "Maze Demo #2 (GPL)", "JRMZ Electronics",
            "2000", 746, FALSE, FALSE,
            0x5000, 0x5174),
    IntellivisionCartridgeConfiguration(
            0xFF68AA22, "Melody Blaster", "Mattel",
            "1983", 24576, TRUE, FALSE,
            0x5000, 0x6FFF, 0xD000, 0xDFFF),
    IntellivisionCartridgeConfiguration(
            0xE00E23E7, "Mickey's Hello World", "Mickey",
            "2000", 222, FALSE, FALSE,
            0x5000, 0x506E),
    IntellivisionCartridgeConfiguration(
            0xE806AD91, "Microsurgeon", "Imagic",
            "1982", 16384, FALSE, FALSE,
            0x4800, 0x67FF),
    IntellivisionCartridgeConfiguration(
            0x9D57498F, "Mind Strike!", "Mattel",
            "1982", 24576, TRUE, FALSE,
            0x5000, 0x6FFF, 0xD000, 0xDFFF),
    IntellivisionCartridgeConfiguration(
            0xBD731E3C, "Minotaur", "Mattel",
            "1981", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x11FB9974, "Mission X", "Mattel",
            "1982", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x5F6E1AF6, "Motocross", "Mattel",
            "1982", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x6B5EA9C4, "Mountain Madness Super Pro Skiing", "INTV",
            "1987", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x598662F2, "Mouse Trap", "Coleco",
            "1982", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0xB50A367, "Mr. Basic Meets Bits 'N Bytes", "Mattel",
            "1983", 24576, TRUE, FALSE,
            0x5000, 0x6FFF, 0xD000, 0xDFFF),
    IntellivisionCartridgeConfiguration(
            0xDBAB54CA, "NASL Soccer", "Mattel",
            "1979", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0x81E7FB8C, "NBA Basketball", "Mattel",
            "1978", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0x4B91CF16, "NFL Football", "Mattel",
            "1978", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0x76564A13, "NHL Hockey", "Mattel",
            "1979", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0x7334CD44, "Night Stalker", "Mattel",
            "1982", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0x5EE2CC2A, "Nova Blast", "Imagic",
            "1983", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0xE5D1A8D2, "Number Jumble", "Mattel",
            "1983", 32768, FALSE, FALSE,
            0x5000, 0x6FFF, 0xD000, 0xDFFF, 0xF000, 0xFFFF),
    IntellivisionCartridgeConfiguration(
            0xA21C31C3, "Pac-Man", "Atarisoft",
            "1983", 24576, FALSE, FALSE,
            0x5000, 0x7FFF),
    IntellivisionCartridgeConfiguration(
            0x6E4E8EB4, "Pac-Man", "INTV",
            "1983", 24576, FALSE, FALSE,
            0x5000, 0x7FFF),
    IntellivisionCartridgeConfiguration(
            0x169E3584, "PBA Bowling", "Mattel",
            "1980", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0xFF87FAEC, "PGA Golf", "Mattel",
            "1979", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0xD7C5849C, "Pinball", "Mattel",
            "1981", 24576, FALSE, FALSE,
            0x5000, 0x6FFF, 0xD000, 0xDFFF),
    IntellivisionCartridgeConfiguration(
            0x9C75EFCC, "Pitfall!", "Activision",
            "1982", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0xBB939881, "Pole Position", "INTV",
            "1986", 32768, FALSE, FALSE,
            0x5000, 0x6FFF, 0x9000, 0xAFFF),
    IntellivisionCartridgeConfiguration(
            0xA982E8D5, "Pong", "Unknown",
            "1999", 4096, FALSE, FALSE,
            0x5000, 0x57FF),
    IntellivisionCartridgeConfiguration(
            0xC51464E0, "Popeye", "Parker Bros",
            "1983", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0xD8C9856A, "Q-bert", "Parker Bros",
            "1983", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0xC7BB1B0E, "Reversi", "Mattel",
            "1984", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0x8910C37A, "River Raid", "Activision",
            "1982-83", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x1682D0B4, "Robot Rubble (Prototype)", "Activision",
            "1983", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0xDCF4B15D, "Royal Dealer", "Mattel",
            "1981", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x47AA7977, "Safecracker", "Imagic",
            "1983", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0xE221808C, "Santa's Helper", "Mattel",
            "1983", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0xE9E3F60D, "Scooby Doo's Maze Chase", "Mattel/Hanna-Barbera",
            "1983", 16384, TRUE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x99AE29A9, "Sea Battle", "Mattel",
            "1980", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0xE0F0D3DA, "Sewer Sam", "Interphase",
            "1983", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x2A4C761D, "Shark! Shark!", "Mattel",
            "1982", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0xFF7CB79E, "Sharp Shot", "Mattel",
            "1982", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0x800B572F, "Slam Dunk Super Pro Basketball", "INTV",
            "1987", 32768, FALSE, FALSE,
            0x5000, 0x6FFF, 0x9000, 0xAFFF),
    IntellivisionCartridgeConfiguration(
            0xBA68FF28, "Slap Shot Super Pro Hockey", "INTV",
            "1987", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x8F959A6E, "Snafu", "Mattel",
            "1981", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0x72C2A343, "Song Player - Back in the USSR (GPL)", "Joseph Zbiciak",
            "1999", 25730, TRUE, FALSE,
            0x5000, 0x6FFF, 0x9000, 0xA240),
    IntellivisionCartridgeConfiguration(
            0x5E3130E1, "Song Player - Copacabana (GPL)", "Joseph Zbiciak",
            "1999", 20766, TRUE, FALSE,
            0x5000, 0x6FFF, 0x9000, 0x988E),
    IntellivisionCartridgeConfiguration(
            0xC91B1D79, "Song Player - Creep (GPL)", "Joseph Zbiciak",
            "1999", 8450, TRUE, FALSE,
            0x5000, 0x6080),
    IntellivisionCartridgeConfiguration(
            0x82D2E346, "Song Player - Nut March (GPL)", "Joseph Zbiciak",
            "1999", 13860, TRUE, FALSE,
            0x5000, 0x6B11),
    IntellivisionCartridgeConfiguration(
            0xFCF06A8B, "Song Player - Nut Reed (GPL)", "Joseph Zbiciak",
            "1999", 10428, TRUE, FALSE,
            0x5000, 0x645D),
    IntellivisionCartridgeConfiguration(
            0x5914220A, "Song Player - Nut Trep (GPL)", "Joseph Zbiciak",
            "1999", 7056, TRUE, FALSE,
            0x5000, 0x5DC7),
    IntellivisionCartridgeConfiguration(
            0x71F5BE4E, "Song Player - Nut Waltz (GPL)", "Joseph Zbiciak",
            "1999", 27384, TRUE, FALSE,
            0x5000, 0x6FFF, 0x9000, 0xA57B),
    IntellivisionCartridgeConfiguration(
            0x437C0E2D, "Song Player - Secret Agent Man (GPL)", "Joseph Zbiciak",
            "1999", 27464, TRUE, FALSE,
            0x5000, 0x6FFF, 0x9000, 0xA5A3),
    IntellivisionCartridgeConfiguration(
            0xDA5D77EA, "Song Player - Take on Me 3 (GPL)", "Joseph Zbiciak",
            "1999", 9722, FALSE, FALSE,
            0x5000, 0x62FC),
    IntellivisionCartridgeConfiguration(
            0x10D721CA, "Song Player - Take on Me 6 (GPL)", "Joseph Zbiciak",
            "1999", 10752, TRUE, FALSE,
            0x5000, 0x64FF),
    IntellivisionCartridgeConfiguration(
            0xCC6F4F90, "Song Player - Too Sexy (GPL)", "Joseph Zbiciak",
            "1999", 13646, TRUE, FALSE,
            0x5000, 0x6AA6),
    IntellivisionCartridgeConfiguration(
            0xE8B8EBA5, "Space Armada", "Mattel",
            "1981", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0xF95504E0, "Space Battle", "Mattel",
            "1979", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0xF8EF3E5A, "Space Cadet", "Mattel",
            "1982", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x39D3B895, "Space Hawk", "Mattel",
            "1981", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0x3784DC52, "Space Spartans", "Mattel",
            "1981", 16384, FALSE, TRUE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0xA95021FC, "Spiker! Super Pro Volleyball", "INTV",
            "1988", 32768, FALSE, FALSE,
            0x5000, 0x6FFF, 0x9000, 0xAFFF),
    IntellivisionCartridgeConfiguration(
            0xB745C1CA, "Stadium Mud Buggies", "INTV",
            "1988", 32768, FALSE, FALSE,
            0x5000, 0x6FFF, 0x9000, 0xAFFF),
    IntellivisionCartridgeConfiguration(
            0x2DEACD15, "Stampede", "Activision",
            "1982", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0x72E11FCA, "Star Strike", "Mattel",
            "1981", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0xD5B0135A, "Star Wars - The Empire Strikes Back", "Parker Bros",
            "1983", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0x4830F720, "Street", "Mattel",
            "1981", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0x3D9949EA, "Sub Hunt", "Mattel",
            "1981", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x8F7D3069, "Super Cobra", "Konami",
            "1983", 16386, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0xBAB638F2, "Super Masters!", "Mattel",
            "1982", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x16BFB8EB, "Super Pro Decathlon", "INTV",
            "1978", 32768, FALSE, FALSE,
            0x5000, 0x6FFF, 0x9000, 0xAFFF),
    IntellivisionCartridgeConfiguration(
            0x32076E9D, "Super Pro Football", "INTV",
            "1986", 32768, FALSE, FALSE,
            0x5000, 0x6FFF, 0x9000, 0xAFFF),
    IntellivisionCartridgeConfiguration(
            0x51B82EB7, "Super Soccer", "Mattel",
            "1983", 32768, FALSE, FALSE,
            0x5000, 0x6FFF, 0xD000, 0xDFFF, 0xF000, 0xFFFF),
    IntellivisionCartridgeConfiguration(
            0x15E88FCE, "Swords and Serpents", "Imagic",
            "1982", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x1F584A69, "Takeover", "Mattel",
            "1982", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0x3E9E62E, "Tennis", "Mattel",
            "1980", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0xD43FD410, "Tetris (GPL)", "Joseph Zbiciak",
            "2000", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0xF3DF94E0, "Thin Ice", "Mattel",
            "1983", 32768, FALSE, FALSE,
            0x5000, 0x6FFF, 0xD000, 0xDFFF, 0xF000, 0xFFFF),
    IntellivisionCartridgeConfiguration(
            0xC1F1CA74, "Thunder Castle", "Mattel",
            "1982", 32768, FALSE, FALSE,
            0x5000, 0x6FFF, 0xD000, 0xDFFF, 0xF000, 0xFFFF),
    IntellivisionCartridgeConfiguration(
            0xD1D352A0, "Tower of Doom", "Mattel",
            "1986", 49152, FALSE, FALSE,
            0x5000, 0x6FFF, 0x9000, 0xAFFF, 0xD000, 0xDFFF, 0xF000, 0xFFFF),
    IntellivisionCartridgeConfiguration(
            0x1AC989E2, "Triple Action", "Mattel",
            "1981", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0x95638C0, "Triple Challenge", "INTV",
            "1986", 45058, FALSE, FALSE,
            0x5000, 0x6FFF, 0x9000, 0xAFFF, 0xC000, 0xC7FF, 0xD000, 0xDFFF),
    IntellivisionCartridgeConfiguration(
            0x7A558CF5, "Tron Maze-A-Tron", "Mattel",
            "1981", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0xCA447BBD, "Tron Deadly Discs", "Mattel",
            "1981", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0x7FB9435, "Tron Solar Sailer", "Mattel",
            "1982", 24576, FALSE, TRUE,
            0x5000, 0x6FFF, 0xD000, 0xDFFF),
    IntellivisionCartridgeConfiguration(
            0x6F23A741, "Tropical Trouble", "Imagic",
            "1982", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x734F3260, "Truckin'", "Imagic",
            "1983", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x275F3512, "Turbo", "Coleco",
            "1983", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x6FA698B3, "Tutankham", "Parker Bros",
            "1983", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0xF093E801, "U.S. Ski Team Skiing", "Mattel",
            "1980", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0x752FD927, "USCF Chess", "Mattel",
            "1981", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0xF9E0789E, "Utopia", "Mattel",
            "1981", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0xA4A20354, "Vectron", "Mattel",
            "1982", 24576, FALSE, FALSE,
            0x5000, 0x6FFF, 0xD000, 0xDFFF),
    IntellivisionCartridgeConfiguration(
            0x6EFA67B2, "Venture", "Coleco",
            "1982", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0xF1ED7D27, "White Water!", "Imagic",
            "1983", 16384, FALSE, FALSE,
            0x5000, 0x6FFF),
    IntellivisionCartridgeConfiguration(
            0x15D9D27A, "World Cup Football", "Nice Ideas",
            "1985", 32768, FALSE, FALSE,
            0x5000, 0x6FFF, 0xD000, 0xDFFF, 0xF000, 0xFFFF),
    IntellivisionCartridgeConfiguration(
            0xA12C27E1, "World Series Major League Baseball", "Mattel",
            "1983", 40960, TRUE, TRUE,
            0x5000, 0x6FFF, 0xD000, 0xFFFF),
    IntellivisionCartridgeConfiguration(
            0x24B667B9, "Worm Whomper", "Activision",
            "1983", 8192, FALSE, FALSE,
            0x5000, 0x5FFF),
    IntellivisionCartridgeConfiguration(
            0x15C65DC5, "Zaxxon", "Coleco",
            "1982", 16384, FALSE, FALSE,
            0x5000, 0x6FFF)
};

IntellivisionRecognitionDatabase IntellivisionRecognitionDatabase::FACTORY;

