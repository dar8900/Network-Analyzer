#include "main.h"
#include "Parameters.h"
#include "Oled.h"
#include "TaskKeyboard.h"
#include "Menus.h"
#include "TaskOled.h"
#include "AlarmMachine.h"
#include "TaskEeprom.h"
#include <math.h>

#define  ALARM_FACTOR_UNIT_BOXPOS   8


// Parametri 
PARAMETERS_S GeneralParams;



enum
{
    COMPOSE = 0,
    DECOMPOSE
};

PARAMETER_ITEM ParametersMenu[MAX_PARAMETER_ITEM] = 
{
    {"Abilitare misura"  , CONFIRM_TYPE    ,  &GeneralParams.EnableMeasure         },
    {"Tensione di misura", INT_VALUE_TYPE  ,  &GeneralParams.MeasureVoltage        },
    {"ADC Offset"        , INT_VALUE_TYPE  ,  &GeneralParams.ADCOffset             },
    {"Scritture in memoria", READ_ONLY_TYPE,  &EepromSavedValue[NUMBER_OF_WRITES_ADDR]    },
};

PARAMETER_ITEM AlarmThrMenu[MAX_ALARM_SETUP_ITEM] = 
{
    {"Soglie corrente "  , FLOAT_VALUE_TYPE    ,  NULL},
    {"Soglie potenza  "  , FLOAT_VALUE_TYPE    ,  NULL},
    {"Soglia energia  "  , FLOAT_VALUE_TYPE    ,  NULL},
};



FL_SCALE TabReScale[MAX_UNIT_FACTOR] = 
{
    {0.001        , 1000.0      , 'm'},
    {1.0          , 1.0         , ' '},
    {1000.0       , 0.001       , 'k'},
    {1000000.0    , 0.000001    , 'M'},
    {1000000000.0 , 0.000000001 , 'G'},

};

uint8_t SearchScaleFlRange(float Value)
{
    uint8_t Range = 0;
    for(Range = 0; Range < MAX_UNIT_FACTOR; Range++)
    {
        if(Value < TabReScale[Range].Value)
        {
            if(Range == 0)
                break;
            else
            {
                Range -= 1;
                break;
            }
        }
    }
    return Range;
}

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

static void ReWriteStr(char OldStr[], char NewStr[], uint8_t Size)
{
    uint8_t i = 0, j = 0;
    for(i = 0; i < Size; i++)
    {
        if(OldStr[i] != '0')
        {
            break;
        }
    }
    for(j = 0; j < Size - i; j++)
    {
        NewStr[j] = OldStr[i + j];
    }
}


static void FloatStr(float *Value, char StrArray[], uint8_t ArraySize, bool ToStr)
{
    float ValueCopy = *Value;
    char NewFlArray[9];
    if(ToStr)
    {
        if(ValueCopy < 0)
            ValueCopy = - ValueCopy;
        snprintf(StrArray, 9, "%08.3f", ValueCopy);
    }
    else
    {
        ReWriteStr(StrArray, NewFlArray, ArraySize);
        ValueCopy = strtof(NewFlArray, NULL);
        *Value = ValueCopy;
    }
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
        LastButtonPressed = NO_PRESS;
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
        LastButtonPressed = NO_PRESS;
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
    uint8_t BoxPos = 0, NumbOfThr = 0, MaxNumThr = 0, ScaleRange = 0, FactorUnitIndex = 0;
    float ThrValue = 0.0;
    char ValueArray[9];
    char *OverUnderThrStr[2] = {"Sovra-soglia", "Sotto-Soglia"};   
    bool ThrSetted = false, ExitFromAll = false;
  
    if(AlarmItem == ENERGY_THRS_ITEM)
        MaxNumThr = 1;
    else 
        MaxNumThr = 2;
    
    for(NumbOfThr = 0; NumbOfThr < MaxNumThr; NumbOfThr++)
    {
        if(NumbOfThr == 0)
            ThrValue = AlarmsParameters[AlarmItem].OverThreshold;
        else
            ThrValue = AlarmsParameters[AlarmItem].UnderThreshold; 
        
        ScaleRange = SearchScaleFlRange(ThrValue);
        FactorUnitIndex = ScaleRange;
        ThrValue *= TabReScale[ScaleRange].ScaleFactor;
        FloatStr(&ThrValue, ValueArray, 9, true);
        ThrSetted = false;
        
        while(!ThrSetted && !ExitFromAll)
        {
            CheckOperation();
            DrawChangeAlarmThrsLoop(BoxPos, ValueArray, OverUnderThrStr[NumbOfThr], TabReScale[FactorUnitIndex].Unit);
            switch(LastButtonPressed)
            {
              case BUTTON_UP:
                if(BoxPos < ALARM_FACTOR_UNIT_BOXPOS)
                {
                    if(ValueArray[BoxPos] > '0')
                        ValueArray[BoxPos]--;
                    else
                        ValueArray[BoxPos] = '9';    
                }
                else
                {
                    if(FactorUnitIndex > 0 )
                        FactorUnitIndex--;
                    else
                        FactorUnitIndex = MAX_UNIT_FACTOR - 1;
                }
                break;
              case BUTTON_DOWN:  
                if(BoxPos < ALARM_FACTOR_UNIT_BOXPOS)
                {
                    if(ValueArray[BoxPos] < '9')
                        ValueArray[BoxPos]++;
                    else
                        ValueArray[BoxPos] = '0'; 
                }
                else
                {
                    if(FactorUnitIndex < MAX_UNIT_FACTOR - 1)
                        FactorUnitIndex++;
                    else
                        FactorUnitIndex = 0;
                }
                break;
              case BUTTON_LEFT:
                ExitFromAll = true;
                break;
              case BUTTON_RIGHT:           
                if(BoxPos < ALARM_FACTOR_UNIT_BOXPOS)
                {
                    BoxPos++;
                    if(ValueArray[BoxPos] == '.' && (BoxPos + 1) != ALARM_FACTOR_UNIT_BOXPOS)
                        BoxPos++;
                }
                else
                    BoxPos = 0;
                break;
              case BUTTON_OK:
                FloatStr(&ThrValue, ValueArray, 9, false);
                ThrValue *= TabReScale[FactorUnitIndex].Value;
                if(NumbOfThr == 0)
                    AlarmsParameters[AlarmItem].OverThreshold = ThrValue;
                else
                    AlarmsParameters[AlarmItem].UnderThreshold = ThrValue;               
                ThrSetted = true;
                break;
              default:
                break;
            }
            LastButtonPressed = NO_PRESS;
            osDelay(WHILE_LOOP_DELAY);
        }
        if(ExitFromAll)
            break;
    }
    if(!ExitFromAll)
    {
        //Salva soglie
    }
    
}
