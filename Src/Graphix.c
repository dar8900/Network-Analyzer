#include <math.h>
#include "main.h"
#include "ADC.h"
#include "Graphix.h"
#include "Oled.h"
#include "DisplayDef.h"
#include "TaskMeasure.h"
#include "TaskKeyboard.h"
#include "Menus.h"

#define TAB_RANGE_LEN   15

extern u8g2_t u8g;
extern uint32_t ADCReadedValue[NUM_SAMPLE];
extern int16_t SinTestGraphic[];

extern float CurrentRMS;
extern float Power;
extern float MeanEnergy;


typedef struct
{
    float Value;
    char  Prefix;
    float RefactorValue;
}FORMAT_INFO;

FORMAT_INFO FormatTable[TAB_RANGE_LEN] = 
{
    {0.00001          , 'm',      1000.0},
    {0.0001           , 'm',      1000.0},
    {0.001            , 'm',      1000.0},
    {1                , ' ',         1.0},
    {10               , ' ',         1.0},
    {100              , ' ',         1.0},
    {1000.0           , 'k',       0.001},
    {10000.0          , 'k',       0.001},
    {100000.0         , 'k',       0.001},
    {1000000.0        , 'M',    0.000001},
    {10000000.0       , 'M',    0.000001},
    {100000000.0      , 'M',    0.000001},
    {1000000000.0     , 'G', 0.000000001},
    {10000000000.0    , 'G', 0.000000001},
    {100000000000.0   , 'G', 0.000000001},
    
};


const char MeasureIniz[] = 
{
    'I',
    'P',
    'E',
};

enum
{
    CURRENT_NAME = 0,
    POWER_NAME,
    ENERGY_NAME
};


const char *MeasureUnit[] = 
{
    "A",
    "W",
    "W/h",
};

enum
{
    CURRENT_UNIT = 0,
    POWER_UNIT,
    ENERGY_UNIT
};

char MeasureStr[8];
char MeasureName[2];
char PrefixMeasureUnit[6];


static uint8_t SearchRange(float Value2Refactor)
{
   uint8_t RangeIndx = 0;
   for(RangeIndx = 0; RangeIndx < TAB_RANGE_LEN; RangeIndx++)
   {
        if(Value2Refactor < FormatTable[RangeIndx].Value)
        {
            if(RangeIndx == 0)
            {
                break;  
            }
            else
            {
                RangeIndx -= 1;
                break;
            }
        }
   }

   return RangeIndx;
}


static void FormatMeasure(uint8_t Page)
{
    uint8_t Range = 0, NameMeas = 0, UnitMeas = 0;
    
    float ActualMeasure = 0.0;
    switch(Page)
    {
      case CURRENT_PAGE:
        ActualMeasure = CurrentRMS;
        NameMeas = CURRENT_NAME;
        UnitMeas = CURRENT_UNIT;
        break;
      case POWER_PAGE:
        ActualMeasure = Power;
        NameMeas = CURRENT_NAME;
        UnitMeas = POWER_UNIT;
        break;
      case ENERGY_PAGE:
        ActualMeasure = MeanEnergy;
        NameMeas = ENERGY_NAME;
        UnitMeas = ENERGY_UNIT;
        break;
      default:
        break;
    }
    Range = SearchRange(ActualMeasure);
    ActualMeasure *= FormatTable[Range].RefactorValue;
    snprintf(MeasureStr, 7, "%3.3f", ActualMeasure); 
    snprintf(PrefixMeasureUnit, 6, "%c%s", FormatTable[Range].Prefix, MeasureUnit[UnitMeas]);
    snprintf(MeasureName, 2, "%c", MeasureName[NameMeas]);
}



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
#ifdef SIM_SIN_WAVE
            YPos = 32 - ((HALF_GRAPHIC_AMPLITUDE * SinTestGraphic[XPos]) / INT16_SCALE);
#else
            if(ADCReadedValue[XPos] > ADC_HALF_MAX_VALUE)
                YPos = 32 - ((HALF_GRAPHIC_AMPLITUDE * ADCReadedValue[XPos]) / ADC_MAX_VALUE);
            else
                YPos = 32 + ((HALF_GRAPHIC_AMPLITUDE * ADCReadedValue[XPos]) / ADC_MAX_VALUE);
#endif            
            u8g2_DrawPixel(&u8g, XPos, YPos);      
        }
        
        u8g2_SendBuffer(&u8g);
        osDelay(500);
    }
    LastButtonPressed = NO_PRESS;
    return true;
}

void DrawMeasure(uint8_t Page)
{
    LastButtonPressed = NO_PRESS;
    
    char PageNumber[4];
    
    snprintf(PageNumber, 4, "%d/%d", (Page + 1), MAX_MEASURE_PAGE);
    
    MessageScreen("Premere Left");
    MessageScreen("per tornare");
       
    FormatMeasure(Page);
    
    u8g2_ClearBuffer(&u8g);
    
    u8g2_SetFont(&u8g, u8g_font_9x18B);        
    u8g2_DrawStr(&u8g, MEASURE_X_POS, TOP_MEASURE_Y_POS, MeasureStr);
    
    u8g2_SetFont(&u8g, u8g_font_8x13);  
    u8g2_DrawStr(&u8g, X_RIGHT_POS(PrefixMeasureUnit), GENERAL_STR_Y_POS(14), PrefixMeasureUnit);
    u8g2_DrawStr(&u8g, X_LEFT_POS, GENERAL_STR_Y_POS(14), MeasureName);
    
    u8g2_SetFont(&u8g, u8g_font_4x6);
    u8g2_DrawStr(&u8g, X_CENTER_POS(PageNumber), BOTTOM_INFO_BAR_Y_POS, PageNumber);
    DrawBottomBarInfo(MEASURE_PAGE);
    
    u8g2_SendBuffer(&u8g);
    
    return;
}