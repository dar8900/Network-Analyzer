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
    uint32_t OldSavedValue = 0;
    if(EepFlag.SaveEnableMeasure)
    {
        EE_ReadInt(ENABLE_MEASURE_ADDR, &OldSavedValue);
        if(OldSavedValue != GeneralParams.EnableMeasure)
        {
            EE_WriteInt(ENABLE_MEASURE_ADDR, GeneralParams.EnableMeasure);
            EeepromSavedValue.NumeroScritture++;
            EE_WriteInt(NUMBER_OF_WRITES_ADDR, EeepromSavedValue.NumeroScritture);
        }
        EepFlag.SaveEnableMeasure = false;
    }
    if(EepFlag.SaveAdcOffset)
    {
        EE_ReadInt(ADC_OFFSET_ADDR, &OldSavedValue);
        if(OldSavedValue != GeneralParams.ADCOffset)
        {
            EE_WriteInt(ADC_OFFSET_ADDR, GeneralParams.ADCOffset);
            EeepromSavedValue.NumeroScritture++;
            EE_WriteInt(NUMBER_OF_WRITES_ADDR, EeepromSavedValue.NumeroScritture);
        }
        EepFlag.SaveAdcOffset = false;
    }
    return;
}

static void ReadParameters(uint8_t ParamItem)
{
    uint32_t SavedValue = 0;
    switch(ParamItem)
    {
      case MEASURE_ENABLE:
        EE_ReadInt(ENABLE_MEASURE_ADDR, &SavedValue);
        GeneralParams.EnableMeasure = (bool)SavedValue;
        break;
      case ADC_OFFSET:
        EE_ReadInt(ADC_OFFSET_ADDR, &SavedValue);
        GeneralParams.ADCOffset = (uint16_t)SavedValue;
        break;
      default:
        break;   
    }
    return;
}

//static void WriteThr()
//{
//    if(EepFlag.SaveThresholds[CURRENT_ALARM])
//    {
//        EE_Write(SOGLIE_ALLARMI_IO_ADDR, AlarmsParameters[CURRENT_ALARM].OverThreshold);
//        EE_Write(SOGLIE_ALLARMI_IU_ADDR, AlarmsParameters[CURRENT_ALARM].UnderThreshold);
//        EeepromSavedValue.NumeroScritture++;
//        EE_Write(NUMBER_OF_WRITES_ADDR, EeepromSavedValue.NumeroScritture);
//        EepFlag.SaveThresholds[CURRENT_ALARM] = false;
//    }
//    if(EepFlag.SaveThresholds[POWER_ALARM])
//    {
//        EE_Write(SOGLIE_ALLARMI_IO_ADDR, AlarmsParameters[POWER_ALARM].OverThreshold);
//        EE_Write(SOGLIE_ALLARMI_IU_ADDR, AlarmsParameters[POWER_ALARM].UnderThreshold);
//        EeepromSavedValue.NumeroScritture++;
//        EE_Write(NUMBER_OF_WRITES_ADDR, EeepromSavedValue.NumeroScritture);
//        EepFlag.SaveThresholds[POWER_ALARM] = false;
//    }
//    if(EepFlag.SaveThresholds[ENERGY_ALARM])
//    {
//        EE_Write(SOGLIE_ALLARMI_IO_ADDR, AlarmsParameters[ENERGY_ALARM].OverThreshold);
//        EeepromSavedValue.NumeroScritture++;
//        EE_Write(NUMBER_OF_WRITES_ADDR, EeepromSavedValue.NumeroScritture);
//        EepFlag.SaveThresholds[ENERGY_ALARM] = false;
//    }
//    return;
//}

//static void WriteEnergy()
//{
//    if(EepFlag.SaveEnergy)
//    {
//        EE_Write(ENERGIA_ADDR, GeneralMeasures.MeanEnergy);
//        EepFlag.SaveEnergy = false;
//    }
//    return;
//}


/* TaskEeprom function */
void TaskEeprom(void const * argument)
{
    for(uint8_t i = 0; i < MAX_PARAMETER_ITEM; i++)
    {
        ReadParameters(i);
    }
    EE_ReadInt(NUMBER_OF_WRITES_ADDR, &EeepromSavedValue.NumeroScritture);
    
    /* Infinite loop */
    for(;;)
    {
        WriteParameters();
        osDelay(250);
    }
    
}

#endif