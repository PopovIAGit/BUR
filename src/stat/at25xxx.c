#include "at25xxx.h"
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

Uns MemDbg = 0;

void AT25XXX_Init(AT25XXX *p)
{
	p->BusyTimer = 0;
	SPI_init(p->SpiId, SPI_MASTER, 0, p->SpiBaud, 8);
	if (AT25XXX_ReadStatus(p)) p->Error = TRUE;				// ��������� ������� ������ ���� ������
	asm(" NOP");
}

void AT25XXX_Update(AT25XXX *p)
{
	register Uns Tmp;

	if (!p->Func) {p->State = 0; return;}
	
	switch (++p->State)
	{
		case 1:
			if (p->Func & F_GETSTATUS)    p->State = 1;						// ��������� ����� ������� ������������ � ��������� State
			else if (p->Func & F_READ)    p->State = 2;						// ��������� �� 1 ������, �.�. ������ ������ switch'�, ��������� ����������������
			else if (p->Func & F_WRITE)   p->State = 3;
			else {p->Func = 0; p->State = 0;}								// ���� ������� ���, �� �������� State
			break;
//-----------------------------------------------------------------
		case 2:																// �������� �������
			SPI_init(p->SpiId, SPI_MASTER, 0, p->SpiBaud, 8);
			Tmp = AT25XXX_ReadStatus(p);									// ������ ������
			
			if (!(p->Func & F_WRITE)) {p->RdData = Tmp; p->Func = 0;}		// ����������� ������� ������, ���������� ���� ������� � RdData, �������� �������
			else if (!(Tmp & 0x01)) p->Func &= ~F_GETSTATUS;				// ��������� ��� RDY � ��������� ��������, ���� �����������, �� ���������� ������ ��� ������, �������� ������� ������ �������
			else if (++p->BusyTimer < p->BusyTime) {p->State = 1; break;}	// ���� ������� ������ � ��� RDY ��������� (���������� ��� ������), �� ��������� ������ ��������� ����������																			
			else {p->Error = TRUE; p->Func = 0;}							// � ��������� ������� - ������
			
			MemDbg++;
			p->State = 0;
			break;
//-----------------------------------------------------------------
		case 3:															// ������ �����
			SPI_init(p->SpiId, SPI_MASTER, 0, p->SpiBaud, 8);
			Tmp = AT25XXX_ReadByte(p, p->Addr);							// ��������� ���� �� ���������� ������ �� ��������� ����������

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
				else {p->Error = TRUE; p->Func = 0;}						// ���� ������� �� ���������� ���������, �� ���������� ������
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
			AT25XXX_WriteEnable(p);										// �������� ������ �� ���������� ������
			break;
		case 5:
			SPI_init(p->SpiId, SPI_MASTER, 0, p->SpiBaud, 8);			// �������������� SPI
			AT25XXX_WriteByte(p, p->Addr, p->WrData);					// ����� ����
			p->Func |= (F_GETSTATUS|F_READ);							// ������� �� ������ ������, ������ ���������� ��������� ���������� ��������
																		// �� ���������� ��������, � ��� �� ������� ���������� ������ ��� ��������
			p->BusyTimer = 0;											// ���������� ������, �.�. ����� ������ ��������� �������� ����������
			p->State = 0;
			break;

	}


}



Byte AT25XXX_ReadStatus(AT25XXX *p)
{
	Byte Status;
	
	SetCs();
	SPI_send(p->SpiId, RDSR);								// �������� ��� ������� �� ������ �������
	Status = SPI_send(p->SpiId, 0);							// ��������� ���� ���������� ��������
	ClrCs();
	
	return Status;
}

void AT25XXX_WriteStatus(AT25XXX *p, Byte Status)
{
	SetCs();
	SPI_send(p->SpiId, WRSR);								// �������� ��� ������� �� ������ � ��������� �������
	SPI_send(p->SpiId, Status);								// �������� ������ ��� ������ � ��������� �������
	ClrCs();
}

void AT25XXX_WaitBusy(AT25XXX *p)
{
	while(AT25XXX_ReadStatus(p) & AT25XXX_RDY);				// �������� � ����� �� ��� ��� ���� �� ������ ������ 0
}

Byte AT25XXX_ReadByte(AT25XXX *p, Uns Addr)
{
	Byte Data;
	
	SetCs();
	SPI_send(p->SpiId, READ);								// �������� ��� ������� �� ������ �����
	SPI_send(p->SpiId, Addr >> 8);							// �������� ������ ������� ���� ������
	SPI_send(p->SpiId, Addr);						// ����� ������� ���� ������
	Data = SPI_send(p->SpiId, 0);
	ClrCs();

	return Data;											// ���������� ���������� ���� ������
}

void AT25XXX_WriteByte(AT25XXX *p, Uns Addr, Byte Data)
{
	SetCs();
	DelayUs(250);
	SPI_send(p->SpiId, WRITE);								// �������� ��� ������� �� ������
	SPI_send(p->SpiId, Addr >> 8);							// �������� ������ ������� ���� ������
	SPI_send(p->SpiId, Addr);								// ����� ������� ���� ������
	SPI_send(p->SpiId, Data);								// �������� ���� �� ������
	ClrCs();
}

void AT25XXX_WriteEnable(AT25XXX *p)
{
	SetCs();
	SPI_send(p->SpiId, WREN);								// �������� ��� ������� �� ��������� ������
	ClrCs();
}

void AT25XXX_WriteDisable(AT25XXX *p)
{
	SetCs();
	SPI_send(p->SpiId, WRDI);								// �������� ��� ������� �� ���������� ������
	ClrCs();
}

