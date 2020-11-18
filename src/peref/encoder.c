#include "csl\csl_spi.h"
#include "csl\csl_utils.h"
#include "encoder.h"
#include "timings.h"
#include "config.h"

#define SetCs()	p->CsFunc(0)
#define ClrCs()	p->CsFunc(1)

Uns initTimer = 40;		// ������ ������������� ��������, 0.2 ������. ���� �� �� ����� 0,
						// ��������� � core �� ���������� � ������ �� ���� �������� �� ��������
LgUns Revolution = 0;



// ������� ��������� ��������
void EncoderUpdate(void)			// 200 Hz
{
	Ram.GroupH.PP90Reg.bit.EncOn = 1;	// �������� �������

	if (GrC->EncoderType == 0)		// encoder Avago
	{
		RevMax = 0x3FFF;
		Calib.RevMax = RevMax;
		Encoder.RevMax = 0x3FFF;

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
		enDPMA15.RevMax = 0x7FFF;

		encoder_DPMA15_GetData(&enDPMA15);
		Revolution = 0x7FFF-enDPMA15.revolution;
	}

/*	RevMax = 0xFFFF;
	Calib.RevMax = RevMax;
	enDPMA15.RevMax = 0x7FFF;
	EncoderTitanUpdate(&enDPMA15);
	Revolution = enDPMA15.revolution;*/
}


// ������� ��������� ��������.
// ������� ������ �� ���� �������� - ����� �����
void AtMegaAvagoEncoderUpdate(ENCODER *p)	// 200 ��
{
	Int Delta;
	Uns absDelta;

    static  Uns  Data;
    static  Uns  Data1, Data2, Data3 = 0;
    static  Uns  counter = 1;
    static  Uns  Error_counter = 0;

	if (GrC->versionPO_PUE == 1)	// ���� ������ �� ��� - 0.014
	{
		// -------- ���������� ������ ��������-------------
		SPI_init(p->SpiId, SPI_MASTER, 0, p->SpiBaud, 8);

		SetCs();												// ������ ������ � ��������
		DelayUs(ENC_ATMEGA_SPI_DELAY_US);

		// 1 ���� - dummy (������ RX)
		SpiaRegs.SPITXBUF = 0x00;
		while(!SpiaRegs.SPISTS.bit.INT_FLAG);
		DelayUs(1);
		Data  = SpiaRegs.SPIRXBUF;

		// 2 ���� - ������� �����
		SpiaRegs.SPITXBUF = 0x00;
		while(!SpiaRegs.SPISTS.bit.INT_FLAG);
		DelayUs(1);
		Data  = SpiaRegs.SPIRXBUF << 8;

		// 3 ���� - ������� �����
		SpiaRegs.SPITXBUF = 0x00;
		while(!SpiaRegs.SPISTS.bit.INT_FLAG);
		DelayUs(1);
		Data  |= SpiaRegs.SPIRXBUF;

		ClrCs();
	}
	else
	{

	//-------------�������� �� ������� ������-------------------------
		// ������ ������ � ��������
		SPI_init(p->SpiId, SPI_MASTER, 0, p->SpiBaud, 8);
		switch (counter)
		{
		case 1:
				SetCs();                                                // ������ ������ � ��������
				DelayUs(ENC_ATMEGA_SPI_DELAY_US);
				Data1  = SPI_send(p->SpiId, 0x00) << 8;
				Data1 |= SPI_send(p->SpiId, 0x00);
				ClrCs();
				counter = 2;
			break;
		case 2:
				SetCs();                                                // ������ ������ � ��������
				DelayUs(ENC_ATMEGA_SPI_DELAY_US);
				Data2  = SPI_send(p->SpiId, 0x00) << 8;
				Data2 |= SPI_send(p->SpiId, 0x00);
				ClrCs();
				counter = 3;
			break;
		case 3:
				SetCs();                                                // ������ ������ � ��������
				DelayUs(ENC_ATMEGA_SPI_DELAY_US);
				Data3  = SPI_send(p->SpiId, 0x00) << 8;
				Data3 |= SPI_send(p->SpiId, 0x00);
				ClrCs();

				if ((Data1 == Data2) && (Data2== Data3))
				{
					Data = Data3;
					Error_counter = 0;
				}
				else if ((Data1 != Data2)&&(Data1 != Data3)&&(Data3 != Data2))
				{
					Error_counter++;
					if (Error_counter >= 8 ) // 8 - 4 ������ �� 1 ������ �������� = ���� 2 ������� � �������� �� ������
					{
						p->Error = 1;
						Error_counter = 0;
					}
				}
				else
				{
					if (Data1 == Data2) Data = Data2;
					if (Data2 == Data3) Data = Data3;
					if (Data1 == Data3) Data = Data3;

					Error_counter = 0;

				}
				counter = 1;
			break;
		}

	}
//-----------------------------------------------------------------------
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
	Uns  Delta;

	static  Uns  Data;
	static  Uns  Data1, Data2, Data3 = 0;
	static  Uns  counter = 1;
	static  Uns  Error_counter = 0;

	if (GrC->versionPO_PUE == 1)	// ���� ������ �� ��� - 0.014
	{
		// -------- ���������� ������ ��������-------------
		SPI_init(p->SpiId, SPI_MASTER, 0, p->SpiBaud, 8);

		SetCs();												// ������ ������ � ��������
		DelayUs(ENC_ATMEGA_SPI_DELAY_US);

		// 1 ���� - dummy (������ RX)
		SpiaRegs.SPITXBUF = 0x00;
		while(!SpiaRegs.SPISTS.bit.INT_FLAG);
		DelayUs(1);
		Data  = SpiaRegs.SPIRXBUF;

		// 2 ���� - ������� �����
		SpiaRegs.SPITXBUF = 0x00;
		while(!SpiaRegs.SPISTS.bit.INT_FLAG);
		DelayUs(1);
		Data  = SpiaRegs.SPIRXBUF << 8;

		// 3 ���� - ������� �����
		SpiaRegs.SPITXBUF = 0x00;
		while(!SpiaRegs.SPISTS.bit.INT_FLAG);
		DelayUs(1);
		Data  |= SpiaRegs.SPIRXBUF;

		ClrCs();
	}
	else
	{
		//-------------�������� �� ������� ������-------------------------
	    // ������ ������ � ��������
	    SPI_init(p->SpiId, SPI_MASTER, 0, p->SpiBaud, 8);
	    switch (counter)
		{
		case 1:
				SetCs();                                                // ������ ������ � ��������
				DelayUs(ENC_ATMEGA_SPI_DELAY_US);
				Data1  = SPI_send(p->SpiId, 0x00) << 8;
				Data1 |= SPI_send(p->SpiId, 0x00);
				ClrCs();
				counter = 2;
			break;
		case 2:
				SetCs();                                                // ������ ������ � ��������
				DelayUs(ENC_ATMEGA_SPI_DELAY_US);
				Data2  = SPI_send(p->SpiId, 0x00) << 8;
				Data2 |= SPI_send(p->SpiId, 0x00);
				ClrCs();
				counter = 3;
			break;
		case 3:
				SetCs();                                                // ������ ������ � ��������
				DelayUs(ENC_ATMEGA_SPI_DELAY_US);
				Data3  = SPI_send(p->SpiId, 0x00) << 8;
				Data3 |= SPI_send(p->SpiId, 0x00);
				ClrCs();

				if ((Data1 == Data2) && (Data2== Data3))
				{
					Data = Data3;
					Error_counter = 0;
				}
				else if ((Data1 != Data2)&&(Data1 != Data3)&&(Data3 != Data2))
				{
					Error_counter++;
					if (Error_counter >= 8 ) // 8 - 4 ������ �� 1 ������ �������� = ���� 2 ������� � �������� �� ������
					{
						p->Error = 1;
					}
				}
				else
				{
					if (Data1 == Data2) Data = Data2;
					if (Data2 == Data3) Data = Data3;
					if (Data1 == Data3) Data = Data3;

					Error_counter = 0;

				}
				counter = 1;
			break;
		}
	}

	//-----------------------------------------------------------------------
	// �������������
	if (initTimer)
	{
		initTimer--;
		p->RevData = Data;
		return;
	}
	//-----------------------------------------------------------------------

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



