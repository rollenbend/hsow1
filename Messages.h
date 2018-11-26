//============================================================================
// Name        : Messages.h
// Author      : ilya
// Version     : 08.05.2018
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#ifndef __Messages_Dpp_Acc_Gyro_
#define __Messages_Dpp_Acc_Gyro_
#include "stm32f30x.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_tim.h"
#include "stm32f30x_gpio.h"
#include "stm32f30x_dma.h"
#include "Initialization.h"
void Timers_Init(void);
extern volatile unsigned int McSecTime;
extern char FLAAG;

class Channel
{
public:
#pragma pack(push,1)

	struct Dpp_Classic
	{
		unsigned char Syncro=0x53;
		unsigned char Size=0x04;
		unsigned int Dpp=0;
		unsigned char Cs=0;
	};

	struct Dpp_Main
	{
		Dpp_Classic dpp_classic;
		short Speed=0;
		unsigned int Timer=0;
		unsigned char Cs=0;
	};
	struct Acc_Gyro
		{
			short Acc[3];
			short Gyro[3];
			unsigned int Timer;
		};
		struct Acc_Message
		{
			unsigned char Syncro=0x7e;
			unsigned char Size=0x11;
			unsigned char Status=0;
			unsigned char Reply=0;
			Acc_Gyro acc_gyro;
			unsigned char Cs=0;
		};
	struct Packet
	{
		Dpp_Main dpp_main;
		Acc_Message acc_message;
	};
#pragma pack(pop)
	int SmoothSpeedCount=0;
	unsigned int McSecTimePrevious=0;
	Packet packet;
	void Incr_Dpp_Classic(void)
		{
		packet.dpp_main.dpp_classic.Cs=0;
		packet.dpp_main.dpp_classic.Dpp+=5;
			for (char i=0; i<sizeof(packet.dpp_main.dpp_classic)-1;i++)
				packet.dpp_main.dpp_classic.Cs+=*(&packet.dpp_main.dpp_classic.Syncro+char(i));
		}
		void Get_Speed(void)
		{
			int PassedTime=int(McSecTime-McSecTimePrevious);
			if (PassedTime>200000) {
				if (TIM1->CR1 & (1<<4))	packet.dpp_main.Speed=(short)(int(10440000*SmoothSpeedCount/PassedTime)); // Direction Bit
				else packet.dpp_main.Speed=(short)(int(-10440000*SmoothSpeedCount/PassedTime));
				McSecTimePrevious=McSecTime;
				SmoothSpeedCount=0;
			}
		}
		void Incr_Dpp_Main(void)
		{
			SmoothSpeedCount++;
			packet.dpp_main.Cs=0;
			packet.dpp_main.Timer=McSecTime;
			for (char i=0; i<sizeof(packet.dpp_main)-sizeof(packet.dpp_main.dpp_classic);i++)
				packet.dpp_main.Cs+=*(&packet.dpp_main.dpp_classic.Cs+char(i));
		}
		void Incr_DPP(void)
		{
			Incr_Dpp_Classic();
			Get_Speed();
			Incr_Dpp_Main();
		}
	void Dpp_Acc_Transmit_DMA()
		{
		packet.acc_message.Cs=0;
		packet.acc_message.acc_gyro.Timer= McSecTime;
			for (unsigned char i=0; i<sizeof(packet.acc_message)-1;i++) packet.acc_message.Cs+=*(&packet.acc_message.Syncro+char(i));
			DMA_Cmd(DMA1_Channel4, DISABLE);
			DMA1_Channel4->CNDTR=sizeof(packet);
			DMA_Cmd(DMA1_Channel4, ENABLE);
		};
	void Get_Acc_Gyro_Data(float* AccBuffer, short* GyroBuffer)
		{
			for (int i=0; i<3; i++) {
				packet.acc_message.acc_gyro.Acc[i]=(short)AccBuffer[i];
				packet.acc_message.acc_gyro.Gyro[i]=(short)GyroBuffer[i];
			}
		}
	void DMA_Packet_Init()
		{
			DMA_InitTypeDef dma;
			DMA_DeInit(DMA1_Channel4);
				DMA_StructInit(&dma);
				dma.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->TDR);
				dma.DMA_MemoryBaseAddr = (uint32_t)&packet;
				dma.DMA_DIR = DMA_DIR_PeripheralDST;
				dma.DMA_BufferSize = sizeof(packet);
				dma.DMA_Priority = DMA_Priority_High;
				dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
				NVIC_SetPriority(DMA1_Channel4_IRQn,0);
				NVIC_EnableIRQ(DMA1_Channel4_IRQn);
				DMA_Init(DMA1_Channel4, &dma);
		};
};









class Message_Dpp
{
private:
#pragma pack(push,1)
	struct Dpp_Classic
	{
		unsigned char Syncro;
		unsigned char Size;
		unsigned int Dpp;
		unsigned char Cs;
	};

	struct Dpp_Main
	{
		Dpp_Classic dpp_classic;
		short Speed;
		unsigned int Timer;
		unsigned char Cs;
	};
#pragma pack(pop)


	int SmoothSpeedCount;
	unsigned int McSecTimePrevious;

public:
	Dpp_Main dpp_main;
//	char dppmass[14];
//	bool DPP_FLAG=0;
	void Dpp_Classic_Init(Dpp_Classic & ref_dpp_classic)
	{
		ref_dpp_classic.Syncro=0x53;
		ref_dpp_classic.Size=sizeof(ref_dpp_classic.Dpp);
		ref_dpp_classic.Dpp=0;
		for (char i=0; i<sizeof(ref_dpp_classic)-1;i++) ref_dpp_classic.Cs+=*(&ref_dpp_classic.Syncro+char(i));
	}
	void Dpp_Main_Init(void)
	{
		Dpp_Classic_Init(dpp_main.dpp_classic);
		dpp_main.Speed=0;
		dpp_main.Timer=0;
		for (unsigned char i=0; i<sizeof(dpp_main)-sizeof(dpp_main.dpp_classic);i++)
			dpp_main.Cs+=*(&dpp_main.dpp_classic.Cs+char(i));
	}
	void DMA_Transmit_Dpp(void)
	{
		//FOR 8-WIRE CABLE
//		for (char i=0; i<sizeof(dpp_main);i++)
//			  	dppmass[i] = (unsigned char)*(&dpp_main.dpp_classic.Syncro+char(i));
//		DPP_FLAG=1;
		for (int i=0;i<200;)i++;
		DMA_Cmd(DMA1_Channel4, DISABLE);
			while (DMA_GetCurrDataCounter(DMA1_Channel4) != 0) {
				DMA_Cmd(DMA1_Channel4, ENABLE);
				for (int i=0;i<10;)i++;
				DMA_Cmd(DMA1_Channel4, DISABLE);
			}
		DMA1_Channel4->CMAR = (uint32_t)&dpp_main;
		DMA1_Channel4->CNDTR=sizeof(dpp_main);
		DMA_Cmd(DMA1_Channel4, ENABLE);
		Toggle_Led(ORANGE);
		for (int i=0;i<200;)i++;
		Toggle_Led(ORANGE);


//		DMA_Cmd(DMA1_Channel7, DISABLE);
//		DMA1_Channel7->CNDTR=sizeof(dpp_main);
//		DMA_Cmd(DMA1_Channel7, ENABLE);
	}
	void Incr_Dpp_Classic(void)
	{
		dpp_main.dpp_classic.Cs=0;
		dpp_main.dpp_classic.Dpp+=5;
		for (char i=0; i<sizeof(dpp_main.dpp_classic)-1;i++)
			dpp_main.dpp_classic.Cs+=*(&dpp_main.dpp_classic.Syncro+char(i));
	}
	void Get_Speed(void)
	{
		int PassedTime=int(McSecTime-McSecTimePrevious);
		if (PassedTime>200000) {
			if (TIM1->CR1 & (1<<4))	dpp_main.Speed=(short)(int(10440000*SmoothSpeedCount/PassedTime)); // Direction Bit
			else dpp_main.Speed=(short)(int(-10440000*SmoothSpeedCount/PassedTime));
			McSecTimePrevious=McSecTime;
			SmoothSpeedCount=0;
		}
	}
	void Incr_Dpp_Main(void)
	{
		SmoothSpeedCount++;
		dpp_main.Cs=0;
		dpp_main.Timer=McSecTime;
		for (char i=0; i<sizeof(dpp_main)-sizeof(dpp_main.dpp_classic);i++)
					dpp_main.Cs+=*(&dpp_main.dpp_classic.Cs+char(i));
	}
	void Incr_Main_Dpp_and_Transmit_DMA()
	{
//		if ((CorrectCount++)%5000)
//		{
			Incr_Dpp_Classic();
			Get_Speed();
			Incr_Dpp_Main();
			DMA_Transmit_Dpp();
//		}
	};

	Message_Dpp()
	{
		SmoothSpeedCount=0;
		McSecTimePrevious=0;
		Dpp_Main_Init();
	};


	void DMA_Dpp_Init(void)
	{
		DMA_InitTypeDef dma;

		 DMA_StructInit(&dma);
			dma.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->TDR);
			dma.DMA_MemoryBaseAddr = (uint32_t)&dpp_main;
			dma.DMA_DIR = DMA_DIR_PeripheralDST;
			dma.DMA_BufferSize = sizeof(dpp_main);
			dma.DMA_Priority = DMA_Priority_High;
			dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
			DMA_Init(DMA1_Channel7, &dma);
		  DMA_Cmd(DMA1_Channel7, ENABLE);
	};
	void Reset_Dpp(void)
	{
		dpp_main.dpp_classic.Dpp=0;
		dpp_main.Timer=0;
		TIM_SetCounter(TIM2, 0);
	}
	void Tim1_Encoder_Init(void);
	void Tim2_For_Marking_Messages_Init(void);
	void Tim3_Imitation_Init(unsigned short Period);
	void Correction(void)
	{
		//while (DMA1->cndtr) {;}
		//Incr_Main_Dpp();
	}
	~Message_Dpp()
	{

	};

};



class Message_Acc
{
private:
#pragma pack(push,1)
	struct Acc_Gyro
	{
		short Acc[3];
		short Gyro[3];
		unsigned int Timer;
	};
	struct Acc_Message
	{
		unsigned char Syncro;
		unsigned char Size;
		unsigned char Status;
		unsigned char Reply;
		Acc_Gyro acc_gyro;
		unsigned char Cs;
	};
#pragma pack(pop)

public:

Acc_Message acc_message;
	void Acc_Message_Init(void)
	{
		acc_message.Syncro=0x7e;
		acc_message.Size=sizeof(acc_message.acc_gyro)+1;
		acc_message.Status=acc_message.Reply=0;
		for (char i=0; i<sizeof(acc_message)-1;i++) acc_message.Cs+=*(&acc_message.Syncro+char(i));
	}
	Message_Acc()
	{
		Acc_Message_Init();
	}
	void Get_Acc_Gyro_Data(float* AccBuffer, short* GyroBuffer)
	{
		for (int i=0; i<3; i++) {
			acc_message.acc_gyro.Acc[i]=(short)AccBuffer[i];
			acc_message.acc_gyro.Gyro[i]=(short)GyroBuffer[i];
		}
	}
	void DMA_Transmit_Acc(void)
	{
//		while (DMA1->ISR&(1<<13))
		//	{ ;} // wait until previous transaction done
//		DMA_ClearITPendingBit(DMA1_IT_TC4);

		//FOR 8-WIRE CABLE
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)  {}// Loop until transmit data register is empty
		for (int i=0;i<200;)i++;
		DMA_Cmd(DMA1_Channel4, DISABLE);
		while (DMA_GetCurrDataCounter(DMA1_Channel4) != 0) {
			DMA_Cmd(DMA1_Channel4, ENABLE);
			for (int i=0;i<10;)i++;
			DMA_Cmd(DMA1_Channel4, DISABLE);
		}
	//	DMA1_Channel4->CMAR = (uint32_t)&acc_message;
		DMA1_Channel4->CNDTR=sizeof(acc_message);
		DMA_Cmd(DMA1_Channel4, ENABLE);
		for (int i=0;i<200;)i++;
	}
	void Make_Acc_Gyro_Message_and_Transmit_DMA(void)
	{
		acc_message.Cs=0;
		acc_message.acc_gyro.Timer= McSecTime;
		for (unsigned char i=0; i<sizeof(acc_message)-1;i++) acc_message.Cs+=*(&acc_message.Syncro+char(i));
		DMA_Transmit_Acc();
	}

	void DMA_Acc_Init()
	{
		DMA_InitTypeDef dma;
		DMA_DeInit(DMA1_Channel4);
			DMA_StructInit(&dma);
			dma.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->TDR);
			dma.DMA_MemoryBaseAddr = (uint32_t)&acc_message;
			dma.DMA_DIR = DMA_DIR_PeripheralDST;
			dma.DMA_BufferSize = sizeof(acc_message);
			dma.DMA_Priority = DMA_Priority_High;
			dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
			NVIC_SetPriority(DMA1_Channel4_IRQn,0);
			NVIC_EnableIRQ(DMA1_Channel4_IRQn);
			DMA_Init(DMA1_Channel4, &dma);
	};

	void Check_Size (void)
	{
		if (acc_message.Size>17)
		{
			Toggle_Led(YELLOW);
		}
	}
	~Message_Acc() {

	}
};
#endif
