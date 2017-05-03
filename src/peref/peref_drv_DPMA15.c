

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
	p->SpiBaud 	= SPI_BRR(1000);  // Частота синхроимпульсов (расчитанная для конктретного чипа)
	p->bitCount=0;        		  // Количество байт/бит данных
	p->RevMax=0;			      // Ограничение значения положения
	p->revolution=0;              // Значение положения с датчика в формате 16 разрядов
	p->EncoderData=0;		      // Данные, получаемые от энкодера
	p->prevRevolution=0;	      // Предыдущее значение энкодера
	p->RevMisc=0;			      // Контрольная сумма/Значение максимальной разницы
	p->RevErrCount=0;		      // Счетчик ошибок
	p->RevTimer=0;		          // Счетчик для расчета ошибки
	p->ResetCounter=0;	          // Счетчик сброса энкодера
	p->ResetDelay=0;		      // Флаг задержки сброса энкодера
	p->Error=0;                   // Признак сбоя
	p->TmpError=0;		          // Признак временного сбоя, который сбрасывается автоматически
	p->CsFunc(0);                 // Функция выбора микросхемы

}

// Функция считывания данных с датчика положения ДПМА-15
void encoder_DPMA15_GetData(EN_DPMA15 *p) // 50 Hz
{
	Byte	Data1, Data2;				// две посылки данных по 8 бит

	//if (Eeprom.  != 0) return; //проверяем не занята ли линия SPI

	SPI_init(p->SpiId, SPI_MASTER, 0, p->SpiBaud, 8);

	SetCs();
	//DelayUs(); 			// Задержка t1 = 10.4 мкСек
	// Получение данных энкодера

	Data1 = SPI_send(p->SpiId, 0x00);
	DelayUs(ENC_ATMEGA_SPI_DELAY_US);
	Data2 = SPI_send(p->SpiId, 0x00);
	ClrCs();
												// Три части по байту склеиваем в одну переменную long
	p->EncoderData  = (Data1 << 8);
	p->EncoderData  |= Data2;
	p->revolution = p->EncoderData & 0x7fff;
}








