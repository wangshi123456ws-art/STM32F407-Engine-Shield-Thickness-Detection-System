#ifndef PROGRAM_OPEN_USART_H
#define PROGRAM_OPEN_USART_H

#include <stdio.h>

#include "stm32f4xx_conf.h"
#include "sys.h"

#define USART_REC_LEN 200u
#define EN_USART1_RX 1
#define EN_USART6_RX 1

#define USART_RX_DONE 0x8000u
#define USART_RX_COUNT_MASK 0x3FFFu

extern u16 USART1_RX_BUF[USART_REC_LEN];
extern u16 USART1_RX_STA;
extern u16 USART6_RX_BUF[USART_REC_LEN];
extern u16 USART6_RX_STA;

void uart_init1(u32 bound);
void uart_init6(u32 bound);

#endif
