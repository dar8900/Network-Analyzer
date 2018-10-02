#include <math.h>
#include "main.h"
#include "ADC.h"
#include "Graphix.h"
#include "Oled.h"
#include "TaskMeasure.h"
#include "TaskKeyboard.h"


extern u8g2_t u8g;
extern uint32_t ADCReadedValue[NUM_SAMPLE];
extern int16_t SinTestGraphic[];


static void DrawGraphicSkeleton()
{
    u8g2_DrawFrame(&u8g, 0, 0 , 128, 64);
    u8g2_DrawLine(&u8g, 64, 0, 64, 64);
    u8g2_DrawLine(&u8g, 0, 32, 128, 32);
    return;
}


bool DrawCurrentWave()
{
    uint8_t XPos = 0, YPos = 0;
    LastButtonPressed = NO_PRESS;
        
    MessageScreen("Premere Left");
    MessageScreen("per tornare");
    
    while(LastButtonPressed != BUTTON_LEFT)
    {
        u8g2_ClearBuffer(&u8g);
        
        DrawGraphicSkeleton();
        
        for(XPos = 0; XPos < MAX_GRAPHIC_WIDTH; XPos++)
        {
            YPos = 32 - ((HALF_GRAPHIC_AMPLITUDE * SinTestGraphic[XPos]) / INT16_SCALE);           
            u8g2_DrawPixel(&u8g, XPos, YPos);      
        }
        
        u8g2_SendBuffer(&u8g);
        osDelay(200);
    }
    LastButtonPressed = NO_PRESS;
    return true;
}