#include <string.h>
#include <stdarg.h>

#include "stm32f1xx_hal.h"

#include "usart.h"
#include "log.h"
#include "msg.h"
#include "timer.h"

#define UART_TIEM_OUT 50

static UART_STRUCT uart1_struct = {0};
static UART_STRUCT uart2_struct = {0};
static UART_STRUCT uart3_struct = {0};
static UART_STRUCT uart4_struct = {0};
static UART_STRUCT uart5_struct = {0};

/*
* 使能串口空闲状态中断, 在接收完成后会产生一个空闲中断以规避需要超时处理的问题
* 开启DMA接收串口数据, 在收到数据后要么产生空闲中断, 要么产生DMA中断
* 问题: 开启DMA接收串口数据的时候, 会直接将当前的数据接收下来, 会在启动的时候产生一个错误接收
*/
void usart_init(void)
{
    LOG_DEBUG("usart init");
    HAL_UART_Receive_IT(&huart1, uart1_struct.rxBuffer, UART_RX_BUF_LEN);
    HAL_UART_Receive_IT(&huart2, uart2_struct.rxBuffer, UART_RX_BUF_LEN);
    HAL_UART_Receive_IT(&huart3, uart3_struct.rxBuffer, UART_RX_BUF_LEN);
    HAL_UART_Receive_IT(&huart4, uart4_struct.rxBuffer, UART_RX_BUF_LEN);
    HAL_UART_Receive_IT(&huart5, uart5_struct.rxBuffer, UART_RX_BUF_LEN);
}

UART_STRUCT *usart_getUartStruct(UART_HandleTypeDef *huart)
{
    UART_STRUCT *pUartStruct = NULL;

    if(huart == &huart1)
    {
        pUartStruct = &uart1_struct;
    }
    else if (huart == &huart2)
    {
        pUartStruct = &uart2_struct;
    }
    else if (huart == &huart3)
    {
        pUartStruct = &uart3_struct;
    }
    else if (huart == &huart4)
    {
        pUartStruct = &uart4_struct;
    }
    else if (huart == &huart5)
    {
        pUartStruct = &uart5_struct;
    }

    return pUartStruct;
}

/*
* 串口动态参数打印字符串接口
*/
void usart_print(UART_HandleTypeDef *huart, const char *fmt, ...)
{
    va_list arg;
    u16 length = 0;
    u8 data[UART_TX_BUF_LEN] = {0};

    va_start(arg, fmt);
    vsnprintf((char *)data, UART_TX_BUF_LEN, fmt, arg);
    va_end(arg);

    length = strlen((const char *)data);

    usart_transmit_data(huart, data, length);
}

/*
* 串口指定长度传输数据接口
*/
int usart_transmit_data(UART_HandleTypeDef *huart, const void *data, u16 length)
{
    if (HAL_OK == HAL_UART_Transmit(huart, (u8 *)data, length, 50))
    {
        return F_SUCCESS;
    }

    return F_FAILED;
}


//uart1
/*
* 串口接受处理函数
*/
static void uart1_recv_proc(void *data, u16 length)
{
    msg_sendData(APP_MAIN, USART1_RECV, data, length);
}

static int usart1_timeout(void)
{
    /*接收到的数据处理函数*/
    uart1_recv_proc(uart1_struct.rxBuffer, UART_RX_BUF_LEN - huart1.RxXferCount);

    /* 清除串口接收缓冲区和DMA接收状态*/
    HAL_UART_AbortReceive_IT(&huart1);
    memset(uart1_struct.rxBuffer, 0, UART_RX_BUF_LEN);
    HAL_UART_Receive_IT(&huart1, uart1_struct.rxBuffer, UART_RX_BUF_LEN);
    return F_SUCCESS;
}

/*
* 串口接收中断
*/
void usart1_recvData_IT(void)
{
    timer_start(TIMER_UART1_TIMEOUT, UART_TIEM_OUT, usart1_timeout);
}

//uart2
/*
* 串口接受处理函数
*/
static void uart2_recv_proc(void *data, u16 length)
{
    msg_sendData(APP_MAIN, USART2_RECV, data, length);
}

static int usart2_timeout(void)
{
    /*接收到的数据处理函数*/
    uart2_recv_proc(uart2_struct.rxBuffer, UART_RX_BUF_LEN - huart2.RxXferCount);

    /* 清除串口接收缓冲区和DMA接收状态*/
    HAL_UART_AbortReceive_IT(&huart2);
    memset(uart2_struct.rxBuffer, 0, UART_RX_BUF_LEN);
    HAL_UART_Receive_IT(&huart2, uart2_struct.rxBuffer, UART_RX_BUF_LEN);
    return F_SUCCESS;
}

/*
* 串口接收中断
*/
void usart2_recvData_IT(void)
{
    timer_start(TIMER_UART2_TIMEOUT, UART_TIEM_OUT, usart2_timeout);
}

//uart3
/*
* 串口接受处理函数
*/
static void uart3_recv_proc(void *data, u16 length)
{
    msg_sendData(APP_MAIN, USART3_RECV, data, length);
}

static int usart3_timeout(void)
{
    /*接收到的数据处理函数*/
    uart3_recv_proc(uart3_struct.rxBuffer, UART_RX_BUF_LEN - huart3.RxXferCount);

    /* 清除串口接收缓冲区和DMA接收状态*/
    HAL_UART_AbortReceive_IT(&huart3);
    memset(uart3_struct.rxBuffer, 0, UART_RX_BUF_LEN);
    HAL_UART_Receive_IT(&huart3, uart3_struct.rxBuffer, UART_RX_BUF_LEN);
    return F_SUCCESS;
}

/*
* 串口接收中断
*/
void usart3_recvData_IT(void)
{
    timer_start(TIMER_UART3_TIMEOUT, UART_TIEM_OUT, usart3_timeout);
}

//uart4
/*
* 串口接受处理函数
*/
static void uart4_recv_proc(void *data, u16 length)
{
    msg_sendData(APP_MAIN, USART4_RECV, data, length);
}

static int usart4_timeout(void)
{
    /*接收到的数据处理函数*/
    uart4_recv_proc(uart4_struct.rxBuffer, UART_RX_BUF_LEN - huart4.RxXferCount);

    /* 清除串口接收缓冲区和DMA接收状态*/
    HAL_UART_AbortReceive_IT(&huart4);
    memset(uart4_struct.rxBuffer, 0, UART_RX_BUF_LEN);
    HAL_UART_Receive_IT(&huart4, uart4_struct.rxBuffer, UART_RX_BUF_LEN);
    return F_SUCCESS;
}

/*
* 串口接收中断
*/
void usart4_recvData_IT(void)
{
    timer_start(TIMER_UART4_TIMEOUT, UART_TIEM_OUT, usart4_timeout);
}

//uart5
/*
* 串口接受处理函数
*/
static void uart5_recv_proc(void *data, u16 length)
{
    msg_sendData(APP_MAIN, USART5_RECV, data, length);
}

static int usart5_timeout(void)
{
    /*接收到的数据处理函数*/
    uart5_recv_proc(uart5_struct.rxBuffer, UART_RX_BUF_LEN - huart5.RxXferCount);

    /* 清除串口接收缓冲区和DMA接收状态*/
    HAL_UART_AbortReceive_IT(&huart5);
    memset(uart5_struct.rxBuffer, 0, UART_RX_BUF_LEN);
    HAL_UART_Receive_IT(&huart5, uart5_struct.rxBuffer, UART_RX_BUF_LEN);
    return F_SUCCESS;
}

/*
* 串口接收中断
*/
void usart5_recvData_IT(void)
{
    timer_start(TIMER_UART5_TIMEOUT, UART_TIEM_OUT, usart5_timeout);
}
