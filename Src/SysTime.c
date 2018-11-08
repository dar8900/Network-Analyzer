#include "main.h"
#include "SysTime.h"
#include "TaskRTC.h"
#include "Parameters.h"

extern __IO uint32_t uwTick;
extern HAL_TickFreqTypeDef uwTickFreq;
extern bool SecondTickMeasure;
extern bool SetChrono;
extern bool ReSetChrono;
extern CHRONO_VAR Crono;

uint32_t  PowerOnTime;

void SystemClock_Config(void)
{

    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInit;
    
    /**Initializes the CPU, AHB and APB busses clocks 
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = 16;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    
    /**Initializes the CPU, AHB and APB busses clocks 
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
        |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    
    /**Configure the Systick interrupt time 
    */
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
    
    /**Configure the Systick 
    */
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    
    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

void HAL_IncTick(void)
{
    uwTick += uwTickFreq;
}

static void GetSecondTick()
{
    TickForSecond += uwTickFreq;
    PowerOnTime++;
    if(SetChrono)
    {
        Crono.millis++;
        if(Crono.millis == 9)
        {
            Crono.millis = 0;
            Crono.cent++;
        }
        if(Crono.cent >= 9)
        {
            Crono.cent = 0;
            Crono.dec++;
        }
        if(Crono.dec >= 9)
        {
            Crono.dec = 0;
            Crono.seconds++;
        }
        if(Crono.seconds >= 59)
        {
            Crono.seconds = 0;
            Crono.minutes++;
        }
        if(Crono.minutes >= 59)
        {
            Crono.minutes = 0;
            Crono.hours++;
        }
        if(Crono.hours >= 23)
        {
            Crono.hours = 0;
            Crono.minutes = 0;
            Crono.seconds = 0;
            Crono.dec = 0;
            Crono.cent = 0;
            Crono.millis = 0;
        }
    }
    if(ReSetChrono)
    {
        SetChrono = false;
        ReSetChrono = false;
        Crono.hours = 0;
        Crono.minutes = 0;
        Crono.seconds = 0;
        Crono.dec = 0;
        Crono.cent = 0;
        Crono.millis = 0;
    }
    if(PowerOnTime == UINT32_MAX)
        PowerOnTime = 0; 
    if(TickForSecond >= 1000)
    {
        SecondTickMeasure = true;  
        TickForSecond = 0;   
    }    
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    /* USER CODE BEGIN Callback 0 */
    
    /* USER CODE END Callback 0 */
    if (htim->Instance == TIM1)
    {
        HAL_IncTick();
        GetSecondTick();
    }
    /* USER CODE BEGIN Callback 1 */
    
    /* USER CODE END Callback 1 */
}

