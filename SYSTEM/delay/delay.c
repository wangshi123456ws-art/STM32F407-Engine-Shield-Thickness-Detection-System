#include "delay.h"

static u8 s_fac_us = 0;

void delay_init(u8 SYSCLK)
{
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    s_fac_us = (u8)(SYSCLK / 8u);
    SysTick->LOAD = 0x00FFFFFFu;
    SysTick->VAL = 0u;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

void delay_us(u32 nus)
{
    u32 ticks;
    u32 told;
    u32 tnow;
    u32 tcnt = 0;
    u32 reload = SysTick->LOAD;

    if (nus == 0u || s_fac_us == 0u)
    {
        return;
    }

    ticks = nus * s_fac_us;
    told = SysTick->VAL;

    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)
            {
                tcnt += told - tnow;
            }
            else
            {
                tcnt += reload - tnow + told;
            }
            told = tnow;

            if (tcnt >= ticks)
            {
                break;
            }
        }
    }
}

void delay_ms(u16 nms)
{
    while (nms > 0u)
    {
        delay_us(1000u);
        nms--;
    }
}
