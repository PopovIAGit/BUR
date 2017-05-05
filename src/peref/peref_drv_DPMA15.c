

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
	p->SpiId = PLIS_SPI;              // Идентификатор SPI-порта (0-SPIA, 1-SPIB, ...)
	p->SpiBaud = SPI_BRR(1000);  // Частота синхроимпульсов (расчитанная для конктретного чипа)
	p->bitCount = 0;        		  // Количество байт/бит данных
	p->RevMax = RevMax;			      // Ограничение значения положения
	p->revolution = 0;              // Значение положения с датчика в формате 16 разрядов
	p->EncoderData = 0;		      // Данные, получаемые от энкодера
	p->prevRevolution = 0;	      // Предыдущее значение энкодера
	p->RevMisc = 5;			      // Контрольная сумма/Значение максимальной разницы
	p->RevErrCount = 0;		      // Счетчик ошибок
	p->RevTimer = 0;		          // Счетчик для расчета ошибки
	p->ResetCounter = 0;	          // Счетчик сброса энкодера
	p->ResetDelay = 0;		      // Флаг задержки сброса энкодера
	p->Error = 0;                   // Признак сбоя
	p->TmpError = 0;		          // Признак временного сбоя, который сбрасывается автоматически
	p->CsFunc(0);                 // Функция выбора микросхемы

	p->RevErrValue = &GrC->RevErrValue;
	p->RevErrLevel = &GrC->RevErrLevel;

}

// Функция считывания данных с датчика положения ДПМА-15
void encoder_DPMA15_GetData(EN_DPMA15 *p) // 50 Hz
{
	Uns Delta, Data, DataWithMask;				// величина изменения, принятые данные, маскированные данные.
	Uns ConnFlag;

	SPI_init(p->SpiId, SPI_MASTER, 0, p->SpiBaud, 8);

	SetCs();

	Data = SPI_send(p->SpiId, 0x00) << 8;	// Особенность посылки: есть всегда старший бит
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
			&& (Delta <= p->RevMax - p->RevMisc))			// 3) Если 5 < Delta < 16383-5 - "скачок данных")
	{
		p->RevErrCount++;
	}
	else
	{
		p->revolution = DataWithMask;			    // 4) Нормальные условия работы энкодера
	}

	p->EncoderData = DataWithMask;
}








