/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "main.h"
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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
extern volatile uint8_t IDLE_HOOK;
extern volatile uint32_t tiempoInicioIdle;
extern TaskHandle_t hTaskADC;
extern TaskHandle_t hTaskLedRapido;
//extern uint32_t Leer_ADC(void);
/* USER CODE END FunctionPrototypes */

/* Hook prototypes */
void vApplicationIdleHook(void);

/* USER CODE BEGIN 2 */

void vApplicationIdleHook( void )
{
   /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */

	    static uint32_t ultimoMensaje = 0;

	    if (IDLE_HOOK == 1)
	    {

	        if ((xTaskGetTickCount() - ultimoMensaje) >= pdMS_TO_TICKS(2500))
	        {
	            ultimoMensaje = xTaskGetTickCount();
	            printf("Estoy en idle\r\n");
	            printf("CPU libre\r\n");
	        }

	        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == GPIO_PIN_SET)
	        {
	           // uint32_t adc = Leer_ADC();
	           // float voltaje = (adc * 3.3f) / 4095.0f;

	            printf("Boton presionado\r\n");
	            //printf("Valor del ADC: %lu  Voltaje: %.2f V\r\n", adc, voltaje);

	            IDLE_HOOK = 0;
	            vTaskResume(hTaskADC);

	        }

	        if ((xTaskGetTickCount() - tiempoInicioIdle) >= pdMS_TO_TICKS(5000))
	        {

	        	IDLE_HOOK = 0;

	            vTaskResume(hTaskLedRapido);
	        }
	    }

}
/* USER CODE END 2 */

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

