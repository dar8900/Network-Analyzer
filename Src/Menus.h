#ifndef MENUS_H
#define MENUS_H

#include <stdint.h>

#define MAX_SETUP_MENU_LINES    3
#define WHILE_LOOP_DELAY    200

typedef struct 
{
     bool (*MenuFunc)(void);
}MENU_ITEM;


typedef enum
{
    GRAPHICS_ITEM = 0,
    MEASURE_ITEM,
    ALARM_STATUS_ITEM,
    ALARM_SETUP_ITEM,
    PARAMETER_SEUP,
    CHONOMETER_ITEM,
    TIME_SET,    
    RESET_ITEM,
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
    MAX_CURRENT_PAGE,
    MIN_CURRENT_PAGE,
    MAX_POWER_PAGE,
    MIN_POWER_PAGE,
    MAX_MEASURE_PAGE
}MEASURE_PAGES;

typedef enum
{
    RESET_ENERGY = 0,
    RESET_MAX_MIN,
    SYSTEM_RESET,
    RESET_DEFAULT,
    MAX_RESET_ITEM
}RESET_ITEMS;


//bool LedCtrl(void);
void MainScreen(void);
bool ParameterSetup(void);
bool ChangeDateTimeMenu(void);
bool ChangeTime(void);
bool ChangeDate(void);
bool Chronometer(void);
bool ResetMenu(void);
void WichReset(const char * ResetTitle, uint8_t ResetType);

bool ShowMeasure(void);
bool ChooseGraphics(void);
bool ShowAlarmStatusList(void);
bool AlarmSetup(void);

#endif