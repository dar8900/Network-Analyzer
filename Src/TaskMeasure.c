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

#define CURRENT_SAMPLE  60

extern bool ConversionEnd;
extern bool SecondTick;
extern uint32_t ADCReadedValue[NUM_SAMPLE]; 


#ifdef SIM_SIN_WAVE
int16_t SinTestGraphic[NUM_TEST_SAMPLE];
#endif

static uint64_t CubeRawValue;
static float CurrentRMS[CURRENT_SAMPLE];
static float EnergyAcc;

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


static void SecondEvent(uint32_t *NumberOfEnergySampling)
{
    static bool NotReEnter = false;
    if(SecondTick)
    {
        if(!NotReEnter)
        {
            if(*NumberOfEnergySampling > 0)
                GeneralMeasures.MeanEnergy += ((EnergyAcc / *NumberOfEnergySampling)/3600.0);
            *NumberOfEnergySampling = 0;
            EnergyAcc = 0;
            CheckAlarm();
            if(AlarmEnergyLed == NO_CONF && !AlarmsReported())
            {
                AlarmEnergyLed = ENERGY_IMPULSE;
            }
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
    uint32_t NumberOfEnergySampling = 0; 
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
                if(GeneralMeasures.MeanCurrentRMS > 0.1)
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
            SecondEvent(&NumberOfEnergySampling);
        } 
        else
        {
            ClearFLArray(CurrentRMS, CURRENT_SAMPLE);
            CheckAlarm();
            GeneralMeasures.MeanCurrentRMS = 0.0;
            GeneralMeasures.Power          = 0.0;
//            GeneralMeasures.MeanEnergy     = 0.0;
            EnergyAcc  = 0.0;
            AlarmEnergyLed = NO_CONF;
        }
        osDelay(20);
    }
    /* USER CODE END TaskMeasure */
}

#endif // ENABLE_MEASURE