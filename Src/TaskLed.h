#ifndef TASK_LED_H
#define TASK_LED_H

#define NLED 4

typedef enum
{
    RED_LED = 0,
    GREEN_LED,
    BLUE_LED,
    AL_EN_LED
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

typedef enum
{
    ALARM_RUNNING = 0,
    ENERGY_IMPULSE,
    NO_CONF,
    MAX_GENERAL_CONF
}GENERAL_LED_CONF;


typedef struct
{
    bool IsOn : 1;
}LED_FLAG;

extern uint8_t LedConf;
extern uint8_t AlarmEnergyLed;

void TaskLed(void const * argument);
#endif