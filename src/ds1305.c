/*======================================================================
Имя файла:          ds1305.с
Автор:              Саидов В.С.
Версия файла:       01.03
Дата изменения:		19/02/10
======================================================================*/

#include "csl\csl_spi.h"
#include "csl\csl_utils.h"
#include "ds1305.h"

#define SetCs()	p->CsFunc(0); DelayUs(1)
#define ClrCs()	p->CsFunc(1)

__inline Byte DS1305_Func(DS1305 *p, Byte Addr, Byte Data);



void DS1305_Init(DS1305 *p)
{
	Byte Reg;
	
	DS1305_Write(p, DS1305_CONTROL_REG, 0);
	DelayUs(10);
	Reg = DS1305_Read(p, DS1305_CONTROL_REG);

	if (Reg & 0xC0) p->Error = TRUE;
}

Byte DS1305_Read(DS1305 *p, Byte Addr)
{
	return DS1305_Func(p, Addr & 0x7F, 0x00);
}

void DS1305_Write(DS1305 *p, Byte Addr, Byte Data)
{
	DS1305_Func(p, Addr | 0x80, Data);
}

void DS1305_Update(DS1305 *p)
{
	register Byte Mode;
	
	if (p->Flag) {p->State = 0; p->Addr = 0; p->Busy = TRUE; p->Flag = FALSE;}
	
	switch(++p->State)
	{
		case 1:
			if (!p->Busy) p->Data = 0;
			else
			{
				p->Data = p->DataBuf[p->Addr];
				if (p->Addr != 0x02) {Mode = 0; p->Data &= 0x7F;}
				else {Mode = p->Data & 0x60; p->Data &= 0x1F;}
				p->Data = Mode | (((p->Data / 10) << 4) | (p->Data % 10));
			}
			break;
		case 2:
			SPI_init(p->SpiId, SPI_MASTER, 3, p->SpiBaud, 8);
			SetCs();
			SPI_send(p->SpiId, !p->Busy ? p->Addr : (p->Addr|0x80));
			p->Data = SPI_send(p->SpiId, p->Data);
			ClrCs();
			break;
		case 3:
			if (!p->Busy)
			{
				if (p->Addr != 0x02) {Mode = 0; p->Data &= 0x7F;}	
				else if (!(p->Data & 0x40)) {Mode = 0; p->Data &= 0x3F;}
				else {Mode = p->Data & 0x60; p->Data &= 0x1F;}
				p->DataBuf[p->Addr] = Mode | (((p->Data >> 4) * 10) + (p->Data & 0x0F));
			}
			if (++p->Addr >= 7) {p->Addr = 0; p->Busy = FALSE;}
			p->State = 0;
			break;
	}
}

__inline Byte DS1305_Func(DS1305 *p, Byte Addr, Byte Data)
{
	SPI_init(p->SpiId, SPI_MASTER, 3, p->SpiBaud, 8);
	
	SetCs();
	SPI_send(p->SpiId, Addr);
	Data = SPI_send(p->SpiId, Data);
	ClrCs();

	return Data;
}







