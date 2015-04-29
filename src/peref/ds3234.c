/*======================================================================
Имя файла:          ds3234.с
Автор:              ????
Версия файла:       ????
Дата изменения:		09/07/14
======================================================================*/
#include "csl\csl_spi.h"
#include "csl\csl_utils.h"
#include "ds3234.h"

#define SetCs()	p->CsFunc(0); DelayUs(1)
#define ClrCs()	p->CsFunc(1)
// ============= ДРАЙВЕР МИКРОСХЕМЫ ЧАСОВ DS1305 =======================

__inline Byte DS3234_Func(DS3234 *p, Byte Addr, Byte Data);
//--------------------------------------------------------
int Ds3234_Temp = 0;

Uns DsDbgTempDelayCounter = 0;
Uns DsDbgTempCounter = 0;
//--------------------------------------------------------
void DS3234_Init(DS3234 *p)
{
	Byte Reg;
	
	DS3234_Write(p, DS3234_CONTROL_REG, 0);
	DelayUs(10);
	Reg = DS3234_Read(p, DS3234_CONTROL_REG);

	if (Reg & DS3234_EOSC) p->Error = TRUE;

	if (p->Error)
		return;

	DS3234_Write(p, DS3234_CONTROL_STATUS_REG, 0);
	// Выключаем температурное преобразование при батарейном питании
	DS3234_Write(p, DS3234_DISTEMPCONV, DS3234_BBTD);
}
//--------------------------------------------------------
Byte DS3234_Read(DS3234 *p, Byte Addr)
{
	return DS3234_Func(p, Addr & 0x7F, 0x00);
}
//--------------------------------------------------------
void DS3234_Write(DS3234 *p, Byte Addr, Byte Data)
{
	DS3234_Func(p, Addr | 0x80, Data);
}
//--------------------------------------------------------
void DS3234_Update(DS3234 *p)
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
//--------------------------------------------------------
__inline Byte DS3234_Func(DS3234 *p, Byte Addr, Byte Data)
{
	SPI_init(p->SpiId, SPI_MASTER, 3, p->SpiBaud, 8);
	
	SetCs();
	SPI_send(p->SpiId, Addr);
	Data = SPI_send(p->SpiId, Data);
	ClrCs();

	return Data;
}
//--------------------------------------------------------

Bool DS3234_CheckBusy(DS3234 *p)
{
	Byte Reg = 0;

	Reg = DS3234_Read(p, DS3234_CONTROL_STATUS_REG);

	return (Reg & DS3234_BSY);
}
//--------------------------------------------------------
void DS3234_TempConv(DS3234 *p)
{
	Byte Reg = 0;

	if (DS3234_CheckBusy(p))
	{
		DsDbgTempDelayCounter++;
		return;
	}

	Reg = DS3234_Read(p, DS3234_CONTROL_REG);

	if (Reg & DS3234_CONV)
	{
		DsDbgTempDelayCounter++;
		return;
	}

	Reg |= DS3234_CONV;

	DS3234_Write(p, DS3234_CONTROL_REG, Reg);

	DsDbgTempCounter++;
}
//--------------------------------------------------------
void DS3234_ReadTemp(DS3234 *p)
{
	Byte Temp = 0;
	Temp = DS3234_Read(p, DS3234_TEMP_MSB);
	
	Ds3234_Temp = Temp & 0x7F;

	DS3234_CheckBusy(p);

//	DS3234_Write(p, DS3234_CONTROL_STATUS_REG, 0);

	Temp = DS3234_Read(p, DS3234_DISTEMPCONV);


//	if (Temp & 0x80)
//	Sign Bit		

}
//--------------------------------------------------------







