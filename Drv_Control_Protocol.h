#ifndef __ARM_SENSOR_PROTOCOL__
#define __ARM_SENSOR_PROTOCOL__

#define MaxDataSize		255
#define MaxCommandsCount	255
#include "Drv_Uart.h"
#include "Initialization.h"
typedef struct
{
	int CommandCode;
	int RelpyCode;
	void *BufAdr;
	int BufSize;
    char *BufP;
} PORT_CMD;

#define SyncroByte 0x7e
enum {WantSyncro,WantSize,WantStatus,WantCommandCode,WantData,WantConsum,ErrorSyncro}; /*CommandStates*/;
enum {No_Error,NotSyncByte,DataSizeError,UnknownCommand,ConsumError}; //ErrorCodes;

#define INIT_COMMAND_COMPLEX(CODE,RPLY,PROC,BUF) RegisterCallBackCommand(CODE, RPLY,(char*)&PROC,&(BUF),sizeof(BUF))
#define INIT_COMMAND_NOPROC(CODE,RPLY,BUF) RegisterCallBackCommand(CODE, RPLY,0,&(BUF),sizeof(BUF))
#define INIT_COMMAND_SYMMETR(CODE,PROC,BUF) INIT_COMMAND_COMPLEX(CODE,CODE,PROC,BUF)
#define INIT_NO_DATA_COMMAND(CODE,RPLY,PROC) RegisterCallBackCommand(CODE, RPLY, (char*)&PROC,(char*)0,0)
#define INIT_NO_DATA_COMMAND_SYMMETR(CODE,PROC) INIT_NO_DATA_COMMAND(CODE,CODE,PROC)


class SENSOR_PORT
{
public:
	SENSOR_PORT()
	{
		ConSummOut=0;
		ConSummIn=0;
		OutPacketCounter=0;
		_Status=0;
		InPacketCounter=RestartOutCounter=0;
		ProtocolStage= WantSyncro;
		NeedToStartCommand=false;
	    ErrorProtocolCode=	No_Error;
	    DataSize=0;
	    DataIndex=0;
	    CommandResult=0;
	    PollingCounter=0;
	    LedOffPeriod=1000000;
	    for (int i=0;i<sizeof(COMMAND);i++) ((char*)(&COMMAND))[i]=0xff;
	};

	virtual   ~SENSOR_PORT()
	{
	}
	
	
	
	int ProtocolStage;
	int OutPacketCounter,InPacketCounter,RestartOutCounter;
	unsigned char ConSummOut,ConSummIn;
	int DataSize,ErrorProtocolCode;
	PORT_CMD COMMAND[MaxCommandsCount];
	volatile unsigned char ObtainedCommand,_Status;
	unsigned char DataBuffer[MaxDataSize];
	int DataIndex;
	int PollingCounter;
	int LedOffPeriod;
	bool NeedToStartCommand;
	bool CommandResult;


	

	virtual void IdleAction()
	{
	}

	virtual void LinkLedON()
	{
	}

	virtual void LinkLedOFF()
	{
	}

	void _SendByte(char Bt)
	{
		LinkLedON();
		ConSummOut+=Bt;
		USART_SendData(USART2, Bt);
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)  {}// Loop until transmit data register is empty 
	//	putChar(Bt);
	}

	bool StartSend(unsigned char status, unsigned char CommandCode, unsigned char *Buf, unsigned char Size)
	{
		int i;
		ConSummOut=0;
		_SendByte(SyncroByte);
		_SendByte(Size+1);
		_SendByte(status);
		_SendByte(CommandCode);
		for (i=0;i<Size;i++)
			_SendByte(Buf[i]);
		_SendByte(ConSummOut);

		OutPacketCounter++;

		return true;
	}
#define SendData(S,C,D) StartSend(S, C, (unsigned char *)(&D), sizeof(D))
	
	void _GetData(void *A)
	{
		if (!A)
			return;
		for (int i=0;i<DataIndex;i++)	((char*)A)[i]=DataBuffer[i];
	}

	bool TestDataSize(unsigned char Size)
	{
		return  (Size==0) || (Size>=MaxDataSize);
	}

	bool TestCommandCode (unsigned char Code)
	{
		return (COMMAND[Code].BufP!=(char*)0xffffffff) && (COMMAND[Code].CommandCode==Code);
	}
	
	bool RegisterCallBackCommand(int CommandID, int ReplyID, char *Proc,void *CommandBuff=0,int CommandSize=0)
	{
		if (CommandID>=MaxCommandsCount) return false;
		COMMAND[CommandID].BufP = Proc;
		COMMAND[CommandID].BufAdr = CommandBuff;
		COMMAND[CommandID].BufSize = CommandSize;
		COMMAND[CommandID].CommandCode = CommandID;
		COMMAND[CommandID].RelpyCode=ReplyID;
	//	iii=0;
//		for (int i=0;i<50;i++)
//			XB[i]=0;
		return true;
	}

public:
 	void ProcessInByte(unsigned char Bt)
	{
		switch(ProtocolStage)
		{
		case WantSyncro:
			if (Bt==SyncroByte)
			{
				ProtocolStage=WantSize;
				ConSummIn=Bt;
			}
			else
			{
				ErrorProtocolCode = NotSyncByte;
			}
			break;
		case WantSize:
			ConSummIn+=Bt;
			if (TestDataSize(Bt))
			{
				ProtocolStage=ErrorSyncro;
				ErrorProtocolCode=DataSizeError;
			}
			else
			{
				DataSize=Bt-1;
				ProtocolStage=WantStatus;
			}

			break;
		case WantStatus:
			_Status=Bt;
			ConSummIn+=Bt;
			ProtocolStage=WantCommandCode;
			break;
		case WantCommandCode:
			ConSummIn+=Bt;

			if (!TestCommandCode(Bt))
			{
				ProtocolStage=ErrorSyncro;
				ErrorProtocolCode=UnknownCommand;
				
			}
			else
			{
				ObtainedCommand=Bt;
				DataIndex=0;
				if (DataSize==0)
					ProtocolStage=WantConsum;
				else{
					ProtocolStage=WantData;
			
				}
			}

			break;
		case WantData:
			if ((DataIndex<DataSize) && (DataIndex<MaxDataSize))
			{
				ConSummIn+=Bt;
				DataBuffer[DataIndex]=Bt;
			}
			if (++DataIndex==DataSize){
				ProtocolStage=WantConsum;

			}
			break;
		case WantConsum:
			if (Bt == ConSummIn){// правильный принятый пакет с которым надо что-то делать.
				NeedToStartCommand=true;
			}
			ConSummIn=0;
			ProtocolStage=WantSyncro;
			break;
		default: ProtocolStage=WantSyncro; break;
		}
	}

	
	
	
	void ProcessControlCommand() //предполагается что будет вызываться в главном цикле main
 	{
		int res;
		if (!NeedToStartCommand)
		{
			if(((PollingCounter++)%LedOffPeriod)==0)
				LinkLedOFF(); // периодически выключаю светодиод обозначающий передачу информации из датчика
			IdleAction();
			return ;
		}
		ConSummIn = 0;

		if (DataSize>COMMAND[ObtainedCommand].BufSize)
			DataSize=COMMAND[ObtainedCommand].BufSize;
		
		_GetData(COMMAND[ObtainedCommand].BufAdr);

	
		//XB[(iii++)%50]=ObtainedCommand;
		
		
		if (COMMAND[ObtainedCommand].BufP != 0)
			res=((int(*)())COMMAND[ObtainedCommand].BufP)();
		else
			res=1;
		if (res)// если надо, то новый толчок для циклических команд, если пришел правильный отклик
		{
			if ((COMMAND[ObtainedCommand].BufAdr) && (COMMAND[ObtainedCommand].BufSize)) // отправляю обратно не больше чем принято, т.е DataSize
			{
				unsigned char OutSize=(COMMAND[ObtainedCommand].BufSize);
				if (DataSize)
					OutSize=DataSize;
				Toggle_Led(YELLOW);
				StartSend(CommandResult, COMMAND[ObtainedCommand].RelpyCode,	(unsigned char*)(COMMAND[ObtainedCommand].BufAdr), OutSize);
			}
			else
			{
				
				StartSend(CommandResult, COMMAND[ObtainedCommand].RelpyCode,0,0);//(unsigned char)DataSize);//
			}
		}
		InPacketCounter++;
		NeedToStartCommand=false;
	}


};

extern void InitDrvControlProtocol();
extern void DrvControlProcess();

#endif
