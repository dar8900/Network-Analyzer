#include "main.h"
#include "TaskEeprom.h"
#include "TaskMeasure.h"
#include "TaskLed.h"
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



//##########################################################################################################

//                                      TOOLS       

//##########################################################################################################



static void CopyCharArray(char OrigArray[], char DestArray[])
{
    uint8_t i, Size = 9;
    for(i = 0; i < Size; i++)
    {
        DestArray[i] = OrigArray[i];
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

static void SaveAllToMem()
{
    if(EepFlag.SaveAll)
    {
        TransferValuesToMem(EepromSavedValue);
        EepFlag.SaveAll = false;
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
    uint8_t ParamIdexShifted = 0;
    if(EepFlag.SaveParameters)
    {
        EepromSavedValue[ENABLE_MEASURE_ADDR] = (uint32_t)GeneralParams.EnableMeasure;
        EepromSavedValue[ADC_OFFSET_ADDR] = (uint32_t)GeneralParams.ADCOffset;
        EepromSavedValue[VOLTAGE_MEASURE_ADDR] = (uint32_t)GeneralParams.MeasureVoltage;
        EepromSavedValue[LOG_MESAURE_ADDR] = (uint32_t)GeneralParams.LogMeasurePeriod;
        EepromSavedValue[ENABLE_SCREENSAVER_ADDR] = (uint32_t)GeneralParams.EnableScreenSaver;
        EepromSavedValue[SCREENSAVER_TYPE_ADDR] = (uint32_t)GeneralParams.ScreenSaverType;
        EepromSavedValue[SCREEN_SAVER_TIMER_ADDR] = (uint32_t)GeneralParams.ScreenSaverTimer;
        EepromSavedValue[LED_CONF_ADDR] = (uint32_t)LedConf;
        EepromSavedValue[ENABLE_SIMULATION_ADDR] = (uint32_t)GeneralParams.EnableSimulation;
        EepromSavedValue[FREQUENCY_SIM_ADDR] = (uint32_t)GeneralParams.Frequency;
        
        for(uint8_t ParamIndexAddr = 0; ParamIndexAddr < (MAX_PARAMETER_ITEM - 1); ParamIndexAddr++)
        {
            if(ParamIndexAddr < 3)
            {
                EE_SingleRead(ParamIndexAddr, &OldValue);
                if(OldValue != EepromSavedValue[ParamIndexAddr])
                {    
                    WriteNumbOfWrites(1);
                    TransferValuesToMem(EepromSavedValue);
                }          
            }
            else
            {
                ParamIdexShifted = ParamIndexAddr - 3;
                ParamIdexShifted += LOG_MESAURE_ADDR;
                EE_SingleRead(ParamIdexShifted, &OldValue);
                if(OldValue != EepromSavedValue[ParamIdexShifted])
                {    
                    WriteNumbOfWrites(1);
                    TransferValuesToMem(EepromSavedValue);
                }                  
            }
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
//      case ADC_OFFSET:
//        GeneralParams.ADCOffset = EepromSavedValue[ADC_OFFSET_ADDR];
//        break;
      case LOG_MEASURE_PERIOD:
        GeneralParams.LogMeasurePeriod = EepromSavedValue[LOG_MESAURE_ADDR];
        break;
      case SCREENSAVER_TYPE:
        GeneralParams.ScreenSaverType = EepromSavedValue[SCREENSAVER_TYPE_ADDR];
        break;
      case ENABLE_SCREENSAVER:
        GeneralParams.EnableScreenSaver = EepromSavedValue[ENABLE_SCREENSAVER_ADDR];
        break;
      case SCREENSAVER_TIMER:
        GeneralParams.ScreenSaverTimer = EepromSavedValue[SCREEN_SAVER_TIMER_ADDR];
        break;
      case LED_CONFIGURATION:
        LedConf = EepromSavedValue[LED_CONF_ADDR];
        break;
      case ENABLE_SIMULATION:
        GeneralParams.EnableSimulation = EepromSavedValue[ENABLE_SIMULATION_ADDR];
        break;
      case FREQUENCY_SIM:
        GeneralParams.Frequency = EepromSavedValue[FREQUENCY_SIM_ADDR];
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

static void FloatToChar(float FLValue, char FloatChar[], uint8_t *Factor)
{
    char TempChars[9];
    uint8_t FactorIndex = 0;
    
    FactorIndex = SearchScaleFlRange(FLValue);
    FLValue *= TabReScale[FactorIndex].ScaleFactor;
    snprintf(TempChars, 9, "%08.3f", FLValue);
    CopyCharArray(TempChars, FloatChar);
    *Factor = FactorIndex;
    
    return;
}


static void WriteFloat(uint16_t ValueAddr, float ValueToSave)
{
    uint16_t AddrEep = 0;
    char FloatToStr[9];
    uint8_t Factor = 0;  
    FloatToChar(ValueToSave, FloatToStr, &Factor);
    for(AddrEep = ValueAddr; AddrEep < (ValueAddr + STR_SIZE - 1); AddrEep++)
    {
        EepromSavedValue[AddrEep] = FloatToStr[AddrEep - ValueAddr];
    }
    EepromSavedValue[AddrEep] = (uint32_t)Factor;
    WriteNumbOfWrites(1);
    TransferValuesToMem(EepromSavedValue);    
}

static void ReadFloat(uint16_t ValueAddr , float *ValueToRead)
{
    uint16_t Addr = 0;
    float Value = 0.0;
    char StrToFloat[8];
    char CopyStr[8];
    uint8_t FactorScale = 0;
    for(Addr = ValueAddr; Addr < (ValueAddr + STR_SIZE - 1); Addr++)
    {
        StrToFloat[Addr - ValueAddr] = (char)EepromSavedValue[Addr];
    }
    FactorScale = EepromSavedValue[Addr];   
    ReWriteStr(StrToFloat, CopyStr, 8);
    Value = strtof(CopyStr, NULL);
    Value /= TabReScale[FactorScale].ScaleFactor;
    *ValueToRead = Value;
}

static void WriteFloatValues()
{
    if(EepFlag.SaveEnergy)
    {
        WriteFloat(ENERGIA_ADDR, GeneralMeasures.MeanEnergy);
        EepFlag.SaveEnergy = false;
    }
    if(EepFlag.SaveCurrentSim)
    {
        WriteFloat(CURRENT_SIM_ADDR_0, GeneralParams.SimulationCurrent);
        EepFlag.SaveCurrentSim = false;
    }
    if(EepFlag.SaveMaxMinCurrent)
    {
        WriteFloat(MAX_CURRENT_0_ADDR, GeneralMeasures.MaxCurrent);
        WriteFloat(MIN_CURRENT_0_ADDR, GeneralMeasures.MinCurrent);
        EepFlag.SaveMaxMinCurrent = false;
    }
    if(EepFlag.SaveMaxMinPower)
    {
        WriteFloat(MAX_POWER_0_ADDR, GeneralMeasures.MaxPower);
        WriteFloat(MIN_POWER_0_ADDR, GeneralMeasures.MinPower);
        EepFlag.SaveMaxMinPower = false;
    }

    return;
}

static void ReadFloatValues()
{
    ReadFloat(ENERGIA_ADDR      , &GeneralMeasures.MeanEnergy);
    ReadFloat(CURRENT_SIM_ADDR_0, &GeneralParams.SimulationCurrent);
    ReadFloat(MAX_CURRENT_0_ADDR, &GeneralMeasures.MaxCurrent);
    ReadFloat(MIN_CURRENT_0_ADDR, &GeneralMeasures.MinCurrent);
    ReadFloat(MAX_POWER_0_ADDR  , &GeneralMeasures.MaxPower);
    ReadFloat(MIN_POWER_0_ADDR  , &GeneralMeasures.MinPower);
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
    ReadFloatValues();
    ReadNumbOfWrites();
}

static void CheckEepromAndTranfer()
{
    if(IsEepromClear())
    {  
        GeneralMeasures.MeanEnergy = 0.0;
        EepromSavedValue[ENABLE_MEASURE_ADDR] = GeneralParams.EnableMeasure = false;
        EepromSavedValue[ADC_OFFSET_ADDR] = GeneralParams.ADCOffset = 2048;
        EepromSavedValue[VOLTAGE_MEASURE_ADDR] = GeneralParams.MeasureVoltage = 220;
        EepromSavedValue[LOG_MESAURE_ADDR] = GeneralParams.LogMeasurePeriod = 15;
        EepromSavedValue[ENABLE_SCREENSAVER_ADDR] = GeneralParams.EnableScreenSaver = false;
        EepromSavedValue[SCREENSAVER_TYPE_ADDR] = GeneralParams.ScreenSaverType = ANALOG_DATE;
        EepromSavedValue[SCREEN_SAVER_TIMER_ADDR] = GeneralParams.ScreenSaverTimer = 10;
        EepromSavedValue[LED_CONF_ADDR] = LedConf = ALL_LED_OFF;
        EepromSavedValue[ENABLE_SIMULATION_ADDR] = GeneralParams.EnableSimulation = false;
        EepromSavedValue[FREQUENCY_SIM_ADDR] = GeneralParams.Frequency = 1;
        
        
        GeneralParams.SimulationCurrent = 1.0;
        
        AlarmsParameters[CURRENT_ALARM].OverThreshold = 1.0;
        AlarmsParameters[CURRENT_ALARM].UnderThreshold = 0.0;
        AlarmsParameters[POWER_ALARM].OverThreshold = 1.0;
        AlarmsParameters[POWER_ALARM].UnderThreshold = 0.0;
        AlarmsParameters[ENERGY_ALARM].OverThreshold = 1.0;
        AlarmsParameters[ENERGY_ALARM].UnderThreshold = 0.0;
        
        EepFlag.SaveParameters = true;
        EepFlag.SaveThresholds[CURRENT_THR_FLAG] = true;
        EepFlag.SaveThresholds[POWER_THR_FLAG]   = true;
        EepFlag.SaveThresholds[ENERGY_THR_FLAG]  = true;
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
        WriteFloatValues();
        if(GeneralParams.EnableMeasure)
        {
            if(!(GlobalTime.minutes % GeneralParams.LogMeasurePeriod) && GlobalDate.year != 0 && !NotResaveEnergy)
            {
                NotResaveEnergy = true;
                EepFlag.SaveEnergy = true;  
                EepFlag.SaveMaxMinCurrent = true;
                EepFlag.SaveMaxMinPower = true;
            } 
            else if((GlobalTime.minutes % GeneralParams.LogMeasurePeriod) && GlobalDate.year != 0 && NotResaveEnergy)
                NotResaveEnergy = false;
        }
        osDelay(1000);
    }
    
}

#endif