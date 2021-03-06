#include "main.h"
#include "TaskOled.h"
#include "TaskLed.h"
#include "TaskKeyboard.h"
#include "Display.h"
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

uint32_t WDogDisplay = TIMEOUT_WDOG;

void WriteTimeDateOccurrenceAlarm(uint8_t AlarmIndex, char *Str1, char *Str2)
{
    snprintf(Str1, 18, "%02d:%02d:%02d %02d/%02d/%02d", AlarmsControls[AlarmIndex].AlarmActivationTimeDate.hours, AlarmsControls[AlarmIndex].AlarmActivationTimeDate.minutes,
             AlarmsControls[AlarmIndex].AlarmActivationTimeDate.seconds,AlarmsControls[AlarmIndex].AlarmActivationTimeDate.day,AlarmsControls[AlarmIndex].AlarmActivationTimeDate.month,
             AlarmsControls[AlarmIndex].AlarmActivationTimeDate.year);
    snprintf(Str2, 3, "%d", AlarmsControls[AlarmIndex].NumbOccurrence);
}

void WDogOsDelay(uint32_t MSec)
{
    while(MSec >= WHILE_LOOP_DELAY)
    {
        WDogDisplay = TIMEOUT_WDOG;
        osDelay(WHILE_LOOP_DELAY);
        MSec -= WHILE_LOOP_DELAY;
    }
    if(MSec > 0)
    {
        WDogDisplay = TIMEOUT_WDOG;
        osDelay(MSec);
    }
    return;
}

void ManageWDog()
{
    if(WDogDisplay == 0)
        HAL_NVIC_SystemReset();        
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
            AlarmEnergyLed = ALARM_RUNNING;
            WDogOsDelay(500);
        }
        if(LastButtonPressed == BUTTON_OK)
        {
            LastButtonPressed = NO_PRESS;
            AlarmsControls[AlarmIndex].Cheked = true;
            AlarmEnergyLed = NO_CONF;
        }
        else
        {           
            AlarmEnergyLed = NO_CONF;            
        }           
            
    }
    
    
}


/* TaskOled function */
void TaskOled(void const * argument)
{
    OledInit();
    ClearScreen();
    WDogOsDelay(500);
    DrawStartLogo(Initial_Logo);
    WDogOsDelay(1000);
    
    /* Infinite loop */
    for(;;)
    {
        MainScreen();
        MainMenu();
    }
}


#endif // ENABLE_OLED