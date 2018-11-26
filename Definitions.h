#ifndef __DEFIN__
#define __DEFIN__

//#define FilterType float
#define FilterType unsigned int
#define CalibrCell unsigned short

// ���� ������ 

#define EmptyCode			0
#define GetDevType		0x01	//������ ���� ����������, ������ ���
#define DevType				0x02	//����� ���� ���������� - 1 ���� (0 � ������, 1 � ������������� �����, 2 � ������������ �����, 3 - ��)
#define GetSerNum			0x03	//������ ��������� ������, 4 �����
#define SetSerNum			0x04	//������ ��������� ������ 4 �����
#define GetSoftVer		0x05	//������ ������ �����, ������ ���
#define SoftVer				0x06	//����� ������ ����� 4 �����
#define GetHardVer		0x07	//������ ������ ������, ������ ���
#define HardVer				0x08	//����� ������ ������ 4 �����

#define GetResetCounter		0x09	//������ �������� �������, ������ ���
#define ResetCounter			0x0A	//����� �������� ������� 4 �����
#define GetWdtCounter			0x0B	//������ �������� ������������ ����������� �������, ������ ���
#define WdtCounter				0x0C	//�������� ������������ ����������� ������� 4 �����
	
// TAKE THIS CODES FOR HSOW1
#define GetSensorData_1		0x11	// WORK MODE //������  ������� ������������� ������, ������ ��� ������ �1
#define SensorData_1		0x12	// IMITATION MODE //������� ������������� ������ 2 ����� ������ �1
#define GetSensorData_2		0x13	// REPLY IMITATION MODE //������  ������� ������������� ������, ������ ��� ������ �2
#define SensorData_2		0x14	// HIGH PASS FILTER //������� ������������� ������ 2 ����� ������ �2
#define SetMode				0x21	// LOW PASS FILTER  // ���������/��������� ������ ��������� ������ ������ 3 �����: 1 ���� � ��� ������, 2 ���� � ��� �������, 2 ���� � �������� ������� (�������).
									// ������ ������� �������� ��������� ������ ������������� ����������
									/*
									������� 0x21 ������������� ��������� ������ ������, ������������ ������� ������:
									��� 0: 1 � ��������� ������ �������������� ������
									��� 1: 1 � ��������� �������
									��� 2: 1 � ��������� �������� �� ������������� �������
									���� 3-5: �������� ������ ���������� ������
									*/	

#define GetMode				0x22	//����� � ������� ������ 3 ����� 1 ���� � ��� ������, 2 ���� � ��� �������, 2 ���� � �������� ������� (�������).

#define StartCalibr			0x31	//������� �������� � ����� ���������� (�������� ������ �����), ������ ���
#define Calibr				0x32	//������� � ����� ���������� ��������, ������ ���
#define StartCalibrPoint	0x34	//������ ����� �������, ������ ���
#define SetCalibrPoint		0x35	//������� �������� �������������� ��������,1-2 ���� � ����� �������������� ��������
#define RdyCalibrPoint		0x36	//������������� �������� �������������, ������ ���
#define RestoreMode			0x37	//��������� � ����������� ������, ������ ���
#define WaitRestore			0x38	//������� � ����������� ������ ��������, ������ ���
#define GetCalibrPoint		0x39	//������ �������� �� ������������� �������,1 ���� � ����� ��������
#define WriteCalibrPoint	0x3B	//�������������� ������ �������� � ������������� �������, 4 �����. 1�-2� ���� � ����� ��������, 2-3 ����� � ���� ��������
#define CalibrPoint			0x3C	//����� �������� ������������� ������� 2 ���� � �������� �� ������������� ������� � ����������� �������

#define SetOffset_1			0x3d    // �������� ��������  ��������� -1 � �����
#define GetOffset_1        0x3e    // ��������� ��������  ��������� -1 � �����
#define QueryOffset_1			0x44	// ������ �������� �������� ��������� �� ������� ���� ��� ��� ��������� -1 � �����

#define SetFirstStageGain1  0x3f 	// ��������� �����.�������� ������� ������� �������� ���������-1 � �����
#define GetFirstStageGain1  0x40	// ��������� �����.�������� ������� ������� �������� ���������-1 � �����
#define QueryFirstGain1			0x46	// ������ �����.�������� ������� ������� �������� ��������� �� ������� ���� ��� ��� ���������-1 � �����

#define SetSecondStageGain1 0x41 	// ��������� �����.�������� ������� ������� �������� ���������-1 � �����
#define GetSecondStageGain1 0x42	// ��������� �����.�������� ������� ������� �������� ���������-1 � �����
#define QuerySecondGain1		0x47	// ������ �����.�������� ������� ������� �������� ��������� �� ������� ���� ��� ��� ���������-1 � �����

#define WriteAmplParam1		0x43	// ������ �� ����-������ ���������� �������� ���������-1 � �����

#define GetSensorTLM		0x48	// ������ ������ ���-�������� ����������� ������ � ����������� ����������
#define SensorTLM			0x49	// ������� �������� ���-�������� ����������� ������ � ����������� ����������

#define SetOffsetIN_1			0x4a    // �������� �������� ����� ���������-1 � �����
#define GetOffsetIN_1     0x4b    // ��������� �������� ����� ���������-1 � �����
#define QueryOffsetIN_1		0x4c	// ������ �������� ����� ��������� �� ������� ���� ��� ��� ���������-1 � �����

#define QueryMode		0x4d	// ������ �������� ������ �� ������� ���� ��� ��� ���������

#define SetMinCode		0x4e	// ������� �������� ������������ ���� �� ����� ���-������ ���������

#define LoadProgAdr		0x4f	// ���������� ����� ������ ��������� � ��� ��� ����� �� ����������
#define GetProgAdr		0x50	// �������� ����� ������ ��������� � ��� ��� ����� �� ����������
#define LoadStartAdr	0x51	// ���������� ��������� ����� ��������� � ���
#define GetStartAdr		0x52	// �������� ��������� ����� ��������� � ���
#define LoadProgData	0x53	// �������� ���������� ������ ����� ����� ��������
#define GetProgData		0x54	// ��������� ���������� ������ ����� ����� �������� ��� ��������
#define UpdateProgram	0x55	// ������� ���������� ������� ��������� �� �����������

#define BurnNextPortion 0x57	// прожиг очередной порции данных

#define LoadPortionOffset   0x6b 	// передача из ПЭВМ в контроллер текущего смещения очередной порции данных для прожига
#define GetPortionOffset	0x6c	// эхо-выдача из контроллера в ПЭВМ текущего смещения очередной порции данных для прожига
#define LoadPorionSize		0x6d	// передача из ПЭВМ в контроллер размера очередной порции данных для прожига
#define GetPortionSize		0x6e	// эхо-выдача из контроллера в ПЭВМ   размера очередной порции данных для прожига
#define StoreLoadTable		0x6f    // запоминание в контроллере таблицы загрузки программы. Обычно выполняется после загрузки всей программы в буферную память

#define SetOffsetIN_Prec1		0x58  // �������� �������� ����� ��������� 12p	-1 � �����
#define GetOffsetIN_Prec1   0x59  // ��������� �������� ����� ��������� 12p -1 � �����
#define QueryOffsetIN_Prec1	0x5a	// ������ �������� ����� ��������� 12p �� ������� ���� ��� ��� ��������� -1 � �����

#define SetOffsetIN_Prec2		0x5b  // �������� �������� ����� ��������� 12p -2 � �����
#define GetOffsetIN_Prec2   0x5c  // ��������� �������� ����� ��������� 12p-2 � �����
#define QueryOffsetIN_Prec2	0x5d	// ������ �������� ����� ��������� 12p �� ������� ���� ��� ��� ���������-2 � �����


#define SetFirstStageGain2  0x5e  // ��������� �����.�������� ������� ������� �������� ��������� -2 � �����
#define GetFirstStageGain2  0x5f	// ��������� �����.�������� ������� ������� �������� ��������� -2 � �����
#define QueryFirstGain2			0x60	// ������ �����.�������� ������� ������� �������� ��������� �� ������� ���� ��� ��� ��������� -2 � �����

#define SetSecondStageGain2 0x61  // ��������� �����.�������� ������� ������� �������� ��������� -2 � �����
#define GetSecondStageGain2 0x62	// ��������� �����.�������� ������� ������� �������� ��������� -2 � �����
#define QuerySecondGain2		0x63	// ������ �����.�������� ������� ������� �������� ��������� �� ������� ���� ��� ��� ��������� -2 � �����

#define WriteAmplParam2			0x64	// ������ �� ����-������ ���������� �������� ���������-2 � �����

#define SetOffset_2			0x65    // �������� ��������  ���������-2 � �����
#define GetOffset_2        0x66    // ��������� ��������  ���������-2 � �����
#define QueryOffset_2			0x67	// ������ �������� �������� ��������� �� ������� ���� ��� ��� ���������-2 � �����

#define SetOffsetIN_2			0x68    // �������� �������� ����� ���������-2 � �����
#define GetOffsetIN_2     0x69    // ��������� �������� ����� ���������-2 � �����
#define QueryOffsetIN_2		0x6a	// ������ �������� ����� ��������� �� ������� ���� ��� ��� ���������-2 � �����

// ����������� ��������� ������ ��� �������������� �������� �������

#define GetSensorData		GetSensorData_1	//������  ������� ������������� ������, ������ ���
#define SensorData			SensorData_1	//������� ������������� ������ 2 �����
#define GetFiltData			GetSensorData_2	//������ ������� ��������������� ������, ������ ���
#define FiltData			SensorData_2	//������� ��������������� ������ 2 �����

#define SetOffset					SetOffset_1			  // �������� ��������  ���������
#define GetOffset					GetOffset_1       // ��������� ��������  ���������
#define QueryOffset				QueryOffset_1			// ������ �������� �������� ��������� �� ������� ���� ��� ��� ���������

#define SetOffsetIN				SetOffsetIN_1    // �������� �������� ����� ���������
#define GetOffsetIN				GetOffsetIN_1    // ��������� �������� ����� ���������
#define QueryOffsetIN			QueryOffsetIN_1 	// ������ �������� ����� ��������� �� ������� ���� ��� ��� ���������

#define SetFirstStageGain   SetFirstStageGain1    // ��������� �����.�������� ������� ������� �������� ���������
#define GetFirstStageGain   GetFirstStageGain1	// ��������� �����.�������� ������� ������� �������� ���������
#define QueryFirstGain			QueryFirstGain1	// ������ �����.�������� ������� ������� �������� ��������� �� ������� ���� ��� ��� ���������

#define SetSecondStageGain  SetSecondStageGain1    // ��������� �����.�������� ������� ������� �������� ���������
#define GetSecondStageGain  GetSecondStageGain1	// ��������� �����.�������� ������� ������� �������� ���������
#define QuerySecondGain			QuerySecondGain1	// ������ �����.�������� ������� ������� �������� ��������� �� ������� ���� ��� ��� ���������

#define SetOffsetIN_Prec		SetOffsetIN_Prec1   // �������� �������� ����� ��������� 12p
#define GetOffsetIN_Prec   	GetOffsetIN_Prec1    // ��������� �������� ����� ��������� 12p
#define QueryOffsetIN_Prec	QueryOffsetIN_Prec1	// ������ �������� ����� ��������� 12p �� ������� ���� ��� ��� ���������

#define WriteAmplParam			WriteAmplParam1

// ���� ������ -����� ������

#define ErrorBit	1 // 0 - ���������� ������, 1- ������ ���������� �����������
#define StreamBit 2 // 0 - �������� �������, 1 - ��������� ������.
#define CalibrBit 4 // 1 - ����� ����������
#define CommunicationError 8 // ������ �����
#define RecodeBit	0x10 // ������� ������������� ��������������-������������� ������
#define WDT_Restart_Bit 0x80 // restart wdt bit

#define No_Filter	0	// �� ������������ ������, // ����� ���� NoFilter - �������� ��-�� ��������� � Qt ������������
#define MedFilter	1	// ��������� ������
#define AverFilter 2	// ����������� ������
#define PulseFilter 3	// ��������� ������


// ���� ������������
#define DeviceTypeError		0
#define DeviceTypeSensor	1 
#define DeviceTypeInterface	2
#define DeviceTypePc		3

//������ ������ 
#define HARD_VERSION		3


#define SyncroByte		0x7e
#define MaxDataSize		255
#define MaxCommandsCount	255


#pragma pack(1)
typedef struct 
{
	char mode; // ������� ����� ������� �������;
	char Filter;
	char depthl;
} MODE;


typedef struct 
{
	unsigned short echoVolt;//  ���-�������� ����������� ������
	unsigned short ZeroVolt;// ���������� ���� ��� ����������
	unsigned short AnalogVolt;// ���������� ���������� �������
	unsigned short temper;	// ����������� ����������
} TLM;

typedef struct 
{
	MODE SMode;
	unsigned char Offset; // ���������� ���� ��� ���������� -����� ���������
	unsigned char OffsetIN; // ���������� ���� ��� ���������� - ���� ���������
	unsigned char Gain1;  // �������� 1-��� �������
	unsigned char Gain2;	// �������� 2 --��� �������
	unsigned short AdcCodeMin; // ����������� ��� �� ������ ��������� - ����� ��� ��� ������ ����������� ������� � �������
	unsigned char OffsetInLow;
	unsigned char ConSum;
} ControllerSettings;

typedef struct
{
	unsigned short NUM;
	unsigned short VAL;
} Calibr_Point;

#pragma pack()
#endif
