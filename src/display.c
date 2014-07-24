#include "csl\csl_io.h"
#include "csl\csl_spi.h"
#include "display.h"
#include "displhal.h"

//WS0010 !!!




// Макросы формирования таймингов
/*
#if defined(_TMS320C2XX)
#define tas_delay()       asm(" RPT #2");  asm(" NOP")
#define tddr_delay()      asm(" RPT #14"); asm(" NOP")
#define tdsw_delay()      asm(" RPT #7");  asm(" NOP")
#define tah_delay()       asm(" RPT #1");  asm(" NOP")
#define tcl_delay()       asm(" RPT #18"); asm(" NOP")

#elif defined(__TMS320C28X__)
*/
#define tas_delay()       asm(" RPT #15 || NOP")
#define tddr_delay()      asm(" RPT #75 || NOP")
#define tdsw_delay()      asm(" RPT #35 || NOP")
#define tah_delay()       asm(" RPT #5  || NOP")
#define tcl_delay()       asm(" RPT #95 || NOP")
/*
#elif defined(__IAR_SYSTEMS_ICC__)
#include <ina90.h>
#define tas_delay()       __delay_cycles(6)
#define tddr_delay()      __delay_cycles(30)
#define tdsw_delay()      __delay_cycles(16)
#define tah_delay()       __delay_cycles(4)
#define tcl_delay()       __delay_cycles(38)

#elif defined(_BCB6_)
#define tas_delay()
#define tddr_delay()
#define tdsw_delay()
#define tah_delay()
#define tcl_delay()
#endif
*/
// Макросы для установки/снятия сигнала выбора микросхемы
#define SetCs()           p->CsFunc(0)
#define ClrCs()           p->CsFunc(1)

// Макросы вывода команды/данных
#define PutComm(c)        IOclr(&p->RS); PutData(p, c)
#define PutChar(c)        IOset(&p->RS); PutData(p, c)




// Локальные функции
__inline void DisplayRestart(TDisplay *);
__inline void DisplayOff(TDisplay *);
__inline void DisplayOn(TDisplay *);
__inline void DisplayPutText(TDisplay *);
static   void SendData(TDisplay *, Byte Data);
static   void PutData(TDisplay *, Byte Data);
__inline Char EncodeData(Char Data);



void DISPL_Update(TDisplay *p)
{
	if (p->CsFunc) SPI_init(p->SpiId, SPI_MASTER, 0, p->SpiBaud, 8);
	DisplayRestart(p);
	if (!p->Restart) DisplayPutText(p);
	else if (!p->Enable) DisplayOff(p);
	else DisplayOn(p);
}

void DISPL_AddSymb(TDisplay *p, Byte Addr, Ptr Data, Byte Count)
{
	memcpy(&Symbols[Addr][0], Data, Count * 7);
}

__inline void DisplayRestart(TDisplay *p)
{
	if (!p->ResTout) p->ResTimer = 0;
	else if (++p->ResTimer >= p->ResTout) {p->State = 0; p->Restart = TRUE; p->ResTimer = 0;}
	if (p->Status != p->Enable) {p->Status = p->Enable; p->State = 0; p->Restart = TRUE;}
}

__inline void DisplayOff(TDisplay *p)
{
	PutComm(DISPLAY_OFF);
	SendData(p, 0x00);
	IOclr(&p->RW);
	IOclr(&p->EN);
	IOclr(&p->RS);
	p->Restart = FALSE;
}

__inline void DisplayOn(TDisplay *p)
{
	if (p->PauseTimer > 0) {p->PauseTimer--; return;}

	switch (++p->State)
	{
		case 1:  PutComm(FUNCTION_SET);	
					p->PauseTimer = DISPL_PAUSE_TIME;		break;
		case 2:  PutComm(DISPLAY_OFF);	p->PauseTimer = DISPL_PAUSE_TIME;		break;
		case 3:  PutComm(__CLR);		p->PauseTimer = DISPL_PAUSE_TIME;		break;
		case 4:  PutComm(ENTRY_MODE_SET);	p->PauseTimer = DISPL_PAUSE_TIME;		break;
		case 5:  PutComm(DISPLAY_ON);	p->PauseTimer = DISPL_PAUSE_TIME;		break;
		case 6: PutComm(0x00|__CGADDR);		p->PauseTimer = DISPL_PAUSE_TIME;
				p->CursorAddr = 0;
				p->Data = &Symbols[0][0];
				break;
		default:
			PutChar(((p->CursorAddr & 0x7) != p->CursorPos) ? *p->Data++ : 0x00);
			if (++p->CursorAddr >= 64) {p->State = 0; p->Restart = FALSE;}
	}



/*	
	switch (++p->State)
	{
		case 1: PutComm(__DL|0x20); p->PauseTimer = 20; break;
		case 2: PutComm(__DL|0x20); p->PauseTimer = 5;  break;
		case 3: PutComm(__DL|0x20);                     break;
		case 4: PutComm(FUNCTION_SET);                  break;
		case 5: PutComm(DISPLAY_OFF);                   break;
		case 6: PutComm(__CLR);     p->PauseTimer = 40; break;
		case 7: PutComm(ENTRY_MODE_SET);                break;
		case 8: PutComm(CUR_DISPL_SHIFT);               break;
		case 9: PutComm(DISPLAY_ON);                    break;
		case 10:
			PutComm(0x00|__CGADDR);
			p->CursorAddr = 0;
			p->Data = &Symbols[0][0];
			break;
		default:
			PutChar(((p->CursorAddr & 0x7) != p->CursorPos) ? *p->Data++ : 0x00);
			if (++p->CursorAddr >= 64) {p->State = 0; p->Restart = FALSE;}
	}
	*/
}

__inline void DisplayPutText(TDisplay *p)
{
	if (p->Enable) switch(++p->State)
	{
		case 1:  PutComm(__FSTR); p->Data = p->HiStr; break;
		case 18: PutComm(__SSTR); p->Data = p->LoStr; break;
		default: PutChar(EncodeData(*p->Data++)); if (p->State == 34) p->State = 0;
	}
}

static void SendData(TDisplay *p, Byte Data)
{
	SetCs(); SPI_send(p->SpiId, Data); ClrCs();
}




static void PutData(TDisplay *p, Byte Data)
{
	IOclr(&p->RW);     tas_delay();
	SendData(p, Data); tdsw_delay();

	IOset(&p->EN);     					// После отправки данных выставили и сняли строб
	DelayUs(1);
	IOclr(&p->EN);     tah_delay();

	IOset(&p->RW);     tcl_delay();
}



__inline Char EncodeData(Char Data)
{
	Data &= 0xFF;
	if (Data == 'Ё')  return 0x40;
	if (Data == 'ё')  return 0x41;
	if ((Int)Data >= 0xC0) return RusTable[Data - 0xC0];
	return Data;
}
