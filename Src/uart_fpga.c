#include "uart_fpga.h"

#include "usart.h"
#include "data.h"
#include "log.h"
#include "algorithm.h"

int uart_fpga_input(const u8 *data, u16 length)
{
    float adData[33] = {0};
    Complex fftData[33] = {0};
    int i = 0;

    if (length < 64)
    {
        LOG_DEBUG("uart input recieved %d, less than 64", length);
        return F_FAILED;
    }

    if (L_TRUE == data_isIgnore())
    {
        data_plusRecieved();
        return F_SUCCESS;
    }

    parseAdData(adData, data);
    PRINT("ad data:\r\n");
    for (i = 0; i < 32; ++i)
    {
        PRINT("%f\r\n", adData[i]);
    }
    fft(adData, fftData, 32);

    data_setInputResult(fftData[1]);
    data_plusRecieved();
    return F_SUCCESS;
}

int uart_fpga_output(const u8 *data, u16 length)
{
    float adData[33] = {0};
    Complex fftData[33] = {0};

    if (length < 64)
    {
        LOG_DEBUG("uart output recieved %d, less than 64", length);
        return F_FAILED;
    }

    if (L_TRUE == data_isIgnore())
    {
        data_plusRecieved();
        return F_SUCCESS;
    }

    parseAdData(adData, data);
    fft(adData, fftData, 32);

    data_setOutputResult(fftData[1]);
    data_plusRecieved();
    return F_SUCCESS;
}
