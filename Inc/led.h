#ifndef _LED_H_
#define _LED_H_

#include "types.h"

#include "stm32f1xx_hal.h"

typedef enum
{
    LED_0 = GPIO_PIN_0,
    LED_1 = GPIO_PIN_1,
    LED_2 = GPIO_PIN_2,
    LED_3 = GPIO_PIN_3,
    LED_4 = GPIO_PIN_4,
    LED_5 = GPIO_PIN_5,
    LED_6 = GPIO_PIN_6,
    LED_7 = GPIO_PIN_7,
} LED_ENUM;

typedef enum
{
    OFF = 0x00,
    ON = 0x01,
} DEVICE_STATE;

int led_change(LED_ENUM led);
int led_write(LED_ENUM led, DEVICE_STATE state);
int leds_write(u8 state);
int led_start(void);
int led_end(void);
int led_checkStart(void);
int led_checkEnd(void);
int led_delay(u32 count);

#endif // !_LED_H_
