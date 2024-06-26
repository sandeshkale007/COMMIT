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
#include "gpdma.h"
#include "icache.h"
#include "usart.h"
#include "memorymap.h"
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
uint8_t buffer1[4];
unsigned char buf[4];
unsigned char pos_0;
unsigned char pos_1;
unsigned char pos_2;
unsigned char pos_3;
uint32_t average = 0;
uint16_t avg[3] = { 0 };
unsigned char rxBuffer[6];  // Buffer to hold received data
uint16_t distance;
char buffer[18];
int sum;
unsigned char test[1];
uint16_t distance;
char buffer[18];

uint8_t flag = 0;

uint8_t lower_limit[] = "Below The Lower Limit\r\n";
uint8_t error_read[] = "Reading Error\r\n";
uint8_t queue_fail[] = "Failed to Receive\r\n";
uint8_t queue_full[] = "Queue is Full\r\n";
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void SystemPower_Config(void);
/* USER CODE BEGIN PFP */
void send_task_callback();
void Enter_Shutdown_Mode(void) {
	// Ensure all wakeup flags are cleared
	PWR->WUSCR |= PWR_WUSCR_CWUF;

	// Enable Wakeup pin (WKUP1) as wakeup source
	PWR->WUCR1 |= PWR_WAKEUP_PIN2_HIGH_1;

	// Select Shutdown mode in power control register
	PWR->CR1 &= ~PWR_CR1_LPMS;
	PWR->CR1 |= (0b11 << PWR_CR1_LPMS_Pos);

	// Set the SLEEPDEEP bit in Cortex System Control Register
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

	// Request Wait For Interrupt
	__WFI();
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

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
	MX_GPDMA1_Init();
	MX_ICACHE_Init();
	MX_UART4_Init();
	MX_UART5_Init();
	MX_LPUART1_UART_Init();
	MX_USART1_UART_Init();
	MX_USART3_UART_Init();
	/* USER CODE BEGIN 2 */

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, 1);
		for (int i = 0; i < 3; i++) {
			HAL_UART_Receive(&huart3, rxBuffer, 4, HAL_MAX_DELAY);
			if (rxBuffer[0] == 0xff) {
				distance = rxBuffer[1] * 256 + rxBuffer[2];
				if (distance == 250 || distance >= 65280) {
					i = -1;
					average = 0;
				} else {

					average = distance + average;
				}
			} else {
				HAL_UART_Receive(&huart3, &flag, 1, 100);
				i = -1;
				average = 0;
			}
		}

		sprintf(buffer, "Distance: %.1f\r\n", (float) average / 30);
		HAL_UART_Transmit(&huart1, (uint8_t*) buffer, sizeof(buffer),
				HAL_MAX_DELAY);
		HAL_Delay(100);
		HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, 0);
		HAL_Delay(100);
		HAL_SuspendTick();
		Enter_Shutdown_Mode();
		HAL_ResumeTick();
		SystemClock_Config();
		HAL_Delay(100);
		SystemPower_Config();
		HAL_Delay(100);

	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1)
			!= HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_0;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
	RCC_OscInitStruct.PLL.PLLMBOOST = RCC_PLLMBOOST_DIV4;
	RCC_OscInitStruct.PLL.PLLM = 3;
	RCC_OscInitStruct.PLL.PLLN = 8;
	RCC_OscInitStruct.PLL.PLLP = 2;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 1;
	RCC_OscInitStruct.PLL.PLLRGE = RCC_PLLVCIRANGE_1;
	RCC_OscInitStruct.PLL.PLLFRACN = 0;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_PCLK3;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV8;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief Power Configuration
 * @retval None
 */
static void SystemPower_Config(void) {

	/*
	 * Switch to SMPS regulator instead of LDO
	 */
	if (HAL_PWREx_ConfigSupply(PWR_SMPS_SUPPLY) != HAL_OK) {
		Error_Handler();
	}
	/* PWR_S3WU_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(PWR_S3WU_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(PWR_S3WU_IRQn);
	/* USER CODE BEGIN PWR */
	/* USER CODE END PWR */
}

/* USER CODE BEGIN 4 */
void send_task_callback() {
	// Infinite loop for the task
	uint16_t distance;
	HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);
	for (int i = 0; i < 5; i++) {
		for (;;) {
//			HAL_UART_Receive_DMA(&huart4, buf, 4);
			HAL_UART_Receive(&huart4, buf, 4, 1000);
			HAL_Delay(1000);
			if (buf[0] != 0xff || (buf[0] == buf[3])) {
				HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin,
						GPIO_PIN_RESET);
				HAL_Delay(50);
				HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin,
						GPIO_PIN_SET);
				HAL_Delay(300);
			} else {
				break;
			}
		}

		if (buf[0] == 0xff) {
			distance = buf[1] * 256 + buf[2];
			if (distance == 250 || distance == 0 || distance >= 65280) {
				flag = 3;
				break;
			} else {
				flag = 1;
			}
		} else {
			flag = 2;
//			break;
		}
		if (flag == 3) {
			printf("%s\r\n", lower_limit);
			HAL_Delay(100);
			flag = 0;
		} else if (flag == 1) {
			printf("Distance: %d\r\n", distance);
			HAL_Delay(100);
			flag = 0;
		} else {
			printf("%s", error_read);
			flag = 0;
		}
		HAL_Delay(300);
	}
	HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);

}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
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
void assert_failed(uint8_t *file, uint32_t line) {
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
