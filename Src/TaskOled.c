#include "main.h"
#include "TaskOled.h"
#include "TaskLed.h"
#include "TaskKeyboard.h"
#include "Oled.h"
#include "Menus.h"
#include "AlarmMachine.h"
#include "cmsis_os.h"

#ifdef ENABLE_OLED

extern uint8_t LedConf;
extern uint8_t LastButtonPressed;
extern char *AlarmList[MAX_ALARM_STR];
extern char *AlarmMotivationStr[2];
extern ALARM_CONTROLS AlarmsControls[MAX_ALARM_NUMBER];

extern char Initial_Logo[];

static void WriteTimeDateOccurrenceAlarm(uint8_t AlarmIndex, char *Str1, char *Str2)
{
    snprintf(Str1, 18, "%02d:%02d:%02d %02d/%02d/%02d", AlarmsControls[AlarmIndex].AlarmActivationTime.hours, AlarmsControls[AlarmIndex].AlarmActivationTime.minutes,
             AlarmsControls[AlarmIndex].AlarmActivationTime.seconds,AlarmsControls[AlarmIndex].AlarmActivationDate.day,AlarmsControls[AlarmIndex].AlarmActivationDate.month,
             AlarmsControls[AlarmIndex].AlarmActivationDate.year);
    snprintf(Str2, 3, "%d", AlarmsControls[AlarmIndex].NumbOccurrence);
}


void CheckOperation()
{
    uint8_t AlarmIndex = 0;
    char AlarmTimeDateStr[20], NumberOccurence[3];
    if(LastButtonPressed == BUTTON_OK_LONG)
    {
        if(LedConf == RGB)
            LedConf = ALL_LED_OFF;
        else if(LedConf == ALL_LED_OFF)
            LedConf = RGB;
        LastButtonPressed = NO_PRESS;
    }
    if(AlarmsActive() && !AlarmsChecked())
    {
        while(LastButtonPressed != BUTTON_OK && AlarmsReported())
        {
            AlarmIndex = LastReportedAlarm();
            WriteTimeDateOccurrenceAlarm(AlarmIndex, AlarmTimeDateStr, NumberOccurence);
            PopUp(AlarmList[AlarmIndex], AlarmMotivationStr[AlarmsControls[AlarmIndex].AlarmMotivation], AlarmTimeDateStr, NumberOccurence, " ");
            osDelay(500);
        }
        if(LastButtonPressed == BUTTON_OK)
        {
            LastButtonPressed = NO_PRESS;
            AlarmsControls[AlarmIndex].Cheked = true;
        }
    }
    
    
}


/* TaskOled function */
void TaskOled(void const * argument)
{
    OledInit();
    
    DrawStartLogo(Initial_Logo);
    osDelay(3000);
    
    /* Infinite loop */
    for(;;)
    {
        MainScreen();
    }
}


#endif // ENABLE_OLED