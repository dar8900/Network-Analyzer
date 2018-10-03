#ifndef GRAPHIX_H
#define GRAPHIX_H

#include "OLEDLIB2/u8g2.h"

#define MAX_GRAPHIC_WIDTH       126
#define MAX_GRAPHIC_AMPLITUDE    62
#define HALF_GRAPHIC_AMPLITUDE  (MAX_GRAPHIC_AMPLITUDE/2)

#define INT16_SCALE           32766 

#define ADC_MAX_VALUE               4095
#define ADC_HALF_MAX_VALUE          2048

bool DrawCurrentWave(void);
void DrawMeasure(uint8_t Page);
#endif