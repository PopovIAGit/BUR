#ifndef _SERIAL_COMMUNICATION_H_
#define _SERIAL_COMMUNICATION_H_

#include "btwt41.h"

#define BT_SCI			SCIA
#define BT_SCI_BAUD		1152

#define PI_ID			SPIC
#define PI_CONN_TOUT	(3.000 * PRD_2KHZ)
#define PI_BAUD			1000		// Частота синхроимпульсов в Гц
#if BUR_M	
	#define PI_FRAME_SIZE	5
#else
	#define PI_FRAME_SIZE	7
#endif

typedef struct
{
	Byte DiscrIn220;						// Дискретный вход 220
	Byte DiscrOut;						// Дискретный выход
	Byte State;							// Состояние обмена
	Bool Connect;						// Состояние связи
	Uns  ConnTimer;						// Таймер связи
	Uns  ConnErr;						// Ошибка свзи
	Byte Counter;						// Счетчик данных
	Byte RxFrame[PI_FRAME_SIZE];		// Буфер
	Byte TxFrame[PI_FRAME_SIZE];		// Буфер
	Byte DiscrOut2;						// Дискретные выхода вторые 8 бит
	Byte DiscrIn24;						// Дискретный вход 24
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

