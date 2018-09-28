#include "main.h"
#include "TaskKeyboard.h"
#include "GPIO.h"

FLAG_KEYBOARD FlagButtons;
uint8_t ButtonPress = NO_PRESS;
uint8_t LastButtonPressed = NO_PRESS;
uint16_t ButtonPressCnt = LONG_PRESS_TIME;

static void CheckButtons()
{
    uint8_t UpPressed    = HAL_GPIO_ReadPin(Up_GPIO_Port, Up_Pin);
    uint8_t DownPressed  = HAL_GPIO_ReadPin(Down_GPIO_Port, Down_Pin);
    uint8_t LeftPressed  = HAL_GPIO_ReadPin(Left_GPIO_Port, Left_Pin);
    uint8_t RightPressed = HAL_GPIO_ReadPin(Right_GPIO_Port, Right_Pin);
    uint8_t OkPressed    = HAL_GPIO_ReadPin(Ok_GPIO_Port, Ok_Pin);
    if(UpPressed == HIGH)
    {
        ButtonPress = BUTTON_UP;
        LastButtonPressed = ButtonPress;
        if(FlagButtons.PressedYet)
            ButtonPressCnt--;
        FlagButtons.PressedYet = true;
    }
    else if(DownPressed == HIGH)
    {
        ButtonPress = BUTTON_DOWN;
        LastButtonPressed = ButtonPress;
        if(FlagButtons.PressedYet)
            ButtonPressCnt--;
        FlagButtons.PressedYet = true;
    }
    else if(LeftPressed == HIGH)
    {
        ButtonPress = BUTTON_LEFT;
        LastButtonPressed = ButtonPress;
        if(FlagButtons.PressedYet)
            ButtonPressCnt--;
        FlagButtons.PressedYet = true;
    }
    else if(RightPressed == HIGH)
    {
        ButtonPress = BUTTON_RIGHT;
        LastButtonPressed = ButtonPress;
        if(FlagButtons.PressedYet)
            ButtonPressCnt--;
        FlagButtons.PressedYet = true;
    }
    else if(OkPressed == HIGH)
    {
        ButtonPress = BUTTON_OK;
        LastButtonPressed = ButtonPress;
        if(FlagButtons.PressedYet)
            ButtonPressCnt--;
        FlagButtons.PressedYet = true;
    }
    else
    {
        ButtonPress = NO_PRESS;
        ButtonPressCnt = LONG_PRESS_TIME;
        FlagButtons.PressedYet = false;
    }    
    if(ButtonPressCnt == 0 && FlagButtons.PressedYet)
    {
        ButtonPressCnt = LONG_PRESS_TIME;
        FlagButtons.PressedYet = false;
        switch(ButtonPress)
        {
          case BUTTON_UP   :
            ButtonPress = BUTTON_UP_LONG;
            LastButtonPressed = ButtonPress;
            break;
          case BUTTON_DOWN :
            ButtonPress = BUTTON_DOWN_LONG;
            LastButtonPressed = ButtonPress;
            break;
          case BUTTON_LEFT :
            ButtonPress = BUTTON_LEFT_LONG;
            LastButtonPressed = ButtonPress;
            break;
          case BUTTON_RIGHT:
            ButtonPress = BUTTON_RIGHT_LONG;
            LastButtonPressed = ButtonPress;
            break;
          case BUTTON_OK   :
            ButtonPress = BUTTON_OK_LONG;
            LastButtonPressed = ButtonPress;
            break;              
          default:
            break;
        }
    }
    return;
}


/* TaskKeyboard function */
void TaskKeyboard(void const * argument)
{
    /* USER CODE BEGIN TaskKeyboard */
    /* Infinite loop */
    for(;;)
    {
        osDelay(1);
    }
    /* USER CODE END TaskKeyboard */
}