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
 * Copyright (c) 2021 John Vedder
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
 *  Private Defines and Macros
 */
UART_Fifo_HandleTypeDef huart_fifo1;
UART_Fifo_HandleTypeDef huart_fifo2;


/**
 *  Private Variables
 */



/**
 *  Public Methods
 */

void UART_Fifo_Init( void )
{
    huart_fifo1.huart = &huart1;
    huart_fifo1.head = NULL;
    huart_fifo1.tail = NULL;

    huart_fifo2.huart = &huart2;
    huart_fifo2.head = NULL;
    huart_fifo2.tail = NULL;

    huart1.TxCpltCallback = UART_Fifo_TxCpltCallback;
    huart2.TxCpltCallback = UART_Fifo_TxCpltCallback;

}

void UART_Fifo_Transmit (UART_Fifo_HandleTypeDef *fifo, UART_Fifo_ItemTypeDef *item)
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

void UART_Fifo_TxCpltCallback (UART_HandleTypeDef *huart)
{
    assert_param(huart != NULL);

    /* count calls for debug */
    ++ISRcount;

    /* select fifo based on which UART this interrupt is for */
    UART_Fifo_HandleTypeDef *fifo = (huart == &huart1) ? &huart_fifo1 : &huart_fifo2;

    /* mark transmit complete on the current fifo item by setting size=0 */
    fifo->head->size = 0;

    /* move fifo head to the next item in the queue */
    fifo->head = fifo->head->next;
    if (fifo->head == NULL)
    {
        /* queue is now empty */
        fifo->tail = NULL;
    }
    else
    {
        /* start DMA engine on the new head of the queue */
        HAL_UART_Transmit_DMA(fifo->huart, fifo->head->data, fifo->head->size);
    }
}

/**
 *  Private Methods
 */
