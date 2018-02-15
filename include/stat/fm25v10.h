#ifndef _FM25V10_H_
#define _FM25V10_H_

#include "std.h"

// ���� ���������� ��������
#define FM25V10_WEL    0x02
#define FM25V10_BP0    0x04
#define FM25V10_BP1    0x08
#define FM25V10_WPEN   0x80

#define WAIT_FOR_EEPROM_READY() while (!IsMemParReady())	{FM25V10_Update(&Eeprom1);

typedef struct {
	Byte  SpiId;					// ������������� SPI ������ (SPIA, SPIB, ...)
	Uns   SpiBaud;					// ������� ���������������
	unsigned long   Addr;			// �����
	Byte  Func;						// �������
	Uns   Count;					// ���������� ���� ������
	Uns  *Buffer;					// ��������� �� ����� ������ Byte ������� �� Uns
	Byte  RdData;					// ������� ��������� ����
	Byte  WrData;					// ������� ���������� ����
	Byte  State;					// ������� ��� ����������
	Bool  Error;					// ���� ������ � ������ ������
	Byte  RetryTimer;				// ������ �������� ����������
	Byte  RetryCount;				// ���������� �������� ����������
	Uns   BusyTimer;				// ������ ��������� ������
	Uns   BusyTime;					// ������������ ����� ������
	void (*CsFunc)(Byte);			// ������� chip select'�
} TFM25V10, *pFM25V10;

void FM25V10_Init(pFM25V10);							// �������������
void FM25V10_Update(pFM25V10);							//
Byte FM25V10_ReadStatus(pFM25V10);						// ������ ���������� ��������
void FM25V10_WriteStatus(pFM25V10, Byte Status);		// ������ ���������� ��������
Byte FM25V10_ReadByte(pFM25V10, unsigned long Addr);				// ������ �����
void FM25V10_WriteByte(pFM25V10, unsigned long Addr, Byte Data);	// ������ �����
void FM25V10_WriteEnable(pFM25V10);					// ���������� ������
void FM25V10_WriteDisable(pFM25V10);					// ���������� ������

#endif
