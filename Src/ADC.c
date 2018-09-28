#include "main.h"
#include "ADC.h"

ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

bool ConversionEnd = false;
uint32_t ADCReadedValue[NUM_SAMPLE]; 

static uint32_t DMABuff[NUM_SAMPLE];

/* ADC1 init function */
void MX_ADC1_Init(void)
{
    
    ADC_ChannelConfTypeDef sConfig;
    
    /**Common config 
    */
    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.ContinuousConvMode = ENABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    
    /**Configure Regular Channel 
    */
    sConfig.Channel = ADC_CHANNEL_1;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    HAL_ADCEx_Calibration_Start(&hadc1);
}

/* ADC2 init function */
void MX_ADC2_Init(void)
{
    
    ADC_ChannelConfTypeDef sConfig;
    
    /**Common config 
    */
    hadc2.Instance = ADC2;
    hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc2.Init.ContinuousConvMode = ENABLE;
    hadc2.Init.DiscontinuousConvMode = DISABLE;
    hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc2.Init.NbrOfConversion = 1;
    if (HAL_ADC_Init(&hadc2) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    
    /**Configure Regular Channel 
    */
    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
    if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    HAL_ADCEx_Calibration_Start(&hadc2);
    
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    memcpy (ADCReadedValue, DMABuff,  NUM_SAMPLE);
    ConversionEnd = true;
}

void ADCConvToDMA()
{
    HAL_ADC_Start_DMA(&hadc1, DMABuff, NUM_SAMPLE);
}
void StopADC_DMA_Conv()
{
    HAL_ADC_Stop_DMA(&hadc1);
}

uint32_t GetSensorValue()
{
    uint32_t ReadedValue = 0;
    HAL_ADC_Start(&hadc2);
    HAL_ADC_PollForConversion(&hadc2, 2);
    ReadedValue = HAL_ADC_GetValue(&hadc2);
    HAL_ADC_Stop(&hadc2);
    return ReadedValue;
}
