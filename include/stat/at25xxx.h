#ifndef _AT25XXX_H_
#define _AT25XXX_H_

#include "std.h"

// ���� ���������� ��������
#define AT25XXX_RDY    0x01
#define AT25XXX_WEN    0x02
#define AT25XXX_BP0    0x04
#define AT25XXX_BP1    0x08
#define AT25XXX_WPEN   0x80


typedef struct AT25XXX {
	Byte  SpiId;					// ������������� SPI ������ (SPIA, SPIB, ...)
	Uns   SpiBaud;					// ������� ���������������
	Uns   Addr;						// �����
	Byte  Func;						// �������
	Uns   Count;					// ���������� ���� ������
	Byte *Buffer;					// ��������� �� ����� ������
	Byte  RdData;					// ������� ��������� ����
	Byte  WrData;					// ������� ���������� ����
	Byte  State;					// ������� ��� ����������
	Bool  Error;					// ���� ������ � ������ ������
	Byte  RetryTimer;				// ������ �������� ����������
	Byte  RetryCount;				// ���������� �������� ����������
	Uns   BusyTimer;				// ������ ��������� ������
	Uns   BusyTime;					// ������������ ����� ������
	void (*CsFunc)(Byte);			// ������� chip select'�
} AT25XXX;

void AT25XXX_Init(AT25XXX *);							// �������������
void AT25XXX_Update(AT25XXX *);							// 
Byte AT25XXX_ReadStatus(AT25XXX *);						// ������ ���������� ��������
void AT25XXX_WriteStatus(AT25XXX *, Byte Status);		// ������ ���������� ��������
void AT25XXX_WaitBusy(AT25XXX *);						// �������� ���������� ������
Byte AT25XXX_ReadByte(AT25XXX *, Uns Addr);				// ������ �����
void AT25XXX_WriteByte(AT25XXX *, Uns Addr, Byte Data);	// ������ �����
void AT25XXX_WriteEnable(AT25XXX *);					// ���������� ������
void AT25XXX_WriteDisable(AT25XXX *);					// ���������� ������


#endif
