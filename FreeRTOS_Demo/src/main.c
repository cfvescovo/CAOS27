/******************************************************************************
 *                                                                            *
 *   FreeRTOS sample application for RTD on S32 platforms                     *
 *                                                                            *
 *   Copyright 2023 NXP                                                       *
 *                                                                            *
 ******************************************************************************/

/* Including necessary configuration files. */
#include "Clock_Ip.h"
#include "FreeRTOS.h"
#include "Lpuart_Uart_Ip.h"
#include "Lpuart_Uart_Ip_Irq.h"
#include "Siul2_Port_Ip.h"
#include "IntCtrl_Ip.h"
#include "task.h"
#include "semphr.h"
#include "string.h"

#define main_TASK_PRIORITY                ( tskIDLE_PRIORITY + 2 )

SemaphoreHandle_t sem_handle;

#define UART_LPUART_INTERNAL_CHANNEL  3

#define MESSAGE "test\n"

/**
* @brief        SendTask is used to give the semaphore
* @details      SendTask give the semaphore every 1 second
*/
void SendTask( void *pvParameters )
{
    (void)pvParameters;
    BaseType_t operation_status;

    for( ;; )
    {
        operation_status = xSemaphoreGive(sem_handle);
        configASSERT(operation_status == pdPASS);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
* @brief        ReceiveTask get the semaphore and toggle pins
* @details      ReceiveTask try to get the semaphore with portMAX_DELAY timeout
*/
void ReceiveTask( void *pvParameters )
{
    (void)pvParameters;
    BaseType_t operation_status;

    for( ;; )
    {
        operation_status = xSemaphoreTake(sem_handle, portMAX_DELAY);
        configASSERT(operation_status == pdPASS);
        Lpuart_Uart_Ip_AsyncSend(UART_LPUART_INTERNAL_CHANNEL, (const uint8 *)MESSAGE, strlen(MESSAGE));
    }
}

/**
* @brief        Main function of the example
* @details      Initializes the used drivers and uses 1 binary Semaphore and
*               2 tasks to send a message over LPUART3.
*/
int main(void)
{

#ifndef VIRT_ENV
    /* Initialize Clock */
    Clock_Ip_StatusType Status_Init_Clock = CLOCK_IP_ERROR;
    Status_Init_Clock = Clock_Ip_Init(Clock_Ip_aClockConfig);

    if (Status_Init_Clock != CLOCK_IP_SUCCESS)
    {
        while(1); /* Error during initialization. */
    }
#else
#endif
    Siul2_Port_Ip_Init(NUM_OF_CONFIGURED_PINS0, g_pin_mux_InitConfigArr0);
    IntCtrl_Ip_Init(&IntCtrlConfig_0);
    Lpuart_Uart_Ip_Init(UART_LPUART_INTERNAL_CHANNEL, &Lpuart_Uart_Ip_xHwConfigPB_3);

    vSemaphoreCreateBinary(sem_handle);
    xTaskCreate( SendTask   , ( const char * const ) "SendTask", configMINIMAL_STACK_SIZE, (void*)0, main_TASK_PRIORITY, NULL );
    xTaskCreate( ReceiveTask, ( const char * const ) "RecTask" , configMINIMAL_STACK_SIZE, (void*)0, main_TASK_PRIORITY + 1, NULL );
    vTaskStartScheduler();

    for( ;; );

    (void)Lpuart_Uart_Ip_Deinit(UART_LPUART_INTERNAL_CHANNEL);

    return 0;
}

/** @} */
