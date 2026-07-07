#ifndef _AD9959_H_
#define _AD9959_H_

#include "include.h"

//AD9959寄存器地址定义
#define CSR_ADD   0x00   //CSR 通道选择寄存器
#define FR1_ADD   0x01   //FR1 功能寄存器1
#define FR2_ADD   0x02   //FR2 功能寄存器2
#define CFR_ADD   0x03   //CFR 通道功能寄存器
#define CFTW0_ADD 0x04   //CTW0 通道频率转换字寄存器
#define CPOW0_ADD 0x05   //CPW0 通道相位转换字寄存器
#define ACR_ADD   0x06   //ACR 幅度控制寄存器
#define LSRR_ADD  0x07   //LSR 通道线性扫描寄存器
#define RDW_ADD   0x08   //RDW 通道线性向上扫描寄存器
#define FDW_ADD   0x09   //FDW 通道线性向下扫描寄存器

//AD9959管脚宏定义
#define CS			PAout(6)
#define SCLK		PBout(1)
#define UPDATE	    PBout(0)
#define PS0			PAout(7)
#define PS1			PAout(2)
#define PS2			PBout(10)
#define PS3			PCout(0)
#define SDIO0		PAout(5)
#define SDIO1		PBout(11)
#define SDIO2		PAout(3)
#define SDIO3		PAout(8)
#define AD9959_PWR	PCout(2)
#define Reset		PCout(3)

#define ACC_FRE_FACTOR	8.589934592

//CSR[7:4]通道选择启用位
#define CH0 0x10
#define CH1 0x20
#define CH2 0x40
#define CH3 0x80




void delay1 (u32 length);
void IntReset(void);	         //AD9959复位
void IO_Update(void);          //AD9959更新数据
void Intserve(void);		       //IO口初始化
void Init_AD9959(void);
void Match_AD9959(uint32_t Fre); 

void AD9959_WriteData(uint8_t RegisterAddress, uint8_t NumberofRegisters, uint8_t *RegisterData);//向AD9959写数据
void Write_CFTW0(uint32_t fre);										//写CFTW0通道频率转换字寄存器
void Write_ACR(uint16_t Ampli);										//写ACR通道幅度转换字寄存器
void Write_CPOW0(uint16_t Phase);									//写CPOW0通道相位转换字寄存器
void AD9959_Set_Fre(uint8_t Channel,uint32_t Freq);                 //写频率
void AD9959_Set_Amp(uint8_t Channel, uint16_t Ampli);               //写幅度
void AD9959_Set_Phase(uint8_t Channel,uint16_t Phase);              //写相位

#endif


