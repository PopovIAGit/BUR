#ifndef _SERIAL_COMMUNICATION_H_
#define _SERIAL_COMMUNICATION_H_

#include "btwt41.h"

#define BT_SCI			SCIA
#define BT_SCI_BAUD		1152

#define PI_ID			SPIC
#define PI_CONN_TOUT	(3.000 * PRD_2KHZ)
#define PI_BAUD			1000		// ������� ��������������� � ��
#if BUR_M	
	#define PI_FRAME_SIZE	5
#else
	#define PI_FRAME_SIZE	7
#endif

typedef struct
{
	Byte DiscrIn220;						// ���������� ���� 220
	Byte DiscrOut;						// ���������� �����
	Byte State;							// ��������� ������
	Bool Connect;						// ��������� �����
	Uns  ConnTimer;						// ������ �����
	Uns  ConnErr;						// ������ ����
	Byte Counter;						// ������� ������
	Byte RxFrame[PI_FRAME_SIZE];		// �����
	Byte TxFrame[PI_FRAME_SIZE];		// �����
	Byte DiscrOut2;						// ���������� ������ ������ 8 ���
	Byte DiscrIn24;						// ���������� ���� 24
} TPiData;



#define BTWT41_DEFAULT { \
	False, 0, 0, 0, BT_COMMAND_MODE, \
	0, 0, (1.00 * PRD_10HZ), False, False, "", "", \
	BtHardwareSetup, CheckSciCommErr, \
	EnableBtRx, EnableBtTx, ReceiveBtByte, \
	TransmitBtByte \
}

extern Bool CsAtBusy;
extern TPiData PiData;
extern TBt Bluetooth;

void SerialCommunicationInit(void);
void PiUpdate(void);

void BtHardwareSetup(Bool Enable);
Bool CheckSciCommErr(void);
void EnableBtRx(void);
void EnableBtTx(void);
Byte ReceiveBtByte(void);
void TransmitBtByte(Byte Data);


#endif

