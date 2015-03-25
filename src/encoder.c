#include "csl\csl_spi.h"
#include "csl\csl_utils.h"
#include "encoder.h"
#include "timings.h"

#define SetCs()	p->CsFunc(0)
#define ClrCs()	p->CsFunc(1)



void AtMegaAvagoEncoderCalc(ENCODER *p)
{
	static Uns errorCount = 0;		// ���������� "���������" �������
	static Uns packetCount = 0;		// ����� ���������� �������
	static Uns alarmTimer = 0;		// ������ �������� �� ������������ ������ (1 �������)
	Uns  Data, Delta;

	SPI_init(p->SpiId, SPI_MASTER, 0, p->SpiBaud, 8);
	SetCs(); 												// ������ ������ � ��������
	Data  = SPI_send(p->SpiId, 0x00) << 8;
	DelayUs(ENC_ATMEGA_SPI_DELAY_US);
	Data |= SPI_send(p->SpiId, 0x00);
	ClrCs();

	Delta = abs(Data - (Uns)p->RevData);

	if ((Data >> p->Count) != 0)
	{
		Delta = (Uns)p->RevMisc + 1;
	}

	if ((Delta < (Uns)p->RevMisc) ||					\
	((Delta >= ((Uns)p->RevMax - (Uns)p->RevMisc)) && 	\
	(Delta <= (Uns)p->RevMax)))
	{
		p->Revolution  = Data & (Uns)p->RevMax;
		p->RevData     = Data;
		p->RevErrCount = 0;

		p->TmpError = false;
	}
	else
	{
		//p->Revolution  = Data;
		p->RevData     = Data;
		errorCount++;		// ����������� ������� ������
	}

	if (packetCount++ > 200) // (200 ��) * (1 ���)
	{
		*p->RevErrValue = errorCount * 100 / packetCount;
		errorCount = 0;
		packetCount = 0;
	}

	if (*p->RevErrValue > *p->RevErrLevel)
	{ 
		if (alarmTimer++ > 200)
			p->Error = true; 
	}
	else
		alarmTimer = 0;
}

