#include "main.h"
#include "TaskOled.h"
#include "TaskLed.h"
#include "TaskKeyboard.h"
#include "Oled.h"
#include "Menus.h"
#include "AlarmMachine.h"

#ifdef ENABLE_OLED

extern uint8_t LedConf;
extern uint8_t LastButtonPressed;

extern char Initial_Logo[];

void CheckOperation()
{
    uint8_t AlarmIndex = 0;
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
        while(LastButtonPressed != BUTTON_OK)
        {
            AlarmIndex = LastActiveAlarm();
            osDelay(500);
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