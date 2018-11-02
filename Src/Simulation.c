#include <math.h>
#include "main.h"
#include "TaskMeasure.h"
#include "ADC.h"
#include "Simulation.h"
#include "Parameters.h"

#define  SAMPLE_TO_ANGLE             (NUM_SAMPLE / 360.0)
#define  OMEGA                       (2 * PIGRECO * GeneralParams.Frequency)

uint32_t ADCReadedValueSim[NUM_SAMPLE]; 
float    RawCurrConv;


void SimAdcWave()
{
    uint8_t Index = 0;
    int16_t Value = 0;
    RawCurrConv = GeneralParams.SimulationCurrent * 120;
    for(Index = 0; Index < NUM_SAMPLE; Index++)
    {
        Value = (int16_t)(RawCurrConv* sin((double)TO_RADIANTS((float)SAMPLE_TO_ANGLE * Index) * OMEGA));
        ADCReadedValueSim[Index] = 2048 - Value;
    }    
        
}