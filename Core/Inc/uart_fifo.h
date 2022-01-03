/**
 ******************************************************************************
 * @file:   uart_fifo.h
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

/* Prevent recursive inclusion */
#ifndef UART_FIFO_H
#define UART_FIFO_H

/**
 *  Include files
 */
#include <stdint.h>

/**
 *  Public Defines and Macros
 */

#define RX_BUFFER_LENGTH    32

/**
  * @brief  Structure to hold one item in the Tx Linked List.
  */
typedef struct UART_TxFifo_ItemStruct
{
    uint8_t                         *data;
    volatile uint16_t               size;
    struct UART_TxFifo_ItemStruct   *next;

} UART_TxFifo_ItemTypeDef;

/**
  * @brief  Structure to hold the UART Transmit Linked List state
  */
typedef struct UART_TxFifo_Struct
{
    UART_HandleTypeDef              *huart;

    /* Linked listed for UART Transmit via DMA */
    volatile UART_TxFifo_ItemTypeDef  *head;
    UART_TxFifo_ItemTypeDef           *tail;

} UART_TxFifo_TypeDef;

/**
  * @brief  Structure to hold the UART Receive Circular buffer.
  */
typedef struct UART_RxFifo_Struct
{
    UART_HandleTypeDef              *huart;

    /* Circular buffer for UART Receive via DMA */
    uint8_t                         RxBuffer[RX_BUFFER_LENGTH];

    /* The index of the "out" position of the circular buffer */
    uint16_t                        RxOut;

    /* Note: The index of the "in" position of the circular buffer is stored in the DMA hardware */

} UART_RxFifo_TypeDef;

/**
 *  Exported Variables
 */
extern UART_TxFifo_TypeDef huart1_txfifo;
extern UART_TxFifo_TypeDef huart2_txfifo;

extern UART_RxFifo_TypeDef huart1_rxfifo;
extern UART_RxFifo_TypeDef huart2_rxfifo;

/**
 *  Public Function Prototypes
 */
void UART_TxFifo_Init( UART_TxFifo_TypeDef *fifo, UART_HandleTypeDef *huart );
void UART_TxFifo_Transmit (UART_TxFifo_TypeDef *fifo, UART_TxFifo_ItemTypeDef *item);
uint16_t UART_TxFifo_IsEmpty(UART_TxFifo_TypeDef *fifo);
void UART_RxFifo_Init( UART_RxFifo_TypeDef *fifo, UART_HandleTypeDef *huart );
int16_t UART_RxFifo_Receive(UART_RxFifo_TypeDef *fifo);
uint16_t UART_RxFifo_IsEmpty(UART_RxFifo_TypeDef *fifo);

#endif /* UART_FIFO_H */
