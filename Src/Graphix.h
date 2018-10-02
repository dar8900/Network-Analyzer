#ifndef GRAPHIX_H
#define GRAPHIX_H

#include "OLEDLIB2/u8g2.h"

#define MAX_GRAPHIC_WIDTH       126
#define MAX_GRAPHIC_AMPLITUDE    62
#define HALF_GRAPHIC_AMPLITUDE  (MAX_GRAPHIC_AMPLITUDE/2)

#define INT16_SCALE           32766 

bool DrawCurrentWave(void);
#endif