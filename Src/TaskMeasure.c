#include <math.h>
#include "main.h"
#include "TaskMeasure.h"
#include "ADC.h"
#include "SysTime.h"
#include "Graphix.h"
#include "Parameters.h"
#include "AlarmMachine.h"
#include "TaskLed.h"


#ifdef ENABLE_MEASURE

#define CURRENT_SAMPLE  40

extern bool ConversionEnd;
extern bool SecondTick;
extern uint32_t ADCReadedValue[NUM_SAMPLE]; 


#ifdef SIM_SIN_WAVE
int16_t SinTestGraphic[NUM_TEST_SAMPLE];
#endif

static uint64_t CubeRawValue;
static float CurrentRMS[CURRENT_SAMPLE];
static float EnergyAcc;
static uint32_t NumberOfEnergySampling; 
MEASURES GeneralMeasures;



#ifdef SIM_SIN_WAVE
void FillTestArray()
{
    uint8_t NCamp = 0;
    uint16_t Angle = 0;
    float SinCalc = 0.0;
    for(NCamp = 0; NCamp < NUM_TEST_SAMPLE; NCamp++)
    {
        Angle = NCamp * 4;
        SinCalc = sin(TO_RADIANTS(Angle));
        SinTestGraphic[NCamp] = (int16_t)(SinCalc * INT16_SCALE);
    }   
}
#endif

static void ClearFLArray(float Array[], uint8_t Size)
{
    for(uint8_t i = 0; i < Size; i++)
    {
        Array[i] = 0.0;
    }
}


static float CalcCurrent(uint64_t QuadraticValue)
{
    float SquareQuadratic;
    QuadraticValue = (QuadraticValue / NUM_SAMPLE); 
    
    // Estraggo il valore quadratico facendolo diventare RMS e trasformandolo in mV 
    SquareQuadratic = (float)sqrt((double)QuadraticValue);
    
    SquareQuadratic =  (TOVOLT(SquareQuadratic)) * 1000;
    
    // Divido i V ottenuti per la sensibilita del sensore per ottere gli A 
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
    static bool RecMaxMin = true;
    if(RecMaxMin && GeneralMeasures.MeanCurrentRMS > 0 && GeneralMeasures.Power > 0)
    {
        GeneralMeasures.MaxCurrent = GeneralMeasures.MeanCurrentRMS;
        GeneralMeasures.MinCurrent = GeneralMeasures.MeanCurrentRMS;
        GeneralMeasures.MaxPower = GeneralMeasures.Power;
        GeneralMeasures.MinPower = GeneralMeasures.Power;
        RecMaxMin = false;
    }
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
//    GeneralParams.MeasureVoltage = VOLTAGE_VALUE_DFLT;
        
#ifdef SIM_SIN_WAVE    
    FillTestArray();
#endif
    
    
    /* Infinite loop */
    for(;;)
    {
        
        if(GeneralParams.EnableMeasure)
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
                    CubeRawValue += ((ADCReadedValue[ValueIndx] - GeneralParams.ADCOffset) * (ADCReadedValue[ValueIndx] - GeneralParams.ADCOffset));
                }
                CurrentRMS[NumberOfCurrentSampling] = CalcCurrent(CubeRawValue);
                NumberOfCurrentSampling++;
                CubeRawValue = 0;
            }
            if(NumberOfCurrentSampling == CURRENT_SAMPLE)
            {
                GeneralMeasures.MeanCurrentRMS = CalcMeanCurrent(CurrentRMS);            
                if(GeneralMeasures.MeanCurrentRMS > 0.114)
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
        }
        osDelay(20);
    }
    /* USER CODE END TaskMeasure */
}

#endif // ENABLE_MEASURE