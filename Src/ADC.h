#ifndef ADC_H
#define ADC_H

#define NUM_SAMPLE  126

void ADCConvToDMA(void);
uint32_t GetSensorValue(void);

void MX_ADC1_Init(void);
void MX_ADC2_Init(void);

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
void StopADC_DMA_Conv(void);
#endif