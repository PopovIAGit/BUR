

#include "csl\csl_spi.h"
#include "csl\csl_memory.h"
#include "peref_drv_DPMA15.h"
//#include "timings.h"
#include "config.h"
//#include "chip\DSP280x_Spi.h"

#define SetCs()	p->CsFunc(0)
#define ClrCs()	p->CsFunc(1)

void encoderDPMA15_Init(EN_DPMA15 *p)
{
	p->SpiId = PLIS_SPI;              // ������������� SPI-����� (0-SPIA, 1-SPIB, ...)
	p->SpiBaud = SPI_BRR(1000);  // ������� ��������������� (����������� ��� ������������ ����)
	p->bitCount = 0;        		  // ���������� ����/��� ������
	p->RevMax = RevMax;			      // ����������� �������� ���������
	p->revolution = 0;              // �������� ��������� � ������� � ������� 16 ��������
	p->EncoderData = 0;		      // ������, ���������� �� ��������
	p->prevRevolution = 0;	      // ���������� �������� ��������
	p->RevMisc = 5;			      // ����������� �����/�������� ������������ �������
	p->RevErrCount = 0;		      // ������� ������
	p->RevTimer = 0;		          // ������� ��� ������� ������
	p->ResetCounter = 0;	          // ������� ������ ��������
	p->ResetDelay = 0;		      // ���� �������� ������ ��������
	p->Error = 0;                   // ������� ����
	p->TmpError = 0;		          // ������� ���������� ����, ������� ������������ �������������
	p->CsFunc(0);                 // ������� ������ ����������

	p->RevErrValue = &GrC->RevErrValue;
	p->RevErrLevel = &GrC->RevErrLevel;

}

// ������� ���������� ������ � ������� ��������� ����-15
void encoder_DPMA15_GetData(EN_DPMA15 *p) // 50 Hz
{
	Uns Delta, Data, DataWithMask;				// �������� ���������, �������� ������, ������������� ������.
	Uns ConnFlag;

	SPI_init(p->SpiId, SPI_MASTER, 0, p->SpiBaud, 8);

	SetCs();

	Data = SPI_send(p->SpiId, 0x00) << 8;	// ����������� �������: ���� ������ ������� ���
	Data |= SPI_send(p->SpiId, 0x00);
	ClrCs();

	DataWithMask = Data & 0x7fff;
	ConnFlag = Data >> 15;

	if (ConnFlag == 0)
	{
		p->Error = True;
	}
	else
	{
		p->Error = false;
	}

	Delta = abs(DataWithMask - p->EncoderData);

	if ((p->RevMisc <= Delta)\
			&& (Delta <= p->RevMax - p->RevMisc))			// 3) ���� 5 < Delta < 16383-5 - "������ ������")
	{
		p->RevErrCount++;
	}
	else
	{
		p->revolution = DataWithMask;			    // 4) ���������� ������� ������ ��������
	}

	p->EncoderData = DataWithMask;
}








