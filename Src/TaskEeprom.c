#include "main.h"
#include "TaskEeprom.h"
#include "TaskMeasure.h"
#include "Parameters.h"
#include "AlarmMachine.h"

#ifdef ENABLE_EEPROM


EEPROM_DATA EeepromSavedValue;

FLAG_EEPROM EepFlag;

static void WriteParameters()
{
    if(EepFlag.SaveEnableMeasure)
    {
        EE_Write(ENABLE_MEASURE_ADDR, GeneralParams.EnableMeasure);
        EepFlag.SaveEnableMeasure = false;
    }
    if(EepFlag.SaveAdcOffset)
    {
        EE_Write(ADC_OFFSET_ADDR, GeneralParams.ADCOffset);
        EepFlag.SaveAdcOffset = false;
    }
    return;
}

static void WriteThr()
{
    if(EepFlag.SaveThresholds[CURRENT_ALARM])
    {
        EE_Write(SOGLIE_ALLARMI_IO_ADDR, AlarmsParameters[CURRENT_ALARM].OverThreshold);
        EE_Write(SOGLIE_ALLARMI_IU_ADDR, AlarmsParameters[CURRENT_ALARM].UnderThreshold);
        EeepromSavedValue.NumeroScritture++;
        EE_Write(NUMBER_OF_WRITES_ADDR, EeepromSavedValue.NumeroScritture);
        EepFlag.SaveThresholds[CURRENT_ALARM] = false;
    }
    if(EepFlag.SaveThresholds[POWER_ALARM])
    {
        EE_Write(SOGLIE_ALLARMI_IO_ADDR, AlarmsParameters[POWER_ALARM].OverThreshold);
        EE_Write(SOGLIE_ALLARMI_IU_ADDR, AlarmsParameters[POWER_ALARM].UnderThreshold);
        EeepromSavedValue.NumeroScritture++;
        EE_Write(NUMBER_OF_WRITES_ADDR, EeepromSavedValue.NumeroScritture);
        EepFlag.SaveThresholds[POWER_ALARM] = false;
    }
    if(EepFlag.SaveThresholds[ENERGY_ALARM])
    {
        EE_Write(SOGLIE_ALLARMI_IO_ADDR, AlarmsParameters[ENERGY_ALARM].OverThreshold);
        EeepromSavedValue.NumeroScritture++;
        EE_Write(NUMBER_OF_WRITES_ADDR, EeepromSavedValue.NumeroScritture);
        EepFlag.SaveThresholds[ENERGY_ALARM] = false;
    }
    return;
}

static void WriteEnergy()
{
    if(EepFlag.SaveEnergy)
    {
        EE_Write(ENERGIA_ADDR, GeneralMeasures.MeanEnergy);
        EepFlag.SaveEnergy = false;
    }
    return;
}


/* TaskEeprom function */
void TaskEeprom(void const * argument)
{
    uint32_t ReadedValue = 10;
    EE_Read(0, &ReadedValue);
    ReadedValue = 11;
    
    /* Infinite loop */
    for(;;)
    {
        osDelay(1000);
    }
    
}

#endif