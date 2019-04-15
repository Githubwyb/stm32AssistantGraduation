#ifndef _ALGORITHM_H_
#define _ALGORITHM_H_

#include "types.h"

int get_RValue(FPGA_RESULT result, Complex *R);
int get_fre(u8 mode);
int get_freLine(void);
int get_divLine(double fre);
int get_freLog(void);
int get_divLog(double fre);
int fft(float *src, Complex *dst, int size_n);

#endif // !_ALGORITHM_H_
