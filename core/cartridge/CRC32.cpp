
#include <fstream>
#include "CRC32.h"

#define CRC32_POLY 0x04C11DB7

using namespace std;

UINT64 reflect(UINT64 ref, char ch) 
{

      UINT64 value(0); 

      // Swap bit 0 for bit 7 
      // bit 1 for bit 6, etc. 
      for(int i = 1; i < (ch + 1); i++) 
      { 
            if(ref & 1) 
                  value |= 1 << (ch - i); 
            ref >>= 1; 
      } 
      return value; 
} 



CRC32::CRC32() {
    UINT32 i, j;
    for (i = 0; i < 256; ++i) {
        crc32_table[i]=reflect(i, 8) << 24; 
        for (j = 0; j < 8; j++) 
            crc32_table[i] = (crc32_table[i] << 1) ^
                    (crc32_table[i] & (1 << 31) ? CRC32_POLY : 0); 
        crc32_table[i] = reflect(crc32_table[i], 32); 
    }
    reset();
}

UINT32 CRC32::getCrc(const CHAR* filename) {
    CRC32 crc;

    ifstream fileStream(filename, ios::in | ios::binary);
    int next = fileStream.get();
    while (next != EOF) {
        crc.update((UINT8)next);
        next = fileStream.get();
    }
    fileStream.close();

    return crc.getValue();
}

void CRC32::reset() {
    crc = 0xFFFFFFFF;
}

void CRC32::update(UINT8 data) {
/*
    crc = (crc << 8) ^ crc32_table[(crc >> 24) ^ data];
*/
    crc = (crc >> 8) ^ crc32_table[(crc & 0xFF) ^ data]; 

}

void CRC32::update(UINT8* data, UINT32 length) {
    for (UINT32 i = 0; i < length; i++)
        //crc = (crc << 8) ^ crc32_table[(crc >> 24) ^ data[i]];
        crc = (crc >> 8) ^ crc32_table[(crc & 0xFF) ^ data[i]]; 
}

UINT32 CRC32::getValue() {
    return ~crc;
}

