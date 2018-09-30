#ifndef MENUS_H
#define MENUS_H

#define MAX_SETUP_MENU_LINES    3

typedef struct 
{
     char *ItemTitle;
     bool (*MenuFunc)(void);
}MENU_ITEM;

typedef enum
{
    LED_GEST = 0,
    PARAMETER_SEUP,
    TIME_SET,
    VOID_FUNC,
    
    MAX_SETUP_ITEM
}SETUP_ITEM_VALUE;

typedef enum
{
    CHANGE_TIME = 0,
    CHANGE_DATE,
    MAX_TIME_DATE_ITEM
}TIME_SETTING_ITEM;


bool LedCtrl(void);
void MainScreen(void);
bool ParameterSetup(void);
bool ChangeDateTimeMenu(void);
bool ChangeTime(void);
bool ChangeDate(void);

bool Inutile(void);

#endif