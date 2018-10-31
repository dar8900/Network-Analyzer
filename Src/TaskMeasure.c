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

#define CURRENT_SAMPLE  40

extern bool ConversionEnd;
extern bool SecondTick;
extern uint32_t ADCReadedValue[NUM_SAMPLE * 2]; 
extern bool EnableSimulation;
extern uint32_t ADCReadedValueSim[NUM_SAMPLE];

static double CubeRawValue;
static float CurrentRMS[CURRENT_SAMPLE];
static float EnergyAcc;
static uint32_t NumberOfEnergySampling; 

static uint8_t SensorSensibility = 60;

MEASURES GeneralMeasures;

uint32_t AdcMeanRawValue[NUM_SAMPLE];


static void ClearFLArray(float Array[], uint8_t Size)
{
    for(uint8_t i = 0; i < Size; i++)
    {
        Array[i] = 0.0;
    }
}


static uint32_t CalcArrayAvarage(uint32_t Array[], uint8_t Size)
{
    uint32_t Mean = 0;
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
    QuadraticValue = (QuadraticValue / NUM_SAMPLE); 
    
    // Estraggo il valore quadratico facendolo diventare RMS e trasformandolo in mV 
    SquareQuadratic = (float)sqrt((double)QuadraticValue);
    
//    SquareQuadratic =  (TOVOLT(SquareQuadratic)) * 1000;
    SquareQuadratic *= 1000;

    
    // Divido i mV ottenuti per la sensibilita del sensore per ottere gli A 
    SquareQuadratic /= SensorSensibility;
    
    return SquareQuadratic;
}


static float CalcMeanCurrent(float CurrentRMS[])
{
    uint8_t CurrentIndx = 0;
    float SumCurr = 0.0;
    for(CurrentIndx = 0; CurrentIndx < CURRENT_SAMPLE; CurrentIndx++)
    {
        SumCurr += CurrentRMS[CurrentIndx];
    }
    SumCurr /= CURRENT_SAMPLE;
    return SumCurr;
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
    if(SecondTick)
    {
        if(!NotReEnter)
        {
            if(NumberOfEnergySampling > 0)
                GeneralMeasures.MeanEnergy += ((EnergyAcc / NumberOfEnergySampling)/3600.0);
            
            NumberOfEnergySampling = 0;
            EnergyAcc = 0;
            CheckAlarm();
            if(AlarmEnergyLed == NO_CONF && !AlarmsReported())
            {
                AlarmEnergyLed = ENERGY_IMPULSE;
            }
            CheckMaxMinCurrentPower();
        }
        NotReEnter = true;
    }
    else
    {
        NotReEnter = false;
    }
}



/* TaskMeasure function */
void TaskMeasure(void const * argument)
{
    uint8_t NumberOfCurrentSampling = 0;
    float AdcRawDiff = 0;
    float OldSimCurrent = 0;
    uint8_t OldFrequency = 0;
    bool CleanAll = true;
    uint8_t GrandRawMeanIndex = 0;
    
    for(;;)
    {            
        if(GeneralParams.EnableMeasure)
        {
            CleanAll = true;
            
            // Se non siamo in simulazione
            if(!GeneralParams.EnableSimulation)   
            {
                while(!ConversionEnd)
                {
                    ADCConvToDMA();                
                }  
                
                if(ConversionEnd)
                {
                    StopADC_DMA_Conv();
                    ConversionEnd = false;
                    
                    AdcMeanRawValue[GrandRawMeanIndex] = CalcArrayAvarage(ADCReadedValue, NUM_SAMPLE * 2);
                    GrandRawMeanIndex++;
                    
                    if(GrandRawMeanIndex == NUM_SAMPLE)
                    {
                        GrandRawMeanIndex = 0;
                        // Calibrazione dell'offset                   
                        if(GeneralParams.ADCOffset == 0)
                        {
                            GeneralParams.ADCOffset = CalcArrayAvarage(AdcMeanRawValue, NUM_SAMPLE);
                        }
                        for(uint8_t ValueIndx = 0; ValueIndx < NUM_SAMPLE; ValueIndx++)
                        {
                            AdcRawDiff = TOVOLT((float)AdcMeanRawValue[ValueIndx]) - TOVOLT((float)GeneralParams.ADCOffset);
                            AdcRawDiff = APROXIMATION(AdcRawDiff, 4);
                            CubeRawValue += (double)(AdcRawDiff * AdcRawDiff);
                        }
                        CurrentRMS[NumberOfCurrentSampling] = CalcCurrent(CubeRawValue);
                        NumberOfCurrentSampling++;
                        CubeRawValue = 0;
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
                    AdcRawDiff = (int32_t)ADCReadedValueSim[ValueIndx] - 2048;
                    CubeRawValue += (double)(AdcRawDiff * AdcRawDiff);
                }
                CurrentRMS[NumberOfCurrentSampling] = CalcCurrent(CubeRawValue);
                NumberOfCurrentSampling++;
                CubeRawValue = 0; 
            }

            if(NumberOfCurrentSampling == CURRENT_SAMPLE)
            {
                GeneralMeasures.MeanCurrentRMS = CalcMeanCurrent(CurrentRMS); 
                GeneralMeasures.MeanCurrentRMS = APROXIMATION(GeneralMeasures.MeanCurrentRMS, 2);
                if(GeneralMeasures.MeanCurrentRMS > 0.30)
                    GeneralMeasures.Power = GeneralMeasures.MeanCurrentRMS * (float)GeneralParams.MeasureVoltage;
                else
                {
                    GeneralMeasures.MeanCurrentRMS = 0.0;
                    GeneralMeasures.Power = 0.0;
                }
                EnergyAcc += GeneralMeasures.Power;
                NumberOfEnergySampling++;
                NumberOfCurrentSampling = 0;                
            } 
            
            // Gestisce gli eventi al secondo
            SecondEvent();
        } 
        else
        {
            if(CleanAll)
            {
                ClearFLArray(CurrentRMS, CURRENT_SAMPLE);
                CheckAlarm();
                GeneralMeasures.MeanCurrentRMS = 0.0;
                GeneralMeasures.Power          = 0.0;
                EnergyAcc                      = 0.0;
                AlarmEnergyLed                 = NO_CONF;
                NumberOfEnergySampling         = 0;
                GeneralParams.ADCOffset        = 0;
                CleanAll = false;
            }
        }
        osDelay(20);
    }
}

#endif // ENABLE_MEASURE