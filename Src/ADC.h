#ifndef ADC_H
#define ADC_H

#define DMA_SAMPLE  20
#define NUM_SAMPLE  126
#define NUM_SAMPLE_CAMP  160


void ADCConvToDMA(void);
uint32_t GetSensorValue(void);

void MX_ADC1_Init(void);
void MX_ADC2_Init(void);

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
void StopADC_DMA_Conv(void);
#endif