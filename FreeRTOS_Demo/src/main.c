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
#include "Dma_Ip.h"
#include "Dma_Ip_Hw_Access.h"
#include "stdio.h"

#define main_TASK_PRIORITY                ( tskIDLE_PRIORITY + 2 )

#define DMA_CH0_BUFFER_DIMENSION           ((uint32)24U)
#define DMA_CH0_CONFIG_LIST_DIMENSION      ((uint32)2U)
#define UART_LPUART_INTERNAL_CHANNEL  3

#define DMA_TRANSFER_SUCCESS_MSG "DMA transfer completed with success\n"
#define DMA_TRANSFER_FAILURE_MSG "DMA transfer failed\n"


SemaphoreHandle_t sem_handle;

uint32 g_DmaCh0_CallbackCounter = 0;

__attribute__(( aligned(32) )) static uint8 DmaCh0_SourceBuffer[DMA_CH0_BUFFER_DIMENSION] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
__attribute__(( aligned(32) )) static uint8 DmaCh0_DestinationBuffer[DMA_CH0_BUFFER_DIMENSION];
Dma_Ip_LogicChannelTransferListType DmaCh0_TransferList[DMA_CH0_CONFIG_LIST_DIMENSION] =
{
    {DMA_IP_CH_SET_SOURCE_ADDRESS, (Dma_Ip_uintPtrType)&DmaCh0_SourceBuffer[0U]},
    {DMA_IP_CH_SET_DESTINATION_ADDRESS, (Dma_Ip_uintPtrType)&DmaCh0_DestinationBuffer[0U]},
};
Dma_Ip_LogicChannelStatusType t_GetChannel0Status;

void DmaCh0_Callback(void)
{
    g_DmaCh0_CallbackCounter++;
}

Std_ReturnType CHECK_Uint8_BufferCompare(uint8 * pSource, uint8 * pDestination, uint32 Length)
{
    uint32 Index = 0U;

    for(Index = 0U; Index < Length; Index++)
    {
        if(pSource[Index] != pDestination[Index])
        {
            return E_NOT_OK;
        }
    }
    return E_OK;
}

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

    char msg[50];

    for( ;; )
    {
        operation_status = xSemaphoreTake(sem_handle, portMAX_DELAY);
        configASSERT(operation_status == pdPASS);
        snprintf(msg, 50, "DMA callback counter: %lu\n", g_DmaCh0_CallbackCounter);
        Lpuart_Uart_Ip_AsyncSend(UART_LPUART_INTERNAL_CHANNEL, (const uint8 *)msg, strlen(msg));
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
    IntCtrl_Ip_InstallHandler(DMATCD0_IRQn, Dma0_Ch0_IRQHandler, NULL_PTR);
    Lpuart_Uart_Ip_Init(UART_LPUART_INTERNAL_CHANNEL, &Lpuart_Uart_Ip_xHwConfigPB_3);
    (void)Dma_Ip_Init(&Dma_Ip_xDmaInitPB);
    Dma_Ip_SetLogicChannelTransferList(DMA_LOGIC_CH_0, DmaCh0_TransferList, DMA_CH0_CONFIG_LIST_DIMENSION);
    Dma_Ip_SetLogicChannelCommand(DMA_LOGIC_CH_0, DMA_IP_CH_SET_SOFTWARE_REQUEST);
    Dma_Ip_GetLogicChannelStatus(DMA_LOGIC_CH_0, &t_GetChannel0Status);
    while(t_GetChannel0Status.Done != TRUE)
    {
        Dma_Ip_GetLogicChannelStatus(DMA_LOGIC_CH_0, &t_GetChannel0Status);
    }
    Std_ReturnType StdStatus = CHECK_Uint8_BufferCompare(DmaCh0_SourceBuffer, DmaCh0_DestinationBuffer, DMA_CH0_BUFFER_DIMENSION);
	if(E_OK != StdStatus)
	{
		Lpuart_Uart_Ip_AsyncSend(UART_LPUART_INTERNAL_CHANNEL, (const uint8 *)DMA_TRANSFER_FAILURE_MSG, strlen(DMA_TRANSFER_FAILURE_MSG));
	} else {
		Lpuart_Uart_Ip_AsyncSend(UART_LPUART_INTERNAL_CHANNEL, (const uint8 *)DMA_TRANSFER_SUCCESS_MSG, strlen(DMA_TRANSFER_SUCCESS_MSG));
	}

    vSemaphoreCreateBinary(sem_handle);
    xTaskCreate( SendTask   , ( const char * const ) "SendTask", configMINIMAL_STACK_SIZE, (void*)0, main_TASK_PRIORITY, NULL );
    xTaskCreate( ReceiveTask, ( const char * const ) "RecTask" , configMINIMAL_STACK_SIZE, (void*)0, main_TASK_PRIORITY + 1, NULL );
    vTaskStartScheduler();

    for( ;; );

    (void)Lpuart_Uart_Ip_Deinit(UART_LPUART_INTERNAL_CHANNEL);

    return 0;
}

/** @} */
