#include "main.h"
#include "Parameters.h"
#include "Oled.h"
#include "TaskKeyboard.h"
#include "Menus.h"
#include "TaskOled.h"
#include <math.h>

// Parametri 
PARAMETERS_S GeneralParams;


enum
{
    COMPOSE = 0,
    DECOMPOSE
};

PARAMETER_ITEM ParametersMenu[MAX_PARAMETER_ITEM] = 
{
    {"Abilitare misura"  , CONFIRM_TYPE    ,  &GeneralParams.EnableMeasure},
    {"ADC Offset"        , INT_VALUE_TYPE  ,  &GeneralParams.ADCOffset    },
};

PARAMETER_ITEM AlarmThrMenu[MAX_ALARM_SETUP_ITEM] = 
{
    {"Soglie corrente "  , FLOAT_VALUE_TYPE    ,  NULL},
    {"Soglie potenza  "  , FLOAT_VALUE_TYPE    ,  NULL},
    {"Soglia energia  "  , FLOAT_VALUE_TYPE    ,  NULL},
};



static void NumbersOperation(uint16_t *Value, uint8_t StoreArray[], uint8_t CompOrDecomp)
{
    uint16_t ValueCopy = *Value;
    uint8_t j = 0;
    if(CompOrDecomp == DECOMPOSE)
    {
        for(j = 0; j < 5; j++)
        {
            StoreArray[j] = ValueCopy/pow(10,(4-j));
            ValueCopy %= (uint16_t)pow(10,(4-j));         
        }
    }
    else
    {
        ValueCopy = 0;
        for(j = 0; j < 5; j++)
        {
            ValueCopy += StoreArray[j] * pow(10,(4-j));
        }
        if(ValueCopy > 65534)
            ValueCopy = 65534;
        *Value = ValueCopy;
    }
    return;
}

bool ChooseYesNo(char *TitleChoice)
{
    char *ChoiceStr[2] = {"Si", "No"};
    uint8_t ChoiceNum = 0, FirstListItem = 0;
    bool ExitChoice = false, Choice = false;
    while(!ExitChoice)
    {
        CheckOperation();
        DrawListLoop(TitleChoice, ChoiceStr, ChoiceNum, FirstListItem, 2, MAX_SETUP_MENU_LINES);
        switch(LastButtonPressed)
        {
          case BUTTON_UP:
            if(ChoiceNum > 0)
                ChoiceNum--;
            else
                ChoiceNum = 1;
            break;
          case BUTTON_DOWN:
            if(ChoiceNum < 1)
                ChoiceNum++;
            else
                ChoiceNum = 0;
            break;
          case BUTTON_LEFT:
            ExitChoice = true;
            break;
          case BUTTON_RIGHT:           
            ExitChoice = true;
            if(ChoiceNum == 0)
                Choice = true;
            else
                Choice = false; 
            break;
          case BUTTON_OK:
            break;
          default:
            break;
        }        
        if(ChoiceNum <= (MAX_SETUP_MENU_LINES - 1))
        {
            FirstListItem = 0;  
        }
        else
        {
            FirstListItem = ChoiceNum - (MAX_SETUP_MENU_LINES - 1);
        } 
        osDelay(WHILE_LOOP_DELAY);
    }
    return Choice;
}

uint16_t ChangeValue(uint16_t ParamValue, uint8_t ParamItem)
{
    uint8_t ValueArray[5] = {0}, BoxPos = 0;
    uint16_t FinalValue = ParamValue;
    bool ExitChangeValue = false, ChangedValue = false;
    NumbersOperation(&FinalValue, ValueArray, DECOMPOSE);
    
    while(!ExitChangeValue)
    {
        CheckOperation();
        DrawChangeValueLoop(BoxPos, ValueArray, ParametersMenu[ParamItem].ItemTitle);
        switch(LastButtonPressed)
        {
          case BUTTON_UP:
            if(ValueArray[BoxPos] > 0)
                ValueArray[BoxPos]--;
            else
                ValueArray[BoxPos] = 9;    
            break;
          case BUTTON_DOWN:
            if(ValueArray[BoxPos] < 9)
                ValueArray[BoxPos]++;
            else
                ValueArray[BoxPos] = 0;   
            break;
          case BUTTON_LEFT:
            ExitChangeValue = true;
            break;
          case BUTTON_RIGHT:           
            if(BoxPos < 4)
                BoxPos++;
            else
                BoxPos = 0;
            break;
          case BUTTON_OK:
            ChangedValue = true;
            ExitChangeValue = true;
            break;
          default:
            break;
        }
        
        osDelay(WHILE_LOOP_DELAY);
    }
    if(ChangedValue)
    {
        NumbersOperation(&FinalValue, ValueArray, COMPOSE);
    }
    return FinalValue;
}

void ChangeAlarmThrs(uint8_t AlarmItem)
{
    uint8_t BoxPos = 0, NumbOfThr = 0;
    float OverUnderThr[2];
    bool ThrSetted = false, ExitFromAll = false;
    
    for(NumbOfThr = 0; NumbOfThr < 2; NumbOfThr++)
    {
        while(!ThrSetted || !ExitFromAll)
        {
            CheckOperation();
            switch(LastButtonPressed)
            {
              case BUTTON_UP:
                break;
              case BUTTON_DOWN:  
                break;
              case BUTTON_LEFT:
                ExitFromAll = true;
                break;
              case BUTTON_RIGHT:           
                if(BoxPos < 4)
                    BoxPos++;
                else
                    BoxPos = 0;
                break;
              case BUTTON_OK:
                break;
              default:
                break;
            }
            
            osDelay(WHILE_LOOP_DELAY);
        }
        if(ExitFromAll)
            break;
    }
    
}