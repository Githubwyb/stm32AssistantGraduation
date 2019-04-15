#include "spi_fpga.h"

#include "stm32f1xx_hal.h"

#include "log.h"
#include "data.h"
#include "msg.h"
#include "spi_dds.h"

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi3;

static u8 spiFpgaRecvData[4];

int spi_fpga_recvStart(void) {
    HAL_SPI_Receive_IT(&hspi1, spiFpgaRecvData, 4);
    return F_SUCCESS;
}

int spi_fpga_printStatus(void) {
    LOG_DEBUG("RxXferCount %d", hspi1.RxXferCount);
    LOG_HEX(spiFpgaRecvData, sizeof(spiFpgaRecvData));
    return F_SUCCESS;
}

int parsSpiFpgaData(const u8 *data) {
    u32 divFre = 0;
    float fpgaFre = 0;

    // LOG_DEBUG("Spi fpga recv:");
    // LOG_BIN(spiFpgaRecvData, sizeof(spiFpgaRecvData));

    divFre = data_ntohl(data);
    if (GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_3)) {
        fpgaFre = FPGA_SYSTEM_TICK / 1.0 / divFre;
    } else {
        fpgaFre = FPGA_SYSTEM_TICK_1 / 1.0 / divFre;
    }
    LOG_DEBUG("Fpga recv divFre %d, fre %.4f", divFre, fpgaFre);
    // spi_fpga_recvStart();
    return F_SUCCESS;
}

int spi_fpgaRecv_IT(void) {
    msg_sendData(APP_MAIN, SPI1_RECV, spiFpgaRecvData, 4);
    spi_fpga_recvStart();
    return F_SUCCESS;
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi == &hspi1) {
        msg_sendCoreCmd(CORE_SPI1_RECV);
    } else if (hspi == &hspi3) {
        msg_sendCoreCmd(CORE_SPI3_RECV);
    }
}
