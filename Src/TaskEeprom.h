#ifndef TASK_EEPROM_H
#define TASK_EEPROM_H

#include <stdint.h>
#include <stdbool.h>
#include "Eeprom.h"

#define EEPROM_VIRTUAL_ADDR_BEGIN   0   
#define MAX_EEPROM_ADDRESS        255

typedef uint32_t EEPROM_DATA;

typedef enum
{
    ENABLE_MEASURE_ADDR    = EEPROM_VIRTUAL_ADDR_BEGIN,
    VOLTAGE_MEASURE_ADDR,
    ADC_OFFSET_ADDR,   
    SOGLIE_ALLARMI_IO_ADDR, 
    SOGLIE_ALLARMI_IU_ADDR =  SOGLIE_ALLARMI_IO_ADDR + STR_SIZE,
    SOGLIE_ALLARMI_PO_ADDR =  SOGLIE_ALLARMI_IU_ADDR + STR_SIZE,
    SOGLIE_ALLARMI_PU_ADDR =  SOGLIE_ALLARMI_PO_ADDR + STR_SIZE,
    SOGLIE_ALLARMI_EO_ADDR =  SOGLIE_ALLARMI_PU_ADDR + STR_SIZE,
    SOGLIE_ALLARMI_EU_ADDR =  SOGLIE_ALLARMI_EO_ADDR + STR_SIZE,
    ENERGIA_ADDR           =  SOGLIE_ALLARMI_EU_ADDR + STR_SIZE,
    LOG_ENERGY_ADDR = 66,
    ENABLE_SCREENSAVER_ADDR,
    SCREENSAVER_TYPE_ADDR,
    NONE4,
    NONE5,
    NONE6,
    NONE7,
    NONE8,
    NONE9,
    NONE10,
    NONE11,
    NONE12,
    NONE13,
    NONE14,
    NONE15,
    NONE16,
    NONE17,
    NONE18,
    NONE19,
    NONE20,
    NONE21,
    NONE22,
    NONE23,
    NONE24,
    NONE25,
    NONE26,
    NONE27,
    NONE28,
    NONE29,
    NONE30,
    NONE31,
    NONE32,
    NONE33,
    NONE34,
    NONE35,
    NONE36,
    NONE37,
    NONE38,
    NONE39,
    NONE40,
    NONE41,
    NONE42,
    NONE43,
    NONE44,
    NONE45,
    NONE46,
    NONE47,
    NONE48,
    NONE49,
    NONE50,
    NONE51,
    NONE52,
    NONE53,
    NONE54,
    NONE55,
    NONE56,
    NONE57,
    NONE58,
    NONE59,
    NONE60,
    NONE61,
    NONE62,
    NONE63,
    NONE64,
    NONE65,
    NONE66,
    NONE67,
    NONE68,
    NONE69,
    NONE70,
    NONE71,
    NONE72,
    NONE73,
    NONE74,
    NONE75,
    NONE76,
    NONE77,
    NONE78,
    NONE79,
    NONE80,
    NONE81,
    NONE82,
    NONE83,
    NONE84,
    NONE85,
    NONE86,
    NONE87,
    NONE88,
    NONE89,
    NONE90,
    NONE91,
    NONE92,
    NONE93,
    NONE94,
    NONE95,
    NONE96,
    NONE97,
    NONE98,
    NONE99,
    NONE100,
    NONE101,
    NONE102,
    NONE103,
    NONE104,
    NONE105,
    NONE106,
    NONE107,
    NONE108,
    NONE109,
    NONE110,
    NONE111,
    NONE112,
    NONE113,
    NONE114,
    NONE115,
    NONE116,
    NONE117,
    NONE118,
    NONE119,
    NONE120,
    NONE121,
    NONE122,
    NONE123,
    NONE124,
    NONE125,
    NONE126,
    NONE127,
    NONE128,
    NONE129,
    NONE130,
    NONE131,
    NONE132,
    NONE133,
    NONE134,
    NONE135,
    NONE136,
    NONE137,
    NONE138,
    NONE139,
    NONE140,
    NONE141,
    NONE142,
    NONE143,
    NONE144,
    NONE145,
    NONE146,
    NONE147,
    NONE148,
    NONE149,
    NONE150,
    NONE151,
    NONE152,
    NONE153,
    NONE154,
    NONE155,
    NONE156,
    NONE157,
    NONE158,
    NONE159,
    NONE160,
    NONE161,
    NONE162,
    NONE163,
    NONE164,
    NONE165,
    NONE166,
    NONE167,
    NONE168,
    NONE169,
    NONE170,
    NONE171,
    NONE172,
    NONE173,
    NONE174,
    NONE175,
    NONE176,
    NONE177,
    NONE178,
    NONE179,
    NONE180,
    NONE181,
    NONE182,
    NONE183,
    NONE184,
    NONE185,
    NONE186,
    NONE187,
    NONE188,
    NONE189,
    NONE190,
    NONE191,
    NONE192,
    NONE193,
    NONE194,
    NONE195,
    NONE196,
    NONE197,
    NONE198,
    
    NUMBER_OF_WRITES_ADDR  =  MAX_EEPROM_ADDRESS     - U_INT32_SIZE, 
}EEPROM_ADDRESS;



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
    bool EraseAll;
}FLAG_EEPROM;

extern FLAG_EEPROM EepFlag;
extern EEPROM_DATA EepromSavedValue[];

void TaskEeprom(void const * argument);
#endif