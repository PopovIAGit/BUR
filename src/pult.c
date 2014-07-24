#include "config.h"



__inline void ResetCapture(TPult *);



void PultImpulseCapture(TPult *p)
{
	if (ECap3Regs.ECFLG.bit.CEVT1)													// Проверяем флаг "захвата" фронта импульса
	{
		if (!p->CapComplete)
		{
			if (p->FirstPulse)														// Для первого импульса
			{
				ECap3Regs.TSCTR = 0;
				ECap3Regs.CAP1 = 0;
				p->FirstPulse = false;
			}

			if (ECap3Regs.CAP1 > SYNC_PULSE_VALUE)									// Ищем синхроимпульс
			{				
				if (++p->SyncPulsesCounter >= SYNC_PULSES_NUM)
				{
					p->CapComplete = true;											// Завершение приема посылки
				}

				// Складываем полученные биты в код
				p->CapCode |= ECap3Regs.ECCTL1.bit.CAP1POL << (SYNC_PULSES_NUM - p->SyncPulsesCounter);
				
				ECap3Regs.TSCTR = 0;			
			}

			ECap3Regs.ECCTL1.bit.CAP1POL = !ECap3Regs.ECCTL1.bit.CAP1POL;			// Меняем польярность CAP-модуля
		}

		p->CapCounter = 0;
		ECap3Regs.ECCTL2.bit.REARM = 1;
		ECap3Regs.ECCLR.bit.CEVT1  = 1;
	}
}



void PultTimer(TPult *p)
{
	if (++p->CapCounter >= PULT_CAP_RESET) 						// Инкрементируем таймер захвата
	{
		ResetCapture(p);
	}
}



void PultKeyExecute(TPult *p)
{
	if (!p->CapComplete)
		return;
	
	if ((p->CapCode >> 10) != 1)								// Проверяем наличие стартового бита
		return;
	
	if (((p->CapCode >> 8) & 0x03) != BUR_IR_ADDRESS)			// Проверяем правильность адреса
		return;

//	if (p->Key != 0)											Успеет поди обработать клавишу

	p->KeyCode = (p->CapCode & 0xFF);							// Извлекаем код посылки

	switch (p->KeyCode)
	{
		case CODE_STOP:		p->Key = KEY_STOP;		break;
		case CODE_OPEN:		p->Key = KEY_OPEN;		break;
		case CODE_CLOSE:	p->Key = KEY_CLOSE;		break;
		case CODE_RIGHT:	p->Key = KEY_RIGHT;		break;
		case CODE_DOWN:		p->Key = KEY_DOWN;		break;
		case CODE_UP:		p->Key = KEY_UP;		break;
		case CODE_LEFT:		p->Key = KEY_LEFT;		break;
		case CODE_ENTER:	p->Key = KEY_ENTER;		break;
		case CODE_ESCAPE:	p->Key = KEY_ESCAPE;	break;
	}

	p->KeyCode = 0;

	ResetCapture(p);
}

__inline void ResetCapture(TPult *p)
{
		p->FirstPulse 		 = true;
		p->CapComplete 		 = false;
		p->CapCode 			 = 0;
		p->CapCounter 		 = 0;
		p->SyncPulsesCounter = 0;

		ECap3Regs.ECCTL1.bit.CAP1POL = 1;
		ECap3Regs.ECCTL2.bit.REARM 	 = 1;
		ECap3Regs.ECCLR.bit.CEVT1 	 = 1;
}
