
#ifndef PALETTE_H
#define PALETTE_H

#include "osd/types.h"

typedef struct _PALETTE
{
    UINT16 m_iNumEntries;
    UINT32 m_iEntries[256];

} PALETTE;

#endif
