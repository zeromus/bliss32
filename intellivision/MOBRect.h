
#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "../core/osd/types.h"

class MOBRect
{

    public:
        BOOL intersects(MOBRect* r) {
	        return !((r->x + r->width <= x) ||
		         (r->y + r->height <= y) ||
		         (r->x >= x + width) ||
		         (r->y >= y + height));
        }

        UINT32 x;
        UINT32 y;
        UINT32 width;
        UINT32 height;

};

#endif
