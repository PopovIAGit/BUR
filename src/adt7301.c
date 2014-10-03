/*======================================================================
Имя файла:          adt7301.c
Автор:              Саидов В.С.
Версия файла:       01.03
Дата изменения:		19/02/10
======================================================================*/

#include "csl\csl_spi.h"
#include "adt7301.h"

#define SetCs()	p->CsFunc(0)
#define ClrCs()	p->CsFunc(1)


void ADT7301_Update(ADT7301 *p)
{
	Uns Data;
	
	SPI_init(p->SpiId, SPI_MASTER, 3, p->SpiBaud, 8);
	
	SetCs();
	Data  = SPI_send(p->SpiId, 0) << 8;
	Data |= SPI_send(p->SpiId, 0);
	ClrCs();
	
	Data = Data >> 5;
	if (Data & 0x0100) Data |= 0xFE00;

	if (abs(Data) < 100) {*p->Temper = Data; p->ErrTimer = 0;}
	else if (++p->ErrTimer >= p->ErrCount) {p->Error = TRUE; p->ErrTimer = 0;}
}
