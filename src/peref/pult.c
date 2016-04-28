#include "config.h"



__inline void ResetCapture(TPult *);



void PultImpulseCapture(TPult *p)
{
	IrDA_NoiseFilter(p);

	if ((ECap3Regs.ECFLG.bit.CEVT1)&&(p->ReadyToRecieve))							// Проверяем флаг "захвата" фронта импульса
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
					p->isFrameRecieved = true;										// Выставляем флаг получения 11 битов данных
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
//===========================================================================================================
// Функция фильтрации шумов ИК-приемника
// Условия фильтря:
// 1) Сигнал команды управления должен состоять из 11 битов
// 2) Сигнал ПДУ должен иметь форму миандра, т.е. после приема 11 бит данных фронт сигнала не должен меняться
//    и уровень сигнала должен держаться в "1" в течение заданного времени
void IrDA_NoiseFilter(TPult *p)
{
	//--- 1 Часть функции --- проверка на изменение фронта сигнала перед приемом фрейма данных
	if (p->ReadyToRecieve == false)						// Если флаг готовности приема еще не выставлен
	{
		if ((ECap3Regs.ECFLG.bit.CEVT1 == 0)&&			// Если изменениия фронта сигнала не происходит
			(ECap3Regs.ECCTL1.bit.CAP1POL == 1))		// и уровень сигнала держится в "1"
		{
			if (p->preFrameTimer++ >= PRE_FRAME_TIMEOUT)// Если таймер успешно досчитал до таймаута 50 мс
			{
				p->preFrameTimer = 0;
				p->ReadyToRecieve = true;				// Считаем, что блок готов к приему сигнала
			}
		}
		else											// Если фронт сигнала изменился
			p->preFrameTimer = 0;						// Таймер сбрасываем. А значит к приему сигнала не готовы
	}
	//--- 2 Часть функции --- проверка на изменение фронта сигнала после приема пакета данных
	if (p->isFrameRecieved)						// Если получен пакет данных
	{
		p->postFrameTimer++;					// начинаем отсчитывать таймер

		if ((ECap3Regs.ECFLG.bit.CEVT1 == 1)||		// Если произошло изменение фронта сигнала
		    (ECap3Regs.ECCTL1.bit.CAP1POL == 0))	// или уровень сигнала равен "0" ...
		{											// то считаем принятый пакет данных "шумом"
			p->postFrameTimer = 0;					// сбрасываем все флаги и выходим из функции
			ResetCapture(p);
			p->ReadyToRecieve = false;				// Выставляем флаг, что мы не готовы к приему новых данных, пока сигнал не "выровняется"
			return;
		}

		if (p->postFrameTimer >= POST_FRAME_TIMEOUT)// Если таймер успешно досчитал до таймаута
		{
			p->isFrameRecieved = false;
			p->CapComplete = true;					// Выставляем флаг завершения приема
		}
	}
}
//===========================================================================================================

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

	if ((p->CapCode >> 9) == 3) 
	{
		p->CapCode = (p->CapCode << 1) & 0x7FF;
	}

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
	p->ReadyToRecieve = false;									// Выставляем флаг, что мы не готовы к приему новых данных, пока сигнал не "выровняется"
}

__inline void ResetCapture(TPult *p)
{
		p->FirstPulse 		 = true;
		p->CapComplete 		 = false;
		p->isFrameRecieved   = false;
		p->CapCode 			 = 0;
		p->CapCounter 		 = 0;
		p->SyncPulsesCounter = 0;

		ECap3Regs.ECCTL1.bit.CAP1POL = 1;
		ECap3Regs.ECCTL2.bit.REARM 	 = 1;
		ECap3Regs.ECCLR.bit.CEVT1 	 = 1;
}
