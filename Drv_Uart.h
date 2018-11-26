
#include "stm32f30x_usart.h"

#include "stm32f30x_gpio.h"

void InitUart(int Speed);// UART configuration
void PutTX(int A);
void UartTransmissionHandler(void);
void UartReceiveHandler(int c);
void UartSendChar(int c);
unsigned char UartSendBuff(unsigned char *Buff, int Size);

void UartInterruptHandler(void);
void Uart_irq_enable (void);
void Uart_irq_disable (void);
void UartSendString(unsigned char *Buff, int Size);
void SendUartNum(int N);

#define SendUart(A)	UartSendString((unsigned char*)(A),sizeof(A))
#define SendUartB(A)	UartSendBuff((unsigned char*)(A),sizeof((A)))



