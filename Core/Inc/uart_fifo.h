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

/**
  * @brief  UART handle Structure definition
  */
typedef struct UART_Fifo_ItemStruct
{
    uint8_t                     *data;
    volatile uint16_t           size;
    struct UART_Fifo_ItemStruct *next;

} UART_Fifo_ItemTypeDef;

/**
  * @brief  UART handle Structure definition
  */
typedef struct UART_Fifo_HandleStruct
{
  UART_HandleTypeDef             *huart;
  volatile UART_Fifo_ItemTypeDef *head;
  UART_Fifo_ItemTypeDef          *tail;

} UART_Fifo_HandleTypeDef;


extern UART_Fifo_HandleTypeDef huart_fifo1;
extern UART_Fifo_HandleTypeDef huart_fifo2;

/**
 *  Public Function Prototypes
 */
void UART_Fifo_Init( void );
void UART_Fifo_Transmit (UART_Fifo_HandleTypeDef *fifo, UART_Fifo_ItemTypeDef *item);
void UART_Fifo_TxCpltCallback (UART_HandleTypeDef *huart);


#endif /* UART_FIFO_H */
