#ifndef _IM_H_
#define _IM_H_

#include "std.h"

#define IM_LOGEV_TYPE			1
#define IM_LOGCMD_TYPE			2
#define IM_LOGPARAMS_TYPE		3

#define IM_RD_BUFFER_SIZE			10
#define IM_WR_BUFFER_SIZE			170
#define IM_DOWNLOAD_BUFFER_SIZE		IM_READ_BUF_SIZE

#define RECEIVE_FUNC_MODE			0
#define DOWNLOAD_FUNC_MODE			1
#define SEND_DATA_MODE				2

#define IM_MAX_FUNC_INDEX_VALUE		4

typedef enum 
{
	imIdle = 0,
	imInit = 1,
	imDeviceInfo = 2,
	imSumLogsInfo = 3,
	imEvLogInfo = 4,
	imCmdLogInfo = 5,
	imParamLogInfo = 6,
	imEvLogMainAddr = 7,
	imEvLogBufAddr = 8,
	imCmdLogAddr = 9,
	imParLogAddr = 10,
	imLogDownload = 11,
	imParamDownload = 12
} TIMFuncState;

typedef struct _TIM
{
	Byte ControlMode;

	Byte FuncState;
	Byte Index;
	Uns  TxIndex;
	Bool CanSendData;
	Bool *IsTxBusy;
	Uns Timer;
	Uns Period;

	Bool CanReadNextRec;
	Bool IsReadRecBusy;
	Bool IsBufReady;
	Bool IsLogTransmit;

	Uns *DeviceDataPtr;
	Uns *DownloadBuffer;
		
	void (*EnableReceive)(void);
	void (*EnableTransmit)(void);
	void (*TransmitByte)(Byte Data);

	Byte RdBuffer[IM_RD_BUFFER_SIZE];
	Byte WrBuffer[IM_WR_BUFFER_SIZE];

} TIM;

// Внешная функция приема байт
void ImReceiveData(TIM *p, Byte Data);
// Основная фунция работы с информационным модулем
void ImUpdate(TIM *p);
// Сброс информационного модуля
void ImReset(TIM *p);
// Внешняя функция чтения журналов в буфер
void ImBufferReader(Byte LogType, Uns RecordNum);
// Внешняя функция таймера
void ImTimer(TIM *p);
#endif
