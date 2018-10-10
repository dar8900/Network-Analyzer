#include "main.h"
#include "TaskLed.h"
#include "GPIO.h"

#ifdef ENABLE_LED

uint8_t LedConf = ALL_LED_OFF;
uint8_t AlarmEnergyLed = NO_CONF;

LED_FLAG LedFlag[NLED];

static bool TurnOffLed(uint8_t WichLed)
{
    bool SwitchSuccess = false;
    switch(WichLed)
    {
      case RED_LED:
        if(LedFlag[RED_LED].IsOn)
        {
            HAL_GPIO_WritePin(Red_GPIO_Port, Red_Pin, LOW);
            LedFlag[RED_LED].IsOn = false;
        }
        SwitchSuccess = true;
        break;
      case GREEN_LED:
        if(LedFlag[GREEN_LED].IsOn)
        {
            HAL_GPIO_WritePin(Green_GPIO_Port, Green_Pin, LOW);
            LedFlag[GREEN_LED].IsOn = false;
        }
        SwitchSuccess = true;
        break;
      case BLUE_LED:
        if(LedFlag[BLUE_LED].IsOn)
        {
            HAL_GPIO_WritePin(Blue_GPIO_Port, Blue_Pin, LOW);
            LedFlag[BLUE_LED].IsOn = false;
        }
        SwitchSuccess = true;
        break;
      case AL_EN_LED:
        if(LedFlag[AL_EN_LED].IsOn)
        {
            HAL_GPIO_WritePin(EnergyLed_GPIO_Port, EnergyLed_Pin, LOW);
            LedFlag[AL_EN_LED].IsOn = false;
        }
        SwitchSuccess = true;
        break;
      default:
        break;
    }
    return SwitchSuccess;
}


static bool TurnOnLed(uint8_t WichLed)
{
    bool SwitchSuccess = false;
    switch(WichLed)
    {
      case RED_LED:
        if(!LedFlag[RED_LED].IsOn)
        {
            HAL_GPIO_WritePin(Red_GPIO_Port, Red_Pin, HIGH);
            LedFlag[RED_LED].IsOn = true;
        }
        SwitchSuccess = true;
        break;
      case GREEN_LED:
        if(!LedFlag[GREEN_LED].IsOn)
        {
            HAL_GPIO_WritePin(Green_GPIO_Port, Green_Pin, HIGH);
            LedFlag[GREEN_LED].IsOn = true;
        }
        SwitchSuccess = true;
        break;
      case BLUE_LED:
        if(!LedFlag[BLUE_LED].IsOn)
        {
            HAL_GPIO_WritePin(Blue_GPIO_Port, Blue_Pin, HIGH);
            LedFlag[BLUE_LED].IsOn = true;
        }
        SwitchSuccess = true;
        break;
      case AL_EN_LED:
        if(!LedFlag[AL_EN_LED].IsOn)
        {
            HAL_GPIO_WritePin(EnergyLed_GPIO_Port, EnergyLed_Pin, HIGH);
            LedFlag[AL_EN_LED].IsOn = true;
        }
        SwitchSuccess = true;
        break;
      default:
        break;
    }
    return SwitchSuccess;
}

static void GesRGBLed()
{
    switch(LedConf)
    {
      case RGB:
        TurnOnLed(RED_LED);
        TurnOnLed(GREEN_LED);
        TurnOnLed(BLUE_LED);
        break;
      case RED:
        TurnOnLed(RED_LED);
        TurnOffLed(GREEN_LED);
        TurnOffLed(BLUE_LED);
        break;
      case GREEN:
        TurnOnLed(GREEN_LED);
        TurnOffLed(RED_LED);
        TurnOffLed(BLUE_LED);
        break;
      case BLUE:
        TurnOnLed(BLUE_LED);
        TurnOffLed(RED_LED);
        TurnOffLed(GREEN_LED);
        break;
      case RED_GREEN:
        TurnOnLed(RED_LED);
        TurnOnLed(GREEN_LED);
        TurnOffLed(BLUE_LED);
        break;
      case RED_BLUE:
        TurnOnLed(BLUE_LED);
        TurnOnLed(RED_LED);
        TurnOffLed(GREEN_LED);
        break;
      case GREEN_BLUE:
        TurnOnLed(BLUE_LED);
        TurnOnLed(GREEN_LED);
        TurnOffLed(RED_LED);
        break;
      case ALL_LED_OFF:
        TurnOffLed(RED_LED);
        TurnOffLed(GREEN_LED);
        TurnOffLed(BLUE_LED);
        break;
      default:
        break;        
    }
}

static void GesAlarmEnergyLed()
{
    switch(AlarmEnergyLed)
    {
      case ALARM_RUNNING:
        TurnOnLed(AL_EN_LED);
        osDelay(200);
        TurnOffLed(AL_EN_LED);
        break;
      case ENERGY_IMPULSE:
        TurnOnLed(AL_EN_LED);
        osDelay(5);
        TurnOffLed(AL_EN_LED);
        break;
      case NO_CONF:
        break;
      default:
        break;
    }
    
}

/* TaskLed function */
void TaskLed(void const * argument)
{
    /* USER CODE BEGIN TaskLed */
    
//    LedConf = RGB;
//    GesLed();
//    LedConf = ALL_LED_OFF;
    
    
    /* Infinite loop */
    for(;;)
    {
        GesRGBLed();
        GesAlarmEnergyLed();
        osDelay(50);
    }
    /* USER CODE END TaskLed */
}


#endif // ENABLE_LED