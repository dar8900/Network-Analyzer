#include "main.h"
#include "TaskEeprom.h"
#include "TaskMeasure.h"
#include "Parameters.h"
#include "AlarmMachine.h"

#ifdef ENABLE_EEPROM

extern FL_SCALE TabReScale[MAX_UNIT_FACTOR];


EEPROM_DATA EepromSavedValue;

FLAG_EEPROM EepFlag;


static void CopyCharArray(char OrigArray[], char DestArray[])
{
    uint8_t i, Size = 9;
    for(i = 0; i < Size; i++)
    {
        DestArray[i] = OrigArray[i];
    }
    return;
}

//##########################################################################################################

//                                      NUMERO SCRITTURE       

//##########################################################################################################

static void WriteNumbOfWrites()
{
    uint32_t OldSavedValue = 0;
    if(EepromSavedValue.NumeroScritture == UINT32_MAX)
    {
        EepromSavedValue.NumeroScritture = 0;
        EepFlag.SaveCounter = true;
    }
    if(EepFlag.SaveCounter)
    {
        EE_ReadInt(NUMBER_OF_WRITES_ADDR, &OldSavedValue);
        if(OldSavedValue != EepromSavedValue.NumeroScritture)
        {
            EE_WriteInt(NUMBER_OF_WRITES_ADDR, EepromSavedValue.NumeroScritture);    
        }
        EepFlag.SaveCounter = false;
    }
}
static void ReadNumbOfWrites()
{
    EE_ReadInt(NUMBER_OF_WRITES_ADDR, &EepromSavedValue.NumeroScritture);
}

//##########################################################################################################

//                                      PARAMETRI        

//##########################################################################################################

static void WriteParameters()
{
    uint32_t OldSavedValue = 0;
    if(EepFlag.SaveParameters)
    {
        EE_ReadInt(ENABLE_MEASURE_ADDR, &OldSavedValue);
        if(OldSavedValue != GeneralParams.EnableMeasure)
        {
            EE_WriteInt(ENABLE_MEASURE_ADDR, GeneralParams.EnableMeasure);
            EepromSavedValue.NumeroScritture++;
            EepFlag.SaveCounter = true;
        }    
        EE_ReadInt(ADC_OFFSET_ADDR, &OldSavedValue);
        if(OldSavedValue != GeneralParams.ADCOffset)
        {
            EE_WriteInt(ADC_OFFSET_ADDR, GeneralParams.ADCOffset);
            EepromSavedValue.NumeroScritture++;
            EepFlag.SaveCounter = true;
        }
        EepFlag.SaveParameters = false;
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

//##########################################################################################################

//                                      SOGLIE      

//##########################################################################################################



static void ThrsToChar(float ThrOver, float ThrUnder, char ThrOverChar[], char ThrUnderChar[], uint8_t FactorOver, uint8_t FactorUnder)
{
    char TempChars[9];
    uint8_t FactorIndex = 0;
    
    FactorIndex = SearchScaleFlRange(ThrOver);
    ThrOver *= TabReScale[FactorIndex].ScaleFactor;
    snprintf(TempChars, 9, "%08.3f", ThrOver);
    CopyCharArray(TempChars, ThrOverChar);
    FactorOver = FactorIndex;
    
    FactorIndex = SearchScaleFlRange(ThrUnder);
    ThrUnder *= TabReScale[FactorIndex].ScaleFactor;
    snprintf(TempChars, 9, "%08.3f", ThrUnder);
    CopyCharArray(TempChars, ThrUnderChar);
    FactorUnder = FactorIndex;
    
    return;
}


static void WriteThr()
{
    uint8_t Index = 0;
    if(EepFlag.SaveThresholds[CURRENT_THR_FLAG])
    {
        ThrsToChar(AlarmsParameters[CURRENT_ALARM].OverThreshold, AlarmsParameters[CURRENT_ALARM].UnderThreshold, EepromSavedValue.Soglie[0].FlToChar, 
                   EepromSavedValue.Soglie[1].FlToChar, EepromSavedValue.Soglie[0].ScaleFactor, EepromSavedValue.Soglie[1].ScaleFactor);
        for(Index = 0; Index < (FLOAT_STR_SIZE - 1); Index++)
        {
            EE_WriteInt(SOGLIE_ALLARMI_IO_ADDR + Index, (uint32_t)EepromSavedValue.Soglie[0].FlToChar[Index]);
            EE_WriteInt(SOGLIE_ALLARMI_IU_ADDR + Index, (uint32_t)EepromSavedValue.Soglie[1].FlToChar[Index]);
        }  
        Index++;
        EE_WriteInt(SOGLIE_ALLARMI_IO_ADDR + Index, (uint32_t)EepromSavedValue.Soglie[0].ScaleFactor);
        EE_WriteInt(SOGLIE_ALLARMI_IU_ADDR + Index, (uint32_t)EepromSavedValue.Soglie[1].ScaleFactor);
        EepFlag.SaveThresholds[CURRENT_THR_FLAG] = false;
        EepromSavedValue.NumeroScritture += 4;
        EepFlag.SaveCounter = true;
    }
    if(EepFlag.SaveThresholds[POWER_THR_FLAG])
    {
        ThrsToChar(AlarmsParameters[POWER_ALARM].OverThreshold, AlarmsParameters[POWER_ALARM].UnderThreshold, EepromSavedValue.Soglie[2].FlToChar, 
                   EepromSavedValue.Soglie[3].FlToChar, EepromSavedValue.Soglie[2].ScaleFactor, EepromSavedValue.Soglie[3].ScaleFactor);
        for(Index = 0; Index < (FLOAT_STR_SIZE - 1); Index++)
        {
            EE_WriteInt(SOGLIE_ALLARMI_PO_ADDR + Index, (uint32_t)EepromSavedValue.Soglie[2].FlToChar[Index]);
            EE_WriteInt(SOGLIE_ALLARMI_PU_ADDR + Index, (uint32_t)EepromSavedValue.Soglie[3].FlToChar[Index]);
        }  
        Index++;
        EE_WriteInt(SOGLIE_ALLARMI_PO_ADDR + Index, (uint32_t)EepromSavedValue.Soglie[2].ScaleFactor);
        EE_WriteInt(SOGLIE_ALLARMI_PU_ADDR + Index, (uint32_t)EepromSavedValue.Soglie[3].ScaleFactor);
        EepFlag.SaveThresholds[POWER_THR_FLAG] = false;
        EepromSavedValue.NumeroScritture += 4;
        EepFlag.SaveCounter = true;
    }
    if(EepFlag.SaveThresholds[ENERGY_THR_FLAG])
    {
        ThrsToChar(AlarmsParameters[ENERGY_ALARM].OverThreshold, AlarmsParameters[ENERGY_ALARM].UnderThreshold, EepromSavedValue.Soglie[4].FlToChar, 
                   EepromSavedValue.Soglie[5].FlToChar, EepromSavedValue.Soglie[4].ScaleFactor, EepromSavedValue.Soglie[5].ScaleFactor);
        for(Index = 0; Index < (FLOAT_STR_SIZE - 1); Index++)
        {
            EE_WriteInt(SOGLIE_ALLARMI_EO_ADDR + Index, (uint32_t)EepromSavedValue.Soglie[4].FlToChar[Index]);
            EE_WriteInt(SOGLIE_ALLARMI_EU_ADDR + Index, (uint32_t)EepromSavedValue.Soglie[5].FlToChar[Index]);
        }  
        Index++;
        EE_WriteInt(SOGLIE_ALLARMI_EO_ADDR + Index, (uint32_t)EepromSavedValue.Soglie[4].ScaleFactor);
        EE_WriteInt(SOGLIE_ALLARMI_EU_ADDR + Index, (uint32_t)EepromSavedValue.Soglie[5].ScaleFactor);
        EepFlag.SaveThresholds[ENERGY_THR_FLAG] = false;
        EepromSavedValue.NumeroScritture += 4;
        EepFlag.SaveCounter = true;
    }
    return;
}

static void ReadCurrentThr()
{
    float Value = 0.0;
    uint32_t CharValue[9][2];
    uint8_t Index = 0;
    for(Index = 0; Index < FLOAT_STR_SIZE; Index++)
    {
        EE_ReadInt(SOGLIE_ALLARMI_IO_ADDR + Index, &CharValue[Index][0]);
        EE_ReadInt(SOGLIE_ALLARMI_IU_ADDR + Index, &CharValue[Index][1]);
        if(Index < (FLOAT_STR_SIZE - 1))
        {
            EepromSavedValue.Soglie[0].FlToChar[Index] = (char)CharValue[Index][0];
            EepromSavedValue.Soglie[1].FlToChar[Index] = (char)CharValue[Index][1];
        }
        else
        {
            EepromSavedValue.Soglie[0].ScaleFactor = CharValue[Index][0];
            EepromSavedValue.Soglie[1].ScaleFactor = CharValue[Index][1];
        }
    }
    Value = strtof(EepromSavedValue.Soglie[0].FlToChar, NULL);
    Value /= TabReScale[EepromSavedValue.Soglie[0].ScaleFactor].ScaleFactor;
    AlarmsParameters[CURRENT_ALARM].OverThreshold = Value;
    
    Value = strtof(EepromSavedValue.Soglie[1].FlToChar, NULL);
    Value /= TabReScale[EepromSavedValue.Soglie[1].ScaleFactor].ScaleFactor;
    AlarmsParameters[CURRENT_ALARM].UnderThreshold = Value;
}

static void ReadPowerThr()
{
    float Value = 0.0;
    uint32_t CharValue[9][2];
    uint8_t Index = 0;
    for(Index = 0; Index < FLOAT_STR_SIZE; Index++)
    {
        EE_ReadInt(SOGLIE_ALLARMI_PO_ADDR + Index, &CharValue[Index][0]);
        EE_ReadInt(SOGLIE_ALLARMI_PU_ADDR + Index, &CharValue[Index][1]);
        if(Index < (FLOAT_STR_SIZE - 1))
        {
            EepromSavedValue.Soglie[2].FlToChar[Index] = (char)CharValue[Index][0];
            EepromSavedValue.Soglie[3].FlToChar[Index] = (char)CharValue[Index][1];
        }
        else
        {
            EepromSavedValue.Soglie[2].ScaleFactor = CharValue[Index][0];
            EepromSavedValue.Soglie[3].ScaleFactor = CharValue[Index][1];
        }
    }
    Value = strtof(EepromSavedValue.Soglie[2].FlToChar, NULL);
    Value /= TabReScale[EepromSavedValue.Soglie[2].ScaleFactor].ScaleFactor;
    AlarmsParameters[POWER_ALARM].OverThreshold = Value;
    
    Value = strtof(EepromSavedValue.Soglie[3].FlToChar, NULL);
    Value /= TabReScale[EepromSavedValue.Soglie[3].ScaleFactor].ScaleFactor;
    AlarmsParameters[POWER_ALARM].UnderThreshold = Value;
}

static void ReadEnergyThr()
{
    float Value = 0.0;
    uint32_t CharValue[9][2];
    uint8_t Index = 0;
    for(Index = 0; Index < FLOAT_STR_SIZE; Index++)
    {
        EE_ReadInt(SOGLIE_ALLARMI_EO_ADDR + Index, &CharValue[Index][0]);
        EE_ReadInt(SOGLIE_ALLARMI_EU_ADDR + Index, &CharValue[Index][1]);
        if(Index < (FLOAT_STR_SIZE - 1))
        {
            EepromSavedValue.Soglie[4].FlToChar[Index] = (char)CharValue[Index][0];
            EepromSavedValue.Soglie[5].FlToChar[Index] = (char)CharValue[Index][1];
        }
        else
        {
            EepromSavedValue.Soglie[4].ScaleFactor = CharValue[Index][0];
            EepromSavedValue.Soglie[5].ScaleFactor = CharValue[Index][1];
        }
    }
    Value = strtof(EepromSavedValue.Soglie[4].FlToChar, NULL);
    Value /= TabReScale[EepromSavedValue.Soglie[4].ScaleFactor].ScaleFactor;
    AlarmsParameters[ENERGY_ALARM].OverThreshold = Value;
    
    Value = strtof(EepromSavedValue.Soglie[5].FlToChar, NULL);
    Value /= TabReScale[EepromSavedValue.Soglie[5].ScaleFactor].ScaleFactor;
    AlarmsParameters[ENERGY_ALARM].UnderThreshold = Value;
}

//##########################################################################################################

//                                      ENERGIA        

//##########################################################################################################

static void EnergyToChar(float Energy, char EnergyChar[], char FactorOver)
{
    char TempChars[9];
    uint8_t FactorIndex = 0;
    
    FactorIndex = SearchScaleFlRange(Energy);
    Energy *= TabReScale[FactorIndex].ScaleFactor;
    snprintf(TempChars, 9, "%08.3f", Energy);
    CopyCharArray(TempChars, EnergyChar);
    FactorOver = TabReScale[FactorIndex].Unit;
    
    return;
}

static void WriteEnergy()
{
    uint8_t Index = 0;
    if(EepFlag.SaveEnergy)
    {
        EnergyToChar(GeneralMeasures.MeanEnergy, EepromSavedValue.Energia.FlToChar, EepromSavedValue.Energia.ScaleFactor);
        for(Index = 0; Index < (FLOAT_STR_SIZE - 1); Index++)
        {
            EE_WriteInt(ENERGIA_ADDR + Index, (uint32_t)EepromSavedValue.Energia.FlToChar[Index]);
        }  
        Index++;
        EE_WriteInt(ENERGIA_ADDR + Index, (uint32_t)EepromSavedValue.Energia.ScaleFactor);
        EepFlag.SaveEnergy = false;        
        EepromSavedValue.NumeroScritture += 2;
        EepFlag.SaveCounter = true;
    }
    return;
}

static void ReadEnergy()
{
    uint8_t Index = 0;
    uint32_t CharValue[9];
    float Value = 0.0;
    
    for(Index = 0; Index < FLOAT_STR_SIZE; Index++)
    {
        EE_ReadInt(ENERGIA_ADDR + Index, &CharValue[Index]);
        if(Index < (FLOAT_STR_SIZE - 1))
        {
            EepromSavedValue.Energia.FlToChar[Index] = (char)CharValue[Index];
        }
        else
        {
            EepromSavedValue.Energia.ScaleFactor = CharValue[Index];
        }
    }
    Value = strtof(EepromSavedValue.Energia.FlToChar, NULL);
    Value /= TabReScale[EepromSavedValue.Energia.ScaleFactor].ScaleFactor;
    GeneralMeasures.MeanEnergy = Value;
}




//##########################################################################################################

//                                      TASK       

//##########################################################################################################

static void TransferToRam()
{
    for(uint8_t i = 0; i < MAX_PARAMETER_ITEM; i++)
    {
        ReadParameters(i);
    }
    ReadNumbOfWrites(); 
    ReadCurrentThr();
    ReadPowerThr();
    ReadEnergyThr();
    ReadEnergy();
}



/* TaskEeprom function */
void TaskEeprom(void const * argument)
{
    TransferToRam();
    
    /* Infinite loop */
    for(;;)
    {
        WriteNumbOfWrites();
        WriteParameters();
        WriteEnergy();
        WriteThr();
        osDelay(250);
    }
    
}

#endif