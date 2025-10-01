/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum {
  STATE_NS_GREEN,   // North-South green
  STATE_NS_YELLOW,  // North-South yellow
  STATE_EW_GREEN,   // East-West green
  STATE_EW_YELLOW   // East-West yellow
} traffic_state_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static const uint32_t time_green_s  = 3;
static const uint32_t time_yellow_s = 2;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
static void setTrafficLight(GPIO_TypeDef* RED_Port, uint16_t RED_Pin,
                            GPIO_TypeDef* YELLOW_Port, uint16_t YELLOW_Pin,
                            GPIO_TypeDef* GREEN_Port, uint16_t GREEN_Pin,
                            GPIO_PinState red, GPIO_PinState yellow, GPIO_PinState green);
static void display7SEG(int num);
static void traffic_light_init(void);
static void traffic_light_loop_once(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// Turn ON/OFF LEDs for one direction
static void setTrafficLight(GPIO_TypeDef* RED_Port, uint16_t RED_Pin,
                            GPIO_TypeDef* YELLOW_Port, uint16_t YELLOW_Pin,
                            GPIO_TypeDef* GREEN_Port, uint16_t GREEN_Pin,
                            GPIO_PinState red, GPIO_PinState yellow, GPIO_PinState green)
{
  HAL_GPIO_WritePin(RED_Port, RED_Pin, red);
  HAL_GPIO_WritePin(YELLOW_Port, YELLOW_Pin, yellow);
  HAL_GPIO_WritePin(GREEN_Port, GREEN_Pin, green);
}
// Show a number 0-9 on 7-segment
static void display7SEG(int num) {
	static const uint8_t seg_code[10] = {
    	    0xC0, // 0
    	    0xF9, // 1
    	    0xA4, // 2
    	    0xB0, // 3
    	    0x99, // 4
    	    0x92, // 5
    	    0x82, // 6
    	    0xF8, // 7
    	    0x80, // 8
    	    0x90  // 9
    };

    if (num < 0 || num > 9) return;

    uint8_t pattern = seg_code[num];

    HAL_GPIO_WritePin(SEG_A_GPIO_Port, SEG_A_Pin, (pattern & (1<<0)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_B_GPIO_Port, SEG_B_Pin, (pattern & (1<<1)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_C_GPIO_Port, SEG_C_Pin, (pattern & (1<<2)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_D_GPIO_Port, SEG_D_Pin, (pattern & (1<<3)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_E_GPIO_Port, SEG_E_Pin, (pattern & (1<<4)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_F_GPIO_Port, SEG_F_Pin, (pattern & (1<<5)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_G_GPIO_Port, SEG_G_Pin, (pattern & (1<<6)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
// Init lights: NS green, EW red
static void traffic_light_init(void)
{
  // NS: RED off, YELLOW off, GREEN on
  setTrafficLight(LED_RED_N_GPIO_Port, LED_RED_N_Pin,
                  LED_YELLOW_N_GPIO_Port, LED_YELLOW_N_Pin,
                  LED_GREEN_N_GPIO_Port, LED_GREEN_N_Pin,
                  1, 1, 0);
  setTrafficLight(LED_RED_S_GPIO_Port, LED_RED_S_Pin,
                  LED_YELLOW_S_GPIO_Port, LED_YELLOW_S_Pin,
                  LED_GREEN_S_GPIO_Port, LED_GREEN_S_Pin,
				  1, 1, 0);

  // EW: RED on, YELLOW off, GREEN off
  setTrafficLight(LED_RED_E_GPIO_Port, LED_RED_E_Pin,
                  LED_YELLOW_E_GPIO_Port, LED_YELLOW_E_Pin,
                  LED_GREEN_E_GPIO_Port, LED_GREEN_E_Pin,
                  0, 1, 1);
  setTrafficLight(LED_RED_W_GPIO_Port, LED_RED_W_Pin,
                  LED_YELLOW_W_GPIO_Port, LED_YELLOW_W_Pin,
                  LED_GREEN_W_GPIO_Port, LED_GREEN_W_Pin,
				  0, 1, 1);
}
// One loop of traffic light
static void traffic_light_loop_once(void)
{
	static traffic_state_t state = STATE_NS_GREEN;
	static int countdown = time_green_s;

	if (countdown <= 0)
	{
		switch (state)
		{
		case STATE_NS_GREEN: // NS green -> NS yellow
			setTrafficLight(LED_RED_N_GPIO_Port, LED_RED_N_Pin,
							LED_YELLOW_N_GPIO_Port, LED_YELLOW_N_Pin,
							LED_GREEN_N_GPIO_Port, LED_GREEN_N_Pin,
							1, 0, 1);
			setTrafficLight(LED_RED_S_GPIO_Port, LED_RED_S_Pin,
							LED_YELLOW_S_GPIO_Port, LED_YELLOW_S_Pin,
							LED_GREEN_S_GPIO_Port, LED_GREEN_S_Pin,
							1, 0, 1);
			setTrafficLight(LED_RED_E_GPIO_Port, LED_RED_E_Pin,
							LED_YELLOW_E_GPIO_Port, LED_YELLOW_E_Pin,
							LED_GREEN_E_GPIO_Port, LED_GREEN_E_Pin,
							0, 1, 1);
			setTrafficLight(LED_RED_W_GPIO_Port, LED_RED_W_Pin,
							LED_YELLOW_W_GPIO_Port, LED_YELLOW_W_Pin,
							LED_GREEN_W_GPIO_Port, LED_GREEN_W_Pin,
							0, 1, 1);

			state = STATE_NS_YELLOW;
			countdown = time_yellow_s;
			break;

		case STATE_NS_YELLOW: // NS yellow -> EW green
			setTrafficLight(LED_RED_E_GPIO_Port, LED_RED_E_Pin,
							LED_YELLOW_E_GPIO_Port, LED_YELLOW_E_Pin,
							LED_GREEN_E_GPIO_Port, LED_GREEN_E_Pin,
							1, 1, 0);
			setTrafficLight(LED_RED_W_GPIO_Port, LED_RED_W_Pin,
							LED_YELLOW_W_GPIO_Port, LED_YELLOW_W_Pin,
							LED_GREEN_W_GPIO_Port, LED_GREEN_W_Pin,
							1, 1, 0);
			setTrafficLight(LED_RED_N_GPIO_Port, LED_RED_N_Pin,
							LED_YELLOW_N_GPIO_Port, LED_YELLOW_N_Pin,
							LED_GREEN_N_GPIO_Port, LED_GREEN_N_Pin,
							0, 1, 1);
			setTrafficLight(LED_RED_S_GPIO_Port, LED_RED_S_Pin,
							LED_YELLOW_S_GPIO_Port, LED_YELLOW_S_Pin,
							LED_GREEN_S_GPIO_Port, LED_GREEN_S_Pin,
							0, 1, 1);

			state = STATE_EW_GREEN;
			countdown = time_green_s;
			break;

		case STATE_EW_GREEN: // EW green -> EW yellow
			setTrafficLight(LED_RED_E_GPIO_Port, LED_RED_E_Pin,
							LED_YELLOW_E_GPIO_Port, LED_YELLOW_E_Pin,
							LED_GREEN_E_GPIO_Port, LED_GREEN_E_Pin,
							1, 0, 1);
			setTrafficLight(LED_RED_W_GPIO_Port, LED_RED_W_Pin,
							LED_YELLOW_W_GPIO_Port, LED_YELLOW_W_Pin,
							LED_GREEN_W_GPIO_Port, LED_GREEN_W_Pin,
							1, 0, 1);
			setTrafficLight(LED_RED_N_GPIO_Port, LED_RED_N_Pin,
							LED_YELLOW_N_GPIO_Port, LED_YELLOW_N_Pin,
							LED_GREEN_N_GPIO_Port, LED_GREEN_N_Pin,
							0, 1, 1);
			setTrafficLight(LED_RED_S_GPIO_Port, LED_RED_S_Pin,
							LED_YELLOW_S_GPIO_Port, LED_YELLOW_S_Pin,
							LED_GREEN_S_GPIO_Port, LED_GREEN_S_Pin,
							0, 1, 1);

			state = STATE_EW_YELLOW;
			countdown = time_yellow_s;
			break;

		case STATE_EW_YELLOW: // EW yellow -> NS green
			setTrafficLight(LED_RED_N_GPIO_Port, LED_RED_N_Pin,
							LED_YELLOW_N_GPIO_Port, LED_YELLOW_N_Pin,
							LED_GREEN_N_GPIO_Port, LED_GREEN_N_Pin,
							1, 1, 0);
			setTrafficLight(LED_RED_S_GPIO_Port, LED_RED_S_Pin,
							LED_YELLOW_S_GPIO_Port, LED_YELLOW_S_Pin,
							LED_GREEN_S_GPIO_Port, LED_GREEN_S_Pin,
							1, 1, 0);
			setTrafficLight(LED_RED_E_GPIO_Port, LED_RED_E_Pin,
							LED_YELLOW_E_GPIO_Port, LED_YELLOW_E_Pin,
							LED_GREEN_E_GPIO_Port, LED_GREEN_E_Pin,
							0, 1, 1);
			setTrafficLight(LED_RED_W_GPIO_Port, LED_RED_W_Pin,
							LED_YELLOW_W_GPIO_Port, LED_YELLOW_W_Pin,
							LED_GREEN_W_GPIO_Port, LED_GREEN_W_Pin,
							0, 1, 1);

			state = STATE_NS_GREEN;
			countdown = time_green_s;
			break;
		}
	}
	display7SEG(countdown);
	HAL_Delay(1000);
	countdown--;
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

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */
  traffic_light_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	traffic_light_loop_once();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
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
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED_RED_N_Pin|LED_YELLOW_N_Pin|LED_GREEN_N_Pin|LED_RED_S_Pin
                          |LED_YELLOW_S_Pin|LED_GREEN_S_Pin|LED_RED_E_Pin|LED_YELLOW_E_Pin
                          |LED_GREEN_E_Pin|LED_RED_W_Pin|LED_YELLOW_W_Pin|LED_GREEN_W_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SEG_A_Pin|SEG_B_Pin|SEG_C_Pin|SEG_D_Pin
                          |SEG_E_Pin|SEG_F_Pin|SEG_G_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED_RED_N_Pin LED_YELLOW_N_Pin LED_GREEN_N_Pin LED_RED_S_Pin
                           LED_YELLOW_S_Pin LED_GREEN_S_Pin LED_RED_E_Pin LED_YELLOW_E_Pin
                           LED_GREEN_E_Pin LED_RED_W_Pin LED_YELLOW_W_Pin LED_GREEN_W_Pin */
  GPIO_InitStruct.Pin = LED_RED_N_Pin|LED_YELLOW_N_Pin|LED_GREEN_N_Pin|LED_RED_S_Pin
                          |LED_YELLOW_S_Pin|LED_GREEN_S_Pin|LED_RED_E_Pin|LED_YELLOW_E_Pin
                          |LED_GREEN_E_Pin|LED_RED_W_Pin|LED_YELLOW_W_Pin|LED_GREEN_W_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : SEG_A_Pin SEG_B_Pin SEG_C_Pin SEG_D_Pin
                           SEG_E_Pin SEG_F_Pin SEG_G_Pin */
  GPIO_InitStruct.Pin = SEG_A_Pin|SEG_B_Pin|SEG_C_Pin|SEG_D_Pin
                          |SEG_E_Pin|SEG_F_Pin|SEG_G_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

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
  while (1)
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
