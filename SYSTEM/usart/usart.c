#include "usart.h"

#define USART1_FRAME_LEN 6u

u16 USART1_RX_BUF[USART_REC_LEN];
u16 USART1_RX_STA = 0;
u16 USART6_RX_BUF[USART_REC_LEN];
u16 USART6_RX_STA = 0;

#if defined(__CC_ARM)
#pragma import(__use_no_semihosting)
struct __FILE
{
    int handle;
};
FILE __stdout;
void _sys_exit(int x)
{
    (void)x;
}
#endif

int fputc(int ch, FILE *f)
{
    (void)f;
    while ((USART1->SR & USART_SR_TC) == 0u)
    {
    }
    USART1->DR = (u8)ch;
    return ch;
}

void uart_init1(u32 bound)
{
    GPIO_InitTypeDef gpio;
    USART_InitTypeDef usart;
    NVIC_InitTypeDef nvic;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

    gpio.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &gpio);

    usart.USART_BaudRate = bound;
    usart.USART_WordLength = USART_WordLength_8b;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_Parity = USART_Parity_No;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &usart);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);

    nvic.NVIC_IRQChannel = USART1_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 3;
    nvic.NVIC_IRQChannelSubPriority = 3;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
}

void uart_init6(u32 bound)
{
    GPIO_InitTypeDef gpio;
    USART_InitTypeDef usart;
    NVIC_InitTypeDef nvic;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);

    gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &gpio);

    usart.USART_BaudRate = bound;
    usart.USART_WordLength = USART_WordLength_8b;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_Parity = USART_Parity_No;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART6, &usart);
    USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART6, ENABLE);

    nvic.NVIC_IRQChannel = USART6_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 5;
    nvic.NVIC_IRQChannelSubPriority = 3;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
}

void USART1_IRQHandler(void)
{
    u8 data;
    u16 count;

    if (USART_GetITStatus(USART1, USART_IT_RXNE) == RESET)
    {
        return;
    }

    data = (u8)USART_ReceiveData(USART1);
    if ((USART1_RX_STA & USART_RX_DONE) != 0u)
    {
        return;
    }

    count = USART1_RX_STA & USART_RX_COUNT_MASK;
    if (count >= USART_REC_LEN)
    {
        USART1_RX_STA = 0;
        return;
    }

    USART1_RX_BUF[count] = data;
    count++;
    USART1_RX_STA = count;

    if (count >= USART1_FRAME_LEN)
    {
        USART1_RX_STA |= USART_RX_DONE;
    }
}

void USART6_IRQHandler(void)
{
    static u16 expected_len = 0;
    u8 data;
    u16 count;

    if (USART_GetITStatus(USART6, USART_IT_RXNE) == RESET)
    {
        return;
    }

    data = (u8)USART_ReceiveData(USART6);
    if ((USART6_RX_STA & USART_RX_DONE) != 0u)
    {
        return;
    }

    count = USART6_RX_STA & USART_RX_COUNT_MASK;
    if (count == 0u && data != 0x5Au)
    {
        expected_len = 0;
        return;
    }
    if (count == 1u && data != 0xA5u)
    {
        USART6_RX_STA = 0;
        expected_len = 0;
        return;
    }
    if (count >= USART_REC_LEN)
    {
        USART6_RX_STA = 0;
        expected_len = 0;
        return;
    }

    USART6_RX_BUF[count] = data;
    count++;
    USART6_RX_STA = count;

    if (count == 3u)
    {
        expected_len = USART6_RX_BUF[2] + 3u;
        if (expected_len > USART_REC_LEN)
        {
            USART6_RX_STA = 0;
            expected_len = 0;
            return;
        }
    }

    if (expected_len != 0u && count >= expected_len)
    {
        USART6_RX_STA |= USART_RX_DONE;
        expected_len = 0;
    }
}
