/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "icache.h"
#include "memorymap.h"
#include "rtc.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void SystemPower_Config(void);
/* USER CODE BEGIN PFP */
//void HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *hrtc)
//{
//	HAL_ResumeTick();
//}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Enter_Shutdown_Mode(void) {
	// Ensure all wakeup flags are cleared
	PWR->WUSCR |= PWR_WUSCR_CWUF;

	// Enable Wakeup pin (WKUP1) as wakeup source
	PWR->WUCR1 |= PWR_WUCR1_WUPEN2;

	// Select Shutdown mode in power control register
	PWR->CR1 &= ~PWR_CR1_LPMS;
	PWR->CR1 |= (0b11 << PWR_CR1_LPMS_Pos);

	// Set the SLEEPDEEP bit in Cortex System Control Register
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

	// Request Wait For Interrupt
	__WFI();
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* Configure the System Power */
  SystemPower_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ICACHE_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */

	HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, RESET);
	HAL_Delay(100);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		HAL_Delay(100);
//		printf("Going to Sleep Mode\r\n");
		HAL_SuspendTick();
		Enter_Shutdown_Mode();
		HAL_ResumeTick();
		SystemClock_Config();
		HAL_Delay(100);
//		printf("Wakeup from sleep mode\r\n");
		HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
		HAL_Delay(100);
		HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
		HAL_Delay(100);
		HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
		HAL_Delay(100);
		HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
		HAL_Delay(100);
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE4) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Power Configuration
  * @retval None
  */
static void SystemPower_Config(void)
{

  /*
   * Switch to SMPS regulator instead of LDO
   */
  if (HAL_PWREx_ConfigSupply(PWR_SMPS_SUPPLY) != HAL_OK)
  {
    Error_Handler();
  }
  /* PVD_PVM_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(PVD_PVM_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(PVD_PVM_IRQn);
/* USER CODE BEGIN PWR */
/* USER CODE END PWR */
}

/* USER CODE BEGIN 4 */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) {
	RTC_AlarmTypeDef sAlarm;
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;
	__HAL_RTC_ALARM_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);
	HAL_RTC_GetAlarm(hrtc, &sAlarm, RTC_ALARM_A, FORMAT_BIN);
	HAL_RTC_GetTime(hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(hrtc, &sDate, RTC_FORMAT_BIN);
	if (sAlarm.AlarmTime.Seconds > 49) {
		sAlarm.AlarmTime.Seconds = (sAlarm.AlarmTime.Seconds + 10) % 60;
		sAlarm.AlarmTime.Minutes = (sAlarm.AlarmTime.Minutes + 1) % 60;
		if (sAlarm.AlarmTime.Minutes == 0) {
			sAlarm.AlarmTime.Hours = (sAlarm.AlarmTime.Hours + 1) % 24;
		}
	} else {
		sAlarm.AlarmTime.Seconds += 10;
	}
	while (HAL_RTC_SetAlarm_IT(hrtc, &sAlarm, FORMAT_BIN) != HAL_OK) {
	}
	HAL_ResumeTick();
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
