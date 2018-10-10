#ifndef TASK_EEPROM_H
#define TASK_EEPROM_H

#include <stdint.h>
#include <stdbool.h>
#include "Eeprom.h"

#define EEPROM_VIRTUAL_ADDR_BEGIN   0   
#define MAX_EEPROM_ADDRESS        256



typedef struct
{
    char FlToChar[8];
    uint8_t ScaleFactor;
    float CharToFl;
}FLOAT_VALUE_EEPROM;

typedef enum
{
    ENABLE_MEASURE_EEITEM = 0,
    ADC_OFFSET_EEITEM,
    CURRENT_THRS_EEITEM,
    POWER_THRS_EEITEM,
    ENERGY_THR_EEITEM,
    ENERGY_EEITEM,
    MAX_EEITEM
}EEPROM_ITEMS;

typedef enum
{
    ENABLE_MEASURE_ADDR    = EEPROM_VIRTUAL_ADDR_BEGIN,
    ADC_OFFSET_ADDR        = ENABLE_MEASURE_ADDR       + U_INT8_SIZE,
    SOGLIE_ALLARMI_IO_ADDR = ADC_OFFSET_ADDR           + U_INT16_SIZE,
    SOGLIE_ALLARMI_IU_ADDR = SOGLIE_ALLARMI_IO_ADDR    + FLOAT_STR_SIZE,
    SOGLIE_ALLARMI_PO_ADDR = SOGLIE_ALLARMI_IU_ADDR    + FLOAT_STR_SIZE,
    SOGLIE_ALLARMI_PU_ADDR = SOGLIE_ALLARMI_PO_ADDR    + FLOAT_STR_SIZE,
    SOGLIE_ALLARMI_EO_ADDR = SOGLIE_ALLARMI_PU_ADDR    + FLOAT_STR_SIZE,
    SOGLIE_ALLARMI_EU_ADDR = SOGLIE_ALLARMI_EO_ADDR    + FLOAT_STR_SIZE,
    ENERGIA_ADDR           = SOGLIE_ALLARMI_EU_ADDR    + FLOAT_STR_SIZE,
    NUMBER_OF_WRITES_ADDR  = MAX_EEPROM_ADDRESS        - U_INT32_SIZE,
  
}EEPROM_ADDRESS;

typedef struct
{
    uint8_t  AbilitaMisura;
    uint16_t AdcOffset;
    FLOAT_VALUE_EEPROM Soglie[6];
    FLOAT_VALUE_EEPROM Energia;  
    uint32_t NumeroScritture;
}EEPROM_DATA;

enum
{
    CURRENT_THR_FLAG = 0,
    POWER_THR_FLAG,
    ENERGY_THR_FLAG
};

typedef struct
{
    bool SaveParameters;
    bool SaveThresholds[3];
    bool SaveEnergy;
    bool SaveCounter;  
}FLAG_EEPROM;

extern FLAG_EEPROM EepFlag;
extern EEPROM_DATA EepromSavedValue;

void TaskEeprom(void const * argument);
#endif