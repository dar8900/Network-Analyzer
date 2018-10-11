#include "main.h"
#include "TaskEeprom.h"
#include "TaskMeasure.h"
#include "Parameters.h"
#include "AlarmMachine.h"

#ifdef ENABLE_EEPROM

#define MAX_DIM_EEPROM_ARRAY    (_EEPROM_FLASH_PAGE_SIZE/4)

extern FL_SCALE TabReScale[MAX_UNIT_FACTOR];


EEPROM_DATA EepromSavedValue[MAX_DIM_EEPROM_ARRAY];

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

static void CopyValueToArray(uint32_t DestArray[], uint16_t Size, uint8_t Index, uint32_t ValueToCopy)
{
    uint16_t Head = 0, Tail = Size - 1, Cnt = 0;   
    while(Head <= Tail)
    {   
        Cnt = (Head + Tail) / 2;
        if(Cnt == Index)
        {
            DestArray[Cnt] = ValueToCopy;
            break;
        }
        else
        {
            Tail = Cnt - 1;
        }
    }
    return;
}

static void ClearArray(uint32_t DestArray[], uint16_t Size)
{
    for(uint16_t i = 0; i < Size; i++)
    {
        DestArray[i] = 0;
    }
    return;
}

static void TransferValuesToMem(uint16_t InitAddress, uint8_t NumbElem, uint32_t ValueToTransfer[])
{
    uint16_t Addr = 0;
    for(Addr = InitAddress; Addr < (InitAddress + NumbElem); Addr++)
    {
        EE_WriteInt(Addr, ValueToTransfer[Addr]);
    }
}

static void TranferMemToRam(uint32_t ValueToTransfer[])
{
    uint16_t Addr = 0;
    for(Addr = EEPROM_VIRTUAL_ADDR_BEGIN; Addr < MAX_DIM_EEPROM_ARRAY; Addr++)
    {
        EE_ReadInt(Addr, &ValueToTransfer[Addr]);
    }
}

static void EraseEeprom()
{
    if(EepFlag.EraseAll)
    {
//        ClearArray(EepromSavedValue, MAX_DIM_EEPROM_ARRAY);  
//        for(uint16_t i = 0; i < MAX_DIM_EEPROM_ARRAY; i++) 
//        {
//            EE_WriteInt(EEPROM_VIRTUAL_ADDR_BEGIN, EepromSavedValue[i]);
//        }
        EE_Format();
        EepFlag.EraseAll = false;
    }
    return;
}

//##########################################################################################################

//                                      NUMERO SCRITTURE       

//##########################################################################################################

static void WriteNumbOfWrites()
{
    uint32_t OldValue = 0;
    
    if(EepFlag.SaveCounter)
    {
        EE_ReadInt(NUMBER_OF_WRITES_ADDR, &OldValue);
        if(EepromSavedValue[NUMBER_OF_WRITES_ADDR] != OldValue)
        {
            EepromSavedValue[NUMBER_OF_WRITES_ADDR]++;
            EE_WriteInt(NUMBER_OF_WRITES_ADDR, EepromSavedValue[NUMBER_OF_WRITES_ADDR]);
        }
        EepFlag.SaveCounter = false;
    }
    return;
}

//##########################################################################################################

//                                      PARAMETRI        

//##########################################################################################################

static void WriteParameters()
{
    uint32_t OldValue = 0;
    if(EepFlag.SaveParameters)
    {
        EepromSavedValue[ENABLE_MEASURE_ADDR] = (uint32_t)GeneralParams.EnableMeasure;
        EepromSavedValue[ADC_OFFSET_ADDR] = (uint32_t)GeneralParams.ADCOffset;
        EepromSavedValue[VOLTAGE_MEASURE_ADDR] = (uint32_t)GeneralParams.MeasureVoltage;
        EE_ReadInt(ENABLE_MEASURE_ADDR, &OldValue);
        if(OldValue != EepromSavedValue[ENABLE_MEASURE_ADDR])
        {           
            EE_WriteInt(ENABLE_MEASURE_ADDR, EepromSavedValue[ENABLE_MEASURE_ADDR]);
        }
        
        EE_ReadInt(ADC_OFFSET_ADDR, &OldValue);
        if(OldValue != EepromSavedValue[ADC_OFFSET_ADDR])
        {            
            EE_WriteInt(ADC_OFFSET_ADDR, EepromSavedValue[ADC_OFFSET_ADDR]);
        }
        
        EE_ReadInt(VOLTAGE_MEASURE_ADDR, &OldValue);
        if(OldValue != EepromSavedValue[VOLTAGE_MEASURE_ADDR])
        {            
            EE_WriteInt(VOLTAGE_MEASURE_ADDR, EepromSavedValue[VOLTAGE_MEASURE_ADDR]);
        }


        EepFlag.SaveParameters = false;
    
    }
    return;
}

static void ReadParameters(uint8_t ParamItem)
{
    switch(ParamItem)
    {
      case MEASURE_ENABLE:
        GeneralParams.EnableMeasure = EepromSavedValue[ENABLE_MEASURE_ADDR];
        break;
      case MEASURE_VOLTAGE:
        GeneralParams.MeasureVoltage = EepromSavedValue[VOLTAGE_MEASURE_ADDR];
        break;
      case ADC_OFFSET:
        GeneralParams.ADCOffset = EepromSavedValue[ADC_OFFSET_ADDR];
        break;
      default:
        break;
    
    }
    return;
}

//##########################################################################################################

//                                      SOGLIE      

//##########################################################################################################



static void ThrsToChar(float ThrOver, float ThrUnder, char ThrOverChar[], char ThrUnderChar[], uint8_t *FactorOver, uint8_t *FactorUnder)
{
    char TempChars[9];
    uint8_t FactorIndex = 0;
    
    FactorIndex = SearchScaleFlRange(ThrOver);
    ThrOver *= TabReScale[FactorIndex].ScaleFactor;
    snprintf(TempChars, 9, "%08.3f", ThrOver);
    CopyCharArray(TempChars, ThrOverChar);
    *FactorOver = FactorIndex;
    
    FactorIndex = SearchScaleFlRange(ThrUnder);
    ThrUnder *= TabReScale[FactorIndex].ScaleFactor;
    snprintf(TempChars, 9, "%08.3f", ThrUnder);
    CopyCharArray(TempChars, ThrUnderChar);
    *FactorUnder = FactorIndex;
    
    return;
}


static void WriteThr()
{
    char OverThresholdChar[8], UnderThresholdChar[8];
    uint16_t AddrEep = 0;
    uint8_t FactorOver = 0, FactorUnder = 0;
    if(EepFlag.SaveThresholds[CURRENT_THR_FLAG])
    {
        ThrsToChar(AlarmsParameters[CURRENT_ALARM].OverThreshold, AlarmsParameters[CURRENT_ALARM].UnderThreshold, OverThresholdChar, UnderThresholdChar,
                   &FactorOver, &FactorUnder);
        for(AddrEep = SOGLIE_ALLARMI_IO_ADDR; AddrEep < (SOGLIE_ALLARMI_IO_ADDR + STR_SIZE - 1); AddrEep++)
        {
            EepromSavedValue[AddrEep] = OverThresholdChar[AddrEep - SOGLIE_ALLARMI_IO_ADDR];
        }
        EepromSavedValue[AddrEep] = (uint32_t)FactorOver;
        for(AddrEep = SOGLIE_ALLARMI_IU_ADDR; AddrEep < (SOGLIE_ALLARMI_IU_ADDR + STR_SIZE - 1); AddrEep++)
        {
            EepromSavedValue[AddrEep] = UnderThresholdChar[AddrEep - SOGLIE_ALLARMI_IU_ADDR];
        }
        EepromSavedValue[AddrEep] = (uint32_t)FactorUnder;
        TransferValuesToMem(SOGLIE_ALLARMI_IO_ADDR, (STR_SIZE * 2), EepromSavedValue);
        EepFlag.SaveThresholds[CURRENT_THR_FLAG] = false;  
    }
    
    if(EepFlag.SaveThresholds[POWER_THR_FLAG])
    {
        ThrsToChar(AlarmsParameters[POWER_ALARM].OverThreshold, AlarmsParameters[POWER_ALARM].UnderThreshold, OverThresholdChar, UnderThresholdChar,
                   &FactorOver, &FactorUnder);
        for(AddrEep = SOGLIE_ALLARMI_PO_ADDR; AddrEep < (SOGLIE_ALLARMI_PO_ADDR + STR_SIZE - 1); AddrEep++)
        {
            EepromSavedValue[AddrEep] = OverThresholdChar[AddrEep - SOGLIE_ALLARMI_PO_ADDR];
        }
        EepromSavedValue[AddrEep] = (uint32_t)FactorOver;
        for(AddrEep = SOGLIE_ALLARMI_PU_ADDR; AddrEep < (SOGLIE_ALLARMI_PU_ADDR + STR_SIZE - 1); AddrEep++)
        {
            EepromSavedValue[AddrEep] = UnderThresholdChar[AddrEep - SOGLIE_ALLARMI_PU_ADDR];
        }
        EepromSavedValue[AddrEep] = (uint32_t)FactorUnder;
        TransferValuesToMem(SOGLIE_ALLARMI_PO_ADDR, (STR_SIZE * 2), EepromSavedValue);   
        EepFlag.SaveThresholds[POWER_THR_FLAG] = false;  
    }
    if(EepFlag.SaveThresholds[ENERGY_THR_FLAG])
    {
        ThrsToChar(AlarmsParameters[ENERGY_ALARM].OverThreshold, AlarmsParameters[ENERGY_ALARM].UnderThreshold, OverThresholdChar, UnderThresholdChar,
                   &FactorOver, &FactorUnder);
        for(AddrEep = SOGLIE_ALLARMI_EO_ADDR; AddrEep < (SOGLIE_ALLARMI_EO_ADDR + STR_SIZE - 1); AddrEep++)
        {
            EepromSavedValue[AddrEep] = OverThresholdChar[AddrEep - SOGLIE_ALLARMI_EO_ADDR];
        }
        EepromSavedValue[AddrEep] = (uint32_t)FactorOver;
        for(AddrEep = SOGLIE_ALLARMI_EU_ADDR; AddrEep < (SOGLIE_ALLARMI_EU_ADDR + STR_SIZE - 1); AddrEep++)
        {
            EepromSavedValue[AddrEep] = UnderThresholdChar[AddrEep - SOGLIE_ALLARMI_EU_ADDR];
        }
        EepromSavedValue[AddrEep] = (uint32_t)FactorUnder;
        TransferValuesToMem(SOGLIE_ALLARMI_EO_ADDR, (STR_SIZE * 2), EepromSavedValue);     
        EepFlag.SaveThresholds[ENERGY_THR_FLAG] = false;  
    }
}

static void ReWriteStr(char OldStr[], char NewStr[], uint8_t Size)
{
    uint8_t i = 0, j = 0;
    for(i = 0; i < Size; i++)
    {
        if(OldStr[i] != '0')
        {
            break;
        }
    }
    for(j = 0; j < Size - i; j++)
    {
        NewStr[j] = OldStr[i + j];
    }
}



static void ReadCurrentThr()
{
    uint8_t Addr = 0;
    float Value = 0.0;
    char OverThresholdChar[8], UnderThresholdChar[8];
    char CopyStr[8];
    
    uint8_t FactorScale = 0;
    for(Addr = SOGLIE_ALLARMI_IO_ADDR; Addr < (SOGLIE_ALLARMI_IO_ADDR + STR_SIZE - 1); Addr++)
    {
        OverThresholdChar[Addr - SOGLIE_ALLARMI_IO_ADDR] = (char)EepromSavedValue[Addr];
    }
    FactorScale = EepromSavedValue[Addr];    
    ReWriteStr(OverThresholdChar, CopyStr, 8);
    Value = strtof(OverThresholdChar, NULL);
    Value /= TabReScale[FactorScale].ScaleFactor;
    AlarmsParameters[CURRENT_ALARM].OverThreshold = Value;
    
    for(Addr = SOGLIE_ALLARMI_IU_ADDR; Addr < (SOGLIE_ALLARMI_IU_ADDR + STR_SIZE - 1); Addr++)
    {
        UnderThresholdChar[Addr - SOGLIE_ALLARMI_IU_ADDR] = (char)EepromSavedValue[Addr];
    }
    FactorScale = EepromSavedValue[Addr];    
    Value = strtof(UnderThresholdChar, NULL);
    Value /= TabReScale[FactorScale].ScaleFactor;
    AlarmsParameters[CURRENT_ALARM].UnderThreshold = Value;
}

static void ReadPowerThr()
{
    uint8_t Addr = 0;
    float Value = 0.0;
    char OverThresholdChar[8], UnderThresholdChar[8];
    uint8_t FactorScale = 0;
    for(Addr = SOGLIE_ALLARMI_PO_ADDR; Addr < (STR_SIZE - 1); Addr++)
    {
        OverThresholdChar[Addr - SOGLIE_ALLARMI_PO_ADDR] = (char)EepromSavedValue[Addr];
    }
    FactorScale = EepromSavedValue[Addr];    
    Value = strtof(OverThresholdChar, NULL);
    Value /= TabReScale[FactorScale].ScaleFactor;
    AlarmsParameters[POWER_ALARM].OverThreshold = Value;
    
    for(Addr = SOGLIE_ALLARMI_PU_ADDR; Addr < (STR_SIZE - 1); Addr++)
    {
        UnderThresholdChar[Addr - SOGLIE_ALLARMI_PU_ADDR] = (char)EepromSavedValue[Addr];
    }
    FactorScale = EepromSavedValue[Addr];    
    Value = strtof(UnderThresholdChar, NULL);
    Value /= TabReScale[FactorScale].ScaleFactor;
    AlarmsParameters[POWER_ALARM].UnderThreshold = Value;

}

static void ReadEnergyThr()
{
    uint8_t Addr = 0;
    float Value = 0.0;
    char OverThresholdChar[8], UnderThresholdChar[8];
    uint8_t FactorScale = 0;
    for(Addr = SOGLIE_ALLARMI_EO_ADDR; Addr < (STR_SIZE - 1); Addr++)
    {
        OverThresholdChar[Addr - SOGLIE_ALLARMI_EO_ADDR] = (char)EepromSavedValue[Addr];
    }
    FactorScale = EepromSavedValue[Addr];    
    Value = strtof(OverThresholdChar, NULL);
    Value /= TabReScale[FactorScale].ScaleFactor;
    AlarmsParameters[ENERGY_ALARM].OverThreshold = Value;
    
    for(Addr = SOGLIE_ALLARMI_EU_ADDR; Addr < (STR_SIZE - 1); Addr++)
    {
        UnderThresholdChar[Addr - SOGLIE_ALLARMI_EU_ADDR] = (char)EepromSavedValue[Addr];
    }
    FactorScale = EepromSavedValue[Addr];    
    Value = strtof(UnderThresholdChar, NULL);
    Value /= TabReScale[FactorScale].ScaleFactor;
    AlarmsParameters[ENERGY_ALARM].UnderThreshold = Value;

}

//##########################################################################################################

//                                      ENERGIA        

//##########################################################################################################

static void EnergyToChar(float Energy, char EnergyChar[], uint8_t *FactorOver)
{
    char TempChars[9];
    uint8_t FactorIndex = 0;
    
    FactorIndex = SearchScaleFlRange(Energy);
    Energy *= TabReScale[FactorIndex].ScaleFactor;
    snprintf(TempChars, 9, "%08.3f", Energy);
    CopyCharArray(TempChars, EnergyChar);
    *FactorOver = TabReScale[FactorIndex].Unit;
    
    return;
}

static void WriteEnergy()
{
    uint16_t AddrEep = 0;
    char EnergyToStr[9];
    uint8_t Factor = 0;
    EnergyToChar(GeneralMeasures.MeanEnergy, EnergyToStr, &Factor);
    for(AddrEep = ENERGIA_ADDR; AddrEep < (STR_SIZE - 1); AddrEep++)
    {
        EepromSavedValue[AddrEep] = EnergyToStr[AddrEep - ENERGIA_ADDR];
    }
    EepromSavedValue[AddrEep] = (uint32_t)Factor;
}

static void ReadEnergy()
{
    uint8_t Addr = 0;
    float Value = 0.0;
    char StrToEnergy[8];;
    uint8_t FactorScale = 0;
    for(Addr = ENERGIA_ADDR; Addr < (STR_SIZE - 1); Addr++)
    {
        StrToEnergy[Addr - ENERGIA_ADDR] = (char)EepromSavedValue[Addr];
    }
    FactorScale = EepromSavedValue[Addr];    
    Value = strtof(StrToEnergy, NULL);
    Value /= TabReScale[FactorScale].ScaleFactor;
    GeneralMeasures.MeanEnergy = Value;
}




//##########################################################################################################

//                                      TASK       

//##########################################################################################################





/* TaskEeprom function */
void TaskEeprom(void const * argument)
{
    TranferMemToRam(EepromSavedValue);
    for(uint8_t i = 0; i < MAX_PARAMETER_ITEM; i++)
    {
        ReadParameters(i);
    }
    ReadCurrentThr();
    
    /* Infinite loop */
    for(;;)
    {
        EraseEeprom();
        WriteParameters();
        WriteThr();
        
        osDelay(250);
    }
    
}

#endif