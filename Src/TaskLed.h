#ifndef TASK_LED_H
#define TASK_LED_H

#define NLED 3

typedef enum
{
    RED_LED = 0,
    GREEN_LED,
    BLUE_LED
}LED_NAME;

typedef enum
{
    RGB = 0,
    RED,
    GREEN,
    BLUE,
    RED_GREEN,
    RED_BLUE,
    GREEN_BLUE,
    ALL_LED_OFF,
    MAX_LED_COMBINATIONS
}LED_COMBINATIONS;

typedef struct
{
    bool IsOn : 1;
}LED_FLAG;

extern uint8_t LedConf;

void TaskLed(void const * argument);
#endif