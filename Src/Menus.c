#include "main.h"
#include "Menus.h"
#include "Oled.h"
#include "TaskOled.h"
#include "TaskRTC.h"
#include "TaskLed.h"
#include "TaskKeyboard.h"
#include "Graphix.h"
#include "Parameters.h"
#include "TaskMeasure.h"


extern bool HalfSecondTick;

extern TIME_VAR GlobalTime;
extern DATE_VAR GlobalDate;

extern uint8_t DaysPerMonth[];
extern bool SettingTimeDate;

extern uint8_t LedConf;

extern MEASURES GeneralMeasures;


extern PARAMETER_ITEM ParametersMenu[MAX_PARAMETER_ITEM];
extern PARAMETER_ITEM AlarmThrMenu[MAX_ALARM_SETUP_ITEM];

MENU_ITEM MainSetupMenu[MAX_SETUP_ITEM] = 
{
    {"Grafici"            , ChooseGraphics    },
    {"Misure"             , ShowMeasure       },
    {"Impostare allarmi"  , AlarmSetup        },
    {"Impostare i Led"    , LedCtrl           },
    {"Modifica parametri" , ParameterSetup    },
    {"Setta l'orario"     , ChangeDateTimeMenu},
    {"Reset"              , ResetMenu         },
}; 

MENU_ITEM TimeSetting[MAX_TIME_DATE_ITEM] = 
{
    {"Cambia orario", ChangeTime},
    {"Cambia data"  , ChangeDate},
};

MENU_ITEM GraphicsMenu[MAX_GRAPHIC_ITEM] = 
{
    {"Forma d'onda I"     , DrawCurrentWave},
};


char *ResetList[MAX_RESET_ITEM] = 
{
    "Reset energia",
    "Riavvia sistema",
};

enum
{
    RESET_ENERGY_TYPE = 0,
    SYSTEM_RESET_TYPE
};

const uint8_t ResetType[MAX_RESET_ITEM] = 
{
    RESET_ENERGY_TYPE,
    SYSTEM_RESET_TYPE,
};

const char *LedCombStr[MAX_LED_COMBINATIONS] = 
{
    "RGB",
    "Rosso",
    "Verde",
    "Blu",
    "Rosso e verde",
    "Rosso e blu",
    "Blu e verde",
    "Tutti spenti",
};



bool ShowMeasure()
{
    uint8_t MeasurePage = CURRENT_PAGE;
    bool ExitShowMeasure = false;
    while(!ExitShowMeasure)
    {
        CheckOperation();
        DrawMeasure(MeasurePage);
        switch(LastButtonPressed)
        {
          case BUTTON_UP:
            break;
          case BUTTON_DOWN:
            break;
          case BUTTON_LEFT:
            ExitShowMeasure = true;
            break;
          case BUTTON_RIGHT:           
            if(MeasurePage < MAX_MEASURE_PAGE - 1)
                MeasurePage++;
            else
                MeasurePage = 0;
            break;
          case BUTTON_OK:
            break;
          default:
            break;
        }
        LastButtonPressed = NO_PRESS;
        osDelay(WHILE_LOOP_DELAY);
    }
    
    return true;
}



bool ChooseGraphics()
{
    uint8_t ItemPos = 0, FirstListItem = 0;
    bool EnterGraphic = false, ExitChooseGraphic = false;
    
    while(!ExitChooseGraphic)
    {
        CheckOperation();
        DrawMenuLoop("Grafici", GraphicsMenu, ItemPos, FirstListItem, MAX_GRAPHIC_ITEM, MAX_SETUP_MENU_LINES);
        switch(LastButtonPressed)
        {
          case BUTTON_UP:
            if(ItemPos > 0)
                ItemPos--;
            else
                ItemPos = MAX_GRAPHIC_ITEM - 1;
            break;
          case BUTTON_DOWN:
            if(ItemPos < MAX_GRAPHIC_ITEM - 1)
                ItemPos++;
            else
                ItemPos = 0;            
            break;
          case BUTTON_LEFT:
            ExitChooseGraphic = true;
            break;
          case BUTTON_RIGHT:
            EnterGraphic = true;
            break;
          case BUTTON_OK:
            break;
          default:
            break;
        }
        if(ItemPos <= (MAX_SETUP_MENU_LINES - 1))
        {
            FirstListItem = 0;  
        }
        else
        {
            FirstListItem = ItemPos - (MAX_SETUP_MENU_LINES - 1);
        }
        if(EnterGraphic)
        {
            LastButtonPressed = NO_PRESS;
            GraphicsMenu[ItemPos].MenuFunc();
            EnterGraphic = false;
        }
        LastButtonPressed = NO_PRESS;
        osDelay(WHILE_LOOP_DELAY);
    }
    return true;
}

bool AlarmSetup()
{
    uint8_t AlarmItem = 0, FirstListItem = 0;
    bool ExitAlarmsetup = false, ChooseAlarm = false;
    while(!ExitAlarmsetup)
    {
        CheckOperation();
        DrawParamLoop("Imposta allarmi", AlarmThrMenu, AlarmItem, FirstListItem, MAX_ALARM_SETUP_ITEM, MAX_SETUP_MENU_LINES);     
        switch(LastButtonPressed)
        {
          case BUTTON_UP:
            if(AlarmItem > 0)
                AlarmItem--;
            else
                AlarmItem = MAX_ALARM_SETUP_ITEM - 1;
            break;
          case BUTTON_DOWN:
            if(AlarmItem < MAX_ALARM_SETUP_ITEM - 1)
                AlarmItem++;
            else
                AlarmItem = 0;            
            break;
          case BUTTON_LEFT:
            ExitAlarmsetup = true;
            break;
          case BUTTON_RIGHT:
            ChooseAlarm = true;
            break;
          case BUTTON_OK:
            break;
            default:
              break;
        } 
        if(AlarmItem <= (MAX_SETUP_MENU_LINES - 1))
        {
            FirstListItem = 0;  
        }
        else
        {
            FirstListItem = AlarmItem - (MAX_SETUP_MENU_LINES - 1);
        }    
        if(ChooseAlarm)
        {
            ChangeAlarmThrs(AlarmItem);
            ChooseAlarm = false;
        }
        LastButtonPressed = NO_PRESS;
        osDelay(WHILE_LOOP_DELAY);
    }
    return true;
}


bool LedCtrl()
{
    uint8_t LedComb = 0, FirstListItem = 0;
    bool ExitLedConf = false, ChoosedLedComb = false;
    while(!ExitLedConf)
    {
        CheckOperation();
        DrawListLoop("Controllo Led", LedCombStr, LedComb, FirstListItem, MAX_LED_COMBINATIONS, MAX_SETUP_MENU_LINES);
        switch(LastButtonPressed)
        {
          case BUTTON_UP:
            if(LedComb > 0)
                LedComb--;
            else
                LedComb = MAX_LED_COMBINATIONS - 1;
            break;
          case BUTTON_DOWN:
            if(LedComb < MAX_LED_COMBINATIONS - 1)
                LedComb++;
            else
                LedComb = 0;            
            break;
          case BUTTON_LEFT:
            ExitLedConf = true;
            break;
          case BUTTON_RIGHT:
            ChoosedLedComb = true;
            ExitLedConf = true;
            break;
          case BUTTON_OK:
            break;
          default:
            break;           
        }
        
        if(LedComb <= (MAX_SETUP_MENU_LINES - 1))
        {
            FirstListItem = 0;  
        }
        else
        {
            FirstListItem = LedComb - (MAX_SETUP_MENU_LINES - 1);
        }
        LastButtonPressed = NO_PRESS;
        osDelay(WHILE_LOOP_DELAY);
    }
    if(ChoosedLedComb)
    {
        LastButtonPressed = NO_PRESS;
        LedConf = LedComb;
        ChoosedLedComb = false;
    }
    return true;
}

bool ParameterSetup()
{
    uint8_t ParamItem = 0, FirstListItem = 0;
    bool ExitParamSetup = false, ChooseParam = false;
    while(!ExitParamSetup)
    {
        CheckOperation();
        DrawParamLoop("Imp.Parametri", ParametersMenu, ParamItem, FirstListItem, MAX_PARAMETER_ITEM, MAX_SETUP_MENU_LINES);     
        switch(LastButtonPressed)
        {
          case BUTTON_UP:
            if(ParamItem > 0)
                ParamItem--;
            else
                ParamItem = MAX_PARAMETER_ITEM - 1;
            break;
          case BUTTON_DOWN:
            if(ParamItem < MAX_PARAMETER_ITEM - 1)
                ParamItem++;
            else
                ParamItem = 0;            
            break;
          case BUTTON_LEFT:
            ExitParamSetup = true;
            break;
          case BUTTON_RIGHT:
            ChooseParam = true;
            break;
          case BUTTON_OK:
            break;
            default:
              break;
        } 
        if(ParamItem <= (MAX_SETUP_MENU_LINES - 1))
        {
            FirstListItem = 0;  
        }
        else
        {
            FirstListItem = ParamItem - (MAX_SETUP_MENU_LINES - 1);
        }    
        if(ChooseParam)
        {
            switch(ParametersMenu[ParamItem].Type)
            {
              case CONFIRM_TYPE:
                *(bool *)ParametersMenu[ParamItem].ParamValue = ChooseYesNo(ParametersMenu[ParamItem].ItemTitle);
                break;
              case INT_VALUE_TYPE:
                *(uint16_t*)ParametersMenu[ParamItem].ParamValue = ChangeValue(*(uint16_t*)ParametersMenu[ParamItem].ParamValue, ParamItem);
                break;
              default:
                break;       
            }
            ChooseParam = false;
        }
        LastButtonPressed = NO_PRESS;
        osDelay(WHILE_LOOP_DELAY);
    }
     
    return true;
}

bool ChangeDateTimeMenu()
{
    uint8_t ItemPos = 0, FirstListItem = 0;
    bool ChangeTimeParam = false, ExitChangeTimeDate = false;   
    while(!ExitChangeTimeDate)
    {
        CheckOperation();
        DrawMenuLoop("Setta ora/data", TimeSetting, ItemPos, FirstListItem, MAX_TIME_DATE_ITEM, MAX_SETUP_MENU_LINES);
        switch(LastButtonPressed)
        {
          case BUTTON_UP:
            if(ItemPos > 0)
                ItemPos--;
            else
                ItemPos = MAX_TIME_DATE_ITEM - 1;
            break;
          case BUTTON_DOWN:
            if(ItemPos < MAX_TIME_DATE_ITEM - 1)
                ItemPos++;
            else
                ItemPos = 0;            
            break;
          case BUTTON_LEFT:
            ExitChangeTimeDate = true;
            break;
          case BUTTON_RIGHT:
            ChangeTimeParam = true;
            break;
          case BUTTON_OK:
            break;
            default:
              break;
        }
        
        if(ItemPos < MAX_SETUP_MENU_LINES)
        {
            FirstListItem = 0;  
        }
        else
        {
            FirstListItem = (MAX_SETUP_MENU_LINES * (ItemPos / MAX_SETUP_MENU_LINES));
        }
        if(ChangeTimeParam)
        {
            LastButtonPressed = NO_PRESS;
            TimeSetting[ItemPos].MenuFunc();
            ChangeTimeParam = false;
        }
        LastButtonPressed = NO_PRESS;
        osDelay(WHILE_LOOP_DELAY);
    }  
    return true;
}

bool ChangeTime()
{   
    uint8_t BoxPos = 0;
    uint8_t Hour = GlobalTime.hours;
    uint8_t Minute = GlobalTime.minutes;
    uint8_t Second = GlobalTime.seconds;
    bool ChangeTimeParam = false, ExitChangeTime = false;   
    SettingTimeDate = true;
    while(!ExitChangeTime)
    {
        CheckOperation();
        DrawTimeDateChangeLoop(BoxPos, TIME_TYPE, Hour, Minute, Second);
        switch(LastButtonPressed)
        {
          case BUTTON_UP:
            switch(BoxPos)
            {
              case 0:
                if(Hour > 0)
                    Hour--;
                else
                    Hour = 23;
                break;
              case 1:
                if(Minute > 0)
                    Minute--;
                else
                    Minute = 59;
                break;
              default:
                break;
            }
            break;
          case BUTTON_DOWN:
            switch(BoxPos)
            {
              case 0:
                if(Hour < 23)
                    Hour++;
                else
                    Hour = 0;
                break;
              case 1:
                if(Minute < 59)
                    Minute++;
                else
                    Minute = 0;
                break;
              default:
                break;
            }          
            break;
          case BUTTON_LEFT:
            ExitChangeTime = true;
            break;
          case BUTTON_RIGHT:
            if(BoxPos < 1)
                BoxPos++;
            else
                BoxPos = 0;
            break;
          case BUTTON_OK:
            ChangeTimeParam = true;
            ExitChangeTime = true;
            break;
          default:
            break;
        }
        LastButtonPressed = NO_PRESS;
        if(ChangeTimeParam)
        {
            SetChangedTime(Hour, Minute);
        }
        osDelay(WHILE_LOOP_DELAY);
    }  
    SettingTimeDate = false;
    return true;
}

bool ChangeDate()
{   
    uint8_t BoxPos = 0;
    uint8_t Day = GlobalDate.day;
    uint8_t Month = GlobalDate.month;
    uint8_t Year = GlobalDate.year;
    bool ChangeDateParam = false, ExitChangeDate = false;   
    SettingTimeDate = true;
    while(!ExitChangeDate)
    {
        CheckOperation();
        DrawTimeDateChangeLoop(BoxPos, DATE_TYPE, Day, Month, Year);    
        switch(LastButtonPressed)
        {
          case BUTTON_UP:
            switch(BoxPos)
            {
              case 0:
                if(Month > 1)
                    Month--;
                else
                    Month = 12;
                break;
              case 1:
                if(Day > 1)
                    Day--;
                else
                    Day = DaysPerMonth[Month];
                break;
              case 2:
                if(Year > 0)
                    Year--;
                else
                    Year = 99;
                break;
              default:
                break;
            }
            break;
          case BUTTON_DOWN:
            switch(BoxPos)
            {
              case 0:
                if(Month < 12)
                    Month++;
                else
                    Month = 1;
                break;
              case 1:
                if(Day < DaysPerMonth[Month])
                    Day++;
                else
                    Day = 1;
                break;
              case 2:
                if(Year < 99)
                    Year++;
                else
                    Year = 0;
                break;
              default:
                break;
            }          
            break;
          case BUTTON_LEFT:
            ExitChangeDate = true;
            break;
          case BUTTON_RIGHT:
            if(BoxPos < 2)
                BoxPos++;
            else
                BoxPos = 0;
            break;
          case BUTTON_OK:
            ExitChangeDate = true;
            ChangeDateParam = true;
            break;
          default:
            break;
        }
        LastButtonPressed = NO_PRESS;
        if(ChangeDateParam)
        {
            SetChangedDate(Day, Month, Year);
        }
        osDelay(WHILE_LOOP_DELAY);
    }  
    SettingTimeDate = false;
    return true;
    
}

void WichReset(char * ResetTitle, uint8_t ResetType)
{
    bool ResetChoose = false;
    ResetChoose = ChooseYesNo(ResetTitle); 
    if(ResetChoose)
    {
        switch(ResetType)
        {
          case RESET_ENERGY_TYPE:
            MessageScreen("Reset in corso");
            GeneralMeasures.MeanEnergy = 0.0;
            break;
          case SYSTEM_RESET_TYPE:
            MessageScreen("Restart in corso");
            HAL_NVIC_SystemReset();
            break;
          default:
            break;
        }
    }
    return;
}

bool ResetMenu()
{
    uint8_t ItemPos = 0, FirstListItem = 0;
    bool ExitResetMenu = false, ChoosedReset = false;
    
    while(!ExitResetMenu)
    {
        CheckOperation();
        DrawListLoop("Reset", ResetList, ItemPos, FirstListItem, MAX_RESET_ITEM, MAX_SETUP_MENU_LINES);
        switch(LastButtonPressed)
        {
          case BUTTON_UP:
            if(ItemPos > 0)
                ItemPos--;
            else
                ItemPos = MAX_RESET_ITEM - 1;
            break;
          case BUTTON_DOWN:
            if(ItemPos < MAX_RESET_ITEM - 1)
                ItemPos++;
            else
                ItemPos = 0;            
            break;
          case BUTTON_LEFT:
            ExitResetMenu = true;
            break;
          case BUTTON_RIGHT:
            ChoosedReset = true;
            break;
          case BUTTON_OK:
            break;
          default:
            break;
        }
        if(ItemPos <= (MAX_SETUP_MENU_LINES - 1))
        {
            FirstListItem = 0;  
        }
        else
        {
            FirstListItem = ItemPos - (MAX_SETUP_MENU_LINES - 1);
        }
        if(ChoosedReset)
        {
            LastButtonPressed = NO_PRESS;
            WichReset(ResetList[ItemPos], ResetType[ItemPos]);
            ChoosedReset = false;
        }
        LastButtonPressed = NO_PRESS;
        osDelay(WHILE_LOOP_DELAY);
    }
    
    return true;
}



void MainMenu()
{
    uint8_t ItemPos = 0, FirstListItem = 0;
    bool EnterMenu = false, ExitMainMenu = false;
    
    while(!ExitMainMenu)
    {
        CheckOperation();
        DrawMenuLoop("Setup", MainSetupMenu, ItemPos, FirstListItem, MAX_SETUP_ITEM, MAX_SETUP_MENU_LINES);
        switch(LastButtonPressed)
        {
          case BUTTON_UP:
            if(ItemPos > 0)
                ItemPos--;
            else
                ItemPos = MAX_SETUP_ITEM - 1;
            break;
          case BUTTON_DOWN:
            if(ItemPos < MAX_SETUP_ITEM - 1)
                ItemPos++;
            else
                ItemPos = 0;            
            break;
          case BUTTON_LEFT:
            ExitMainMenu = true;
            break;
          case BUTTON_RIGHT:
            EnterMenu = true;
            break;
          case BUTTON_OK:
            break;
          default:
            break;
        }
        
        if(ItemPos <= (MAX_SETUP_MENU_LINES - 1))
        {
            FirstListItem = 0;  
        }
        else
        {
            FirstListItem = ItemPos - (MAX_SETUP_MENU_LINES - 1);
        }
        if(EnterMenu)
        {
            LastButtonPressed = NO_PRESS;
            MainSetupMenu[ItemPos].MenuFunc();
            EnterMenu = false;
        }
        LastButtonPressed = NO_PRESS;
        osDelay(WHILE_LOOP_DELAY);
    }
}


void MainScreen()
{
    bool EnterMenu = false;
    LastButtonPressed = NO_PRESS;
    while(1)
    {
        CheckOperation();     
        DrawMainScreenLoop();
        switch(LastButtonPressed)
        {
          case BUTTON_OK:
            EnterMenu = true;
            break;
          case BUTTON_UP:
          case BUTTON_DOWN:
          case BUTTON_LEFT:
          case BUTTON_RIGHT:
          default:
            break;
        }   
        if(EnterMenu)
        {
            LastButtonPressed = NO_PRESS;
            EnterMenu = false;
            break;
        }
        LastButtonPressed = NO_PRESS;
        osDelay(WHILE_LOOP_DELAY);
    }
    MainMenu();
}
