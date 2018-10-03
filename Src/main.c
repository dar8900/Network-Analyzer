
/**
******************************************************************************
* @file           : main.c
* @brief          : Main program body
******************************************************************************

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "SysTime.h"
#include "TaskOled.h"
#include "TaskLed.h"
#include "TaskRTC.h"
#include "TaskMeasure.h"
#include "TaskKeyboard.h"
#include "GPIO.h"
#include "ADC.h"
#include "DMA.h"
#include "I2C.h"
#include "Oled.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/





osThreadId OledHandle;
osThreadId KeyboardHandle;
osThreadId MeasureHandle;
osThreadId LedHandle;
osThreadId RTCHandle;

char *ErrorMsg;
uint16_t ErrorLine;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/**
* @brief  The application entry point.
*
* @retval None
*/

static void Setup()
{
    /* MCU Configuration----------------------------------------------------------*/
    
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();
    
    /* Configure the system clock */
    SystemClock_Config();
    
    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_ADC1_Init();
    MX_I2C1_Init();
    
}

static void CreateTask()
{
    /* Create the thread(s) */
    
#ifdef ENABLE_OLED
    /* definition and creation of Oled */
    osThreadDef(Oled, TaskOled, osPriorityNormal, 0, 256);
    OledHandle = osThreadCreate(osThread(Oled), NULL);
#endif
 
#ifdef ENABLE_KEYBOARD    
    /* definition and creation of Keyboard */
    osThreadDef(Keyboard, TaskKeyboard, osPriorityBelowNormal, 0, 64);
    KeyboardHandle = osThreadCreate(osThread(Keyboard), NULL);
#endif 
 
#ifdef ENABLE_MEASURE
    /* definition and creation of Measure */
    osThreadDef(Measure, TaskMeasure, osPriorityHigh, 0, 128);
    MeasureHandle = osThreadCreate(osThread(Measure), NULL);
#endif 

#ifdef ENABLE_LED    
    /* definition and creation of Led */
    osThreadDef(Led, TaskLed, osPriorityLow, 0, 64);
    LedHandle = osThreadCreate(osThread(Led), NULL);
#endif
    
#ifdef ENABLE_RTC
    /* definition and creation of RTC */
    osThreadDef(RTC, TaskRTC, osPriorityHigh, 0, 64);
    RTCHandle = osThreadCreate(osThread(RTC), NULL);
#endif   
 
    return;
}


int main(void)
{
    
    Setup();
    
    CreateTask(); 
    
    /* Start scheduler */
    osKernelStart();
    
    /* We should never get here as control is now taken by the scheduler */
    
    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {  }
    
    
}

/**
* @brief System Clock Configuration
* @retval None
*/



/**
* @brief  Period elapsed callback in non blocking mode
* @note   This function is called  when TIM1 interrupt took place, inside
* HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
* a global variable "uwTick" used as application time base.
* @param  htim : TIM handle
* @retval None
*/


/**
* @brief  This function is executed in case of error occurrence.
* @param  file: The file name as string.
* @param  line: The line in file as a number.
* @retval None
*/
void _Error_Handler(char *file, int line)
{  
    ErrorMsg = file;
    ErrorLine = (uint16_t)line;
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    while(1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{ 
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
    tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
* @}
*/

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
