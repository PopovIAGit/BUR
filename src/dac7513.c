#include "csl\csl_spi.h"
#include "dac7513.h"

#define SetCs()	p->CsFunc(0)
#define ClrCs()	p->CsFunc(1)


void DAC7513_Update(DAC7513 *p)
{
	register Int Data = *p->Value;

	if (*p->BusyCsFlag)	return;
	
  switch(p->Mode)
  {
    case DAC7513_OFF:
      Data = 0;
	  p->Data = 0;
      break;
    case DAC7513_DIRECT:
      break;
    case DAC7513_CURR:
      if (Data < 0)    Data = 0;
      if (Data > 2500) Data = 2500;
      break;
    case DAC7513_PR_0_25:
      Data = (Data * 5) >> 1;
      if (Data < 0)    Data = 0;
      if (Data > 2500) Data = 2500;
      break;
    case DAC7513_PR_4_20:
      Data = (Data << 3) / 5 + 400;
      if (Data < 400)  Data = 400;
      if (Data > 2000) Data = 2000;
      break;
    default:
      return;
  }

  if (p->Mode > DAC7513_DIRECT)
    p->Data = (Int)(((LgInt)Data * (LgInt)p->Gain) >> 12) + p->Offset;
  //p->Data = ((Uns)(p->PowerMode & 0x03) << 12) | Data;

	SPI_init(p->SpiId, SPI_MASTER, 1, p->SpiBaud, 8);
	
	SetCs();
	SPI_send(p->SpiId, (p->Data >> 8) & 0xFF);
	SPI_send(p->SpiId, p->Data & 0xFF);
	ClrCs();
}





