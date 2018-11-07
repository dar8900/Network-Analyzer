#ifndef ALARM_MACHINE_H
#define ALARM_MACHINE_H

#include <stdint.h>
#include <stdbool.h>
#include "TaskRTC.h"

typedef enum
{
    CURRENT_ALARM = 0,
    POWER_ALARM,
    ENERGY_ALARM,
    MAX_ALARM_NUMBER

}ALARM_NUM;


typedef enum
{
    CURRENT_ALARM_STR = 0,
    POWER_ALARM_STR,
    ENERGY_ALARM_STR,
    MAX_ALARM_STR

}ALARM_LIST_STR;


typedef struct
{
    double    *AlarmMeasure;
    double    OverThreshold;
    double    UnderThreshold;
         
}ALARM_PARAMS;

typedef struct
{
    bool     Active;
    bool     Cheked;
    bool     RePorted;
    uint32_t NumbOccurrence;
    TIME_VAR AlarmActivationTimeDate;
    uint8_t  AlarmMotivation;
    
}ALARM_CONTROLS;

enum
{
    OVER_THR = 0,
    UNDER_THR,
    NO_ALARM
};

extern ALARM_PARAMS AlarmsParameters[MAX_ALARM_NUMBER];
extern ALARM_CONTROLS AlarmsControls[MAX_ALARM_NUMBER];

void CheckAlarm(void);
bool AlarmsReported(void);
bool AlarmsActive(void);
bool AlarmsChecked(void);
uint8_t LastReportedAlarm(void);

#endif