#ifndef GPIO_H
#define GPIO_H

typedef enum
{
    LOW = 0,
    HIGH
}DIGITAL_STATE;

void MX_GPIO_Init(void);
#endif