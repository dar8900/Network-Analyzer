#include "main.h"
#include "TaskMeasure.h"
#include "ADC.h"
#include "SysTime.h"

extern bool ConversionEnd;
extern uint32_t ADCReadedValue[NUM_SAMPLE]; 

/* TaskMeasure function */
void TaskMeasure(void const * argument)
{
    /* USER CODE BEGIN TaskMeasure */
    /* Infinite loop */
    for(;;)
    {
        while(!ConversionEnd)
        {
            ADCConvToDMA();
            DelayMs(1);
        }
        if(ConversionEnd)
        {
            StopADC_DMA_Conv();
            ConversionEnd = false;
        }
        
    }
    /* USER CODE END TaskMeasure */
}