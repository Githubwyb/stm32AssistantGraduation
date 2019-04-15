#include "led.h"

#include "timer.h"
#include "spi_dds.h"

int led_change(LED_ENUM led)
{
    if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(GPIOC, led))
    {
        HAL_GPIO_WritePin(GPIOC, led, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOC, led, GPIO_PIN_RESET);
    }

    return F_SUCCESS;
}

int led_write(LED_ENUM led, DEVICE_STATE state)
{
    if (state == ON)
    {
        HAL_GPIO_WritePin(GPIOC, led, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOC, led, GPIO_PIN_SET);
    }

    return F_SUCCESS;
}

int leds_write(u8 state)
{
    HAL_GPIO_WritePin(GPIOC, state, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, ~state, GPIO_PIN_SET);

    return F_SUCCESS;
}

static int led_timer_startCb(void)
{
    static u8 flag = 0;
    if (flag)
    {
        flag = 0;
        led_write(LED_1, ON);
        timer_start(TIMER_LED_BUZZER, 100, led_timer_startCb);
    }
    else
    {
        flag = 1;
        led_write(LED_1, OFF);
        timer_start(TIMER_LED_BUZZER, 900, led_timer_startCb);
    }
    return F_SUCCESS;
}

int led_start(void)
{
    led_write(LED_1, ON);
    timer_start(TIMER_LED_BUZZER, 100, led_timer_startCb);
    return F_SUCCESS;
}

static int led_timer_endCb(void)
{
    led_write(LED_1, OFF);
    return F_SUCCESS;
}

int led_end(void)
{
    led_write(LED_1, ON);
    // dds_waveSeting(0, 0, SIN_WAVE, 0);
    timer_stop(TIMER_LED_BUZZER);
    timer_start(TIMER_LED_BUZZER, 500, led_timer_endCb);
    return F_SUCCESS;
}

static int led_timer_checkCb(void)
{
    led_change(LED_0);
    timer_start(TIMER_LED_BUZZER, 200, led_timer_checkCb);
    return F_SUCCESS;
}

int led_checkStart(void)
{
    timer_start(TIMER_LED_BUZZER, 200, led_timer_checkCb);
    return F_SUCCESS;
}

static int led_timer_delayCb(void)
{
    led_write(LED_1, OFF);
    return F_SUCCESS;
}

int led_delay(u32 count)
{
    led_write(LED_1, ON);
    timer_start(TIMER_LED_BUZZER, count, led_timer_delayCb);
    return F_SUCCESS;
}

static int led_timer_checkEndCb(void)
{
    led_write(LED_0, ON);
    return F_SUCCESS;
}

int led_checkEnd(void)
{
    led_write(LED_0, OFF);
    timer_stop(TIMER_LED_BUZZER);
    timer_start(TIMER_LED_BUZZER, 500, led_timer_checkEndCb);
    return F_SUCCESS;
}
