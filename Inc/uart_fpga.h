#ifndef _UART_FPGA_H_
#define _UART_FPGA_H_

#include "types.h"

int uart_fpga_input(const u8 *data, u16 length);
int uart_fpga_output(const u8 *data, u16 length);

#endif // !_UART_FPGA_H_
