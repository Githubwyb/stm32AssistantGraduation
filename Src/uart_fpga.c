#include "uart_fpga.h"

#include "usart.h"
#include "data.h"
#include "log.h"
#include "algorithm.h"

float adOriginData[] = {4.271062,
                4.202686,
                3.965812,
                3.570208,
                3.059829,
                2.410256,
                1.680098,
                0.879121,
                0.051282,
                -0.766789,
                -1.553114,
                -2.288156,
                -2.942613,
                -3.479853,
                -3.868132,
                -4.129426,
                -4.227106,
                -4.156288,
                -3.926740,
                -3.555556,
                -3.030525,
                -2.412699,
                -1.699634,
                -0.888889,
                -0.083028,
                0.766789,
                1.560440,
                2.288156,
                2.942613,
                3.514041,
                3.924298,
                4.168498};

static int computeAd(const float adValue)
{
    float Res = 0;
    int databits = 0;

    Res = adValue * 409.5;
    if (Res < 0)
    {
        Res += 0x800;
        databits = Res;
        databits &= 0x7ff;
        databits += 0x800;
    } else {
        databits = Res;
        databits &= 0x7ff;
    }

    return databits;
}

void parseAdData(u8 *input, const float *source)
{
    int i;
    // int j = 0;
    u8 low = 0;
    u8 high = 0;
    u32 result = 0;

    for (i = 0; i < 32; i++)
    {
        result = computeAd(source[i]);
        low = result & 0xff;
        high = result >> 8;
        input[i * 2] = low;
        input[i * 2 + 1] = high;
    }
}

int uart_fpga_inputSend(void)
{
    u8 inputSendData[64];
    parseAdData(inputSendData, adOriginData);
    usart_transmit_data(&huart3, inputSendData, 64);
    // LOG_HEX(inputSendData, 64);
    return F_SUCCESS;
}

int uart_fpga_outputSend(void)
{
    u8 outputSendData[64];
    parseAdData(outputSendData, adOriginData);
    usart_transmit_data(&huart5, outputSendData, 64);
    // LOG_HEX(outputSendData, 64);
    return F_SUCCESS;
}
