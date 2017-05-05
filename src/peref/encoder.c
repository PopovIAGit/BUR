#include "csl\csl_spi.h"
#include "csl\csl_utils.h"
#include "encoder.h"
#include "timings.h"
#include "config.h"

#define SetCs()	p->CsFunc(0)
#define ClrCs()	p->CsFunc(1)

Uns initTimer = 100;	// ������ ������������� ��������, 0.5 ������. ���� �� �� ����� 0,
						//��������� � core �� ���������� � ������ �� ���� �������� �� ��������
LgUns Revolution = 0;

// ������� ��������� ��������
void EncoderUpdate(void)			// 200 Hz
{
	if (GrC->EncoderType == 0)		// encoder Avago
	{
		RevMax = 0x3FFF;
		Calib.RevMax = RevMax;
		Encoder.RevMax = RevMax;

		if (!GrC->EncoderCalcMethod)	// ���� ������ ����� �������� ������ �������� � 1 (������)
		{
			AtMegaAvagoEncoderUpdate(&Encoder);
		}
		else							// ���� ������ ������������ ����� �������� ������ ��������
		{
			AtMegaAvagoEncoderCalc(&Encoder);
		}

		Revolution = Encoder.Revolution;
	}
	else if (GrC->EncoderType == 1)  // Encoder SKB IS
	{
		RevMax = 0x7FFF;
		Calib.RevMax = RevMax;
		Encoder.RevMax = RevMax;

		encoder_DPMA15_GetData(&enDPMA15);
		Revolution = enDPMA15.revolution;
	}
}

// ������� ��������� ��������.
// ������� ������ �� ���� �������� - ����� �����
void AtMegaAvagoEncoderUpdate(ENCODER *p)	// 200 ��
{
	Int Delta;
	Uns absDelta;
	Uns  Data;

	// ������ ������ � ��������
	SPI_init(p->SpiId, SPI_MASTER, 0, p->SpiBaud, 8);
	SetCs(); 												// ������ ������ � ��������
	DelayUs(ENC_ATMEGA_SPI_DELAY_US);
	Data  = SPI_send(p->SpiId, 0x00) << 8;

	Data |= SPI_send(p->SpiId, 0x00);
	ClrCs();

	// �������������
	if (initTimer)
	{
		initTimer--;
		p->RevData = Data;
		return;
	}

	// �������� �� ���� ������
	if (Data > 0x3fff)			// 1) ���� ������, ��������� � �������� ��������� ������������ ��������,
	{
		p->errorCounter++;			// �� �������������� ������� ������, � �������� ������ �� ������������
		return;
	}
	// ������� ����� ������� � ����������
	Delta = Data - (Uns)p->RevData;
	absDelta = abs(Delta);

	if (p->skipDefectFl)								// 2a) ����� ���� � ���, ��� ������ �����������
	{
		if (absDelta)										// ���� ��������� ���������
		{
			 if (absDelta < p->RevMisc)
			 {
				p->goodPosition = (p->goodPosition + Delta)&0x3fff;	// ������������ goodPosition
				p->Revolution  = p->goodPosition;
			 }
			 else if (p->RevMax - p->RevMisc < absDelta)		// ������� ����� 0
			 {
				 p->goodPosition = (p->goodPosition + absDelta)&0x3fff;
				 p->Revolution  = p->goodPosition;
			 }
			 else
			{
				p->errorCounter++;							// ���� ������ ������ ������ �� �� � ����, ����������� ������� ������
			}

			if (( (Int)(p->goodPosition - p->RevMisc - 2) < (Int)Data)\
			  &&(Data < p->goodPosition + p->RevMisc + 2))	// ���� ��������� ������ ������� � goodposition - ���������� ����
			{
				p->skipDefectFl = false;
			}
		}
	}
	else 													// 2�) ���������� ��������� (��������� �� ����)
	{
		if ((p->RevMisc <= absDelta)\
	       &&(absDelta <= p->RevMax - p->RevMisc))			// 3) ���� 5 < Delta < 16383-5 - "������ ������")
		{
			p->skipDefectFl = true;
			p->goodPosition = p->RevData;
			p->errorCounter++;
		}
		else
		{
			p->Revolution  = Data;						// 4) ���������� ������� ������ ��������
		}
	}
	p->RevData = Data;
}

// ������� ������ �� ���� ������� ��������� (����� �����)
void EncoderProtectionUpdate(ENCODER *p)	// 10 ��
{
	static Uns packetCount = 0;
	static Uns alarmTimer = 0;
	static Uns defectTimer = 0;

	if (GrC->EncoderCalcMethod) return;				// ������� �� �����������, ���� ������ ������ ����� ��������� ������ ��

	if (packetCount++ > PRD_10HZ) // (10 ��) * (1 ���)
	{
		*p->RevErrValue = p->errorCounter * 5 / packetCount;
		p->errorCounter = 0;
		packetCount = 0;
	}
	if (*p->RevErrValue > *p->RevErrLevel)
	{
		if (alarmTimer++ > PRD_10HZ)
			p->Error = true;
	}
	else
		alarmTimer = 0;

	if (p->skipDefectFl)
	{
		if (!IsStopped())	// ���� � ��������
		{
			if (defectTimer++ > 2 * PRD_10HZ) p->Error = true;	// �������� �� ������������ - 2 �������
		}
		else
			defectTimer = 0;
	}
}

Uns encoderReset = 0;		// ���� "����������" ��������
Uns ResetTimer = 0;
Uns goodPacket = true;		// ���� "�������� ������"

void AtMegaAvagoEncoderCalc(ENCODER *p) // 200 Hz
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
		goodPacket = true;	
	}
	else
	{
		goodPacket = false;
		errorCount++;		// ����������� ������� ������
		encoderReset = true;
	}

	if ((goodPacket)&&(!encoderReset))
	{
		p->Revolution  = Data & (Uns)p->RevMax;
	}
	p->RevData     = Data;

	if (encoderReset)
	{
		ResetTimer++;
		if (ResetTimer > ENC_RESET_DELAY_LEVEL)
		{
			ResetTimer = 0;
			encoderReset = false;
		}
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

