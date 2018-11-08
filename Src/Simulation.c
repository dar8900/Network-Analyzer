#include <math.h>
#include "main.h"
#include "TaskMeasure.h"
#include "ADC.h"
#include "Simulation.h"
#include "Parameters.h"

#define  SAMPLE_TO_ANGLE             2.88
#define  OMEGA                       (2 * PIGRECO * GeneralParams.Frequency)

extern float OldSimCurrent;

uint32_t ADCReadedValueSim[NUM_SAMPLE]; 
float    RawCurrConv;
bool Shift;
uint8_t RawCorr = 134;

void SimAdcWave()
{
    uint8_t Index = 0;
    int16_t Value = 0;
    if(GeneralParams.EnableSimulation)
        RawCurrConv = GeneralParams.SimulationCurrent * RawCorr;
    else
    {
        if(!Shift)
            Shift = true;
        else
            Shift = false;
        RawCurrConv = GeneralMeasures.MeanCurrentRMS * RawCorr;
    }
    for(Index = 0; Index < NUM_SAMPLE; Index++)
    {
        if(Shift)
            Value = (int16_t)(RawCurrConv* sin((double)TO_RADIANTS(((double)SAMPLE_TO_ANGLE * Index + 60)) * (double)GeneralParams.Frequency));
        else
            Value = (int16_t)(RawCurrConv* sin((double)TO_RADIANTS(((double)SAMPLE_TO_ANGLE * Index)) * (double)GeneralParams.Frequency));
        ADCReadedValueSim[Index] = 2048 + Value;
    }    
        
}