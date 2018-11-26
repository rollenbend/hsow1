#include "Sensors.h"
#include "Messages.h"
#include "stm32f30x_dma.h"
#include "Initialization.h"
float A[4][4]={{0.99376, 0.014382, -0.0086219, -16},\
		{0.042502, 0.997122, 0.025546, -25.05},\
		{-0.02043, -0.006402, 0.972808, -20.428},\
		{-0.01583, -0.01509, -0.0159, 0.97253}};// если хотим g как 1000mg (подбор, значения лучше)


uint8_t accgyroBuffer[]={126,17,0,0, 0x67,0x2b, 0x57,0, 0xc8,0x1 , 0x67,0x2b, 0x57,0, 0xc8,0x1 ,0,0,0,0,0xff};//{'2','3',' ','-','5',' ','1','0','4','6'};
int Togg_Count=0;
Message_Acc acc;
volatile unsigned int McSecTime=0;
float AccBuffer[3]={1,2,3};
short GyroBuffer[3]={0};
Channel channel;
void Read_and_Transmit_Sensors_Data(void)
{
	McSecTime=TIM2->CNT;
	//ReadAcc (AccBuffer); // правка от 06.09 когда плата от сломанного датчика неисправно работает
	Toggle_Led(ORANGE);
	for (int i=0;i<1000;i++) {;}
	Toggle_Led(ORANGE);
	ReadGyro(GyroBuffer);
	channel.Get_Acc_Gyro_Data(AccBuffer, GyroBuffer);
	channel.Dpp_Acc_Transmit_DMA();

//	accgyroBuffer[1]=sizeof(accgyroBuffer)-4;
//	accgyroBuffer[10] = (int8_t)GyroBuffer[0];
//	accgyroBuffer[11] = ((int8_t)((int16_t)(((int16_t)GyroBuffer[0])>>8)));
//	accgyroBuffer[12] = (int8_t)GyroBuffer[1];
//	accgyroBuffer[13] = ((int8_t)((int16_t)(((int16_t)GyroBuffer[1])>>8)));
//	accgyroBuffer[14] = (int8_t)GyroBuffer[2];
//	accgyroBuffer[15] = ((int8_t)((int16_t)(((int16_t)GyroBuffer[2])>>8)));
//
//
//		  accgyroBuffer[16] = (uint8_t)McSecTime;
//		  accgyroBuffer[17] = ((uint8_t)((uint32_t)(((uint32_t)McSecTime)>>8)));
//		  accgyroBuffer[18] = ((uint8_t)((uint32_t)(((uint32_t)McSecTime)>>16)));
//		  accgyroBuffer[19] = ((uint8_t)((uint32_t)(((uint32_t)McSecTime)>>24)));
//				accgyroBuffer[20]=0;
//				for (int i=0; i<sizeof(accgyroBuffer)-1; i++) accgyroBuffer[20]+=accgyroBuffer[i];
//
//				for (int i=0;i<sizeof(accgyroBuffer);i++) {
//						USART_SendData(USART1, accgyroBuffer[i]);
//						while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)  {}// Loop until transmit data register is empty
//					}
//	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)  {;}// Loop until transmit data register is empty
//			for (char i=0; i<sizeof(acc.acc_message);i++) {
//				unsigned char byte = (unsigned char)*(&acc.acc_message.Syncro+char(i));
//				USART_SendData(USART1, byte);
//				while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)  {;}// Loop until transmit data register is empty
//			}
	if (!((Togg_Count++)%100)) Toggle_Led(BLUE);
}


void ACC_INIT(void)
{
  LSM303DLHCAcc_InitTypeDef AInitStruct;
  LSM303DLHCAcc_FilterConfigTypeDef FInitStructure;

   /* Fill the accelerometer structure */
  AInitStruct.Power_Mode = LSM303DLHC_NORMAL_MODE;				//NORMAL or LOWPOWER MODE (CTRL_REG1 ODR[3])
  AInitStruct.AccOutput_DataRate = LSM303DLHC_ODR_1344_HZ;  //output data rate				(CTRL_REG1)
  AInitStruct.Axes_Enable= LSM303DLHC_AXES_ENABLE;				//enable x, y and z axes	(CTRL_REG1)
  AInitStruct.AccFull_Scale = LSM303DLHC_FULLSCALE_16G;		//full scale - "polnaya shkala"   (CTRL_REG4)
  AInitStruct.BlockData_Update = LSM303DLHC_BlockUpdate_Continous;	//Block data update. Default value: 0; (0: continuous update, 1: output registers not updated until MSB and LSB have been read (CTRL_REG4)
  AInitStruct.Endianness=LSM303DLHC_BLE_LSB;	//??? Big/little endian data selection. Default value 0.(0: data LSB @ lower address, 1: data MSB @ lower address) AInitStruct.High_Resolution=LSM303DLHC_HR_ENABLE; (CTRL_REG4)
	AInitStruct.High_Resolution = LSM303DLHC_HR_ENABLE;  /* High Resolution enabling/disabling */
  /* Configure the accelerometer main parameters */
  LSM303DLHC_AccInit(&AInitStruct);

  /* Fill the accelerometer LPF structure ; CTRL_REG2 register*/
	/* mode, cutoff frquency, Filter status, Click, AOI1 and AOI2 */

  FInitStructure.HighPassFilter_Mode_Selection =LSM303DLHC_HPM_NORMAL_MODE; //??? rejim filtra verhnih chastot 00 Normal mode (reset reading HP_RESET_FILTER) 01 Reference signal for filtering 10 Normal mode 11 Autoreset on interrupt event
  FInitStructure.HighPassFilter_CutOff_Frequency = LSM303DLHC_HPFCF_64;			//vybor chastoty sreza (8, 16, 32, 64)
	FInitStructure.HighPassFilter_FDS =  LSM303DLHC_HIGHPASSFILTER_DISABLE;//LSM303DLHC_HIGHPASSFILTER_ENABLE;
  FInitStructure.HighPassFilter_AOI1 = LSM303DLHC_HPF_AOI1_DISABLE;
  FInitStructure.HighPassFilter_AOI2 = LSM303DLHC_HPF_AOI2_DISABLE;

  /* Configure the accelerometer LPF main parameters */
  LSM303DLHC_AccFilterConfig(&FInitStructure);

  uint8_t tmp=0x40;
  LSM303DLHC_Write(ACC_I2C_ADDRESS, TEMP_CFG_REG, &tmp);

}
void FilterIni()
{

}
void Filter()
{
	short x[50]={123};
	short y[50]={-2};
	short sum=0;
	for (int j=0;j<1000;j++) {
		for (int i=0;i<50;i++) {
			sum+=x[i]*y[i];
		}
		sum=0;
	}
}
void ReadAcc (float* pfData)
{
  int16_t pnRawData[3];
  uint8_t ctrlx[2];
  uint8_t buffer[6], cDivider;
  uint8_t i = 0;
  float LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_2g;

  /* Read the register content */
  LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_CTRL_REG4_A, ctrlx,2);
  LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_OUT_X_L_A, buffer, 6);

  if(ctrlx[1]&0x40)
    cDivider=64;
  else
    cDivider=16;

  /* check in the control register4 the data alignment*/
  if(!(ctrlx[0] & 0x40) || (ctrlx[1] & 0x40)) /* Little Endian Mode or FIFO mode */
  {
    for(i=0; i<3; i++)
    {
    	pnRawData[i]=((int16_t)((uint16_t)buffer[2*i+1] << 8) + buffer[2*i])/cDivider; //pfData[i]=((int16_t)((((int16_t)buffer[2*i]) << 8) + buffer[2*i+1]));
    }
  }
  else /* Big Endian Mode */
  {
    for(i=0; i<3; i++)
      pnRawData[i]=((int16_t)((uint16_t)buffer[2*i] << 8) + buffer[2*i+1])/cDivider;
  }
  /* Read the register content */
  LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_CTRL_REG4_A, ctrlx,2);


  if(ctrlx[1]&0x40)
  {
    /* FIFO mode */
    LSM_Acc_Sensitivity = 0.25;
  }
  else
  {
    /* normal mode */
    /* switch the sensitivity value set in the CRTL4*/
    switch(ctrlx[0] & 0x30)
    {
    case LSM303DLHC_FULLSCALE_2G:
      LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_2g;
      break;
    case LSM303DLHC_FULLSCALE_4G:
      LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_4g;
      break;
    case LSM303DLHC_FULLSCALE_8G:
      LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_8g;
      break;
    case LSM303DLHC_FULLSCALE_16G:
      LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_16g;
      break;
    }
  }

  /* Obtain the mg value for the three axis */
  for(i=0; i<3; i++)
  {
	  pfData[i]=(float)(pnRawData[i]/LSM_Acc_Sensitivity);
  }
}
void GYRO_INIT (void)
{
	L3GD20_InitTypeDef InitStruct;
//	L3GD20_FilterConfigTypeDef FilterStruct;

  InitStruct.Power_Mode = L3GD20_MODE_ACTIVE;       /* Power-down/Sleep/Normal Mode */
  InitStruct.Output_DataRate = L3GD20_OUTPUT_DATARATE_4;                    /* OUT data rate */
  InitStruct.Axes_Enable = L3GD20_AXES_ENABLE;      /* Axes enable */
  InitStruct.Band_Width = L3GD20_BANDWIDTH_4;                         /* Bandwidth selection */
  InitStruct.BlockData_Update = L3GD20_BlockDataUpdate_Continous;                   /* Block Data Update */
  InitStruct.Endianness = L3GD20_BLE_LSB;                         /* Endian Data selection */
  InitStruct.Full_Scale = L3GD20_FULLSCALE_500;     /* Full Scale selection */

	L3GD20_Init(&InitStruct);

	//void L3GD20_RebootCmd(void);

	/*INT1 Interrupt Configuration Functions */
	//void L3GD20_INT1InterruptCmd(uint8_t InterruptState);
	//void L3GD20_INT2InterruptCmd(uint8_t InterruptState);
	//void L3GD20_INT1InterruptConfig(L3GD20_InterruptConfigTypeDef *L3GD20_IntConfigStruct);
	//uint8_t L3GD20_GetDataStatus(void);

/* High Pass Filter Configuration Functions */


//	FilterStruct.HighPassFilter_Mode_Selection = L3GD20_HPM_NORMAL_MODE;      /* Internal filter mode */
//  FilterStruct.HighPassFilter_CutOff_Frequency = L3GD20_HPFCF_5;    /* High pass filter cut-off frequency */
//
//	L3GD20_FilterConfig(&FilterStruct);

	L3GD20_FilterCmd(L3GD20_HIGHPASSFILTER_DISABLE);
}


void ReadGyro (short* pfData)
{
//	for(short i=0; i<3; i++)
//	  {
//	    pfData[i]=(short)i;
//	  }
//void L3GD20_Write(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite);
//void L3GD20_Read(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);
//uint16_t LSM303DLHC_Read(uint8_t DeviceAddr, uint8_t RegAddr, uint8_t* pBuffer, uint16_t NumByteToRead)

  static uint8_t buffer[6] = {0};
 // uint8_t CTRLB = 0;
 // uint16_t Magn_Sensitivity_XY = 0, Magn_Sensitivity_Z = 0;
  uint8_t i =0;
 // LSM303DLHC_Read(MAG_I2C_ADDRESS, LSM303DLHC_CRB_REG_M, &CTRLB, 1);

  L3GD20_Read(buffer,   L3GD20_OUT_X_H_ADDR,  1);
  L3GD20_Read(buffer+1, L3GD20_OUT_X_L_ADDR,  1);
  L3GD20_Read(buffer+2, L3GD20_OUT_Y_H_ADDR,  1);
  L3GD20_Read(buffer+3, L3GD20_OUT_Y_L_ADDR,  1);
  L3GD20_Read(buffer+4, L3GD20_OUT_Z_H_ADDR,  1);
  L3GD20_Read(buffer+5, L3GD20_OUT_Z_L_ADDR,  1);


  for(i=0; i<3; i++)
  {
    pfData[i]=(int16_t)((((int16_t)buffer[2*i]) << 8) + buffer[2*i+1]);
  }
//  pfData[2]=((float)((int16_t)((((int16_t)buffer[4]) << 8) + buffer[5])));
}



void Calibr(float *AccBuffer, float *CalibrAccBuffer) // настройка ортоганальности и выравнивание шкал акселерометра
{
	uint8_t i = 0;
	uint8_t j = 0;
	CalibrAccBuffer[0]=A[0][3];
	CalibrAccBuffer[1]=A[1][3];
	CalibrAccBuffer[2]=A[2][3];
	for(i=0; i<3; i++)
	{
		for(j=0; j<3; j++)
		{
			CalibrAccBuffer[i]+=AccBuffer[j]*A[i][j];
		}
	}
//CalibrAccBuffer[0]=A[0][0]*AccBuffer[0]+A[0][1]*AccBuffer[1]+A[0][2]*AccBuffer[2]+A[0][3];
//CalibrAccBuffer[1]=A[1][0]*AccBuffer[0]+A[1][1]*AccBuffer[1]+A[1][2]*AccBuffer[2]+A[1][3];
//CalibrAccBuffer[2]=A[2][0]*AccBuffer[0]+A[2][1]*AccBuffer[1]+A[2][2]*AccBuffer[2]+A[2][3];
}
uint32_t L3GD20_TIMEOUT_UserCallback(void)
{
  return 0;
}

uint32_t LSM303DLHC_TIMEOUT_UserCallback(void)
{
	Toggle_Led(GREEN);
	I2C_Cmd(I2C2, DISABLE);
	 ACC_INIT();
	return 0;
}
