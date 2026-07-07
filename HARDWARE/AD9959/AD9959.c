#include "include.h"




u8 FR1_DATA[3] = {0xD0,0x00,0x00};   //20倍频 Charge pump control = 75uA FR1<23> -- VCO gain control =0时 system clock below 160 MHz;
u8 FR2_DATA[2] = {0x00,0x00};       //default Value = 0x0000

															
//AD9959初始化
void Init_AD9959(void)  
{ 
	
	GPIO_InitTypeDef  GPIO_InitStructure;       	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC, ENABLE);	 //PA,PB,PC端口时钟使能
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_5|GPIO_Pin_2|GPIO_Pin_7|GPIO_Pin_6|GPIO_Pin_8;//初始化管脚PA2.3.5.6.7.8
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 		 //推挽输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO口速度为2MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_10|GPIO_Pin_11;//初始化管脚PB0.1.10.11                                             			
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		 //推挽输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO口速度为2MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_2|GPIO_Pin_3;	//初始化管脚PC0.2.3		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		 //推挽输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//输出模式：推挽	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO口速度为2MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化GPIOC
		
	Intserve();  //IO口初始化
    IntReset();  //AD9959复位  
	
	//初始化功能寄存器
    AD9959_WriteData(FR1_ADD,3,FR1_DATA);//写功能寄存器1
    AD9959_WriteData(FR2_ADD,2,FR2_DATA);//
  

	
} 


void Match_AD9959(uint32_t Fre)  
  {
   
  AD9959_Set_Fre(CH0, Fre);	//设置通道0频率
	AD9959_Set_Fre(CH1, Fre);	//设置通道1频率
	AD9959_Set_Fre(CH2, Fre);	//设置通道2频率
	AD9959_Set_Fre(CH3, Fre);	//设置通道3频率
		
	AD9959_Set_Amp(CH0, 1023); 	//设置通道0幅度控制值1023，范围0~1023
	AD9959_Set_Amp(CH1, 1023); 	//设置通道1幅度控制值1023，范围0~1023
	AD9959_Set_Amp(CH2, 1023); 	//设置通道2幅度控制值1023，范围0~1023
	AD9959_Set_Amp(CH3, 1023); 	//设置通道3幅度控制值1023，范围0~1023

	AD9959_Set_Phase(CH0, 0);	//设置通道0相位控制值0(0度)，范围0~16383
	AD9959_Set_Phase(CH1, 0);	//设置通道1相位控制值4096(90度)，范围0~16383
	AD9959_Set_Phase(CH2, 0);	//设置通道2相位控制值8192(180度)，范围0~16383
	AD9959_Set_Phase(CH3, 0);	//设置通道3相位控制值12288(270度)，范围0~16383
	
	IO_Update();            	//AD9959更新数据,调用此函数后，上述操作生效！！！！
		
	}


//延时
void delay1 (u32 length)
{
	
	length = length*12;
   while(length--);
}



//IO口初始化
void Intserve(void)		   
{   
	AD9959_PWR=0;
    CS = 1;
    SCLK = 0;
    UPDATE = 0;
    PS0 = 0;
    PS1 = 0;
    PS2 = 0;
    PS3 = 0;
    SDIO0 = 0;
    SDIO1 = 0;
    SDIO2 = 0;
    SDIO3 = 0;
}



//AD9959复位
void IntReset(void)	  
{
    Reset = 0;
	delay1(1);
	Reset = 1;
	delay1(30);
	Reset = 0;
}



//AD9959更新数据
void IO_Update(void)  
{
	UPDATE = 0;
	delay1(2);
	UPDATE = 1;
	delay1(4);
	UPDATE = 0;
}



/*************************************************************************************************
** 函数名称 ：void AD9959_WriteData(u8 RegisterAddress, u8 NumberofRegisters, u8 *RegisterData)
** 函数功能 ：使用模拟SPI向AD9959写数据
** 入口参数 ：RegisterAddress: 寄存器地址
						NumberofRegisters: 要写入的字节数
						*RegisterData: 数据起始地址
** 出口参数 ：无
** 函数说明 ：无
***************************************************************************************************/
void AD9959_WriteData(uint8_t RegisterAddress, uint8_t NumberofRegisters, uint8_t *RegisterData)
{
	uint8_t	ControlValue = 0;
	uint8_t	ValueToWrite = 0;
	uint8_t	RegisterIndex = 0;
	uint8_t	i = 0;

	ControlValue = RegisterAddress;
    //写入地址
	SCLK = 0;
	CS = 0;	 
	for(i=0; i<8; i++)
	{
		SCLK = 0;
		if(0x80 == (ControlValue & 0x80))
		SDIO0= 1;	  
		else
		SDIO0= 0;	  
		SCLK = 1;
		ControlValue <<= 1;
	}
	SCLK = 0;
   //写入数据
	for (RegisterIndex=0; RegisterIndex<NumberofRegisters; RegisterIndex++)
	{
		ValueToWrite = RegisterData[RegisterIndex];
		for (i=0; i<8; i++)
		{
			SCLK = 0;
			if(0x80 == (ValueToWrite & 0x80))
			SDIO0= 1;	  
			else
			SDIO0= 0;	  
			SCLK = 1;
			ValueToWrite <<= 1;
		}
		SCLK = 0;		
	}	
  CS = 1;
} 



/*************************************************************************************************
** 函数名称 ：void Write_CFTW0(uint32_t fre)
** 函数功能 ：写CFTW0通道频率转换字寄存器
** 入口参数 ： Freq:	写入频率，范围0~200 000 000 Hz
** 出口参数 ：无
** 函数说明 ：无
***************************************************************************************************/
void Write_CFTW0(uint32_t fre)
{
	  uint8_t CFTW0_DATA[4] ={0x00,0x00,0x00,0x00};	//中间变量
	  uint32_t Temp;            
	  Temp=(uint32_t)fre*ACC_FRE_FACTOR;	 
	  CFTW0_DATA[3]=(uint8_t)Temp;
	  CFTW0_DATA[2]=(uint8_t)(Temp>>8);
	  CFTW0_DATA[1]=(uint8_t)(Temp>>16);
	  CFTW0_DATA[0]=(uint8_t)(Temp>>24);		
	  AD9959_WriteData(CFTW0_ADD,4,CFTW0_DATA);//CTW0 address 0x04
}



/*************************************************************************************************
** 函数名称 ：void Write_ACR(uint16_t Ampli)
** 函数功能 ：写ACR通道幅度转换字寄存器
** 入口参数 ：Ampli:    输出幅度,范围0~1023，控制值0~1023对应输出幅度0~500mVpp左右
** 出口参数 ：无
** 函数说明 ：无
***************************************************************************************************/
void Write_ACR(uint16_t Ampli)
{ 
	uint32_t A_temp=0;
	uint8_t ACR_DATA[3] = {0x00,0x00,0x00};//default Value = 0x--0000 Rest = 18.91/Iout 
    A_temp=Ampli|0x1000;
	
	ACR_DATA[1] = (uint8_t)(A_temp>>8); //高位数据
	ACR_DATA[2] = (uint8_t)A_temp;  //低位数据
    AD9959_WriteData(ACR_ADD,3,ACR_DATA); //ACR address 0x06.CHn设定幅度
}



/*************************************************************************************************
** 函数名称 ：void Write_CPOW0(uint16_t Phase)
** 函数功能 ：写CPOW0通道相位转换字寄存器
** 入口参数 ：Phase:		输出相位,范围：0~16383(对应角度：0°~360°)
** 出口参数 ：无
** 函数说明 ：无
***************************************************************************************************/
void Write_CPOW0(uint16_t Phase)
{
	uint8_t CPOW0_data[2] = {0x00,0x00};
	CPOW0_data[1]=(uint8_t)Phase;
	CPOW0_data[0]=(uint8_t)(Phase>>8);

	AD9959_WriteData(CPOW0_ADD,2,CPOW0_data);//CPOW0 address 0x05.CHn设定相位
}



/*************************************************************************************************
** 函数名称 ：void AD9959_Set_Fre(uint8_t Channel,uint32_t Freq)
** 函数功能 ：设置通道的输出频率
** 入口参数 ：Channel:  输出通道  CH0-CH3
						 Freq:     输出频率，范围0~200 000 000 Hz
** 出口参数 ：无
** 函数说明 ：无
***************************************************************************************************/
void AD9959_Set_Fre(uint8_t Channel,uint32_t Freq)
{	 
	uint8_t CHANNEL[1] = {0x00};
		
	CHANNEL[0]=Channel;
	AD9959_WriteData(CSR_ADD,1,CHANNEL);//控制寄存器写入CHn通道，选择CHn
    Write_CFTW0(Freq);//输出CHn设定频率																																			 
} 



/*************************************************************************************************
** 函数名称 ：void AD9959_Set_Amp(uint8_t Channel, uint16_t Ampli)
** 函数功能 ：设置通道的输出幅度
** 入口参数 ：Channel:  输出通道 CH0-CH3
							Ampli:    输出幅度,范围0~1023，控制值0~1023对应输出幅度0~500mVpp左右
** 出口参数 ：无
** 函数说明 ：无
***************************************************************************************************/
void AD9959_Set_Amp(uint8_t Channel, uint16_t Ampli)
{ 
	uint8_t CHANNEL[1] = {0x00};

	CHANNEL[0]=Channel;
	AD9959_WriteData(CSR_ADD,1,CHANNEL); //控制寄存器写入CHn通道，选择CHn
	Write_ACR(Ampli);							//	CHn设定幅度
}



/*************************************************************************************************
** 函数名称 ：void AD9959_Set_Phase(uint8_t Channel,uint16_t Phase)
** 函数功能 ：设置通道的输出相位
** 入口参数 ：Channel:  输出通道 CH0-CH3
							Phase:		输出相位,范围：0~16383(对应角度：0°~360°)
** 出口参数 ：无
** 函数说明 ：无
***************************************************************************************************/
void AD9959_Set_Phase(uint8_t Channel,uint16_t Phase)
{
	uint8_t CHANNEL[1] = {0x00};
	CHANNEL[0]=Channel;

	AD9959_WriteData(CSR_ADD,1,CHANNEL); //控制寄存器写入CHn通道，选择CHn
	Write_CPOW0(Phase);//CHn设定相位
}





