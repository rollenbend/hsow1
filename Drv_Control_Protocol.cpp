/*
 * Drv_Sensor_Protocol.cpp
 *
 *  Created onHandler(){ Jul 21, 2016
 *      AuthorHandler(){ mike
 */

#include "Drv_Control_Protocol.h"
#include "Drv_Uart.h"
#include "string.h"
#include "Definitions.h"
#include "math.h"
#include "LoadProg.h"
#include "Messages.h"
#include "Drv_Control_Protocol.h"

SENSOR_PORT sens;
extern Message_Dpp dpp;
extern "C" void USART2_IRQHandler()
{
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		sens.ProcessInByte(USART_ReceiveData(USART2));// receive data
	}
}

int Normal_Work_Mode_Handler()
{
	TIM_Cmd(TIM3, DISABLE);
	dpp.Reset_Dpp();
	TIM_Cmd(TIM1, ENABLE);
	return 1;
}
unsigned short Period;

int Imitation_Mode_Handler(unsigned short Period)
{
	TIM_Cmd(TIM1, DISABLE);
	dpp.Reset_Dpp();
	dpp.Tim3_Imitation_Init(Period);
	return 1;
}



int Set_HighPass_Filter()
{
	return 1;
}
int Set_LowPass_Filter()
{
	return 1;
}

int BurnPortionHandler()
{
#define CounterMax 100
	volatile int Counter;
	for (Counter=1;Counter<CounterMax;Counter++)
	{
		if (ProgNextPortion(ProgLoadAdr+PortionOffset))
			break;
	}
	if (Counter>=CounterMax)
		ProgRes=0;
	else
		ProgRes=Counter;
	return 1;
}

int ProgLoadAdrHandler()
{
	ChangeLoadTable();
	return 1;
}

int UpdateProgramHandler()
{
	UpdateProgramAction();
	return 1;
}

int StoreLoadTableHandler()
{
	StoreLoadTableAction();
	return 1;
}


void InitDrvControlProtocol()
{
	sens.INIT_NO_DATA_COMMAND_SYMMETR(GetSensorData_1, Normal_Work_Mode_Handler); // work mode
	sens.INIT_COMMAND_COMPLEX(SensorData_1,GetSensorData_2,Imitation_Mode_Handler,Period); // imitation mode

	sens.INIT_COMMAND_COMPLEX(LoadProgAdr,GetProgAdr,ProgLoadAdrHandler,ProgLoadAdr);
	sens.INIT_COMMAND_NOPROC(LoadStartAdr,GetStartAdr,ProgStartAdr);
	sens.INIT_COMMAND_NOPROC(LoadProgData,GetProgData,ProgTmpBuf);
	sens.INIT_COMMAND_COMPLEX(BurnNextPortion,BurnNextPortion,BurnPortionHandler,ProgRes);
	sens.INIT_COMMAND_NOPROC(LoadPortionOffset, GetPortionOffset,PortionOffset);
	sens.INIT_COMMAND_NOPROC(LoadPorionSize, GetPortionSize,  PortionSize);
	sens.INIT_NO_DATA_COMMAND_SYMMETR(UpdateProgram,UpdateProgramHandler);
	sens.INIT_NO_DATA_COMMAND_SYMMETR(StoreLoadTable,StoreLoadTableHandler);
}
