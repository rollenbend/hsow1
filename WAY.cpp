#include "Messages.h"

// uint8_t DppSend[]={0x53,0x04,0,0,0,0,0x57,0xc8,0,0,0,0,0,0xff}; // packet 14 baits where 7b - dpp, 7b - speed,timer
//
//uint32_t DPP=0;
//short Speed=0;
//short McSecTimePrevious=0;


char FLAAG=0;




extern Channel channel;



Message_Dpp dpp;

extern "C" void TIM1_UP_TIM16_IRQHandler()
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update)){
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
//	DPP+=5;
//	__disable_irq ();
//	if (TIM1->CR1 & (1<<4))	Speed=(short)(int(10440000*(McSecTimePrevious-McSecTime))); // Direction Bit
//				else Speed=(short)(int(-10440000*(McSecTimePrevious-McSecTime)));
//				McSecTimePrevious=McSecTime;
//
//			DppSend[2] = (uint8_t)DPP;
//		  DppSend[3] = ((uint8_t)((uint32_t)(((uint32_t)DPP)>>8)));
//		  DppSend[4] = ((uint8_t)((uint32_t)(((uint32_t)DPP)>>16)));
//		  DppSend[5] = ((uint8_t)((uint32_t)(((uint32_t)DPP)>>24)));
//		  DppSend[6] = 0;
//		  DppSend[6] = 0x57+DppSend[2]+DppSend[3]+DppSend[4]+DppSend[5]; // cs dpp
//			DppSend[7] = (uint8_t)Speed;
//			DppSend[8] = ((uint8_t)((uint16_t)(((int16_t)Speed)>>8)));
//				DppSend[9] = (uint8_t)McSecTime;
//		  DppSend[10] = ((uint8_t)((uint32_t)(((uint32_t)McSecTime)>>8)));
//		  DppSend[11] = ((uint8_t)((uint32_t)(((uint32_t)McSecTime)>>16)));
//		  DppSend[12] = ((uint8_t)((uint32_t)(((uint32_t)McSecTime)>>24)));
//		  DppSend[13] = 0;
//			DppSend[13]=DppSend[6]+DppSend[7]+DppSend[8]+DppSend[9]+DppSend[10]+DppSend[11]+DppSend[12]; // cs speed
//	for (int i=0;i<14;i++) {
//		USART_SendData(USART1, DppSend[i]);
//		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)  {}// Loop until transmit data register is empty
//	}
//	//dpp.Incr_Main_Dpp_and_Transmit_DMA();
//	Toggle_Led(YELLOW);
//	__enable_irq ();
	Toggle_Led(YELLOW);
	channel.Incr_DPP();
	FLAAG=1;
	}
}
extern "C" void TIM3_IRQHandler()
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update)){
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	dpp.Incr_Main_Dpp_and_Transmit_DMA();
	}
}



void Timers_Init(void)
{
	dpp.Tim1_Encoder_Init();
	dpp.Tim2_For_Marking_Messages_Init();
}
void Message_Dpp::Tim1_Encoder_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
		TIM_TimeBaseInitTypeDef tim;

		RCC_TIMCLKConfig(RCC_TIM1CLK_HCLK);
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);


		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	  /* Tim1 CH1,CH2 pin configuration */
	    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;
	    GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_6);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_6);

		tim.TIM_Prescaler = 0;
		tim.TIM_CounterMode = TIM_CounterMode_Up;
		tim.TIM_Period = 159;
		tim.TIM_ClockDivision = TIM_CKD_DIV1;
		tim.TIM_RepetitionCounter = 0;

		NVIC_SetPriority(TIM1_UP_TIM16_IRQn,0);
		NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);
		TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
		TIM_UIFRemap(TIM1, ENABLE);
		TIM_UpdateRequestConfig(TIM1, TIM_UpdateSource_Global);

		TIM_TimeBaseInit(TIM1, &tim);
		TIM_EncoderInterfaceConfig(TIM1, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
		TIM_Cmd(TIM1, ENABLE);

}
void Message_Dpp::Tim2_For_Marking_Messages_Init(void)
{
	TIM_TimeBaseInitTypeDef tim;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		tim.TIM_Prescaler = 63; // APB1 clock = 32MHz, BUT! BEFORE TIMERS x2, THEN TIM2CLK=64 MHz; doing 1MHz frequency
		tim.TIM_CounterMode = TIM_CounterMode_Up;
		tim.TIM_Period = 0xFFFFFFFF;
		tim.TIM_ClockDivision = TIM_CKD_DIV1;
		tim.TIM_RepetitionCounter = 0;
		TIM_TimeBaseInit(TIM2, &tim);
		TIM_Cmd(TIM2, ENABLE);
}

void Message_Dpp::Tim3_Imitation_Init(unsigned short Speed)
{
	unsigned short Period;
	if (!Speed) Period=1;
	Period=1811/Speed; // 2êHz*5 - Frequency*dpp step

	TIM_TimeBaseInitTypeDef tim;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		tim.TIM_Prescaler = 31999; //  TIM2CLK=64 MHz; doing 2kHz frequency, then Period=0.5ms
		tim.TIM_CounterMode = TIM_CounterMode_Up;
		tim.TIM_Period = Period;
		tim.TIM_ClockDivision = TIM_CKD_DIV1;
		tim.TIM_RepetitionCounter = 0;
		TIM_TimeBaseInit(TIM3, &tim);
		TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
		NVIC_SetPriority(TIM3_IRQn,0);
		NVIC_EnableIRQ(TIM3_IRQn);
		TIM_Cmd(TIM3, ENABLE);
}
