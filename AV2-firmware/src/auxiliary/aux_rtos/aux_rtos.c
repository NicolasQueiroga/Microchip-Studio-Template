#include "aux_rtos.h"
#include "auxiliary/aux_time/aux_time.h"
#include "auxiliary/aux_functions/aux_functions.h"
#include "coffee/coffee.h"
#include <string.h>

// global variables --------------------------------------------------------------
volatile QueueHandle_t xQueueBut;
QueueHandle_t get_but_queue(void) { return xQueueBut; }

volatile QueueHandle_t xQueueADC;
QueueHandle_t get_adc_queue(void) { return xQueueADC; }

volatile QueueHandle_t xQueueRx;
QueueHandle_t get_rx_queue(void) { return xQueueRx; }

volatile xTaskHandle xHandleOled;
xTaskHandle get_handle_oled(void) { return xHandleOled; }

volatile xTaskHandle xHandleBut;
xTaskHandle get_handle_but(void) { return xHandleBut; }

volatile xTaskHandle xHandleADC;
xTaskHandle get_handle_adc(void) { return xHandleADC; }

volatile xTaskHandle xHandleRx;
xTaskHandle get_handle_rx(void) { return xHandleRx; }
// -------------------------------------------------------------------------------

// RTOS configuration ------------------------------------------------------------
void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName)
{
    printf("stack overflow %x %s\r\n", pxTask, (portCHAR *)pcTaskName);
    while (1)
        ;
}
void vApplicationIdleHook(void) {}
void vApplicationTickHook(void) {}
void vApplicationMallocFailedHook(void) { configASSERT((volatile void *)NULL); }
// -------------------------------------------------------------------------------

// init RTOS ---------------------------------------------------------------------
void init_rtos(void)
{
    xQueueBut = xQueueCreate(32, sizeof(char));
    xQueueADC = xQueueCreate(32, sizeof(uint32_t));
    xQueueRx = xQueueCreate(32, sizeof(char));

    if (xTaskCreate(task_oled, "oled", TASK_OLED_STACK_SIZE, NULL, TASK_OLED_STACK_PRIORITY, &xHandleOled) != pdPASS)
        printf("Failed to create oled task\r\n");

    if (xTaskCreate(task_adc, "adc", TASK_ADC_STACK_SIZE, NULL, TASK_ADC_STACK_PRIORITY, &xHandleADC) != pdPASS)
        printf("Failed to create adc task\r\n");

    if (xTaskCreate(task_but, "BUT", TASK_BUT_STACK_SIZE, NULL, TASK_BUT_STACK_PRIORITY, &xHandleBut) != pdPASS)
        printf("Failed to create BUT task\r\n");

    if (xTaskCreate(task_receive, "receive", TASK_RECEIVE_STACK_SIZE, NULL, TASK_RECEIVE_STACK_PRIORITY, &xHandleRx) != pdPASS)
        printf("Failed to create receive task\r\n");

    vTaskSuspend(xHandleOled);
    vTaskSuspend(xHandleADC);

    vTaskStartScheduler();
}
// -------------------------------------------------------------------------------

// RTOS tasks --------------------------------------------------------------------
void task_oled(void *pvParameters)
{
    gfx_mono_draw_string("Exemplo RTOS", 0, 0, &sysfont);
    gfx_mono_draw_string("oii", 0, 20, &sysfont);
    char queue = 0;

    for (;;)
    {
        // apenas para demonstrar o funcionamento das coisas
        // voc� deve remover isso!
        coffee_heat_on();
        vTaskDelay(2000);
        coffe_pump_on();
        vTaskDelay(2000);
        coffe_pump_off();
        vTaskDelay(2000);
        coffee_heat_off();
        vTaskDelay(2000);
    }
}

void task_but(void *pvParameters)
{
    char queue;
    uint32_t adc;

    for (;;)
    {
        if (xQueueReceive(xQueueBut, &queue, (TickType_t)500))
        {
            printf("but%c\n", queue);
            if (queue == '0')
            {
                tc_stop(TC0, 1);
                tc_stop(TC0, 2);
                tc_stop(TC1, 0);
                pio_set(LED1_PIO, LED1_PIO_IDX_MASK);
                pio_set(LED2_PIO, LED2_PIO_IDX_MASK);
                pio_set(LED3_PIO, LED3_PIO_IDX_MASK);
            }
            if (queue == '1')
            {
                char p[] = {'U', 0, 0, 'X'};
                send_package(p, 4);
                tc_start(TC0, 1);
                clear_screen();
                draw_string("btn1");
            }
            if (queue == '2')
            {
                tc_start(TC0, 2);
                clear_screen();
                draw_string("btn2");
            }
            if (queue == '3')
            {
                tc_start(TC1, 0);
                clear_screen();
                draw_string("btn3");
            }
        }
    }
}

void task_adc(void *pvParameters)
{
    uint32_t adc;
    while (1)
    {
        if (xQueueReceive(xQueueADC, &(adc), 1000))
            printf("adc: %d\n", adc);
        else
            printf("Nao chegou um novo dado em 1 segundo");
    }
}

void task_receive(void *pvParameters)
{
    char p_data[32];
    char p_cnt = 0;
    char p_do = 0;

    for (;;)
    {
        char c;
        if (xQueueReceive(xQueueRx, &c, 0))
        {
            printf("%c\n", c);
            p_data[p_cnt++] = c;
            if (p_data[p_cnt - 1] == 'X')
                p_do = 1;
        }
        if (p_do)
        {
            p_cnt = 0;
            p_do = 0;
            if (p_data[2] == 0xff)
            {
                vTaskResume(xHandleADC);
                vTaskResume(xHandleOled);
            }
        }
    }
}

// -------------------------------------------------------------------------------
