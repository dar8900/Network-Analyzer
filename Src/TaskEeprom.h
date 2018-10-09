#ifndef TASK_EEPROM_H
#define TASK_EEPROM_H

#include <stdint.h>
#include <stdbool.h>
#include "Eeprom.h"

#define EEPROM_VIRTUAL_ADDR_BEGIN   0   
#define MAX_EEPROM_ADDRESS        256

typedef enum
{
    ENABLE_MEASURE_ADDR    = EEPROM_VIRTUAL_ADDR_BEGIN,
    ADC_OFFSET_ADDR        = ENABLE_MEASURE_ADDR       + U_INT8_SIZE,
    SOGLIE_ALLARMI_IO_ADDR = ADC_OFFSET_ADDR           + U_INT16_SIZE,
    SOGLIE_ALLARMI_IU_ADDR = SOGLIE_ALLARMI_IO_ADDR    + U_INT32_SIZE,
    SOGLIE_ALLARMI_PO_ADDR = SOGLIE_ALLARMI_IU_ADDR    + U_INT32_SIZE,
    SOGLIE_ALLARMI_PU_ADDR = SOGLIE_ALLARMI_PO_ADDR    + U_INT32_SIZE,
    SOGLIE_ALLARMI_EO_ADDR = SOGLIE_ALLARMI_PU_ADDR    + U_INT32_SIZE,
    ENERGIA_ADDR           = SOGLIE_ALLARMI_EO_ADDR    + U_INT32_SIZE,
    NUMBER_OF_WRITES_ADDR  = MAX_EEPROM_ADDRESS        - U_INT32_SIZE,
  
}EEPROM_ADDRESS;

typedef struct
{
    uint8_t  AbilitaMisura;
    uint16_t AdcOffset;
    uint32_t SoglieAllarmi[5];
    uint32_t Energia;  
    uint32_t NumeroScritture;
}EEPROM_DATA;

typedef struct
{
    bool SaveEnableMeasure;
    bool SaveAdcOffset;
    bool SaveThresholds[3];
    bool SaveEnergy;
    bool SaveCounter;  
}FLAG_EEPROM;


void TaskEeprom(void const * argument);
#endif