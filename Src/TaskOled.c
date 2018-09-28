#include "main.h"
#include "TaskOled.h"
#include "TaskLed.h"
#include "TaskKeyboard.h"
#include "Oled.h"
#include "Menus.h"

extern uint8_t LedConf;
extern uint8_t LastButtonPressed;

void CheckOperation()
{
    if(LastButtonPressed == BUTTON_OK_LONG)
    {
        if(LedConf == RGB)
            LedConf = ALL_LED_OFF;
        else if(LedConf == ALL_LED_OFF)
            LedConf = RGB;
        LastButtonPressed = NO_PRESS;
    }
}


/* TaskOled function */
void TaskOled(void const * argument)
{
    OledInit();
    
    /* USER CODE BEGIN 5 */
    /* Infinite loop */
    for(;;)
    {
        MainScreen();
    }
    /* USER CODE END 5 */ 
}