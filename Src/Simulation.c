#include <math.h>
#include "main.h"
#include "TaskMeasure.h"
#include "ADC.h"
#include "Simulation.h"
#include "Parameters.h"

extern uint32_t ADCReadedValue[NUM_SAMPLE]; 

#define  RAW_CURRENT_CONV(Current)   (331.52 * Current)
#define  SAMPLE_TO_ANGLE             (NUM_SAMPLE / 360.0)
#define  OMEGA                       (2 * PIGRECO * 50)

float SimCurrentValue = 1.0;

void SimAdcWave()
{
    uint8_t Index = 0;
    int16_t Value = 0;
    while(GeneralParams.ADCOffset == 0)
    {  
        osDelay(100);
    }
    for(Index = 0; Index < NUM_SAMPLE; Index++)
    {
        Value = (int16_t)(RAW_CURRENT_CONV(SimCurrentValue)* sin((double)TO_RADIANTS((float)SAMPLE_TO_ANGLE * Index) * OMEGA));
        ADCReadedValue[Index] = GeneralParams.ADCOffset - Value;
    }    
        
}