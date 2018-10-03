#include "main.h"
#include "TaskMeasure.h"
#include "ADC.h"
#include "SysTime.h"
#include "Graphix.h"
#include <math.h>

#ifdef ENABLE_MEASURE

extern bool ConversionEnd;
extern bool SecondTick;
extern uint32_t ADCReadedValue[NUM_SAMPLE]; 

bool EnableMeasure = false;

#ifdef SIM_SIN_WAVE
int16_t SinTestGraphic[NUM_TEST_SAMPLE];
#endif

uint64_t CubeRawValue;
float CurrentRMS;
float Power;
float MeanEnergy, EnergyAcc;


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

static float CalcCurrent(uint64_t QuadraticValue, uint8_t NSampling)
{
    float SquareQuadratic;
    QuadraticValue = (QuadraticValue / (NUM_SAMPLE * NSampling)); 
    SquareQuadratic = (float)sqrt((double)QuadraticValue);
    return SquareQuadratic;
}

uint32_t TimeCtrl;

/* TaskMeasure function */
void TaskMeasure(void const * argument)
{
        
    uint8_t NumberOfCurrentSampling = 0;
    uint32_t NumberOfEnergySampling = 0;  

#ifdef SIM_SIN_WAVE    
    FillTestArray();
#endif
    
    /* Infinite loop */
    for(;;)
    {
        
        if(EnableMeasure)
        {
            TimeCtrl = HAL_GetTick();
            while(!ConversionEnd)
            {
                ADCConvToDMA();                
            }
            TimeCtrl -= HAL_GetTick();
            
            if(ConversionEnd)
            {
                StopADC_DMA_Conv();
                ConversionEnd = false;
                NumberOfCurrentSampling++;
                for(uint8_t ValueIndx = 0; ValueIndx < NUM_SAMPLE; ValueIndx++)
                {
                    CubeRawValue += (ADCReadedValue[ValueIndx] * ADCReadedValue[ValueIndx]);
                }
            }
            if(NumberOfCurrentSampling == 4)
            {
                CurrentRMS = CalcCurrent(CubeRawValue, NumberOfCurrentSampling);
                Power = CurrentRMS * VOLTAGE_VALUE;
                EnergyAcc += Power;
                NumberOfEnergySampling++;
                NumberOfCurrentSampling = 0;                
            } 
            if(SecondTick)
            {
                if(NumberOfEnergySampling > 0)
                    MeanEnergy += (EnergyAcc / NumberOfEnergySampling);
                NumberOfEnergySampling = 0;
                EnergyAcc = 0;
            }
        }     
    }
    /* USER CODE END TaskMeasure */
}

#endif // ENABLE_MEASURE