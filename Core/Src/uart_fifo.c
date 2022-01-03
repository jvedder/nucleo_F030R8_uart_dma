/**
 ******************************************************************************
 * @file:   uart_fifo.c
 * @author: John Vedder
 * @brief:  FIFO/queue wrapper for UART using DMA
 ******************************************************************************
 */
/**
 ******************************************************************************
 * MIT License
 *
 * Copyright (c) 2022 John Vedder
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this hardware, software, and associated documentation files (the "Product"),
 * to deal in the Product without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Product, and to permit persons to whom the Product is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Product.
 *
 * THE PRODUCT IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE PRODUCT OR THE USE OR OTHER DEALINGS IN THE
 * PRODUCT.
 *
 ******************************************************************************
 */

/**
 *  Include files
 */
#include "main.h"
#include "usart.h"
#include "uart_fifo.h"


/**
 *  Exported Variables
 */
UART_TxFifo_TypeDef huart1_txfifo;
UART_TxFifo_TypeDef huart2_txfifo;

UART_TxFifo_TypeDef *TxFifos[] = {&huart1_txfifo, &huart1_txfifo, NULL};

UART_RxFifo_TypeDef huart1_rxfifo;
UART_RxFifo_TypeDef huart2_rxfifo;


/**
 *  Function Prototypes
 */
void UART_TxFifo_TxCpltCallback (UART_HandleTypeDef *huart);

/**
 *  Public Methods
 */

void UART_TxFifo_Init( UART_TxFifo_TypeDef *fifo, UART_HandleTypeDef *huart )
{
    assert_param(fifo != NULL);
    assert_param(huart != NULL);

    fifo->huart = huart;
    fifo->head = NULL;
    fifo->tail = NULL;

    huart->TxCpltCallback = UART_TxFifo_TxCpltCallback;
}

void UART_TxFifo_Transmit (UART_TxFifo_TypeDef *fifo, UART_TxFifo_ItemTypeDef *item)
{
    assert_param(fifo != NULL);
    assert_param(item != NULL);

    uint32_t fifo_was_empty = 0;

    /* do nothing if this item is empty */
    if (item->size == 0) return;

    /* Mark this item as the last in liked list */
    item->next = NULL;

    /* Start Atomic */
    __disable_irq();
    if (fifo->head == NULL)
    {
        /* fifo is empty so new item is both the head and tail item */
        fifo_was_empty = 1;
        fifo->head = item;
        fifo->tail = item;
    }
    else
    {
        /* link the tail item to this new item  */
        fifo->tail->next = item;
        fifo->tail = item;
    }
    __enable_irq();
    /* End Atomic */

    /* if fifo was empty, we need to start the DMA engine */
    if (fifo_was_empty)
    {
        HAL_UART_Transmit_DMA(fifo->huart, item->data, item->size);
    }
}

uint16_t UART_TxFifo_IsEmpty(UART_TxFifo_TypeDef *fifo)
{
    assert_param(fifo != NULL);

    return (fifo->head == NULL) ? 1 : 0;
}

void UART_RxFifo_Init( UART_RxFifo_TypeDef *fifo, UART_HandleTypeDef *huart )
{
    assert_param(fifo != NULL);
    assert_param(huart != NULL);

    fifo->huart = huart;
    HAL_UART_Receive_DMA(fifo->huart, fifo->RxBuffer, RX_BUFFER_LENGTH);
}

int16_t UART_RxFifo_Receive(UART_RxFifo_TypeDef *fifo)
{
    assert_param(fifo != NULL);

    /* get transfer count from circular DMA, which counts down */
    uint32_t RxIn = RX_BUFFER_LENGTH - fifo->huart->hdmarx->Instance->CNDTR;

    /* return -1 if FIFO is empty */
    int16_t data = -1;

    /* if circular buffer is not empty */
    if (fifo->RxOut != RxIn)
    {
        /* pull current item out of circular buffer */
        data = fifo->RxBuffer[fifo->RxOut];

        /* move index to the next item; wrap if at end of buffer */
        fifo->RxOut = (fifo->RxOut + 1) % RX_BUFFER_LENGTH;
    }
    return data;
}

uint16_t UART_RxFifo_IsEmpty(UART_RxFifo_TypeDef *fifo)
{
    assert_param(fifo != NULL);

    /* get transfer count from circular DMA, which counts down */
    uint32_t RxIn = RX_BUFFER_LENGTH - fifo->huart->hdmarx->Instance->CNDTR;

    /* if circular buffer is not empty */
    return (fifo->RxOut == RxIn) ? 1 : 0;
}

void UART_TxFifo_TxCpltCallback (UART_HandleTypeDef *huart)
{
    assert_param(huart != NULL);

    /* count calls for debug */
    ++ISRcount;

    /* select FIFO based on which UART this interrupt is for */
    // UART_TxFifo_TypeDef *fifo = (huart == &huart1) ? &huart1_fifo : &huart2_fifo;
    UART_TxFifo_TypeDef **fifo = TxFifos;
    while (fifo)
    {
        if ( (*fifo)->huart == huart) break;
        fifo++;
    }
    if (*fifo == NULL) return;

    /* mark transmit complete on the current FIFO item by setting size=0 */
    (*fifo)->head->size = 0;

    /* move FIFO head to the next item in the queue */
    (*fifo)->head = (*fifo)->head->next;
    if ((*fifo)->head == NULL)
    {
        /* queue is now empty */
        (*fifo)->tail = NULL;
    }
    else
    {
        /* start DMA engine on the new head of the queue */
        HAL_UART_Transmit_DMA((*fifo)->huart, (*fifo)->head->data, (*fifo)->head->size);
    }
}

/**
 *  Private Methods
 */
