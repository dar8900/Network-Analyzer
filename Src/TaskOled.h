#ifndef TASK_OLED_H
#define TASK_OLED_H

#define TIMEOUT_WDOG    3000

void CheckOperation(void);
void WriteTimeDateOccurrenceAlarm(uint8_t AlarmIndex, char *Str1, char *Str2);
void WDogOsDelay(uint32_t MSec);
void ManageWDog(void);
void TaskOled(void const * argument);
#endif