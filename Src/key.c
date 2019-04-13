#include "key.h"

#include "stm32f1xx_hal.h"

#include "msg.h"

typedef struct {
    u8 p;
    u8 r;
    GPIO_TypeDef *gpiox;
    u16 gpio;
    GPIO_PinState releaseState;
} KEY_STRUCT;

static KEY_STRUCT key_map[] = {
    {0, 0, GPIOA, GPIO_PIN_0, GPIO_PIN_RESET},
    {0, 0, GPIOE, GPIO_PIN_2, GPIO_PIN_SET},
    {0, 0, GPIOE, GPIO_PIN_3, GPIO_PIN_SET},
    {0, 0, GPIOE, GPIO_PIN_4, GPIO_PIN_SET},
};

int key_detectProc(void)
{
    u8 i = 0;
    KEY_MSG key_msg;
    for (i = 0; i < sizeof(key_map) / sizeof(key_map[0]); i++) {
        if(key_map[i].releaseState == HAL_GPIO_ReadPin(key_map[i].gpiox, key_map[i].gpio))
        {
            key_map[i].r++;
            if(key_map[i].r == 2)
            {
                key_map[i].p = 0;
                key_msg.key = (KEY_ENUM)i;
                key_msg.state = KEY_RELEASE;
                msg_sendData(APP_MAIN, KEY_CHANGE, &key_msg, sizeof(key_msg));
            }
        }
        else
        {
            key_map[0].p++;
            if(key_map[0].p == 2)
            {
                key_map[0].r = 0;
                key_msg.key = (KEY_ENUM)i;
                key_msg.state = KEY_PRESS;
                msg_sendData(APP_MAIN, KEY_CHANGE, &key_msg, sizeof(key_msg));
            }
        }

        if(key_map[i].p >= 250)
        {
            key_map[i].p = 250;
        }
        if(key_map[i].r >= 250)
        {
            key_map[i].r = 250;
        }
    }

    return F_SUCCESS;
}
