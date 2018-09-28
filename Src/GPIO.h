#ifndef GPIO_H
#define GPIO_H

typedef enum
{
    HIGH = 0,
    LOW
}DIGITAL_STATE;

void MX_GPIO_Init(void);
#endif