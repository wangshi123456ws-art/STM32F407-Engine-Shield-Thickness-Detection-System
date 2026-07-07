#ifndef PROGRAM_OPEN_DELAY_H
#define PROGRAM_OPEN_DELAY_H

#include "sys.h"

void delay_init(u8 SYSCLK);
void delay_ms(u16 nms);
void delay_us(u32 nus);

#endif
