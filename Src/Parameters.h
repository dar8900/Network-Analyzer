#ifndef PARAMS_FUNCTIONS_H
#define PARAMS_FUNCTIONS_H
#include <stdbool.h>
#include <stdint.h>


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
uint16_t ChangeValue(uint16_t ParamValue);

#endif
