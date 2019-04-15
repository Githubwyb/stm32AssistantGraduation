#ifndef _SPI_DDS_H_
#define _SPI_DDS_H_

#include "types.h"

#define TRI_WAVE 0
#define SIN_WAVE 1
#define SQU_WAVE 2

int dds_waveSeting(double Freq, u32 Freq_SFR, u32 WaveMode, u32 Phase);

int spi_dds_recvStart(void);
int parsSpiDdsData(void);

#endif // !_SPI_DDS_H_
