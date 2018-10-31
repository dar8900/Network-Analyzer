#ifndef PARAMS_FUNCTIONS_H
#define PARAMS_FUNCTIONS_H
#include <stdbool.h>
#include <stdint.h>

typedef struct 
{
    bool     EnableMeasure;
    bool     EnableSimulation;
    uint32_t ADCOffset;
    uint16_t MeasureVoltage;
    bool     EnableLog;
    uint8_t  LogMeasurePeriod;
    bool     EnableScreenSaver;
    uint8_t  ScreenSaverType;
    uint16_t ScreenSaverTimer;
    float    SimulationCurrent;
    uint8_t  Frequency;
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
}ENUM_VALUE_ITEM;


typedef struct 
{
     uint8_t Type;
     void *ParamValue;
     void *EnumValuePtr;
     uint8_t  MaxEnumItems;
}PARAMETER_ITEM;

typedef enum
{
    MEASURE_ENABLE = 0,
    ENABLE_SIMULATION,
    CURRENT_SIM,
    FREQUENCY_SIM,
    MEASURE_VOLTAGE,
    ENABLE_LOG,
    LOG_MEASURE_PERIOD,
    ADC_OFFSET,
    ENABLE_SCREENSAVER,
    SCREENSAVER_TYPE,
    SCREENSAVER_TIMER,
    LED_CONFIGURATION,
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

typedef enum
{
    ANALOG_DATE = 0,
    DIGITAL_DATE,
    ANALOG_ONLY,
    MAX_SCREENSAVER_TYPE
}SCREENSAVER_TYPE_ENUM;



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
bool ChooseYesNo(const char *TitleChoice);
uint16_t ChangeValue(uint16_t ParamValue, uint8_t ParamItem);
float ChangeValueFl(float ParamValue, uint8_t ParamItem);
void ChangeAlarmThrs(uint8_t AlarmItem);
int8_t ChangeEnumValue(uint8_t ParamItem);
#endif
