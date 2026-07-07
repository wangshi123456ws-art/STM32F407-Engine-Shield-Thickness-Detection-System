#ifndef _Upper_Computer_H_
#define _Upper_Computer_H_

#include "include.h"

extern uint32_t freq;
extern uint16_t gain;

extern void Recv_Instruction(void);
void Set_Freq(uint32_t temp_Fre);
void Choice_Freq(uint8_t freq_Name);
void Choice_Gain(uint8_t gain_Name);
void Detection_Cycle(void);




#endif

