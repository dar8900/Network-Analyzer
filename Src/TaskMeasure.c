#include <math.h>
#include "main.h"
#include "TaskMeasure.h"
#include "ADC.h"
#include "SysTime.h"
#include "Graphix.h"
#include "Parameters.h"
#include "AlarmMachine.h"
#include "TaskLed.h"
#include "Simulation.h"


#ifdef ENABLE_MEASURE

#define CURRENT_SAMPLE  10
#define ALPHA   0.6666666

extern bool ConversionEnd;
extern bool SecondTickMeasure;
extern uint32_t ADCReadedValue[NUM_SAMPLE]; 
extern bool EnableSimulation;
extern uint32_t ADCReadedValueSim[NUM_SAMPLE];
extern uint32_t WDogDisplay;

static double CubeRawValue;
static double CurrentRMS[CURRENT_SAMPLE];
static float EnergyAcc;
static float CurrentMeasureThr = 0.4;
static uint32_t NumberOfEnergySampling; 

static uint8_t SensorSensibility = 100;
static uint16_t NumberOfCurrentSampling;

MEASURES GeneralMeasures;
float OldSimCurrent = 0;
uint8_t OldFrequency = 0;

static void ClearDBArray(double Array[], uint8_t Size)
{
    for(uint8_t i = 0; i < Size; i++)
    {
        Array[i] = 0.0;
    }
}


uint32_t CalcArrayAvarage(uint32_t Array[], uint8_t Size)
{
    uint32_t Mean = 0;
    for(uint8_t i = 0; i < Size; i++)
    {
        Mean += Array[i];
    }
    Mean /= Size;
    return Mean;   
}

double CalcArrayAvarageDb(double Array[], uint8_t Size)
{
    double Mean = 0;
    for(uint8_t i = 0; i < Size; i++)
    {
        Mean += Array[i];
    }
    Mean /= Size;
    return Mean;   
}

static float CalcCurrent(double QuadraticValue)
{
    float SquareQuadratic;
    QuadraticValue = (QuadraticValue / NUM_SAMPLE_CAMP); 
    
    // Estraggo il valore quadratico facendolo diventare RMS e trasformandolo in mV 
    SquareQuadratic = (float)sqrt((double)QuadraticValue);
    SquareQuadratic *= 1000;
 
    // Divido i mV ottenuti per la sensibilita del sensore per ottere gli A 
    if(!GeneralParams.EnableSimulation)
        SquareQuadratic /= SensorSensibility;
    else
        SquareQuadratic /= 100;
    
    return SquareQuadratic;
}


static double CalcMeanCurrent(double CurrentRMS, uint16_t Size)
{
    CurrentRMS /= Size;
    return CurrentRMS;
}


static void CheckMaxMinCurrentPower()
{   
    // Max Min Current
    if(GeneralMeasures.MaxCurrent < GeneralMeasures.MeanCurrentRMS)
        GeneralMeasures.MaxCurrent = GeneralMeasures.MeanCurrentRMS;
    
    if(GeneralMeasures.MeanCurrentRMS != 0)
    {      
        if(GeneralMeasures.MinCurrent == 0)
            GeneralMeasures.MinCurrent = GeneralMeasures.MeanCurrentRMS;
        else
        {
            if(GeneralMeasures.MinCurrent > GeneralMeasures.MeanCurrentRMS)
                GeneralMeasures.MinCurrent = GeneralMeasures.MeanCurrentRMS;
        }
    }
    
    // Max Min Power
    if(GeneralMeasures.MaxPower < GeneralMeasures.Power)
        GeneralMeasures.MaxPower = GeneralMeasures.Power;
    
    if(GeneralMeasures.Power != 0)
    {
        if(GeneralMeasures.MinPower == 0)
            GeneralMeasures.MinPower = GeneralMeasures.Power;
        else
        {
            if(GeneralMeasures.MinPower > GeneralMeasures.Power)
                GeneralMeasures.MinPower = GeneralMeasures.Power;
        }
    }
    return;
}


static void SecondEvent()
{
    static bool NotReEnter = false;
    if(SecondTickMeasure)
    {
        SecondTickMeasure = false;
        
        if(NumberOfEnergySampling > 0)
            GeneralMeasures.MeanEnergy += ((EnergyAcc / NumberOfEnergySampling)/3600.0);
        
        CheckAlarm();
        if(AlarmEnergyLed == NO_CONF && !AlarmsReported())
        {
            AlarmEnergyLed = ENERGY_IMPULSE;
        }
        CheckMaxMinCurrentPower();
        
        // Simulo un'onda sinusoidale con l'ampiezza della corrente misurata 
        if(OldSimCurrent != GeneralMeasures.MeanCurrentRMS || OldFrequency != GeneralParams.Frequency)
        {
            SimAdcWave();
            OldSimCurrent = GeneralMeasures.MeanCurrentRMS;
            OldFrequency = GeneralParams.Frequency;
        }
        
        
        NumberOfEnergySampling = 0;
        EnergyAcc = 0;
    }
}



/* TaskMeasure function */
void TaskMeasure(void const * argument)
{
    float AdcRawDiff = 0;
    bool CleanAll = true;
    uint8_t GrandRawMeanIndex = 0;
    osDelay(1000);
    for(;;)
    { 
        if(WDogDisplay > 0)
        {
            WDogDisplay--;
            ManageWDog();
        }
        if(GeneralParams.EnableMeasure)
        {
            CleanAll = true;
            
            // Se non siamo in simulazione
            if(!GeneralParams.EnableSimulation)   
            {                
                if(ConversionEnd)
                {
                    ConversionEnd = false;
                    
                    // Calibrazione dell'offset                   
                    if(GeneralParams.ADCOffset == 0)
                    {
                        GeneralParams.ADCOffset = CalcArrayAvarage(ADCReadedValue, NUM_SAMPLE_CAMP);
                    }
                    for(uint8_t ValueIndx = 0; ValueIndx < NUM_SAMPLE_CAMP; ValueIndx++)
                    {
                        AdcRawDiff = ((TOVOLT((float)ADCReadedValue[ValueIndx]) * ALPHA) - (TOVOLT((float)GeneralParams.ADCOffset)) * ALPHA);
                        CubeRawValue += (double)(AdcRawDiff * AdcRawDiff);
                    }
                }
            }
            // Se siamo in simulazione
            else
            {
                if(OldSimCurrent != GeneralParams.SimulationCurrent || OldFrequency != GeneralParams.Frequency)
                {
                    SimAdcWave();
                    OldSimCurrent = GeneralParams.SimulationCurrent;
                    OldFrequency = GeneralParams.Frequency;
                }
                
                for(uint8_t ValueIndx = 0; ValueIndx < NUM_SAMPLE; ValueIndx++)
                {
                    AdcRawDiff = (TOVOLT((float)ADCReadedValueSim[ValueIndx]) - TOVOLT((float)2048)) / VOLTAGE_DIV_ALPHA;
                    CubeRawValue += (double)(AdcRawDiff * AdcRawDiff);
                }
            }
            CurrentRMS[NumberOfCurrentSampling] = CalcCurrent(CubeRawValue); 
            NumberOfCurrentSampling++;
            if(NumberOfCurrentSampling == CURRENT_SAMPLE)
            {
                NumberOfCurrentSampling = 0;                
                GeneralMeasures.MeanCurrentRMS = CalcArrayAvarageDb(CurrentRMS, CURRENT_SAMPLE);      
                ClearDBArray(CurrentRMS, CURRENT_SAMPLE);
            }   
            if(GeneralMeasures.MeanCurrentRMS > CurrentMeasureThr)
                GeneralMeasures.Power = GeneralMeasures.MeanCurrentRMS * (double)GeneralParams.MeasureVoltage;
            else
            {
                GeneralMeasures.MeanCurrentRMS = 0.0;
                GeneralMeasures.Power = 0.0;
            }
            CubeRawValue = 0;
            
            // Calcolo energia Acc
            EnergyAcc += GeneralMeasures.Power;
            NumberOfEnergySampling++;
            // Gestisce gli eventi al secondo
            SecondEvent();
        } 
        else
        {
            if(CleanAll)
            {
                ClearDBArray(CurrentRMS, CURRENT_SAMPLE);
//                CheckAlarm();
                GeneralMeasures.MeanCurrentRMS = 0.0;
                GeneralMeasures.Power          = 0.0;
                EnergyAcc                      = 0.0;
                AlarmEnergyLed                 = NO_CONF;
                NumberOfEnergySampling         = 0;
                GeneralParams.ADCOffset        = 0;
                CleanAll = false;
            }
        }
        WDogOsDelay(30);
    }
}

#endif // ENABLE_MEASURE