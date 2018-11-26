#include "Initialization.h"
extern Message_Dpp dpp;
extern Message_Acc acc;
extern Channel channel;
void INIT_ALL(void)
{
	SetSysClockTo64();
	gpio_ini();
	Init_Usart2_for_TransmitDpp_and_LoadProgram();
	Init_Usart1_For_TransmitAccGyro();
	Timers_Init();
	//ACC_INIT();  // правка от 06.09 когда плата от сломанного датчика неисправно работает
	GYRO_INIT ();
	FLASH_Unlock();
}
void gpio_ini(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_10|GPIO_Pin_11;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void Init_Usart2_for_TransmitDpp_and_LoadProgram() // FOR WAY CODE
{
		// Объявляем переменные
		GPIO_InitTypeDef gpio;
		USART_InitTypeDef usart;

    // Запускаем тактирование
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_USARTCLKConfig(RCC_USART2CLK_SYSCLK);

		// Инициализация нужных пинов контроллера, для USART2 –
    // PA2 - transmit и PA3 - receive
    GPIO_StructInit(&gpio);

    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &gpio);

    // И вот еще функция, которой не было при работе с STM32F10x,
    // но которую нужно вызывать при использовании STM32F3xx
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_7);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_7);

   // А теперь настраиваем модуль USART
   USART_StructInit(&usart);
   usart.USART_BaudRate           	= 115200;
	 usart.USART_Mode                	= USART_Mode_Tx|USART_Mode_Rx; //  USART_Mode_Rx;
   USART_Init(USART2, &usart);

	 NVIC_EnableIRQ(USART2_IRQn);
	 USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
//	DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);
//    NVIC_EnableIRQ(DMA1_Channel7_IRQn);

	 USART_Cmd(USART2, ENABLE);
   USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);

	dpp.DMA_Dpp_Init();
}


 void Init_Usart1_For_TransmitAccGyro() // FOR ACCEL AND GYRO
{
		// Объявляем переменные
		GPIO_InitTypeDef gpio1;
		USART_InitTypeDef usart1;

    /* enable clocking Запускаем тактирование */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
RCC_USARTCLKConfig(RCC_USART1CLK_SYSCLK);

	/* ini pins Инициализация нужных пинов контроллера,
	 * для USART1 : PB4 - transmit
	 */

    GPIO_StructInit(&gpio1);
    gpio1.GPIO_Mode = GPIO_Mode_AF;
    gpio1.GPIO_Pin = GPIO_Pin_6;
    gpio1.GPIO_Speed = GPIO_Speed_50MHz;
    gpio1.GPIO_OType = GPIO_OType_PP;
    gpio1.GPIO_PuPd = GPIO_PuPd_UP;
			GPIO_Init(GPIOB, &gpio1);

    /* И вот еще функция, которой не было при работе с STM32F10x,
     но которую нужно вызывать при использовании STM32F3xx*/
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_7);

   /* А теперь настраиваем модуль USART 1 */
   USART_StructInit(&usart1);
   usart1.USART_BaudRate           		= 921600;//460800;////1382400
   usart1.USART_Mode                	= USART_Mode_Tx; //  USART_Mode_Tx(dma);
   USART_Init(USART1, &usart1);

	USART_Cmd(USART1, ENABLE);
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);

	channel.DMA_Packet_Init();
}
void SetSysClockTo64(void)
{
    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/
    /* RCC system reset(for debug purpose) */
    RCC_DeInit();

		RCC_AdjustHSICalibrationValue(16);

  /* Enable HSi */
    RCC_HSICmd(ENABLE);

    /* Wait till HSE is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET) {;};

        /* Enable Prefetch Buffer */
        FLASH_PrefetchBufferCmd(ENABLE);
			while (FLASH->ACR && 1<<5 == RESET) {;} // check

        /* Flash 2 wait state */
        FLASH_SetLatency( FLASH_Latency_2);
			while (FLASH->ACR && 1<<1 == RESET) {;} // check


        /* AHB clock (HCLK) = SYSCLK = 64 MHz */
        RCC_HCLKConfig( RCC_SYSCLK_Div1);

        /* PCLK2(APB2) = HCLK = 64 MHz */
        RCC_PCLK2Config( RCC_HCLK_Div1);

        /* PCLK1(APB1) = HCLK/2 = 32MHz */
        RCC_PCLK1Config( RCC_HCLK_Div2);

		//		RCC_PREDIV1Config(RCC_PREDIV1_Div1); //no division
        /* PLLCLK = 8/2MHz * 16 = 64 MHz */
        RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_16);

        /* Enable PLL */
        RCC_PLLCmd( ENABLE);

				/* Wait till PLL is ready */
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {;}

        /* Select PLL as system clock source */
        RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK);

        /* Wait till PLL is used as system clock source */
        while (RCC_GetSYSCLKSource() != 0x08)
        {;
        }
}

void FlashReProgramError(void)
{
	while (1)
		{
			RED_ON;
			for (int j=0; j<3000000; j++) {;}
			RED_OFF;
			for (int j=0; j<3000000; j++) {;}
		}
}
void Toggle_Led(uint16_t GPIO_Pin_x)
{
	((GPIOB->ODR & GPIO_Pin_x) == (uint32_t)Bit_RESET)?GPIO_SetBits(GPIOB, GPIO_Pin_x): GPIO_ResetBits(GPIOB, GPIO_Pin_x);
}


