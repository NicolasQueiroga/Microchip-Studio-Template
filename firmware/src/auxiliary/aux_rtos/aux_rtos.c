#include "auxiliary/aux_functions/aux_functions.h"
#include "auxiliary/aux_time/aux_time.h"
#include "aux_rtos.h"
#include <stdio.h>

// semaphores

// queues
volatile QueueHandle_t xQueueADC;
volatile QueueHandle_t xQueuePROC;

// getters & setters
QueueHandle_t get_adc_queue(void)
{
    return xQueueADC;
}
void set_adc_queue(QueueHandle_t val)
{
    xQueueADC = val;
    if (xQueueADC == NULL)
        printf("falha em criar a queue \n");
}

QueueHandle_t get_proc_queue(void)
{
    return xQueuePROC;
}
void set_proc_queue(QueueHandle_t val)
{
    xQueuePROC = val;
    if (xQueuePROC == NULL)
        printf("falha em criar a queue \n");
}

// rtos functions
/**
 * @brief Called if stack overflow during execution
 */
void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName)
{
    printf("stack overflow %u %s\r\n", pxTask, (portCHAR *)pcTaskName);
    /* If the parameters have been corrupted then inspect pxCurrentTCB to
     * identify which task has overflowed its stack.
     */
    for (;;)
    {
    }
}

/**
 * @brief This function is called by FreeRTOS idle task
 */
void vApplicationIdleHook(void) { pmc_sleep(SAM_PM_SMODE_SLEEP_WFI); }

/**
 * @brief This function is called by FreeRTOS each tick
 */
void vApplicationTickHook(void) {}

/**
 * @brief
 */
void vApplicationMallocFailedHook(void)
{
    /* Called if a call to pvPortMalloc() fails because there is insufficient
    free memory available in the FreeRTOS heap.  pvPortMalloc() is called
    internally by FreeRTOS API functions that create tasks, queues, software
    timers, and semaphores.  The size of the FreeRTOS heap is set by the
    configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */

    /* Force an assert. */
    configASSERT((volatile void *)NULL);
}

// tasks
void task_proc(void *pvParameters)
{
  config_AFEC_pot(AFEC_POT, AFEC_POT_ID, AFEC_POT_CHANNEL, AFEC_pot_Callback);
  TC_init(TC0, ID_TC1, 1, 10);
  tc_start(TC0, 1);

  adcData adc;
  int i = -1;
  int mean;

  while (1)
  {
    if (xQueueReceive(xQueuePROC, &(adc), 1000))
    {
      int sum = 0;
      while (++i < 10)
        sum += adc.value;
      i = -1;
      mean = sum / 10;
      xQueueSend(xQueueADC, (void *)&mean, 10);
    }
  }
}

void task_adc(void *pvParameters)
{

  // vari�vel para recever dados da fila
  int mean;

  while (1)
  {
    if (xQueueReceive(xQueueADC, &(mean), 1000))
      printf("mean: %d \n", mean);
    else
      printf("Nao chegou um novo dado em 1 segundo");
  }
}
