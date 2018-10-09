#ifndef PARAMS_FUNCTIONS_H
#define PARAMS_FUNCTIONS_H
#include <stdbool.h>
#include <stdint.h>

typedef struct 
{
    bool EnableMeasure;
    uint16_t ADCOffset;
}PARAMETERS_S;

typedef enum
{
    CONFIRM_TYPE = 0,
    INT_VALUE_TYPE,
    FLOAT_VALUE_TYPE,
    MAX_TYPE
}PARAMETER_TYPES;

typedef struct 
{
     char *ItemTitle;
     uint8_t Type;
     void *ParamValue;
}PARAMETER_ITEM;

typedef enum
{
    MEASURE_ENABLE = 0,
    ADC_OFFSET,
    MAX_PARAMETER_ITEM
}PARAMETER_ITEMS;

typedef enum
{
    CURRENT_THRS_ITEM = 0,
    POWER_THRS_ITEM,
    ENERGY_THRS_ITEM,
    MAX_ALARM_SETUP_ITEM
}ALARM_THRS_ITEM;

extern PARAMETERS_S GeneralParams;



bool ChooseYesNo(char *TitleChoice);
uint16_t ChangeValue(uint16_t ParamValue, uint8_t ParamItem);
void ChangeAlarmThrs(uint8_t AlarmItem);

#endif
