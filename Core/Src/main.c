/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/*
 *
 * Integrantes:
 * Victor Hugo Barrera Garcia y Sergio Garcia Hernandez
 *
 * Conclusion:
 * En esta practica se implemento un sistema multitarea en STM32
 * utilizando FreeRTOS, donde se controlo el parpadeo de un LED en
 * dos velocidades diferentes y se realizo la lectura del ADC al
 * presionar un boton.
 *
 * Se comprobo que las tareas pueden coordinarse mediante suspension
 * y reanudacion, permitiendo que el sistema ejecute primero el modo
 * rapido durante 5 segundos, despues el modo lento durante 5 segundos
 * y finalmente entre a un estado de espera.
 *
 * Durante el estado de espera, si se presiona el boton, el sistema
 * lee el valor del ADC, calcula el voltaje correspondiente y lo
 * muestra por UART. Si no se presiona el boton, el programa regresa
 * automaticamente al modo rapido despues de 5 segundos.
 *
 * Esta practica permitio reforzar conceptos importantes de sistemas
 * embebidos en tiempo real, como tareas, retardos, prioridades,
 * Idle Hook, lectura analogica y comunicacion serial.
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"

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
ADC_HandleTypeDef hadc1;

UART_HandleTypeDef huart2;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE BEGIN PV */
TaskHandle_t hTaskLedRapido = NULL;
TaskHandle_t hTaskLedLento  = NULL;
TaskHandle_t hTaskADC       = NULL;
#define PRIORIDAD_LEDRapido 2
#define PRIORIDAD_LEDLento  2
#define PRIORIDAD_ADC  2

volatile uint8_t IDLE_HOOK = 0;
volatile uint32_t tiempoInicioIdle = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
void StartDefaultTask(void *argument);

/* USER CODE BEGIN PFP */
int uart2_write(int ch);
int __io_putchar(int ch);

void vTaskLedRapido(void *pvParameters);
void vTaskLedLento(void *pvParameters);
void vTaskADC(void *pvParameters);
uint32_t Leer_ADC(void);

//float LeerVoltajeADC(void);

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
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  printf("vic y Sergio\n\r");
  xTaskCreate(vTaskLedRapido,
              "LedRapido",
              512,
              NULL,
			  PRIORIDAD_LEDRapido,
			  &hTaskLedRapido);

  xTaskCreate(vTaskLedLento,
              "LedLento",
			  512,
              NULL,
			  PRIORIDAD_LEDLento,
			  &hTaskLedLento);

   xTaskCreate(vTaskADC,
              "ADC",
			  512,
              NULL,
			  PRIORIDAD_ADC,
			  &hTaskADC);



  vTaskSuspend(hTaskLedLento);
  vTaskSuspend(hTaskADC);

  vTaskStartScheduler();

  /* USER CODE END 2 */



  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */



  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */


  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD2_Pin PA9 */
  GPIO_InitStruct.Pin = LD2_Pin|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

// Función usada para enviar un carácter por UART2
int uart2_write(int ch)
{
    // Espera hasta que el registro de transmisión esté vacío
    while (!(USART2->SR & USART_SR_TXE)) {}

    // Envía el carácter por el registro de datos de USART2
    USART2->DR = (ch & 0xFF);

    // Regresa el carácter enviado
    return ch;
}

// Redirección de printf hacia UART2
int __io_putchar(int ch)
{
    uart2_write(ch);   // Envía cada carácter de printf por UART2
    return ch;
}

// Tarea encargada del parpadeo rápido del LED
void vTaskLedRapido(void *pvParameters)
{
    for (;;)
    {
        printf("LED_R\r\n");   // Indica por UART que inició el modo rápido

        // Guarda el tiempo inicial del modo rápido
        uint32_t tiempoInicio = xTaskGetTickCount();

        // Mantiene el parpadeo rápido durante 5 segundos
        while ((xTaskGetTickCount() - tiempoInicio) < pdMS_TO_TICKS(5000))
        {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9);   // Cambia el estado del LED
            vTaskDelay(pdMS_TO_TICKS(100));          // Retardo de 100 ms
        }

        // Apaga el LED al terminar el modo rápido
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);

        // Reanuda la tarea del LED lento
        vTaskResume(hTaskLedLento);

        // Suspende esta tarea hasta que otra tarea la vuelva a reanudar
        vTaskSuspend(NULL);
    }
}

// Tarea encargada del parpadeo lento del LED
void vTaskLedLento(void *pvParameters)
{
    for (;;)
    {
        printf("LED_L\r\n");   // Indica por UART que inició el modo lento

        // Guarda el tiempo inicial del modo lento
        uint32_t tiempoInicio = xTaskGetTickCount();

        // Mantiene el parpadeo lento durante 5 segundos
        while ((xTaskGetTickCount() - tiempoInicio) < pdMS_TO_TICKS(5000))
        {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9);   // Cambia el estado del LED
            vTaskDelay(pdMS_TO_TICKS(500));          // Retardo de 500 ms
        }

        // Apaga el LED al terminar el modo lento
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);

        // Activa la bandera para indicar que el sistema entra al estado IDLE
        IDLE_HOOK = 1;

        // Guarda el tiempo en que inició el estado IDLE
        tiempoInicioIdle = xTaskGetTickCount();

        // Suspende esta tarea hasta que vuelva a ser reanudada
        vTaskSuspend(NULL);
    }
}

// Tarea encargada de leer el ADC y mostrar el voltaje
void vTaskADC(void *pvParameters)
{
    for (;;)
    {
        // Lee el valor digital del ADC
        uint32_t adc = Leer_ADC();

        // Convierte el valor ADC a voltaje considerando referencia de 3.3 V y resolución de 12 bits
        float voltaje = (adc * 3.3f) / 4095.0f;

        // Imprime por UART el valor crudo del ADC y su equivalente en voltaje
        printf("Valor del ADC: %lu  Voltaje: %.2f V\r\n", adc, voltaje);

        // Pequeño retardo para evitar lecturas demasiado rápidas
        vTaskDelay(pdMS_TO_TICKS(500));

        // Después de leer el ADC, regresa inmediatamente al modo de LED rápido
        vTaskResume(hTaskLedRapido);

        // Suspende la tarea ADC hasta que el botón vuelva a activarla
        vTaskSuspend(NULL);
    }
}

// Función para obtener el valor del ADC
uint32_t Leer_ADC(void)
{
    HAL_ADC_Start(&hadc1);                         // Inicia la conversión ADC
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY); // Espera hasta que termine la conversión
    return HAL_ADC_GetValue(&hadc1);               // Regresa el valor digital leído
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
#ifdef USE_FULL_ASSERT
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
