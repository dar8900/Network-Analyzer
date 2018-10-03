#include "main.h"
#include "Menus.h"
#include "Oled.h"
#include "TaskOled.h"
#include "TaskRTC.h"
#include "TaskKeyboard.h"
#include "Graphix.h"

extern bool HalfSecondTick;

extern TIME_VAR GlobalTime;
extern DATE_VAR GlobalDate;

extern uint8_t DaysPerMonth[];
extern bool SettingTimeDate;


MENU_ITEM MainSetupMenu[MAX_SETUP_ITEM] = 
{
    {"Grafici"            , ChooseGraphics    },
    {"Misure"             , ShowMeasure       },
    {"Gestione Led"       , LedCtrl           },
    {"Gestione parametri" , ParameterSetup    },
    {"Setta l'orario"     , ChangeDateTimeMenu},
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
            if(MeasurePage < MAX_MEASURE_PAGE - 1)
                MeasurePage++;
            else
                MeasurePage = MAX_MEASURE_PAGE - 1;
            break;
          case BUTTON_RIGHT:
            ExitShowMeasure = true;
            break;
          case BUTTON_OK:
            break;
          default:
            break;
        }
        
        osDelay(200);
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
        osDelay(100);
    }
    return true;
}

bool LedCtrl()
{
    
    return true;
}

bool ParameterSetup()
{
    
    return true;
}

bool ChangeDateTimeMenu()
{
    uint8_t ItemPos = 0, FirstListItem = 0;
    bool ChangeTimeParam = false, ExitChangeTimeDate = false;   
    while(!ExitChangeTimeDate)
    {
        CheckOperation();
        if(HalfSecondTick)
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
        osDelay(100);
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
        osDelay(100);
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
        osDelay(100);
    }  
    SettingTimeDate = false;
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
        osDelay(100);
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
        LastButtonPressed = NO_PRESS;   
        osDelay(100);
        if(EnterMenu)
        {
            EnterMenu = false;
            break;
        }
    }
    MainMenu();
}
