#ifndef _UART_WIFI_H_
#define _UART_WIFI_H_

#include "types.h"
#include "data.h"

int uart_wifi_proc(const u8 *data, u16 length);
int uart_wifi_sendData(u8 *data, u16 length);
int uart_wifi_sendResult(FPGA_RESULT result);
int uart_wifi_sendSetting(SETTING *pSetting);
int uart_wifi_sendEnd(void);
int uart_wifi_sendSetOk(void);
int uart_wifi_sendSetFailed(void);

#endif // !_UART_WIFI_H_
