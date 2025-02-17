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
#include "stdlib.h"

#define main_TASK_PRIORITY                ( tskIDLE_PRIORITY + 2 )

#define DMA_CH0_BUFFER_DIMENSION           ((uint32)128U)
#define DMA_CH0_CONFIG_LIST_DIMENSION      ((uint32)2U)
#define UART_LPUART_INTERNAL_CHANNEL  3

#define SETUP_MSG "Setup completed\n"

#define DMA_TRANSFER_SUCCESS_MSG "DMA transfer completed successfully\n"
#define DMA_TRANSFER_FAILURE_MSG "DMA transfer failed\n"

#define SEED_MSG "Insert a seed between 0 and 9: "

#define MAX 10
#define MIN 1

SemaphoreHandle_t consumer_go;
SemaphoreHandle_t producer_go;

uint32 g_DmaCh0_CallbackCounter = 0;

uint8 DmaCh0_SourceBuffer[DMA_CH0_BUFFER_DIMENSION];
uint8 DmaCh0_DestinationBuffer[DMA_CH0_BUFFER_DIMENSION];
Dma_Ip_LogicChannelTransferListType DmaCh0_TransferList[DMA_CH0_CONFIG_LIST_DIMENSION] =
{
    {DMA_IP_CH_SET_SOURCE_ADDRESS, (Dma_Ip_uintPtrType)&DmaCh0_SourceBuffer[0U]},
    {DMA_IP_CH_SET_DESTINATION_ADDRESS, (Dma_Ip_uintPtrType)&DmaCh0_DestinationBuffer[0U]},
};
Dma_Ip_LogicChannelStatusType t_GetChannel0Status;

void DmaCh0_Callback(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    g_DmaCh0_CallbackCounter++;
    xSemaphoreGiveFromISR(consumer_go, &xHigherPriorityTaskWoken);
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
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

    uint8 seed_buf[10];
    unsigned int seed;
    char msg[10];

    for( ;; )
    {
    	operation_status = xSemaphoreTake(producer_go, portMAX_DELAY);
    	configASSERT(operation_status == pdPASS);
    	Lpuart_Uart_Ip_SyncSend(UART_LPUART_INTERNAL_CHANNEL, (const uint8 *)SEED_MSG, strlen(SEED_MSG), portMAX_DELAY);
    	Lpuart_Uart_Ip_SyncReceive(UART_LPUART_INTERNAL_CHANNEL, seed_buf, 1, portMAX_DELAY);
    	seed = atoi((const char *)seed_buf);
    	srand(seed);
        snprintf(msg, 10, "%u\n", seed);
    	Lpuart_Uart_Ip_SyncSend(UART_LPUART_INTERNAL_CHANNEL, (const uint8 *)msg, strlen(msg), portMAX_DELAY);
        Dma_Ip_SetLogicChannelTransferList(DMA_LOGIC_CH_0, DmaCh0_TransferList, DMA_CH0_CONFIG_LIST_DIMENSION);
    	for (int i = 0; i < 128; i++) {
    		// fill the buffer with random numbers between MIN and MAX
    		DmaCh0_SourceBuffer[i] = rand() % (MAX - MIN + 1) + MIN;
    	}
    	Dma_Ip_SetLogicChannelCommand(DMA_LOGIC_CH_0, DMA_IP_CH_SET_SOFTWARE_REQUEST);
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
        operation_status = xSemaphoreTake(consumer_go, portMAX_DELAY);
        configASSERT(operation_status == pdPASS);
        Std_ReturnType StdStatus = CHECK_Uint8_BufferCompare(DmaCh0_SourceBuffer, DmaCh0_DestinationBuffer, DMA_CH0_BUFFER_DIMENSION);
		if(E_OK != StdStatus)
		{
			Lpuart_Uart_Ip_AsyncSend(UART_LPUART_INTERNAL_CHANNEL, (const uint8 *)DMA_TRANSFER_FAILURE_MSG, strlen(DMA_TRANSFER_FAILURE_MSG));
		} else {
			Lpuart_Uart_Ip_AsyncSend(UART_LPUART_INTERNAL_CHANNEL, (const uint8 *)DMA_TRANSFER_SUCCESS_MSG, strlen(DMA_TRANSFER_SUCCESS_MSG));
		}
        snprintf(msg, 50, "DMA callback counter: %lu\n", g_DmaCh0_CallbackCounter);
        Lpuart_Uart_Ip_AsyncSend(UART_LPUART_INTERNAL_CHANNEL, (const uint8 *)msg, strlen(msg));
        snprintf(msg, 50, "First number generated: %u\n", DmaCh0_SourceBuffer[0]);
        Lpuart_Uart_Ip_AsyncSend(UART_LPUART_INTERNAL_CHANNEL, (const uint8 *)msg, strlen(msg));
        xSemaphoreGive(producer_go);
    }
}

/**
* @brief        Main function of the example
* @details      Initializes the used drivers and uses 1 binary Semaphore and
*               2 tasks to send a message over LPUART3.
*/
int main(void)
{

	// This does not do anything on our virtual environment
    Clock_Ip_Init(Clock_Ip_aClockConfig);

    Siul2_Port_Ip_Init(NUM_OF_CONFIGURED_PINS0, g_pin_mux_InitConfigArr0);
    IntCtrl_Ip_Init(&IntCtrlConfig_0);
    Lpuart_Uart_Ip_Init(UART_LPUART_INTERNAL_CHANNEL, &Lpuart_Uart_Ip_xHwConfigPB_3);
    IntCtrl_Ip_InstallHandler(DMATCD0_IRQn, Dma0_Ch0_IRQHandler, NULL_PTR);
    (void)Dma_Ip_Init(&Dma_Ip_xDmaInitPB);

    Lpuart_Uart_Ip_AsyncSend(UART_LPUART_INTERNAL_CHANNEL, (const uint8 *)SETUP_MSG, strlen(SETUP_MSG));

    consumer_go = xSemaphoreCreateBinary();
    producer_go = xSemaphoreCreateBinary();

    xTaskCreate( SendTask   , ( const char * const ) "SendTask", configMINIMAL_STACK_SIZE, (void*)0, main_TASK_PRIORITY, NULL );
    xTaskCreate( ReceiveTask, ( const char * const ) "RecTask" , configMINIMAL_STACK_SIZE, (void*)0, main_TASK_PRIORITY, NULL );
    xSemaphoreGive(producer_go);
    vTaskStartScheduler();

    for( ;; );

    (void)Lpuart_Uart_Ip_Deinit(UART_LPUART_INTERNAL_CHANNEL);

    return 0;
}

/** @} */
