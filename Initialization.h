#include "stm32f30x.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_gpio.h"
#include "stm32f30x_usart.h"
#include "stm32f30x_dma.h"
#include "stm32f30x_flash.h"
#include "Sensors.h"
#include "stdlib.h"
#include "Messages.h"

void INIT_ALL(void);
void gpio_ini(void);
void Init_Usart2_for_TransmitDpp_and_LoadProgram();
void Init_Usart1_For_TransmitAccGyro();
void SetSysClockTo64(void);
void FlashReProgramError(void);
void Toggle_Led(uint16_t GPIO_Pin_x);

#define RED    GPIO_Pin_0
#define YELLOW GPIO_Pin_1
#define ORANGE GPIO_Pin_2
#define BLUE   GPIO_Pin_10
#define GREEN  GPIO_Pin_11

#define MINI_DELAY for (int j=0; j<1000; j++) {;}

#define RED_ON GPIO_SetBits(GPIOB, GPIO_Pin_0);
#define RED_OFF GPIO_ResetBits(GPIOB, GPIO_Pin_0);

#define ORANGE_ON GPIO_SetBits(GPIOB, GPIO_Pin_1);
#define ORANGE_OFF GPIO_ResetBits(GPIOB, GPIO_Pin_1);

#define YELLOW_ON GPIO_SetBits(GPIOB, GPIO_Pin_2);
#define YELLOW_OFF GPIO_ResetBits(GPIOB, GPIO_Pin_2);

#define BLUE_ON GPIO_SetBits(GPIOB, GPIO_Pin_10);
#define BLUE_OFF GPIO_ResetBits(GPIOB, GPIO_Pin_10);

#define GREEN_ON GPIO_SetBits(GPIOB, GPIO_Pin_11);
#define GREEN_OFF GPIO_ResetBits(GPIOB, GPIO_Pin_11);
