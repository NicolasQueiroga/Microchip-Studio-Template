#ifndef AUX_RTOS_H_
#define AUX_RTOS_H_

#include "auxiliary/aux_hw/aux_hw.h"

// auxiliary data structures ----------------------------------------------------
// -------------------------------------------------------------------------------

// global variables --------------------------------------------------------------
QueueHandle_t get_but_queue(void);
QueueHandle_t get_adc_queue(void);
QueueHandle_t get_rx_queue(void);
xTaskHandle get_handle_oled(void);
xTaskHandle get_handle_but(void);
xTaskHandle get_handle_adc(void);
// -------------------------------------------------------------------------------

// RTOS configuration ------------------------------------------------------------
#define TASK_OLED_STACK_SIZE (1024 * 6 / sizeof(portSTACK_TYPE))
#define TASK_OLED_STACK_PRIORITY (tskIDLE_PRIORITY)
#define TASK_BUT_STACK_SIZE (2048 / sizeof(portSTACK_TYPE))
#define TASK_BUT_STACK_PRIORITY (tskIDLE_PRIORITY)
#define TASK_ADC_STACK_SIZE (1024 * 10 / sizeof(portSTACK_TYPE))
#define TASK_ADC_STACK_PRIORITY (tskIDLE_PRIORITY)
#define TASK_RECEIVE_STACK_SIZE (1024 * 6 / sizeof(portSTACK_TYPE))
#define TASK_RECEIVE_STACK_PRIORITY (tskIDLE_PRIORITY)
void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName);
void vApplicationIdleHook(void);
void vApplicationTickHook(void);
void vApplicationMallocFailedHook(void);
void xPortSysTickHandler(void);
// -------------------------------------------------------------------------------

// init RTOS ---------------------------------------------------------------------
void init_rtos(void);
// -------------------------------------------------------------------------------

// RTOS tasks --------------------------------------------------------------------
void task_oled(void *pvParameters);
void task_but(void *pvParameters);
void task_adc(void *pvParameters);
void task_receive(void *pvParameters);
// -------------------------------------------------------------------------------

#endif