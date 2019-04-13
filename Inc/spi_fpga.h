#ifndef _SPI_FPGA_H_
#define _SPI_FPGA_H_

#include "types.h"

int fpga_sendData(u8 *data, u16 length);
int fpga_writeDivFre(u32 divFre);
int fpga_start(void);
int fpga_end(void);

#endif // !_SPI_FPGA_H_
