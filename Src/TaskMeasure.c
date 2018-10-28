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
extern uint32_t ADCReadedValue[NUM_SAMPLE]; 
extern bool EnableSimulation;

static double CubeRawValue;
static float CurrentRMS[CURRENT_SAMPLE];
static float EnergyAcc;
static uint32_t NumberOfEnergySampling; 

static uint8_t SensorSensibility = 66;

MEASURES GeneralMeasures;


static void ClearFLArray(float Array[], uint8_t Size)
{
    for(uint8_t i = 0; i < Size; i++)
    {
        Array[i] = 0.0;
    }
}


static uint32_t CalcMeanAdcOffset()
{
    uint8_t i = 0;
    uint64_t MeanOffset = 0;
    for(i = 0; i < NUM_SAMPLE; i++)
    {
        MeanOffset += ADCReadedValue[i];
    }
    MeanOffset /= NUM_SAMPLE;
    return (uint32_t)MeanOffset;
}

static float CalcCurrent(double QuadraticValue)
{
    float SquareQuadratic;
    QuadraticValue = (QuadraticValue / NUM_SAMPLE); 
    
    // Estraggo il valore quadratico facendolo diventare RMS e trasformandolo in mV 
    SquareQuadratic = (float)sqrt((double)QuadraticValue);
    
    SquareQuadratic =  (TOVOLT(SquareQuadratic)) * 1000;
    
    // Divido i mV ottenuti per la sensibilita del sensore per ottere gli A 
    SquareQuadratic /= CURR_SENSOR_SENSITIVITY;
    
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
    if(GeneralMeasures.MaxCurrent < GeneralMeasures.MeanCurrentRMS)
        GeneralMeasures.MaxCurrent = GeneralMeasures.MeanCurrentRMS;
    
    if(GeneralMeasures.MinCurrent > GeneralMeasures.MeanCurrentRMS)
    {
        if(GeneralMeasures.MeanCurrentRMS > 0)
            GeneralMeasures.MinCurrent = GeneralMeasures.MeanCurrentRMS;
    }
    
    if(GeneralMeasures.MaxPower < GeneralMeasures.Power)
        GeneralMeasures.MaxPower = GeneralMeasures.Power;
    
    if(GeneralMeasures.MinPower > GeneralMeasures.Power)
    {
        if(GeneralMeasures.Power > 0)
            GeneralMeasures.MinPower = GeneralMeasures.Power;
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
    int32_t AdcRawDiff = 0;
    float OldSimCurrent = 0;
    uint8_t OldFrequency = 0;
    uint16_t Noise = (uint16_t)SENSOR_NOISE_TO_RAW_VAL;
    int32_t AdcCorrection = 0;
    bool NotDoCorrection = false;
    
    /* Infinite loop */
    for(;;)
    {            
        if(GeneralParams.EnableMeasure)
        {
            if(GeneralParams.ADCOffset == 0)
            {
                GeneralParams.ADCOffset = CalcMeanAdcOffset();
                if(!NotDoCorrection && GeneralParams.ADCOffset > 0)
                {
                    AdcCorrection = 2085 - GeneralParams.ADCOffset;
                    GeneralParams.ADCOffset += AdcCorrection;
                    NotDoCorrection = true;
                }
                
            }

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
                    for(uint8_t ValueIndx = 0; ValueIndx < NUM_SAMPLE; ValueIndx++)
                    {
                        AdcRawDiff = (int32_t)(ADCReadedValue[ValueIndx] + AdcCorrection) - (int32_t)GeneralParams.ADCOffset;
                        CubeRawValue += (double)(AdcRawDiff * AdcRawDiff);
                    }
                    CurrentRMS[NumberOfCurrentSampling] = CalcCurrent(CubeRawValue);
                    NumberOfCurrentSampling++;
                    CubeRawValue = 0;
                }
            }
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
                    AdcRawDiff = (int32_t)ADCReadedValue[ValueIndx] - 2048;
                    CubeRawValue += (double)(AdcRawDiff * AdcRawDiff);
                }
                CurrentRMS[NumberOfCurrentSampling] = CalcCurrent(CubeRawValue);
                NumberOfCurrentSampling++;
                CubeRawValue = 0; 
            }

            if(NumberOfCurrentSampling == CURRENT_SAMPLE)
            {
                GeneralMeasures.MeanCurrentRMS = CalcMeanCurrent(CurrentRMS); 
                GeneralMeasures.MeanCurrentRMS = (floor(GeneralMeasures.MeanCurrentRMS *100))/100;
                if(GeneralMeasures.MeanCurrentRMS > 0.11)
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
            ClearFLArray(CurrentRMS, CURRENT_SAMPLE);
            CheckAlarm();
            GeneralMeasures.MeanCurrentRMS = 0.0;
            GeneralMeasures.Power          = 0.0;
            EnergyAcc  = 0.0;
            AlarmEnergyLed = NO_CONF;
            NumberOfEnergySampling = 0;
            GeneralParams.ADCOffset = 0;
        }
        osDelay(20);
    }
    /* USER CODE END TaskMeasure */
}

#endif // ENABLE_MEASURE