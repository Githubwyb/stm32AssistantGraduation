#include "gpioExit.h"

#include "stm32f1xx_hal.h"

#include "msg.h"
#include "spi_fpga.h"
#include "log.h"

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    switch (GPIO_Pin) {
        case GPIO_PIN_0:
            if (GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_0)) {
                //FPGA_START_1
                msg_sendCoreCmd(CORE_GPIO_EXIT_RISING_0);
            } else {
                //FPGA_START_0
                msg_sendCoreCmd(CORE_GPIO_EXIT_FALLING_0);
            }

            break;

        case GPIO_PIN_1:
            if (GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_1)) {
                //FPGA_RST_1
                msg_sendCoreCmd(CORE_GPIO_EXIT_RISING_1);
            } else {
                //FPGA_RST_0
                msg_sendCoreCmd(CORE_GPIO_EXIT_FALLING_1);
            }
            break;

        case GPIO_PIN_2:
            // LOG_DEBUG("FPGA start recv");

            break;

        default:
            break;
    }
}
