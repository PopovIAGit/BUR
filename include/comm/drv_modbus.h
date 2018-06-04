
#ifndef DRV_MODBUS_
#define DRV_MODBUS_

#include "responses.h"

#define MB_DATA_MAX			50
#define MB_FRAME_MAX		(MB_DATA_MAX * 2 + 9)

#define MB_READ_REGS		0x03
#define MB_DIAGNOSTICS		0x08
#define MB_WRITE_REGS		0x10
#define MB_WRITE_REG		0x6
#define MB_REP_SLAVE_ID		0x11

#define PLIS_SPI			SPIA
#define PLIS_BRR			SPI_BRR(4000)

typedef void (*TTrEnable)(Uint16);

// Driver parameters
typedef struct {
	Uint16 UartID;
	Uint16 Slave;
	Uint16 BaudRate;
	Uint16 Parity;
	Uint16 Timeout1_5;
	Uint16 Timeout3_5;
	Uint16 TimeoutPre; //may habara
	Uint16 TimeoutPost; //may habara
	Uint16 TimeoutAck;
	Uint16 TimeoutTestDelay; //may habara
	TTrEnable TrEnable;
} TMbParams;

// Driver frame
typedef struct {
	Uint16 ConnFlg;  // флаг используетс€ только дл€ индикации св€зи
	Uint16 Exception;
	Uint16 NewMsg;
	Uint16 Flg3_5;
	Uint16 Acknoledge;
	Uint16 Timer1_5;
	Uint16 Timer3_5;
	Uint16 TimerPre;  //may habara
	Uint16 TimerPost; //may habara
	Uint16 TimerConn;
	Uint16 TimerAck;
	Uint16 TimerTestDelay; //may habara
	Uint16 Counter;
	Uint16 Size;
	Uint16 Crc;
	Uint16 CrcIndex;
	Uint16 Buf[MB_FRAME_MAX];
} TMbFrame;

// Driver statistic
typedef struct {
	Uint16 RxBytesCount;
	Uint16 TxBytesCount;
} TMbStat;

// Drive object
typedef struct {
	TMbParams Params;
	TMbFrame  Frame;
	TMbStat   Stat;
} TMbPort;

extern TMbPort Mb;
extern Bool MbConnect;

void SerialCommInit(void);
void SerialCommUpdate(TMbPort *);
void SerialCommRxHandler(TMbPort *);
void SerialCommTxHandler(TMbPort *);
void SerialCommTimings(TMbPort *);
void SerialCommRefresh(void);
void ModBusControl(void);

#endif
