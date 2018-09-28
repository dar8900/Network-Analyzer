#include "main.h"
#include "TaskMeasure.h"
#include "ADC.h"
#include "SysTime.h"

#ifdef ENABLE_MEASURE

extern bool ConversionEnd;
extern uint32_t ADCReadedValue[NUM_SAMPLE]; 

bool EnableMeasure = false;

/* TaskMeasure function */
void TaskMeasure(void const * argument)
{
    /* USER CODE BEGIN TaskMeasure */
    /* Infinite loop */
    for(;;)
    {
        if(EnableMeasure)
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
        osDelay(1);
    }
    /* USER CODE END TaskMeasure */
}

#endif // ENABLE_MEASURE