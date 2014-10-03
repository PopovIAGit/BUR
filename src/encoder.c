#include "csl\csl_spi.h"
#include "csl\csl_utils.h"
#include "encoder.h"
#include "timings.h"

#define SetCs()	p->CsFunc(0)
#define ClrCs()	p->CsFunc(1)



void FrabaEncoderCalc(ENCODER *p)
{
	Byte Count = 14;
	Uns AbsRevolution = 0, GrayCode;
	
	SPI_init(p->SpiId, SPI_MASTER, 3, p->SpiBaud, 8);
	DelayUs(4);
	
	SetCs();
	GrayCode  = SPI_send(p->SpiId, 0xFF) << 8;
	GrayCode |= SPI_send(p->SpiId, 0xFF);
	ClrCs();
	
	p->GrayCode = GrayCode;
	
	GrayCode &= 0x7FFF;
	while (Count--)
	{
		GrayCode = GrayCode >> 1;
		AbsRevolution = AbsRevolution ^ GrayCode;
	}
	
	p->Revolution = p->RevMax - AbsRevolution;
}


void EleSyEncoderCalc(ENCODER *p)
{
	Byte Data;
	
	SPI_init(p->SpiId, SPI_MASTER, 0, p->SpiBaud, 8);
	
	SetCs(); DelayUs(1);
	Data = SPI_send(p->SpiId, !p->State ? 0x80 : 0x00);
	DelayUs(1); ClrCs();
	
	if (!p->State)
	{
		p->RevData = 0UL;
		p->RevMisc = 0;
		p->State   = 1;
	}
	else if (p->State <= p->Count)
	{
		p->RevData = (p->RevData << 8) | (LgUns)Data;
		p->RevMisc = p->RevMisc + Data;
		p->State++;
	}
	else
	{
		if (Data != ((~p->RevMisc) & 0xFF)) p->RevErrCount++; // SVS.1
		else p->Revolution = p->RevData & p->RevMax;
		if (++p->RevTimer >= 1000) // SVS.2
		{
			if (p->RevErrCount >= *p->RevErrLevel) p->Error = TRUE;
			*p->RevErrValue = p->RevErrCount;
			p->RevErrCount  = 0;
			p->RevTimer     = 0;
		}
		p->State = 0;
	}
}

void AvagoEncoderCalc(ENCODER *p)
{
	Byte Shift;
	Uns  Data, Delta;
	
	SPI_init(p->SpiId, SPI_MASTER, 2, p->SpiBaud, 8);
	
	SetCs(); DelayUs(1);
	Data  = SPI_send(p->SpiId, 0x00) << 8;
	Data |= SPI_send(p->SpiId, 0x00);
	DelayUs(1); ClrCs();
	
	Shift = 16 - p->Count;
	Data  = (Data >> (Shift - 1)) & ((1U << p->Count) - 1);
	
	Delta = abs((Data - (Uns)p->RevData) << Shift) >> Shift;
	if (Delta < (Uns)p->RevMisc)
	{
		p->Revolution  = Data & (Uns)p->RevMax;
		p->RevData     = Data;
		p->RevErrCount = 0;
	}
	else
	{
		*p->RevErrValue = *p->RevErrValue + 1;
		if (++p->RevErrCount >= *p->RevErrLevel) p->Error = TRUE;
	}
}

void AtMegaAvagoEncoderCalc(ENCODER *p)
{
	Uns  Data, Delta;

	if (p->Error)										// Ошибка энкодера - выходим
		return;
		
	if (p->ResetDelay)										// Задержка для сброса энкодера
	{
		if (++p->ResetCounter >= ENC_RESET_DELAY_LEVEL)
		{
			p->ResetDelay = false;
			p->ResetCounter = 0;
		}
		else
			return;											// Пока счетчик считает, данные не запрашиваем
	}

	SPI_init(p->SpiId, SPI_MASTER, 0, p->SpiBaud, 8);
	
	SetCs(); 												// Запрос данных с энкодера
	Data  = SPI_send(p->SpiId, 0x00) << 8;
	DelayUs(ENC_ATMEGA_SPI_DELAY_US);
	Data |= SPI_send(p->SpiId, 0x00);
	ClrCs();

	if (p->ResetCounter >= ENC_STARTUP_LEVEL)
	{
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
			if (++p->RevErrCount >= *p->RevErrLevel) 			// Превысили порог счетчика ошибок
			{
				*p->RevErrValue = *p->RevErrValue + 1;			// Инкрементируем количество сбоев энкодера
				p->TmpError = true;
								
				if (*p->RevErrValue < ENC_RESET_LEVEL)			// Сбрасываем определенное количество раз
				{
					p->ResetDelay = true;
					p->ResetCounter = 0;
				}
				else											// Порог сброса превышен, выставляе  ошибку
				{
					p->Error = true;
				}
			}
		}
	}
	else
	{
		p->ResetCounter++;
		p->RevData = Data;
		p->Revolution  = Data & (Uns)p->RevMax;	
	}
}

