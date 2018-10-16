#include "main.h"
#include "TaskEeprom.h"
#include "TaskMeasure.h"
#include "Parameters.h"
#include "AlarmMachine.h"

#ifdef ENABLE_EEPROM

#define MAX_DIM_EEPROM_ARRAY    (_EEPROM_FLASH_PAGE_SIZE/4)
#define BLANK_VALUE             0xFFFFFFFF

extern FL_SCALE TabReScale[MAX_UNIT_FACTOR];
extern TIME_VAR GlobalTime;
extern DATE_VAR GlobalDate;

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

/*
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
*/

static void ClearArray(uint32_t DestArray[], uint16_t Size)
{
    for(uint16_t i = 0; i < Size; i++)
    {
        DestArray[i] = 0;
    }
    return;
}

static void TransferValuesToMem(uint32_t ValueToTransfer[])
{
    EE_Write(ValueToTransfer);
}

static void TranferMemToRam(uint32_t ValueToTransfer[])
{
    EE_MultiRead(EEPROM_VIRTUAL_ADDR_BEGIN, MAX_DIM_EEPROM_ARRAY, ValueToTransfer);
}

static void EraseEeprom()
{
    if(EepFlag.EraseAll)
    {
        EE_Format();
        TranferMemToRam(EepromSavedValue);
        EepFlag.EraseAll = false;
    }
    return;
}


static bool IsEepromClear()
{
    for(uint16_t i = 0; i < MAX_DIM_EEPROM_ARRAY; i++)
    {
        if(EepromSavedValue[i] != BLANK_VALUE)
            return false;
    }          
    return true;
}



//##########################################################################################################

//                                      NUMERO SCRITTURE       

//##########################################################################################################

static void WriteNumbOfWrites(uint8_t Numb)
{
    EepromSavedValue[NUMBER_OF_WRITES_ADDR] += Numb;
    return;
}

static void ReadNumbOfWrites()
{
    EE_SingleRead(NUMBER_OF_WRITES_ADDR, &EepromSavedValue[NUMBER_OF_WRITES_ADDR]);
    if(EepromSavedValue[NUMBER_OF_WRITES_ADDR] == BLANK_VALUE)
    {
        EepromSavedValue[NUMBER_OF_WRITES_ADDR] = 0;
        TransferValuesToMem(EepromSavedValue);
    }   
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
        EepromSavedValue[LOG_ENERGY_ADDR] = (uint32_t)GeneralParams.LogEnergyPeriod;
        EepromSavedValue[ENABLE_SCREENSAVER_ADDR] = (uint32_t)GeneralParams.EnableScreenSaver;
        EepromSavedValue[SCREENSAVER_TYPE_ADDR] = (uint32_t)GeneralParams.ScreenSaverType;
        
        for(uint8_t ParamIndexAddr = 0; ParamIndexAddr < (MAX_PARAMETER_ITEM - 1); ParamIndexAddr++)
        {
            if(ParamIndexAddr < 3)
            {
                EE_SingleRead(ParamIndexAddr, &OldValue);
                if(OldValue != EepromSavedValue[ParamIndexAddr])
                {    
//                    WriteNumbOfWrites(1);
//                    TransferValuesToMem(EepromSavedValue);
                }          
            }
            else
            {
                ParamIndexAddr -= 3;
                ParamIndexAddr += LOG_ENERGY_ADDR;
                EE_SingleRead(ParamIndexAddr, &OldValue);
                if(OldValue != EepromSavedValue[ParamIndexAddr])
                {    
//                    WriteNumbOfWrites(1);
//                    TransferValuesToMem(EepromSavedValue);
                }                  
            }
        }

        
        EE_SingleRead(ENABLE_MEASURE_ADDR, &OldValue);
        if(OldValue != EepromSavedValue[ENABLE_MEASURE_ADDR])
        {    
            WriteNumbOfWrites(1);
            TransferValuesToMem(EepromSavedValue);
        }
        
        EE_SingleRead(ADC_OFFSET_ADDR, &OldValue);
        if(OldValue != EepromSavedValue[ADC_OFFSET_ADDR])
        {     
            WriteNumbOfWrites(1);
            TransferValuesToMem(EepromSavedValue);
        }
        
        EE_SingleRead(VOLTAGE_MEASURE_ADDR, &OldValue);
        if(OldValue != EepromSavedValue[VOLTAGE_MEASURE_ADDR])
        {       
            WriteNumbOfWrites(1);
            TransferValuesToMem(EepromSavedValue);
        }

        EE_SingleRead(LOG_ENERGY_ADDR, &OldValue);
        if(OldValue != EepromSavedValue[LOG_ENERGY_ADDR])
        {       
            WriteNumbOfWrites(1);
            TransferValuesToMem(EepromSavedValue);
        }
        
        EE_SingleRead(ENABLE_SCREENSAVER_ADDR, &OldValue);
        if(OldValue != EepromSavedValue[ENABLE_SCREENSAVER_ADDR])
        {       
            WriteNumbOfWrites(1);
            TransferValuesToMem(EepromSavedValue);
        }

        EE_SingleRead(SCREENSAVER_TYPE_ADDR, &OldValue);
        if(OldValue != EepromSavedValue[SCREENSAVER_TYPE_ADDR])
        {       
            WriteNumbOfWrites(1);
            TransferValuesToMem(EepromSavedValue);
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
      case LOG_ENERGY_PERIOD:
        GeneralParams.LogEnergyPeriod = EepromSavedValue[LOG_ENERGY_ADDR];
        break;
      case SCREENSAVER_TYPE:
        GeneralParams.ScreenSaverType = EepromSavedValue[SCREENSAVER_TYPE_ADDR];
        break;
      case ENABLE_SCREENSAVER:
        GeneralParams.EnableScreenSaver = EepromSavedValue[ENABLE_SCREENSAVER_ADDR];
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
        WriteNumbOfWrites(1);
        TransferValuesToMem(EepromSavedValue);
        osDelay(10);
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
        WriteNumbOfWrites(1);
        TransferValuesToMem(EepromSavedValue);
        osDelay(10);
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
        WriteNumbOfWrites(1);
        TransferValuesToMem(EepromSavedValue);
        osDelay(10);
        EepFlag.SaveThresholds[ENERGY_THR_FLAG] = false;  
    }
}

static void ReWriteStr(char OldStr[], char NewStr[], uint8_t Size)
{
    uint8_t i = 0, j = 0;
    for(i = 0; i < Size; i++)
    {
        if(OldStr[i] > '0' || (OldStr[i] == '0' && OldStr[i + 1] == '.'))
        {
            break;
        }
    }
    for(j = 0; j < Size; j++)
    {
        if((i + j) < Size )
            NewStr[j] = OldStr[i + j];
        else
            NewStr[j] = '0';
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
    Value = strtof(CopyStr, NULL);
    Value /= TabReScale[FactorScale].ScaleFactor;
    AlarmsParameters[CURRENT_ALARM].OverThreshold = Value;
    
    for(Addr = SOGLIE_ALLARMI_IU_ADDR; Addr < (SOGLIE_ALLARMI_IU_ADDR + STR_SIZE - 1); Addr++)
    {
        UnderThresholdChar[Addr - SOGLIE_ALLARMI_IU_ADDR] = (char)EepromSavedValue[Addr];
    }
    FactorScale = EepromSavedValue[Addr];    
    ReWriteStr(UnderThresholdChar, CopyStr, 8);
    Value = strtof(CopyStr, NULL);
    Value /= TabReScale[FactorScale].ScaleFactor;
    AlarmsParameters[CURRENT_ALARM].UnderThreshold = Value;
}

static void ReadPowerThr()
{
    uint8_t Addr = 0;
    float Value = 0.0;
    char OverThresholdChar[8], UnderThresholdChar[8];
    char CopyStr[8];
    uint8_t FactorScale = 0;
    for(Addr = SOGLIE_ALLARMI_PO_ADDR; Addr < (SOGLIE_ALLARMI_PO_ADDR + STR_SIZE - 1); Addr++)
    {
        OverThresholdChar[Addr - SOGLIE_ALLARMI_PO_ADDR] = (char)EepromSavedValue[Addr];
    }
    FactorScale = EepromSavedValue[Addr]; 
    ReWriteStr(OverThresholdChar, CopyStr, 8);
    Value = strtof(CopyStr, NULL);
    Value /= TabReScale[FactorScale].ScaleFactor;
    AlarmsParameters[POWER_ALARM].OverThreshold = Value;
    
    for(Addr = SOGLIE_ALLARMI_PU_ADDR; Addr < (SOGLIE_ALLARMI_PU_ADDR + STR_SIZE - 1); Addr++)
    {
        UnderThresholdChar[Addr - SOGLIE_ALLARMI_PU_ADDR] = (char)EepromSavedValue[Addr];
    }
    FactorScale = EepromSavedValue[Addr];    
    ReWriteStr(UnderThresholdChar, CopyStr, 8);
    Value = strtof(CopyStr, NULL);
    Value /= TabReScale[FactorScale].ScaleFactor;
    AlarmsParameters[POWER_ALARM].UnderThreshold = Value;

}

static void ReadEnergyThr()
{
    uint8_t Addr = 0;
    float Value = 0.0;
    char OverThresholdChar[8], UnderThresholdChar[8];
    char CopyStr[8];
    uint8_t FactorScale = 0;
    for(Addr = SOGLIE_ALLARMI_EO_ADDR; Addr < (SOGLIE_ALLARMI_EO_ADDR + STR_SIZE - 1); Addr++)
    {
        OverThresholdChar[Addr - SOGLIE_ALLARMI_EO_ADDR] = (char)EepromSavedValue[Addr];
    }
    FactorScale = EepromSavedValue[Addr];    
    ReWriteStr(OverThresholdChar, CopyStr, 8);
    Value = strtof(CopyStr, NULL);
    Value /= TabReScale[FactorScale].ScaleFactor;
    AlarmsParameters[ENERGY_ALARM].OverThreshold = Value;
    
    for(Addr = SOGLIE_ALLARMI_EU_ADDR; Addr < (SOGLIE_ALLARMI_EU_ADDR + STR_SIZE - 1); Addr++)
    {
        UnderThresholdChar[Addr - SOGLIE_ALLARMI_EU_ADDR] = (char)EepromSavedValue[Addr];
    }
    FactorScale = EepromSavedValue[Addr];    
    ReWriteStr(UnderThresholdChar, CopyStr, 8);
    Value = strtof(CopyStr, NULL);
    Value /= TabReScale[FactorScale].ScaleFactor;
    AlarmsParameters[ENERGY_ALARM].UnderThreshold = Value;

}

//##########################################################################################################

//                                      VALORI FLOAT GENERALI        

//##########################################################################################################

static void EnergyToChar(float Energy, char EnergyChar[], uint8_t *Factor)
{
    char TempChars[9];
    uint8_t FactorIndex = 0;
    
    FactorIndex = SearchScaleFlRange(Energy);
    Energy *= TabReScale[FactorIndex].ScaleFactor;
    snprintf(TempChars, 9, "%08.3f", Energy);
    CopyCharArray(TempChars, EnergyChar);
    *Factor = FactorIndex;
    
    return;
}

static void WriteEnergy()
{
    uint16_t AddrEep = 0;
    char EnergyToStr[9];
    uint8_t Factor = 0;
    if(EepFlag.SaveEnergy)
    {   
        EnergyToChar(GeneralMeasures.MeanEnergy, EnergyToStr, &Factor);
        for(AddrEep = ENERGIA_ADDR; AddrEep < (ENERGIA_ADDR + STR_SIZE - 1); AddrEep++)
        {
            EepromSavedValue[AddrEep] = EnergyToStr[AddrEep - ENERGIA_ADDR];
        }
        EepromSavedValue[AddrEep] = (uint32_t)Factor;
        WriteNumbOfWrites(1);
        TransferValuesToMem(EepromSavedValue);
        EepFlag.SaveEnergy = false;
    }
}

static void ReadEnergy()
{
    uint8_t Addr = 0;
    float Value = 0.0;
    char StrToEnergy[8];
    char CopyStr[8];
    uint8_t FactorScale = 0;
    for(Addr = ENERGIA_ADDR; Addr < (ENERGIA_ADDR + STR_SIZE - 1); Addr++)
    {
        StrToEnergy[Addr - ENERGIA_ADDR] = (char)EepromSavedValue[Addr];
    }
    FactorScale = EepromSavedValue[Addr];   
    ReWriteStr(StrToEnergy, CopyStr, 8);
    Value = strtof(CopyStr, NULL);
    Value /= TabReScale[FactorScale].ScaleFactor;
    GeneralMeasures.MeanEnergy = Value;
}

//##########################################################################################################

//                                     VALORI A DFLT O IN RAM
//##########################################################################################################

static void TranferToGlobalVars()
{
    for(uint8_t i = 0; i < MAX_PARAMETER_ITEM; i++)
    {
        ReadParameters(i);
    }
    ReadCurrentThr();
    ReadPowerThr();
    ReadEnergyThr();
    ReadEnergy();
    ReadNumbOfWrites();
}

static void CheckEepromAndTranfer()
{
    if(IsEepromClear())
    {  
        GeneralMeasures.MeanEnergy = 0.0;
        GeneralParams.EnableMeasure = false;
        GeneralParams.ADCOffset = 2048;
        GeneralParams.MeasureVoltage = 220;
        GeneralParams.LogEnergyPeriod = 15;
        AlarmsParameters[CURRENT_ALARM].OverThreshold = 1.0;
        AlarmsParameters[CURRENT_ALARM].UnderThreshold = 0.0;
        AlarmsParameters[POWER_ALARM].OverThreshold = 1.0;
        AlarmsParameters[POWER_ALARM].UnderThreshold = 0.0;
        AlarmsParameters[ENERGY_ALARM].OverThreshold = 1.0;
        AlarmsParameters[ENERGY_ALARM].UnderThreshold = 0.0;
        
        EepFlag.SaveParameters = true;
        EepFlag.SaveThresholds[0] = true;
        EepFlag.SaveThresholds[1] = true;
        EepFlag.SaveThresholds[2] = true;
        EepFlag.SaveEnergy = true;
    }
    else
    {
        TranferToGlobalVars();
    } 
}
//##########################################################################################################


//##########################################################################################################

//                                      TASK       

//##########################################################################################################




/* TaskEeprom function */
void TaskEeprom(void const * argument)
{
    bool NotResaveEnergy = false;
    TranferMemToRam(EepromSavedValue);
    CheckEepromAndTranfer();

    
    /* Infinite loop */
    for(;;)
    {
        EraseEeprom();
        WriteParameters();
        WriteThr();
        WriteEnergy();
        if(!(GlobalTime.minutes % GeneralParams.LogEnergyPeriod) && GlobalDate.year != 0 && !NotResaveEnergy)
        {
            NotResaveEnergy = true;
            EepFlag.SaveEnergy = true;  
        } 
        else if((GlobalTime.minutes % GeneralParams.LogEnergyPeriod) && GlobalDate.year != 0 && NotResaveEnergy)
            NotResaveEnergy = false;

        osDelay(100);
    }
    
}

#endif