#ifndef I2C_H
#define I2C_H

#define	STM32_HAL_I2C_TIMEOUT	2000

extern I2C_HandleTypeDef hi2c1;

void MX_I2C1_Init(void);

#endif