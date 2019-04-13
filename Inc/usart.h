#ifndef _USART_H_
#define _USART_H_

#include "types.h"

#include "stm32f1xx_hal.h"

#define UART_RX_BUF_LEN (200)
#define UART_TX_BUF_LEN (500)

typedef struct
{
    u8 isTransmiting;
    u8 rxBuffer[UART_RX_BUF_LEN];
} UART_STRUCT;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;

void usart_init(void);
UART_STRUCT *usart_getUartStruct(UART_HandleTypeDef *huart);
void usart_print(UART_HandleTypeDef *huart, const char *fmt, ...);
int usart_transmit_data(UART_HandleTypeDef *huart, const void *data, u16 length);
void usart1_recvData_IT(void);
void usart2_recvData_IT(void);
void usart3_recvData_IT(void);
void usart4_recvData_IT(void);
void usart5_recvData_IT(void);

#endif // !_USART_H_
