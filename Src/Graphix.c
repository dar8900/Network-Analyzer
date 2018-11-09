#include <math.h>
#include "main.h"
#include "ADC.h"
#include "Graphix.h"
#include "Display.h"
#include "DisplayDef.h"
#include "TaskMeasure.h"
#include "TaskKeyboard.h"
#include "Menus.h"
#include "Parameters.h"
#include "TaskRTC.h"

#define TAB_RANGE_LEN   15

#define CLOCK_RADIUS    28
#define CLOCK_X_CENTER  (127 - CLOCK_RADIUS)
#define CLOCK_Y_CENTER  32

extern u8g2_t u8g;
extern uint32_t ADCReadedValue[NUM_SAMPLE];
extern uint32_t ADCReadedValueSim[NUM_SAMPLE];
extern int16_t SinTestGraphic[];
extern TIME_VAR GlobalTime;

//extern uint16_t ACDOffset;

typedef struct
{
    float Value;
    char  Prefix;
    float RefactorValue;
}FORMAT_INFO;

const FORMAT_INFO FormatTable[TAB_RANGE_LEN] = 
{
    {0.1              , 'm',      1000.0},
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

const char *MaxMinName[] = 
{
    "Imax",
    "Imin",
    "Pmax",
    "Pmin",
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
        ActualMeasure = APROXIMATION(GeneralMeasures.MeanCurrentRMS, 3);
        NameMeas = CURRENT_NAME;
        UnitMeas = CURRENT_UNIT;
        break;
      case POWER_PAGE:
        ActualMeasure = APROXIMATION(GeneralMeasures.Power, 3);
        NameMeas = POWER_NAME;
        UnitMeas = POWER_UNIT;
        break;
      case ENERGY_PAGE:
        ActualMeasure = APROXIMATION(GeneralMeasures.MeanEnergy, 3);
        NameMeas = ENERGY_NAME;
        UnitMeas = ENERGY_UNIT;
        break;
      case MAX_CURRENT_PAGE:
        ActualMeasure = APROXIMATION(GeneralMeasures.MaxCurrent, 3);
        NameMeas = CURRENT_NAME;
        UnitMeas = CURRENT_UNIT;
        break;
      case MIN_CURRENT_PAGE:
        ActualMeasure = APROXIMATION(GeneralMeasures.MinCurrent, 3);
        NameMeas = CURRENT_NAME;
        UnitMeas = CURRENT_UNIT;
        break;
      case MAX_POWER_PAGE:
        ActualMeasure = APROXIMATION(GeneralMeasures.MaxPower, 3);
        NameMeas = POWER_NAME;
        UnitMeas = POWER_UNIT;
        break;
      case MIN_POWER_PAGE:
        ActualMeasure = APROXIMATION(GeneralMeasures.MinPower, 3);
        NameMeas = POWER_NAME;
        UnitMeas = POWER_UNIT;
        break;
      default:
        break;
    }
    Range = SearchRange(ActualMeasure);
    ActualMeasure *= FormatTable[Range].RefactorValue;
    snprintf(MeasureStr, 7, "%3.3f", ActualMeasure); 
    snprintf(PrefixMeasureUnit, 6, "%c%s", FormatTable[Range].Prefix, MeasureUnit[UnitMeas]);
    snprintf(MeasureName, 2, "%c", MeasureIniz[NameMeas]);
}



static void DrawGraphicSkeleton()
{
    u8g2_DrawFrame(&u8g, 0, 0 , SCREEN_MAX_WIDTH, SCREEN_MAX_HIGH);
    u8g2_DrawLine(&u8g, SCREEN_MAX_HIGH, 0, SCREEN_MAX_HIGH, SCREEN_MAX_HIGH);
    u8g2_DrawLine(&u8g, 0, (SCREEN_MAX_HIGH/2), SCREEN_MAX_WIDTH, (SCREEN_MAX_HIGH/2));
    return;
}

//uint32_t AdcValueToGraphic[MAX_GRAPHIC_WIDTH];

bool DrawCurrentWave()
{
    uint8_t XPos = 0, YPos = 0;
    LastButtonPressed = NO_PRESS;
        
    MessageScreen("Premere Left");
    MessageScreen("per tornare");
       
    
    while(LastButtonPressed != BUTTON_LEFT)
    {
        u8g2_ClearBuffer(&u8g);
        
//        memcpy(AdcValueToGraphic, ADCReadedValue, sizeof(ADCReadedValue));
        
        DrawGraphicSkeleton();

        for(XPos = 0; XPos < MAX_GRAPHIC_WIDTH; XPos++)
        {
//            if(!GeneralParams.EnableSimulation)
//            {
//                YPos = 30 - ((HALF_GRAPHIC_AMPLITUDE * (AdcValueToGraphic[XPos] - (GeneralParams.ADCOffset))) / (GeneralParams.ADCOffset));       
//            }
//            else
//            {
                YPos = 31 - ((HALF_GRAPHIC_AMPLITUDE * (ADCReadedValueSim[XPos] - 2048)) / 2048);  
//            }
            u8g2_DrawPixel(&u8g, XPos, YPos);      
        }
        
        u8g2_SendBuffer(&u8g);
        WDogOsDelay(500);
    }
    LastButtonPressed = NO_PRESS;
    return true;
}

void DrawMeasure(uint8_t Page)
{    
    char PageNumber[4];
    
    snprintf(PageNumber, 4, "%d/%d", (Page + 1), MAX_MEASURE_PAGE);
          
    FormatMeasure(Page);
    
    u8g2_ClearBuffer(&u8g);
    
    // Scrittura misura
    u8g2_SetFont(&u8g, u8g_font_courR18);  
    u8g2_DrawStr(&u8g, MEASURE_X_POS, GENERAL_STR_Y_POS(20), MeasureStr);
    
    // Scrittura udm e nome
    u8g2_SetFont(&u8g, u8g_font_8x13);  
    if(Page == ENERGY_PAGE)
    {
        u8g2_SetFont(&u8g, u8g_font_6x13);
        u8g2_DrawStr(&u8g, X_RIGHT_POS(PrefixMeasureUnit), GENERAL_STR_Y_POS(20), PrefixMeasureUnit);
    }
    else
        u8g2_DrawStr(&u8g, X_RIGHT_POS(PrefixMeasureUnit), GENERAL_STR_Y_POS(20), PrefixMeasureUnit);
    
    if(Page >=  MAX_CURRENT_PAGE && Page <= MIN_POWER_PAGE)
    {
        u8g2_SetFont(&u8g, u8g2_font_5x8_tf);    
        u8g2_DrawStr(&u8g, X_CENTER_POS(MaxMinName[Page - MAX_CURRENT_PAGE]), GENERAL_STR_Y_POS(40), MaxMinName[Page - MAX_CURRENT_PAGE]);
    }
    else
    {
        u8g2_SetFont(&u8g, u8g_font_8x13);    
        u8g2_DrawStr(&u8g, X_LEFT_POS, GENERAL_STR_Y_POS(20), MeasureName);
    }
    u8g2_SetFont(&u8g, u8g_font_4x6);
    u8g2_DrawStr(&u8g, X_CENTER_POS(PageNumber), BOTTOM_INFO_BAR_Y_POS, PageNumber);
    
    DrawTopInfoBar();
    DrawBottomBarInfo(MEASURE_PAGE);
    
    u8g2_SendBuffer(&u8g);
    
    return;
}



void DrawClock()
{
    char Date[9], Time[9];
    float Meno90 = (float)TO_RADIANTS(90.0);
    snprintf(Date, 9, "%02d/%02d/%02d", GlobalTime.day, GlobalTime.month, GlobalTime.year);
    snprintf(Time, 9, "%02d:%02d:%02d", GlobalTime.hours, GlobalTime.minutes, GlobalTime.seconds);
    int16_t XPosHour =   (int16_t) (CLOCK_RADIUS - 10) * cos((double)TO_RADIANTS(30*(GlobalTime.hours%12))- Meno90);
    int16_t YPosHour =   (int16_t) (CLOCK_RADIUS - 10) * sin((double)TO_RADIANTS(30*(GlobalTime.hours%12))- Meno90); 
    int16_t XPosMinute = (int16_t) (CLOCK_RADIUS - 4) * cos((double)TO_RADIANTS(6*GlobalTime.minutes ) - Meno90);
    int16_t YPosMinute = (int16_t) (CLOCK_RADIUS - 4) * sin((double)TO_RADIANTS(6*GlobalTime.minutes ) - Meno90);
    int16_t XPosSecond = (int16_t) (CLOCK_RADIUS - 2) * cos((double)TO_RADIANTS(6*GlobalTime.seconds ) - Meno90);
    int16_t YPosSecond = (int16_t) (CLOCK_RADIUS - 2) * sin((double)TO_RADIANTS(6*GlobalTime.seconds ) - Meno90);
    
    u8g2_ClearBuffer(&u8g);
    u8g2_SetFont(&u8g, u8g_font_7x13B);
    
    switch(GeneralParams.ScreenSaverType)
    {
      case ANALOG_DATE:
        u8g2_DrawCircle(&u8g, CLOCK_X_CENTER, CLOCK_Y_CENTER, CLOCK_RADIUS, U8G2_DRAW_ALL);
        u8g2_DrawCircle(&u8g, CLOCK_X_CENTER, CLOCK_Y_CENTER, 3, U8G2_DRAW_ALL);
        
        // ORE
        u8g2_DrawLine(&u8g, CLOCK_X_CENTER, CLOCK_Y_CENTER, CLOCK_X_CENTER + XPosHour,   CLOCK_Y_CENTER + YPosHour);
              
       // MINUTI
        u8g2_DrawLine(&u8g, CLOCK_X_CENTER, CLOCK_Y_CENTER, CLOCK_X_CENTER + XPosMinute, CLOCK_Y_CENTER + YPosMinute);
                
        // SECONDI  
        u8g2_DrawLine(&u8g, CLOCK_X_CENTER, CLOCK_Y_CENTER, CLOCK_X_CENTER + XPosSecond, CLOCK_Y_CENTER + YPosSecond); 
        
        
        u8g2_DrawStr(&u8g, X_LEFT_POS, GENERAL_STR_Y_POS(29), Date);
        break;
      case DIGITAL_DATE:
        u8g2_DrawStr(&u8g, X_LEFT_POS, GENERAL_STR_Y_POS(29), Date);    
        u8g2_DrawStr(&u8g, X_RIGHT_POS(Time), GENERAL_STR_Y_POS(29), Time); 
        break;
      case ANALOG_ONLY:
        u8g2_DrawCircle(&u8g, CLOCK_X_CENTER - 35, CLOCK_Y_CENTER, CLOCK_RADIUS, U8G2_DRAW_ALL);
        u8g2_DrawCircle(&u8g, CLOCK_X_CENTER - 35, CLOCK_Y_CENTER, 3, U8G2_DRAW_ALL);
        
        // ORE
        u8g2_DrawLine(&u8g, CLOCK_X_CENTER - 35, CLOCK_Y_CENTER, (CLOCK_X_CENTER - 35) + XPosHour, CLOCK_Y_CENTER + YPosHour);
        
        // MINUTI
        u8g2_DrawLine(&u8g, CLOCK_X_CENTER - 35, CLOCK_Y_CENTER, (CLOCK_X_CENTER - 35) + XPosMinute, CLOCK_Y_CENTER + YPosMinute);
        
        // SECONDI  
        u8g2_DrawLine(&u8g, CLOCK_X_CENTER - 35, CLOCK_Y_CENTER, (CLOCK_X_CENTER - 35) + XPosSecond, CLOCK_Y_CENTER + YPosSecond); 
        
        break;
      default:
        break;  
    }                 
    u8g2_SendBuffer(&u8g);   
                                
}