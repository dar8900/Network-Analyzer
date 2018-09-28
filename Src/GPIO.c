#include "main.h"
#include "GPIO.h"

/** Configure pins as 
* Analog 
* Input 
* Output
* EVENT_OUT
* EXTI
*/

void MX_GPIO_Init(void)
{
    
    GPIO_InitTypeDef GPIO_InitStruct;
    
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, Red_Pin|Blue_Pin|Green_Pin|EnergyLed_Pin, GPIO_PIN_SET);
    
    /*Configure GPIO pins : Red_Pin Blue_Pin Green_Pin EnergyLed_Pin */
    GPIO_InitStruct.Pin = Red_Pin|Blue_Pin|Green_Pin|EnergyLed_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /*Configure GPIO pins : Up_Pin Down_Pin */
    GPIO_InitStruct.Pin = Up_Pin|Down_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /*Configure GPIO pins : Left_Pin Right_Pin Ok_Pin */
    GPIO_InitStruct.Pin = Left_Pin|Right_Pin|Ok_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
}