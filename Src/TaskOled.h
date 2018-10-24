#ifndef TASK_OLED_H
#define TASK_OLED_H

void CheckOperation(void);
void WriteTimeDateOccurrenceAlarm(uint8_t AlarmIndex, char *Str1, char *Str2);
void TaskOled(void const * argument);
#endif