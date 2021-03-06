/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "dma.h"
#include "usart.h"
#include "gpio.h"
#include "uart_fifo.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define LED_ON()        HAL_GPIO_WritePin(LED_GRN_GPIO_Port, LED_GRN_Pin, GPIO_PIN_SET)
#define LED_OFF()       HAL_GPIO_WritePin(LED_GRN_GPIO_Port, LED_GRN_Pin, GPIO_PIN_RESET)
#define LED_TOGGLE()    HAL_GPIO_TogglePin(LED_GRN_GPIO_Port, LED_GRN_Pin)


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* Buffer used for transmission */
//                     123456789-1234 5
uint8_t TxBuffer1[] = "HELLO WORLD 1\r\n";
uint8_t TxBuffer2[] = "HELLO WORLD 2\r\n";
uint8_t TxBuffer3[] = "HELLO WORLD 3\r\n";
uint8_t TxBuffer4[] = "HELLO WORLD 4\r\n";

UART_TxFifo_ItemTypeDef item1;
UART_TxFifo_ItemTypeDef item2;
UART_TxFifo_ItemTypeDef item3;
UART_TxFifo_ItemTypeDef item4;

/* debug instrumentation */
__IO uint32_t ISRcount = 0;
uint32_t start_tick = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */

void configure_tx_items( void );
int16_t check_UART_rx();

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_USART2_UART_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();

  UART_TxFifo_Init( &huart1_txfifo, &huart1 );
  UART_TxFifo_Init( &huart2_txfifo, &huart2 );

  UART_RxFifo_Init( &huart1_rxfifo, &huart1 );
  UART_RxFifo_Init( &huart2_rxfifo, &huart2 );


  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */

  printf("\r\nNUCLEO_F030R8_UART_DMA\r\n");
  printf( "Build: %s %s\r\n", __DATE__, __TIME__ );

#if 0
  /* Wait for button press */
  while (HAL_GPIO_ReadPin(BTN_USER_GPIO_Port, BTN_USER_Pin) == GPIO_PIN_SET)
  {
      /* toggle LED while waiting */
      HAL_GPIO_TogglePin(LED_GRN_GPIO_Port, LED_GRN_Pin);
      HAL_Delay(500);
  }
  /* turn off LED */
  HAL_GPIO_WritePin(LED_GRN_GPIO_Port, LED_GRN_Pin, GPIO_PIN_RESET);
#endif


#if 1
  ISRcount = 0;
  configure_tx_items();
  printf("TEST #1\r\n");
  printf("Loading Tx Fifo\r\n");
  UART_TxFifo_Transmit(&huart1_txfifo, &item1);
  UART_TxFifo_Transmit(&huart1_txfifo, &item2);
  UART_TxFifo_Transmit(&huart1_txfifo, &item3);
  UART_TxFifo_Transmit(&huart1_txfifo, &item4);
  printf("Tx ISRCount %ld\r\n", ISRcount);  /* should be 0 if we are fast enough */
  printf("Delay...\r\n");
  HAL_Delay(100);
  printf("ISRCount %ld (should be 4)\r\n", ISRcount); /* should be 4 as fifo (fifo empty) */
  printf("TxFifo.head is NULL: %s (should be true)\r\n", (huart1_txfifo.head == NULL ? "true" : "false"));
  printf("TxFifo.tail is NULL: %s (should be true)\r\n", (huart1_txfifo.tail == NULL ? "true" : "false"));

  /* Empty the RX fifo */
  while ( check_UART_rx() >= 0 );
  printf("Test #1 Done.\r\n\r\n");
#endif

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
#if 1
  configure_tx_items();
  printf("TEST #2\r\n");
  printf("Transmitting...\r\n");
  start_tick = HAL_GetTick();
  UART_TxFifo_Transmit( &huart1_txfifo, &item1);
  while ( HAL_GetTick( ) < start_tick + 100 )
      check_UART_rx( );

  UART_TxFifo_Transmit( &huart1_txfifo, &item2 );
  while ( HAL_GetTick( ) < start_tick + 200 )
      check_UART_rx( );

  UART_TxFifo_Transmit( &huart1_txfifo, &item3 );
  while ( HAL_GetTick( ) < start_tick + 300 )
      check_UART_rx( );

  UART_TxFifo_Transmit( &huart1_txfifo, &item4 );
  while ( HAL_GetTick( ) < start_tick + 400 )
      check_UART_rx( );
  printf( "Test #2 Done.\r\n\r\n" );
#endif

#if 1
  configure_tx_items();
  printf("TEST #3\r\n");
  printf("Transmitting...\r\n");
  start_tick = HAL_GetTick();
  UART_TxFifo_Transmit( &huart1_txfifo, &item1);
  UART_TxFifo_Transmit( &huart1_txfifo, &item2 );
  UART_TxFifo_Transmit( &huart1_txfifo, &item3 );
  UART_TxFifo_Transmit( &huart1_txfifo, &item4 );

  while ( HAL_GetTick( ) < start_tick + 400 )
      check_UART_rx( );
  printf( "Test #3 Done.\r\n\r\n" );
#endif

  while (1)
  {
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* DMA1_Channel4_5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_5_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_5_IRQn);
  /* DMA1_Channel2_3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
  /* USART1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USART2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
}

/* USER CODE BEGIN 4 */

void configure_tx_items( void )
{
    item1.data = TxBuffer1;
    item1.size = 15;
    item2.data = TxBuffer2;
    item2.size = 15;
    item3.data = TxBuffer3;
    item3.size = 15;
    item4.data = TxBuffer4;
    item4.size = 15;
}


int16_t check_UART_rx( void )
{
    int16_t data = UART_RxFifo_Receive(&huart1_rxfifo);
    if (data == '\r')
    {
        printf("`\\r'");
    }
    else if (data == '\n')
    {
        printf("`\\n'\r\n");
    }
    else if (data >= 0)
    {
        printf("`%c'", (uint8_t) data);
    }
    return data;
}

void blink_fault( uint32_t code )
{
    /* choose roughly clock speed / 24 forDELAY_LOOPS */
    #define DELAY_LOOPS 2000000
    uint32_t count;
    uint32_t delay;

    HAL_GPIO_WritePin( LED_GRN_GPIO_Port, LED_GRN_Pin, GPIO_PIN_SET );
    while ( 1 )
    {
        for ( count = code; count; --count )
        {
            HAL_GPIO_WritePin( LED_GRN_GPIO_Port, LED_GRN_Pin, GPIO_PIN_SET );
            for ( delay = DELAY_LOOPS; delay; --delay )
                /* spin wait */;
            HAL_GPIO_WritePin( LED_GRN_GPIO_Port, LED_GRN_Pin, GPIO_PIN_RESET );
            for ( delay = DELAY_LOOPS; delay; --delay )
                /* spin wait */;
        }
        for ( delay = 4 * DELAY_LOOPS; delay; --delay )
            /* spin wait */;
    }
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
  // __disable_irq();
  while (1)
  {
      // Slow LED Blink on Error (1 sec on 1sec off)
      LED_ON();
      HAL_Delay(1000);
      LED_OFF();
      HAL_Delay(1000);
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
    printf("assert_failed\r\n");
    printf("File: %s\r\n", file);
    printf("Line: %ld\r\n", line;)
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
