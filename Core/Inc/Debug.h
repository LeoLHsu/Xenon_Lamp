#ifndef __DEBUG_H__
#define __DEBUG_H__
#include "main.h"
#include "usart.h"

#ifdef DEBUG

// #define USE_DEBUG_RECEIVE
#define USE_DEBUG_RINGBUFFER

#define Debug_TxCallBack_ISR            UART1_TxCallBack_ISR
#define Debug_RxCallBack_ISR            UART1_RxCallBack_ISR
#define Debug_Tx_SendData(x)            HAL_UART_Transmit_IT(&huart1, (uint8_t *)&x, 1)

extern void Debug_Initial(void);
extern void Debug_Handler(void);
extern void Debug_TxCallBack_ISR(void);
extern void Debug_RxCallBack_ISR(uint8_t Data);

#ifdef USE_DEBUG_RINGBUFFER
#define printData           printf
#define printMsg            printf
#else
extern void printMsg(void *str);
extern void printData(void *str, int32_t value);
#endif

#else
#define Debug_Initial()
#define Debug_Handler()
#define Debug_TxCallBack_ISR()
#define Debug_RxCallBack_ISR(x)
#define printMsg(x)
#define printData(x, y)
#endif

#endif