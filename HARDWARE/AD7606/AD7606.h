#ifndef _AD7606_H_
#define _AD7606_H_

#include "include.h"

#define CH_NUM			8				// 采集8通道 

// 定义GPIO端口 
#define RCC_SCK 	RCC_AHB1Periph_GPIOB
#define PORT_SCK	GPIOB
#define PIN_SCK		GPIO_Pin_3

#define RCC_MOSI 	RCC_AHB1Periph_GPIOB
#define PORT_MOSI	GPIOB
#define PIN_MOSI	GPIO_Pin_5

#define RCC_MISO 	RCC_AHB1Periph_GPIOB
#define PORT_MISO	GPIOB
#define PIN_MISO	GPIO_Pin_4

#define SCK_0()		GPIO_ResetBits(PORT_SCK, PIN_SCK)
#define SCK_1()		GPIO_SetBits(PORT_SCK, PIN_SCK)

#define MOSI_0()	GPIO_ResetBits(PORT_MOSI, PIN_MOSI)
#define MOSI_1()	GPIO_SetBits(PORT_MOSI, PIN_MOSI)

#define MISO_IS_HIGH()	(GPIO_ReadInputDataBit(PORT_MISO, PIN_MISO) == Bit_SET)
	
	
	
// 片选 
#define AD_CS_0()					    	GPIO_ResetBits(PORT_CS, PIN_CS)
#define AD_CS_1()						    GPIO_SetBits(PORT_CS, PIN_CS)

// 设置量程 
#define AD_RANGE_5V()					  GPIO_ResetBits(PORT_RANGE, PIN_RANGE)
#define AD_RANGE_10V()					GPIO_SetBits(PORT_RANGE, PIN_RANGE)	//置1

// 复位引脚 
#define AD_RESET_LOW()					GPIO_ResetBits(PORT_RESET, PIN_RESET)
#define AD_RESET_HIGH()					GPIO_SetBits(PORT_RESET, PIN_RESET)

// 起始信号
#define	AD_CONVST_LOW()					GPIO_ResetBits(PORT_CONVST, PIN_CONVST)
#define	AD_CONVST_HIGH()				GPIO_SetBits(PORT_CONVST, PIN_CONVST)

	
	

// CSN片选 
#define RCC_CS 		RCC_AHB1Periph_GPIOF
#define PORT_CS		GPIOF
#define PIN_CS		GPIO_Pin_7

//RESET 
#define RCC_RESET 	RCC_AHB1Periph_GPIOH
#define PORT_RESET	GPIOB
#define PIN_RESET	  GPIO_Pin_13
//RANGE 		
#define RCC_RANGE 	RCC_AHB1Periph_GPIOA
#define PORT_RANGE 	GPIOA
#define PIN_RANGE 	GPIO_Pin_4

#define RESET_0()	GPIO_ResetBits(PORT_RESET, PIN_RESET)
#define RESET_1()	GPIO_SetBits(PORT_RESET, PIN_RESET)

// CONVST		
#define RCC_CONVST 	RCC_AHB1Periph_GPIOH
#define PORT_CONVST	GPIOB
#define PIN_CONVST 	GPIO_Pin_12

// BUSY 
#define RCC_BUSY 	  RCC_AHB1Periph_GPIOH
#define PORT_BUSY 	GPIOF
#define PIN_BUSY  	GPIO_Pin_6
 
#define BUSY_IS_LOW()				(GPIO_ReadInputDataBit(PORT_BUSY, PIN_BUSY) == Bit_RESET)

//过采样端口配置
#define AD_OS0_PIN                     GPIO_Pin_13
#define AD_OS0_GPIO_PORT               GPIOG		
#define AD_OS0_GPIO_CLK                RCC_AHB1Periph_GPIOG

#define AD_OS1_PIN                     GPIO_Pin_14
#define AD_OS1_GPIO_PORT               GPIOG		
#define AD_OS1_GPIO_CLK                RCC_AHB1Periph_GPIOG

#define AD_OS2_PIN                     GPIO_Pin_15
#define AD_OS2_GPIO_PORT               GPIOG		
#define AD_OS2_GPIO_CLK                RCC_AHB1Periph_GPIOG

#define AD_OS0_0()							PGout(13)=0;
#define AD_OS0_1()							PGout(13)=1;

#define AD_OS1_0()							PGout(14)=0;
#define AD_OS1_1()							PGout(14)=1;

#define AD_OS2_0()							PGout(15)=0;
#define AD_OS2_1()							PGout(15)=1;


void InitAD7606(void);
void AD7606_Scan(void);
void AD7606_Mak(void);
extern int16_t AD7606_ReadAdc(uint8_t _ch);

extern float s_volt[9];

#endif



