#include "include.h"

void AD7606_ConfigGPIO(void);
void AD7606_Reset(void);	
void AD7606_SetInputRange(uint8_t _ucRange);
void AD7606_SetOS(uint8_t _ucMode);
	

//static int16_t s_adc_now[8];

//static int16_t s_dat[8];

int16_t s_adc_now[9];
float s_volt[9];
int16_t s_dat[9];



//AD7606初始化
void InitAD7606(void)
{
	AD7606_ConfigGPIO();		    // 配置GPIO ,采用的软件SPI
	AD7606_SetInputRange(1);	    // 设置采样量程 
	AD7606_SetOS(5);                // 设置过采样模式 
	AD7606_Reset();				    // 硬件复位复AD7606 
	AD_CONVST_HIGH();			    //CONVST脚设置为高电平	
}



//ADC7606各个端口初始化
void AD7606_ConfigGPIO(void)
{
		
	GPIO_InitTypeDef  GPIO_InitStructure;

	// 打开GPIO时钟 	
	RCC_AHB1PeriphClockCmd(RCC_SCK | RCC_MOSI | RCC_MISO, ENABLE);
  //初始化SPI
	// 配置几个推完输出IO 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	  	// 设为输出口 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		// 设为推挽模式 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	// 上下拉电阻不使能 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;	// IO口最大速度   25

	GPIO_InitStructure.GPIO_Pin = PIN_SCK;
	GPIO_Init(PORT_SCK, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PIN_MOSI;
	GPIO_Init(PORT_MOSI, &GPIO_InitStructure);

	//配置GPIO为浮动输入模式(实际上CPU复位后就是输入状态) 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		   // 设为输入口 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		// 设为推挽模式 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	// 无需上下拉电阻 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;	// IO口最大速度 

	GPIO_InitStructure.GPIO_Pin = PIN_MISO;
	GPIO_Init(PORT_MISO, &GPIO_InitStructure);
	
	

	//打开GPIO时钟 
	RCC_AHB1PeriphClockCmd(RCC_CS | RCC_RANGE | RCC_BUSY | RCC_RESET | RCC_CONVST | RCC_AHB1Periph_GPIOG, ENABLE);
	
  //初始化其余端口
	// 配置几个推完输出IO 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		       // 设为输出口 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		     // 设为推挽模式 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	     // 上下拉电阻不使能 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	   // IO口最大速度 

	GPIO_InitStructure.GPIO_Pin = PIN_RESET;
	GPIO_Init(PORT_RESET, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PIN_CONVST;
	GPIO_Init(PORT_CONVST, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = PIN_RANGE;
	GPIO_Init(PORT_RANGE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = PIN_CS;
	GPIO_Init(PORT_CS, &GPIO_InitStructure);

	// 配置GPIO为浮动输入模式(实际上CPU复位后就是输入状态) 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		        // 设为输入口 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		      // 设为推挽模式 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	      // 无需上下拉电阻 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	      // IO口最大速度 

	GPIO_InitStructure.GPIO_Pin = PIN_BUSY;
	GPIO_Init(PORT_BUSY, &GPIO_InitStructure);


    //配置OS0-2 GPIO 
	GPIO_InitStructure.GPIO_Pin = AD_OS0_PIN;            //PG13
	GPIO_Init(AD_OS0_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = AD_OS1_PIN;            //PG14
	GPIO_Init(AD_OS1_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = AD_OS2_PIN;
	GPIO_Init(AD_OS2_GPIO_PORT, &GPIO_InitStructure);    //PG15

}


/**********************************************************************************************************
*	函 数 名: AD7606_SetOS
*	功能说明: 设置过采样模式（数字滤波，硬件求平均值)
*	形    参：_ucMode : 0-6  0表示无过采样，1表示2倍，2表示4倍，3表示8倍，4表示16倍
*				                   5表示32倍，6表示64倍
*	返 回 值: 无
**********************************************************************************************************/

void AD7606_SetOS(uint8_t _ucMode)
{
	if (_ucMode == 1)
	{
		AD_OS2_0();
		AD_OS1_0();
		AD_OS0_1();
	}
	else if (_ucMode == 2)
	{
		AD_OS2_0();
		AD_OS1_1();
		AD_OS0_0();
	}
	else if (_ucMode == 3)
	{
		AD_OS2_0();
		AD_OS1_1();
		AD_OS0_1();
	}
	else if (_ucMode == 4)
	{
		AD_OS2_1();
		AD_OS1_0();
		AD_OS0_0();
	}
	else if (_ucMode == 5)
	{
		AD_OS2_1();
		AD_OS1_0();
		AD_OS0_1();
	}
	else if (_ucMode == 6)
	{
		AD_OS2_1();
		AD_OS1_1();
		AD_OS0_0();
	}
	else	// 按0处理 
	{
		AD_OS2_0();
		AD_OS1_0();
		AD_OS0_0();
	}
}





/**********************************************************************************************************
*	函 数 名: AD7606_SetInputRange
*	功能说明: 配置AD7606模拟信号输入量程。
*	形    参: _ucRange : 0 表示正负5V   1表示正负10V
*	返 回 值: 无
**********************************************************************************************************/

void AD7606_SetInputRange(uint8_t _ucRange)
{
	if (_ucRange == 0)
	{
		AD_RANGE_5V();	// 设置为正负5V 
	}
	else
	{
		AD_RANGE_10V();	// 设置为正负10V 
	}
}



/**********************************************************************************************************
*	函 数 名: AD7606_RESET
*	功能说明: 硬件复位
*	形    参：无
*	返 回 值: 无
**********************************************************************************************************/

void AD7606_Reset(void)
{
 	// AD7606是高电平复位，要求最小脉宽50ns
	AD_RESET_LOW();
	
	AD_RESET_HIGH();
	AD_RESET_HIGH();
	AD_RESET_HIGH();
	AD_RESET_HIGH();
	
	AD_RESET_LOW();
}


/**********************************************************************************************************
*	函 数 名: bsp_SpiDelay
*	功能说明: 时序延迟
*	形    参: 无
*	返 回 值: 无
**********************************************************************************************************/

void spiDelay(void)
{
#if 1
	uint32_t i;

	
		//延迟5时， F407 (168MHz主频） GPIO模拟，实测 SCK 周期 = 480ns (大约2M)
	for (i = 0; i < 5; i++);
#else
	 //不添加延迟语句， F407 (168MHz主频） GPIO模拟，实测 SCK 周期 = 200ns (大约5M)
#endif
}



/**********************************************************************************************************
*	函 数 名: ad7606_ReadBytes
*	功能说明: 读取AD7606的采样结果
*	形    参：
*	返 回 值: 无
**********************************************************************************************************/

uint8_t spiRead1(void)
{	
	uint8_t i;
	uint8_t read = 0;

	for (i = 0; i < 8; i++)
	{
		SCK_0();
		spiDelay();
		read = read << 1;
		if (MISO_IS_HIGH())
		{
			read++;
		}
		SCK_1();
		spiDelay();
	}
	return read;
}




/**********************************************************************************************************
*	函 数 名: ad7606_StartConv
*	功能说明: 启动AD7606的ADC转换
*	形    参：无
*	返 回 值: 无
**********************************************************************************************************/

void AD7606_StartConv(void)
{
	//上升沿开始转换，低电平持续时间至少25ns  
	AD_CONVST_LOW();
	AD_CONVST_LOW();
	AD_CONVST_LOW();	// 连续执行2次，低电平约50ns 
	
	AD_CONVST_HIGH();
}



/**********************************************************************************************************
*	函 数 名: AD7606_scan
*	功能说明: 扫描调用本函数，用于读取AD转换器数据
*	形    参：无
*	返 回 值: 无
**********************************************************************************************************/

void AD7606_Scan(void) 		                                   // 此函数代码按照时序编写 
{		
	uint8_t i = 0;
	
	AD7606_StartConv();	                                       // 给开始信号	
	while(1)
	{
		if (BUSY_IS_LOW())	   
		 {
			AD_CS_0();                                         // SPI片选 = 0 
			s_adc_now[i] = spiRead1();					
			s_adc_now[i] = s_adc_now[i] * 256 + spiRead1();    // 读数据 	
			AD_CS_1();                                         // SPI片选 = 1 
			i++;				
		 }
		 
		if(i>=CH_NUM)
		 {		
			break;
		 }

	}	
		
}	  



/**********************************************************************************************************
*	函 数 名: GetAdcFormFifo
*	功能说明: 从FIFO中读取一个ADC值
*	形    参：_usReadAdc : 存放ADC结果的变量指针
*	返 回 值: 1 表示OK，0表示暂无数据
**********************************************************************************************************/

int16_t AD7606_ReadAdc(uint8_t _ch)
{
	int16_t sAdc;	
	
	sAdc = s_adc_now[_ch];
	
	return sAdc;
}



/**********************************************************************************************************
*	函 数 名: AD7606_Mak
*	功能说明: 处理采样后的数据
*	形    参：无
*	返 回 值: 无
**********************************************************************************************************/

void AD7606_Mak(void)
{
	uint8_t i;
	double adc;

	for (i = 6;i < CH_NUM; i++)
	{	
		s_dat[i] = AD7606_ReadAdc(i);
		
		adc = s_dat[i];
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 0)
		{
			s_volt[i] = (float)((adc * 5000.0) / 32767.0);
		}
		else
		{
			s_volt[i] =(float)((adc * 10000.0) / 32767.0);
		}

	}
}


