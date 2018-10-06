#ifndef PARAMS_FUNCTIONS_H
#define PARAMS_FUNCTIONS_H
#include <stdbool.h>
#include <stdint.h>

typedef struct 
{
    bool EnableMeasure;
    uint16_t ADCOffset;
}PARAMETERS_S;


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



bool ChooseYesNo(char *TitleChoice);
uint16_t ChangeValue(uint16_t ParamValue, uint8_t ParamItem);

#endif
