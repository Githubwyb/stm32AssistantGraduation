#ifndef _SPI_FPGA_H_
#define _SPI_FPGA_H_

#include "types.h"

int spi_fpga_recvStart(void);
int parsSpiFpgaData(const u8 *data);
int spi_fpga_printStatus(void);
int spi_fpgaRecv_IT(void);

#endif // !_SPI_FPGA_H_
