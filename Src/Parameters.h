#ifndef PARAMS_FUNCTIONS_H
#define PARAMS_FUNCTIONS_H
#include <stdbool.h>
#include <stdint.h>

typedef struct 
{
    bool     EnableMeasure;
    uint16_t ADCOffset;
    uint16_t MeasureVoltage;
    uint8_t  LogEnergyPeriod;
    bool     EnableScreenSaver;
    uint8_t  ScreenSaverType;
}PARAMETERS_S;

typedef enum
{
    CONFIRM_TYPE = 0,
    INT_VALUE_TYPE,
    ENUM_VALUE_TYPE,
    FLOAT_VALUE_TYPE,
    READ_ONLY_TYPE,
    MAX_TYPE
}PARAMETER_TYPES;

typedef struct
{
    uint8_t EnumValue;
    char    *EnumTitle;
}ENUM_VALUE_ITEM;


typedef struct 
{
     char *ItemTitle;
     uint8_t Type;
     void *ParamValue;
     void *EnumValuePtr;
     uint8_t  MaxEnumItems;
}PARAMETER_ITEM;

typedef enum
{
    MEASURE_ENABLE = 0,
    MEASURE_VOLTAGE,
    LOG_ENERGY_PERIOD,
    ADC_OFFSET,
    ENABLE_SCREENSAVER,
    SCREENSAVER_TYPE,
    NUMBER_MEMORY_WRITES,
    MAX_PARAMETER_ITEM
}PARAMETER_ITEMS;

typedef enum
{
    CURRENT_THRS_ITEM = 0,
    POWER_THRS_ITEM,
    ENERGY_THRS_ITEM,
    MAX_ALARM_SETUP_ITEM
}ALARM_THRS_ITEM;

typedef struct
{
    float Value;
    float ScaleFactor;
    char  Unit;
}FL_SCALE;





enum
{
    FACTOR_MILLI = 0,
    FACTOR_NULL,
    FACTOR_KILO,
    FACTOR_MEGA,
    FACTOR_GIGA,
    MAX_UNIT_FACTOR
};


extern PARAMETERS_S GeneralParams;


uint8_t SearchScaleFlRange(float Value);
bool ChooseYesNo(char *TitleChoice);
uint16_t ChangeValue(uint16_t ParamValue, uint8_t ParamItem);
void ChangeAlarmThrs(uint8_t AlarmItem);
int8_t ChangeEnumValue(uint8_t ParamItem);
#endif
