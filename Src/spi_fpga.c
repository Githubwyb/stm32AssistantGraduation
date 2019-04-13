#include "spi_fpga.h"

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#include "log.h"
#include "data.h"
#include "timer.h"

#define FPGA_SPI_CS_ENABLE HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET)
#define FPGA_SPI_CS_DISABLE HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET)

#define FPGA_START_0 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET)
#define FPGA_START_1 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET)

#define FPGA_RST_0 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET)
#define FPGA_RST_1 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET)

extern SPI_HandleTypeDef hspi2;

int fpga_sendData(u8 *data, u16 length)
{
    int rc = 0;

    FPGA_SPI_CS_ENABLE;
    rc = HAL_SPI_Transmit(&hspi2, data, length, 50);
    FPGA_SPI_CS_DISABLE;
    PRINT("send to fpga: ");
    LOG_BIN(data, length);

    return rc;
}

int fpga_writeDivFre(u32 divFre)
{
    u8 sendData[4] = {0};

    data_htonl(divFre, sendData);

    return fpga_sendData(sendData, 4);
}

int fpga_start(void)
{
    FPGA_RST_0;
    timer_start(TIMER_FPGA_TIMEOUT, 2, NULL);
    while(L_TRUE == timer_isTimerStart(TIMER_FPGA_TIMEOUT))
    {
        osDelay(1);
    }
    FPGA_RST_1;

    FPGA_START_1;

    return F_SUCCESS;
}

int fpga_end(void)
{
    FPGA_START_0;
    FPGA_RST_0;

    return F_SUCCESS;
}
