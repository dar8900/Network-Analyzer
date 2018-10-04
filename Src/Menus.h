#ifndef MENUS_H
#define MENUS_H

#include <stdint.h>

#define MAX_SETUP_MENU_LINES    3

typedef struct 
{
     char *ItemTitle;
     bool (*MenuFunc)(void);
}MENU_ITEM;

typedef struct 
{
     char *ItemTitle;
     uint8_t Type;
     void *ParamValue;
}PARAMETER_ITEM;

typedef enum
{
    CONFIRM_TYPE = 0,
    VALUE_TYPE,
    MAX_TYPE
}PARAMETER_TYPES;

typedef enum
{
    GRAPHICS_ITEM = 0,
    MEASURE_ITEM,
    LED_GEST,
    PARAMETER_SEUP,
    TIME_SET,    
    MAX_SETUP_ITEM
}SETUP_ITEM_VALUE;

typedef enum
{
    CHANGE_TIME = 0,
    CHANGE_DATE,
    MAX_TIME_DATE_ITEM
}TIME_SETTING_ITEM;

typedef enum
{
    CURRENT_WAVEFORM = 0,
//    POWER,
//    ENERGY,
    MAX_GRAPHIC_ITEM
}GRAPHIC_ITEMS;

typedef enum
{
    CURRENT_PAGE = 0,
    POWER_PAGE,
    ENERGY_PAGE,
    MAX_MEASURE_PAGE
}MEASURE_PAGES;


typedef enum
{
    MEASURE_ENABLE = 0,
    ADC_OFFSET,
    MAX_PARAMETER_ITEM
}PARAMETER_ITEMS;

bool ChooseYesNo(char *TitleChoice);
bool LedCtrl(void);
void MainScreen(void);
bool ParameterSetup(void);
bool ChangeDateTimeMenu(void);
bool ChangeTime(void);
bool ChangeDate(void);

bool ShowMeasure(void);
bool ChooseGraphics(void);

#endif