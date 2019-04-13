#include "timer.h"

#include "led.h"
#include "log.h"
#include "key.h"

static TIMER_STRUCT timer_handler[TIMER_NUM];

void timer1_cb(void const * argument)
{
    static u32 count = 0;
    timer_update_IT();
    count++;

// #ifdef DEBUG
//     if(count % 500 == 0)
//     {
//         // LOG_DEBUG("timer happened");
//         led_change(LED_0);
//     }
// #endif

    if(count % 10 == 0)
    {
        key_detectProc();
    }
}

int timer_update_IT(void)
{
    int i = 0;
    TIMER_CB pFun;

    for(i = 0; i < TIMER_NUM; i++)
    {
        if(L_TRUE == timer_handler[i].isStart)
        {
            timer_handler[i].count++;

            if(timer_handler[i].count > timer_handler[i].period)
            {
                timer_handler[i].isStart = L_FALSE;
                if(timer_handler[i].pFun)
                {
                    pFun = timer_handler[i].pFun;
                    timer_handler[i].pFun = NULL;
                    pFun();
                }
            }
        }
    }

    return F_SUCCESS;
}

int timer_start(TIMER_ENUM id, u32 period, TIMER_CB pFun)
{
    timer_handler[id].period = period;
    timer_handler[id].count = 0;
    timer_handler[id].pFun = pFun;
    timer_handler[id].isStart = L_TRUE;
    return F_SUCCESS;
}

int timer_stop(TIMER_ENUM id)
{
    timer_handler[id].isStart = L_FALSE;
    return F_SUCCESS;
}

int timer_isTimerStart(TIMER_ENUM id)
{
    return timer_handler[id].isStart;
}

int timer_delay_ms(u32 count)
{
    timer_start(TIMER_DELAY, count, NULL);
    while(L_TRUE == timer_isTimerStart(TIMER_DELAY)){}
    return F_SUCCESS;
}
