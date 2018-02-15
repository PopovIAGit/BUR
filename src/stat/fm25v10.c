#include "fm25v10.h"
#include "csl\csl_spi.h"
#include "csl\csl_memory.h"


// ���� �������
#define WRSR		0x01
#define WRITE		0x02
#define READ		0x03
#define WRDI		0x04
#define RDSR		0x05
#define WREN 		0x06

// ������� ������ ���������
#define SetCs()  p->CsFunc(0)
#define ClrCs()  p->CsFunc(1)
// ����� ��������� ���������� ��������. �������� ���, ������� ��������� ������
#define STATUS_MASK				0xBF
// ����� ��� ����� ������� ���
#define ADDRESS_HIGH_MASK		0x10000


void FM25V10_Init(TFM25V10 *p)
{
	p->BusyTimer = 0;
	SPI_init(p->SpiId, SPI_MASTER, 0, p->SpiBaud, 8);
	if (FM25V10_ReadStatus(p))
		p->Error = TRUE;				// ��������� ������� ������ ���� ������
	asm(" NOP");
}

void FM25V10_Update(TFM25V10 *p)
{
	register Uns Tmp;

	if (!p->Func) {p->State = 0; return;}

	switch (++p->State)
	{
		case 1:
			if (p->Func & F_READ)    p->State = 2;						// ��������� �� 1 ������, �.�. ������ ������ switch'�, ��������� ����������������
			else if (p->Func & F_WRITE)   p->State = 3;
			else {p->Func = 0; p->State = 0;}								// ���� ������� ���, �� �������� State
			break;
//-----------------------------------------------------------------
		case 3:															// ������ �����
			SPI_init(p->SpiId, SPI_MASTER, 0, p->SpiBaud, 8);
			Tmp = FM25V10_ReadByte(p, p->Addr);							// ��������� ���� �� ���������� ������ �� ��������� ����������

			if (!(p->Func & F_WRITE))									// ������� ������ �����������, ������ ������ ����
			{
				if (!(p->Addr & 0x1)) p->RdData = Tmp;					// ������� ����� ������, ������ ������� ����. ���������� � RdData
				else
				{
					p->RdData = (Tmp << 8) | p->RdData;					// ����� ��������. ���� �������. ��������� ������� � ������� ���� � RdData. ���� �� ����� ��������, �� ����� ����� ������� ���������� � Uns
					*((Uns *)p->Buffer) = p->RdData;					// �������� ��� ��������� � Uns � ���������� ������ �� RdData � ������� ������ ������
				}
			}
			else if (Tmp != p->WrData)									// ���� ������� ������ � ��������� �������� �� ������, �� �������� ������� ��������� ������ �� ��������� � ������� ��� ������
			{
				if (++p->RetryTimer < p->RetryCount) p->Func &= ~F_READ;	// �������������� ������� ������� � ���������, ���� �� ��� �������. ���� ����, �� �������� ������� ������,
																			// ����� ����� �� ������� ��������� ���������� ���������� � ��������� ���� ������
				else
				{
					p->Error = TRUE; p->Func = 0;								// ���� ������� �� ���������� ���������, �� ���������� ������
				}
				p->State = 0;
				break;														// ������� �� case
			}
			else {p->RetryTimer = 0; p->Func &= ~F_READ;}					// � ��������� ������� �������� ������� ������� � �������� ������� ������

			if (p->Count > 1)												// ���� ��� ����� ��� ������
			{
				if (p->Buffer && (p->Addr & 0x1))	p->Buffer++;			// ���������, ��� ����� �� ������ � ��� ����� �������� (������� ����������� ������� ����, ����� �������)
				p->Addr++;
				p->Count--;
			}
			else
			{
				p->RetryTimer = 0;
				p->Count = 0;
				p->Func = 0;
			}

			p->State = 0;
			break;
//-------------------------------------------------------------------------
		case 4:
			if (!p->Buffer) Tmp = 0;					// ����� ����
			else Tmp = *((Uns *)p->Buffer);				// ���������� ������ �� ������ �� ��������� ����������

			if (!(p->Addr & 0x1)) 						// ���� 1 - �� ������� ����, ���� 0, �� �������.
				 p->WrData = (Byte)(Tmp & 0xFF);		// ���� ������� ���� �����. ������ - 0011 0000 0000 ���� �� ������.
														// ������������ �� �����, ��� ��������� ������� ��� � �������� ��� � 8-�������
			else p->WrData = (Byte)(Tmp >> 8);			// �������� ������� ����� � �������� � 8-������� ����

			SPI_init(p->SpiId, SPI_MASTER, 0, p->SpiBaud, 8);			// �������������� SPI
			FM25V10_WriteEnable(p);										// �������� ������ �� ���������� ������
			break;
		case 5:
			SPI_init(p->SpiId, SPI_MASTER, 0, p->SpiBaud, 8);			// �������������� SPI
			FM25V10_WriteByte(p, p->Addr, p->WrData);					// ����� ����
			p->Func |= (F_READ);										// ������� �� ������ ������, ������ ���������� ��������� ������������ ���������� ������
																		// �� ���������� ��������, � ��� �� ������� ���������� ������ ��� ��������
			p->BusyTimer = 0;											// ���������� ������, �.�. ����� ������ ��������� �������� ����������
			p->State = 0;
			break;
	}
}


Byte FM25V10_ReadStatus(TFM25V10 *p)
{
	Byte Status;

	SetCs();
	SPI_send(p->SpiId, RDSR);								// �������� ��� ������� �� ������ �������
	Status = SPI_send(p->SpiId, 0);							// ��������� ���� ���������� ��������
	ClrCs();

	return Status & STATUS_MASK;
}

void FM25V10_WriteStatus(TFM25V10 *p, Byte Status)
{
	SetCs();

	SPI_send(p->SpiId, WRSR);								// �������� ��� ������� �� ������ � ��������� �������
	SPI_send(p->SpiId, Status);								// �������� ������ ��� ������ � ��������� �������

	ClrCs();
}

Byte FM25V10_ReadByte(TFM25V10 *p, unsigned long Addr)
{
	Byte Data;

	SetCs();

	SPI_send(p->SpiId, READ);								// �������� ��� ������� �� ������ �����
	SPI_send(p->SpiId, (Addr & ADDRESS_HIGH_MASK) >> 16);	// Addr >> 16
	SPI_send(p->SpiId, (Addr >> 8) & 0xFF);					// �������� ������ ������� ���� ������
	SPI_send(p->SpiId, Addr & 0xFF);						// ����� ������� ���� ������
	Data = SPI_send(p->SpiId, 0);

	ClrCs();

	return Data;											// ���������� ���������� ���� ������
}

void FM25V10_WriteByte(TFM25V10 *p, unsigned long Addr, Byte Data)
{
	SetCs();


	SPI_send(p->SpiId, WRITE);								// �������� ��� ������� �� ������
	SPI_send(p->SpiId, (Addr & ADDRESS_HIGH_MASK) >> 16);	// Addr >> 16
	SPI_send(p->SpiId, Addr >> 8);							// �������� ������ ������� ���� ������
	SPI_send(p->SpiId, Addr);								// ����� ������� ���� ������
	SPI_send(p->SpiId, Data);								// �������� ���� �� ������

	ClrCs();
}

void FM25V10_WriteEnable(TFM25V10 *p)
{
	SetCs();

	SPI_send(p->SpiId, WREN);								// �������� ��� ������� �� ��������� ������

	ClrCs();
}

void FM25V10_WriteDisable(TFM25V10 *p)
{
	SetCs();
	SPI_send(p->SpiId, WRDI);								// �������� ��� ������� �� ���������� ������
	ClrCs();
}


