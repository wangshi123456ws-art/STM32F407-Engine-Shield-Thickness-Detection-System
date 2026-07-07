#include "include.h"

// 检测参数的设置

int16_t select_Channel = 0;
uint32_t freq = 0; // 检测的激励频率，单位为Hz
uint16_t gain = 0; // 信号方法增益

// 检测前参数设置标志位
int8_t FreqIsSet = 0;    // 接收频率设置标志位
int8_t GainIsSet = 0;    // 接收增益设置标志位
int8_t ZeroCalIsSet = 0; // 接收校准设置标志位

// 存储幅值，相位的采集值和转换值
int16_t amplitude_Acquisition = 0;  // 幅值的采集值
int16_t phase_Acquisition = 0;      // 相位的采集值
int16_t amplitude_Conversion = 0;   // 幅值转换后的值
int16_t phase_Conversion = 0;       // 相位转换后的值

// 用于滤波的数据存储
const int16_t save_Count = 20; // 缓存数据的个数
int16_t save_AmplitudeArr[save_Count];
int16_t save_PhaseArr[save_Count];

// 对检测值进行校0
uint16_t cal_Num = 0;
int16_t calibrationIsSet = 0;     //校零
int16_t calibration_Amplitude[8] = {0};
int16_t calibration_Phase[8] = {0};


// 是否发送数据给上位机标志位
int8_t b_Snedto_PC = 0;
uint8_t b_DiapWay = 0;

int32_t delay_us1 = 25000;

// 存储8个通道的当前相位值
int16_t presrnt_Phase[8] = {0};


// 存储当前和上一个编码器位置值
int16_t distanceArr[2] = {0, 0};

// 编码器运行的圈数
int32_t turn = 0;

void Choice_Freq(uint8_t freq_Name);
void Set_Freq(uint32_t temp_Fre);
void Choice_Gain(uint8_t gain_Name);

/**********************************************************************************************************
函数名：Recv_Instruction
函数功能：接收上位机发送的对检测前参数设置的指令
输入参数：无
输出参数：无
其他说明：目前只设置了激励频率，信号放大增益
**********************************************************************************************************/
void Recv_Instruction(void)
{
	while (1)
	{
			
		if(USART1_RX_STA & 0x8000)
		{
				
			Choice_Freq(USART1_RX_BUF[0]);
			Choice_Gain(USART1_RX_BUF[1]);
					
			USART1_RX_STA = 0;
		}
		
		// 判断指令是否设置正确
		if (FreqIsSet == 1 && GainIsSet == 1)
		{
			USART1_RX_STA = 0;
			//printf(" bbb\r\n" );
			break;
		}
	}
}




/**********************************************************************************************************
函数名：Choice_Freq
函数功能：选择并设置激励频率
被本函数调用的函数清单：
调用本函数的函数清单：
输入参数：freq_Name-频率设置的编号，需要与上位机做好衔接
输出参数：
其他说明：
*********************************************************************************************************/
void Choice_Freq(uint8_t freq_Name)
{
	// 对接收的指令进行解码
	switch(freq_Name)
	{
		case 0x00:
		{
			freq = 5000;
			Set_Freq(freq);
			FreqIsSet = 1;
			break;
		}
		case 0x01:
		{
			freq = 10000;
			Set_Freq(freq);
			FreqIsSet = 1;
			break;
		}	
		case 0x02:
		{
			freq = 20000;
			Set_Freq(freq);
			FreqIsSet = 1;
			break;
		}	
		case 0x03:
		{
			freq = 40000;
			Set_Freq(freq);
			FreqIsSet = 1;
			break;
		}
		case 0x04:
		{
			freq = 60000;
			Set_Freq(freq);
			FreqIsSet = 1;
			break;
		}	
		default:
		{
			break;
		}
	}
}

void Set_Freq(uint32_t temp_Fre)
{
	Match_AD9959(temp_Fre);
}



/**********************************************************************************************************
函数名：Choice_Gain
函数功能：选择并设置检测信号的增益
被本函数调用的函数清单：
调用本函数的函数清单：Recv_Instruction，
输入参数：
输出参数：
其他说明：
*********************************************************************************************************/
void Choice_Gain(uint8_t gain_Name)
{
	// 对接收的指令进行解码
	switch (gain_Name)
	{
		case 0x10:
		{
			gain = 1;
			GainIsSet = 1;
			break;
		}
		case 0x11:
		{
			gain = 2;
			GainIsSet = 1;
			break;
		}
		case 0x12:
		{
			gain = 5;
			GainIsSet = 1;
			break;
		}
		case 0x13:
		{
			gain = 10;
			GainIsSet = 1;
		}
	}
}

/************************************************
函数名：
函数功能：将AD7607采样的值等效转换为相应的电压值
被本函数调用的函数清单：AD7606_ReadAdc，
调用本函数的函数清单：
输入参数：
输出参数：
其他说明：
************************************************/
void Signalto_Voltage()
{
	// ADC的第7和第8个通道分别采集幅值和相位信息
	amplitude_Acquisition = AD7606_ReadAdc(6);	
	phase_Acquisition = AD7606_ReadAdc(7);
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 0)
	{
		amplitude_Conversion = gain * amplitude_Acquisition * 5000 / 32767;
		phase_Conversion = gain * phase_Acquisition * 5000 / 32767;
	}
	else
	{
		amplitude_Conversion = gain * amplitude_Acquisition * 10000 / 32767;
		phase_Conversion = gain * phase_Acquisition * 10000 / 32767;
	}
		
}


/************************************************
函数名：Phase_Mean_Filtering
函数功能：对采集的相位进行滤波
被本函数调用的函数清单：
调用本函数的函数清单：
输入参数：
输出参数：
其他说明：
************************************************/
void Phase_Mean_Filtering()
{
	int16_t i;
	int32_t sum = 0; // 接收数据的累加
	
	for (i = 0; i < save_Count; i++)
	{
		// AD7606采集数据并保存
		AD7606_Scan();
	
		// 读取相应通道的数据并转换成实际电压值
		Signalto_Voltage();
		
		save_PhaseArr[i] = phase_Conversion; // 保存读取的相位电压
		
		sum += save_PhaseArr[i]; // 将数据进行累加
	}
	
	presrnt_Phase[select_Channel] = (uint16_t)sum / save_Count; // 获得均值电压
	
}

/************************************************
函数名：
函数功能：检测值进行校0
被本函数调用的函数清单：
调用本函数的函数清单：AD7606_ReadAdc，
输入参数：
输出参数：
其他说明：
************************************************/
void Calibration_Zero()
{
	// 采集多少个点进行校0
	int16_t cal_Count = 100;
	
	// cal_Count个点的值的总和
	int32_t cal_sum = 0;
	
	int i;
	
	for (i = 0; i < cal_Count; i++)
	{
		// 信号采集
		AD7606_Scan();

		// 读取相应通道的数据并转换成实际电压值
		Signalto_Voltage();
		
		cal_sum += phase_Conversion;
	}
	
	// 获得校0时需要减去的数
	calibration_Phase[select_Channel] = (int16_t)(cal_sum / cal_Count);
}


/************************************************
函数名：System_Reset()
函数功能：系统复位程序
被本函数调用的函数清单：
调用本函数的函数清单：AD7606_ReadAdc，
输入参数：
输出参数：
其他说明：
************************************************/
void System_Reset()
{
	/// 检测参数的设置
	select_Channel = 0; // 当前激励和采集的通道，8个通道(0-7)
	freq = 0; // 检测的激励频率，单位为Hz
	gain = 0; // 信号方法增益


	/// 检测前参数设置标志位
	FreqIsSet = 0; // 接收频率设置标志位
	GainIsSet = 0; // 接收增益设置标志位
	ZeroCalIsSet = 0; // 接收校准设置标志位

	// 存储幅值，相位的采集值和转换值
	amplitude_Acquisition = 0; // 幅值的采集值
	phase_Acquisition = 0; // 相位的采集值
	amplitude_Conversion = 0; // 幅值转换后的值
	phase_Conversion = 0; // 相位转换后的值
	
	// 编码器运行的圈数
	turn = 0;
	
	/// 存储当前和上一个编码器位置值
	distanceArr[0] = 0;
	distanceArr[1] = 0;
	
	/// 对检测值进行校0
	cal_Num = 0;
	calibrationIsSet = 0;	
	
	b_Snedto_PC = 0;
	
	b_DiapWay = 0;
}

/************************************************
函数名：Sendto_Usart_1
函数功能：向串口1发送数据
被本函数调用的函数清单：USART_SendData，
调用本函数的函数清单：
输入参数：t-占1个字节的16进制数
输出参数：无
其他说明：
************************************************/
void Sendto_Usart_1(unsigned char t)
{
	USART_SendData(USART1, t);         //向串口1发送数据
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET); // 等待发送结束
}


/************************************************
函数名：Send_FullDatato_Pc()
函数功能：发送一组完整的数据给上位机
被本函数调用的函数清单：
调用本函数的函数清单：
输入参数：
输出参数：
其他说明：
************************************************/
void Send_FullDatato_Pc()
{	
	// 发送帧头
	Sendto_Usart_1(0xAA);
	Sendto_Usart_1(0xBB);	
	

	
	Sendto_Usart_1(presrnt_Phase[0] >> 8);
	Sendto_Usart_1(presrnt_Phase[0]);
	
	Sendto_Usart_1(presrnt_Phase[6] >> 8);
	Sendto_Usart_1(presrnt_Phase[6]);
	
	Sendto_Usart_1(presrnt_Phase[4] >> 8);
	Sendto_Usart_1(presrnt_Phase[4]);
	
	Sendto_Usart_1(presrnt_Phase[2] >> 8);
	Sendto_Usart_1(presrnt_Phase[2]);
	
	Sendto_Usart_1(presrnt_Phase[7] >> 8);
	Sendto_Usart_1(presrnt_Phase[7]);
	
	Sendto_Usart_1(presrnt_Phase[5] >> 8);
	Sendto_Usart_1(presrnt_Phase[5]);
	
	Sendto_Usart_1(presrnt_Phase[3] >> 8);
	Sendto_Usart_1(presrnt_Phase[3]);
	
	Sendto_Usart_1(presrnt_Phase[1] >> 8);
	Sendto_Usart_1(presrnt_Phase[1]);
	
	// 发送编码器位置值
	Sendto_Usart_1(distanceArr[1] >> 8);
	Sendto_Usart_1(distanceArr[1]);
	
	// 发送帧尾

	Sendto_Usart_1(0xAA);
	Sendto_Usart_1(0xBB);	
}


/************************************************
函数名：
函数功能：正式进入检测死循环
被本函数调用的函数清单：
调用本函数的函数清单：AD7606_ReadAdc，
输入参数：
输出参数：
其他说明：
************************************************/
void Detection_Cycle()
{
	while (1)
	{
		// 对相应通道的引脚进行配置
		
		
		//Select_Channel();
		
		// 强制延迟，太快了
		while (delay_us1 > 0)
		{
			delay_us1--;
		}
		
		delay_us1 = 25000;
		
		// 判断是否下达指令
		
		if(USART1_RX_STA&0x8000)
		{
			// 判断是否下发校零指令
			if (USART1_RX_BUF[2] == 0x20)
			{
				// 执行校0函数
				calibrationIsSet = 1;
				
				USART1_RX_STA = 0;
			}
			
			// 判断是否下发复位指令
			if (USART1_RX_BUF[3] == 0x30)
			{
				// 运行复位程序
				System_Reset();
				
				USART1_RX_STA = 0;
				// 跳出检测循环
				break;
			}
			
			// 判断是否下发发送数据指令
			if (USART1_RX_BUF[4] == 0x40)
			{
				b_Snedto_PC = 1;
			}
			else if (USART1_RX_BUF[4] == 0x41)
			{
				b_Snedto_PC = 0;
			}
			
			// 判断是否下发显示方式指令
			if (USART1_RX_BUF[5] == 0x50)
			{
				b_DiapWay = 0;
			}
			else if (USART1_RX_BUF[5] == 0x51)
			{
				b_DiapWay = 1;
			}
			
			USART1_RX_STA = 0;
		}
		
		/// 对信号进行校0
		if (calibrationIsSet & 1)
		{
			Calibration_Zero();
			
			// 校0次数++
			cal_Num++;
			
			// 判断是否每个通道都进行校0
			if (cal_Num == 8)
			{
				// 对8个通道都校0完成
				calibrationIsSet = 0;
				// 校0数量清0，以方便下次校0
				cal_Num = 0;
			}			
		}
			
		// 对相位信号进行均值滤波
		Phase_Mean_Filtering();
		
		presrnt_Phase[select_Channel] -= calibration_Phase[select_Channel];
		
		if (presrnt_Phase[select_Channel] < 0)
		{
			presrnt_Phase[select_Channel] = -presrnt_Phase[select_Channel];
		}	
		
//		// 如果信号浮动小于3，直接强制为0
//		if ( presrnt_Phase[select_Channel] <= 3)
//		{
//			presrnt_Phase[select_Channel] = 0;
//		}
		
				
		// 切换检测的通道
		select_Channel++;
		
		// 一次完整的切换通道完成
		if (select_Channel == 8)
		{
			// 获取当前编码器位置值
			
			
			
			
			
			//AD7606_DispDistance();




			
			
			// 判断检测按钮是否打开
			if (b_Snedto_PC == 1)
			{
				// 判断显示方式
				if (b_DiapWay == 0)
				{
					// 将一次完整的数据发送给上位机
					Send_FullDatato_Pc();
				}
				else 
				{
					if (distanceArr[1] != distanceArr[0])
					{
						// 将一次完整的数据发送给上位机
						Send_FullDatato_Pc();
					}
				}
			}
			
			// 把当前的位置值覆盖上一次的值
			distanceArr[0] = distanceArr[1];
			
			// 检测通道重新从0开始
			select_Channel = 0;
		}
		
	}
}
