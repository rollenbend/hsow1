#include "FlashPrg.h"
#include "Initialization.h"
#include "Drv_Uart.h"
#include "LoadProg.h"

unsigned int ProgStartAdr;
unsigned int ProgLoadAdr;
unsigned int PortionOffset;
unsigned int PortionSize;
unsigned char ProgRes;

unsigned char ProgTmpBuf[256];  // то что заполняется при приеме пакета из ПЭВМ

extern void Pulse2(uint16_t GPIO_Pin_x);
extern bool UpdateProgramAndReboot(int Start,int Size);

extern int Delimiter ();

extern unsigned int SoftVersion;
#define AddToSoftVersion(A) for(int i=0;i<sizeof(A);i++)SoftVersion+=(unsigned int)A[i]

void InitLoadProg()
{
	AddToSoftVersion(__DATE__); AddToSoftVersion(__TIME__);
}
///*
// * #define FLASH_PAGE_SIZE         ((uint32_t)0x00000800)   /* FLASH Page Size */
//#define FLASH_USER_START_ADDR   ((uint32_t)0x08020000)   /* Start @ of user Flash area */
//#define FLASH_USER_END_ADDR     ((uint32_t)0x08034000)   /* End @ of user Flash area */
//
//#define FlashSectorSize	FLASH_PAGE_SIZE
//#define ProgBufArea FLASH_USER_START_ADDR
//#define FlashStart  0x08000000
// *
// */

LoadTableElement LoadTable[256];
int CurrentLoadTableElement=0;
int CurrentTableSize=0;
unsigned int StorePosition=ProgBufArea;
unsigned int SectionSize=0;

void StoreLoadTableAction()// вызывается в конце загрузки файла программы в буферную память
{
	ChangeLoadTable();
	// Таблица сформирована
	CurrentTableSize=CurrentLoadTableElement;
	CurrentLoadTableElement=0; // на случай новых загрузок
}

bool EnableToStoreElement=false;

void ChangeLoadTable()
{
	if (CurrentLoadTableElement) // если это не первый элемент, то фиксируем размер предыдущего элемента
	{
		LoadTable[CurrentLoadTableElement-1].ProgramSectionSize=SectionSize;
	} else
		StorePosition=ProgBufArea;
	SectionSize=0;
	EnableToStoreElement=true; // запоминание реально происходит позже, при выполнении ProgNextPortion
}

bool ProgNextPortion(unsigned int Address) // это тот адрес, который прилетает их НЕХ-файла
{
	volatile int t;
	volatile	unsigned int OffsetFromFlashStart;

#define LocalStorePlace StorePosition+i*sizeof(int)
#define DataInRomBuf *(int *)(LocalStorePlace)
#define DataEqual() (((int*)&ProgTmpBuf)[i]==DataInRomBuf)


	if (EnableToStoreElement)
	{
		LoadTable[CurrentLoadTableElement  ].SourceTableAddress=StorePosition;// место сохранения очередного куска программы в буфере
		LoadTable[CurrentLoadTableElement++].ProgramDestinationAddress=Address;
		EnableToStoreElement=false;
	}

	int limit=PortionSize/sizeof(int);
	if (PortionSize%sizeof(int))
		limit++;
	for (int i=0;i<limit;i++)
	{
																								 //
		if (((LocalStorePlace)%FlashSectorSize)==0) // Похоже надо стирать страницу
		{
			if (FLASH_ErasePage(LocalStorePlace)!= FLASH_COMPLETE)
			{
				t=8;// чтобы было куда ставить точку останова
				return false;
			}
		}

__IO uint32_t * tmp= (__IO uint32_t *)(StorePosition+i*sizeof(int));
uint32_t vtmp=*tmp;
uint32_t check=0xffffffff;
		if (vtmp != check)
			FlashReProgramError();

		if (FLASH_ProgramWord(LocalStorePlace, ((int*)&ProgTmpBuf)[i]) != FLASH_COMPLETE)
		{
//#define FLASH_ER_PRG_TIMEOUT1 ((uint32_t)0x00F00000)
//			if (FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT1) == FLASH_BUSY)
//				for (;;) Pulse2(GPIO_Pin_9);
//			if (FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT1) == FLASH_ERROR_WRP)
//				for (;;) Pulse2(GPIO_Pin_10);
//			if (FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT1) == FLASH_ERROR_PROGRAM)
//				for (;;) Pulse2(GPIO_Pin_11);

			t=DataInRomBuf;// чтобы было куда ставить точку останова
			return false;
		}

		if (!DataEqual())
		{
			t=DataInRomBuf;// чтобы было куда ставить точку останова
			return false; // Если после программирования не совпало, то караул!!
		}
	}


	//StorePosition+=PortionSize;
	StorePosition+=limit*sizeof(int);
	SectionSize+=PortionSize;

	return true;

}

/*
 * Данная процедура прожигает ранее принятую во временный буфер программу по реальным исполняемым адресам
 * Данная процедура специально перенесена в конец ПЗУ чтобы избежать всяких пересечений с исполняемой пока еще программой
 * Возврат из данной процедуры происходит в новую загруженную программу происходит как при холодном старте
 */

/* Add new section at end of FLASH in mem.ld like this:
 * MEMORY
 * {
 * 	PLOADER (rx) : ORIGIN = 0x0801D000, LENGTH = 2K
 * ...
 * }
 *
 *  placing my named section at given address in sections.ld:
 * .FinalProg 0x0801D000 :
 * {
 *   KEEP(*(.FinalProg))  // keep my variable even if not referenced
 * } > PLOADER
 *
 *
 *
 * !!! исключен символ DEBUG из _startup.c по причине __data_end_guard != DATA_END_GUARD_VALUE */


#define BOTTOM 1

#ifdef BOTTOM
	#define PLACE_AT_BOTTOM	 __attribute__((section (".FinalProg")))
#else
	#define PLACE_AT_BOTTOM
#endif

FLASH_Status PLACE_AT_BOTTOM Local_FLASH_GetStatus(void)
{
  static FLASH_Status FLASHstatus = FLASH_COMPLETE;

  if((FLASH->SR & FLASH_FLAG_BSY) == FLASH_FLAG_BSY)
  {
    FLASHstatus = FLASH_BUSY;
  }
  else
  {
    if((FLASH->SR & (uint32_t)FLASH_FLAG_WRPERR)!= (uint32_t)0x00)
    {
      FLASHstatus = FLASH_ERROR_WRP;
    }
    else
    {
      if((FLASH->SR & (uint32_t)(FLASH_SR_PGERR)) != (uint32_t)0x00)
      {
        FLASHstatus = FLASH_ERROR_PROGRAM;
      }
      else
      {
        FLASHstatus = FLASH_COMPLETE;
      }
    }
  }
  /* Return the FLASH Status */
  return FLASHstatus;
}

FLASH_Status  PLACE_AT_BOTTOM  Local_FLASH_WaitForLastOperation(uint32_t Timeout)
{
  static FLASH_Status status = FLASH_COMPLETE;
  /* Check for the FLASH Status */
  status = Local_FLASH_GetStatus();

  /* Wait for a FLASH operation to complete or a TIMEOUT to occur */
  while((status == FLASH_BUSY) && (Timeout != 0x00))
  {
    status = Local_FLASH_GetStatus();
    Timeout--;
  }

  if(Timeout == 0x00 )
  {
    status = FLASH_TIMEOUT;
  }
  /* Return the operation status */
  return status;
}

FLASH_Status  PLACE_AT_BOTTOM   Local_FLASH_ErasePage(uint32_t Page_Address)
{
  static FLASH_Status status = FLASH_COMPLETE;
#define FLASH_ER_PRG_TIMEOUT1         ((uint32_t)0x00B00000)

  /* Wait for last operation to be completed */
  status = Local_FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT1);

  if(status == FLASH_COMPLETE)
  {
    /* If the previous operation is completed, proceed to erase the page */
    FLASH->CR |= FLASH_CR_PER;
    FLASH->AR  = Page_Address;
    FLASH->CR |= FLASH_CR_STRT;

    /* Wait for last operation to be completed */
    status = Local_FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT1);

    /* Disable the PER Bit */
    FLASH->CR &= ~FLASH_CR_PER;
  }

  /* Return the Erase Status */
  return status;
}

FLASH_Status PLACE_AT_BOTTOM  Local_FLASH_ProgramWord(uint32_t Address, uint32_t Data)
{
  static FLASH_Status status = FLASH_COMPLETE;
  static __IO uint32_t tmp = 0;
#define FLASH_ER_PRG_TIMEOUT1         ((uint32_t)0x00B00000)
  /* Wait for last operation to be completed */
  status = Local_FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT1);

  if(status == FLASH_COMPLETE)
  {
    /* If the previous operation is completed, proceed to program the new first
    half word */
    FLASH->CR |= FLASH_CR_PG;

    *(__IO uint16_t*)Address = (uint16_t)Data;

    /* Wait for last operation to be completed */
    status = Local_FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT1);

    if(status == FLASH_COMPLETE)
    {
      /* If the previous operation is completed, proceed to program the new second
      half word */
      tmp = Address + 2;

      *(__IO uint16_t*) tmp = Data >> 16;

      /* Wait for last operation to be completed */
      status = Local_FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT1);

    }
    /* Disable the PG Bit */
    FLASH->CR &= ~FLASH_CR_PG;
  }

  /* Return the Program Status */
  return status;
}

bool PLACE_AT_BOTTOM  Local_FLASH_Write(uint32_t Address, uint32_t Data)
{
#define _DataInRomBuf *(int *)(Address)

	static volatile int t;
	if (((Address)%FlashSectorSize)==0) // Похоже надо стирать страницу
	{
		if (Local_FLASH_ErasePage(Address)!= FLASH_COMPLETE)
		{
			t=8;// чтобы было куда ставить точку останова
			return false;
		}
	}
	if (Local_FLASH_ProgramWord(Address, Data) != FLASH_COMPLETE)
	{
		t=_DataInRomBuf;// чтобы было куда ставить точку останова
		return false;
	}

	if (Data!=_DataInRomBuf)
	{
		t=_DataInRomBuf;// чтобы было куда ставить точку останова
		return false; // Если после программирования не совпало, то караул!!
	}
	return true;
}

void PLACE_AT_BOTTOM UpdateProgramActionEnd();

void PLACE_AT_BOTTOM UpdateProgramAction()
{
	__disable_irq(); // необходимо запретить прерывания как минимум на время перезаписи таблицы прерываний и всех обработчиков прерываний
	for (static int i=0;i<CurrentTableSize;i++)
	{
		if ( 				// проверяем-не перезатираем ли сами себя
				(LoadTable[i].ProgramDestinationAddress>=(int)(&Local_FLASH_GetStatus)) &&
				(LoadTable[i].ProgramDestinationAddress<=(int)(&UpdateProgramActionEnd))
			)
		continue;
		for (static unsigned int j=0;j<LoadTable[i].ProgramSectionSize/sizeof(uint32_t);j++)
		{
			if (!Local_FLASH_Write(LoadTable[i].ProgramDestinationAddress+j*sizeof(uint32_t), ((uint32_t*)(LoadTable[i].SourceTableAddress))[j]))
			{
				// здесь надо бы послать сигнал по СОМ порту в ПЭВМ для доклада о катастрофе
				FlashReProgramError(); // все пропало, теперь только эмулятор...
			}
		}

	}
	// здесь надо бы послать сигнал по СОМ порту в ПЭВМ для доклада об успехе
	for (int i=0;i<10;i++) // чуть помигаем разными глазами если все хорошо
	{
		GPIOE->BSRR = GPIO_Pin_10;
				for (int j=0; j<1000000; j++){;}
			  	GPIOE->BRR = GPIO_Pin_10;
				for (int j=0; j<1000000; j++){;}
	}


	SCB->AIRCR=(0x05FA0000)+4; // аппаратный сброс (+4) - работает при наличии ключа (0x05FA0000)

// возможная программная альтернатива аппаратному сбросу -могут быть проблемы из-за повторной инициализации

//extern  void (* const g_pfnVectors[])(void);
//	void (*Run)()=(void(*)())g_pfnVectors[1];//
	//Run();
}

void PLACE_AT_BOTTOM UpdateProgramActionEnd() {;} // нужно только для отметки конца прожигающей секции

