/*======================================================================
Имя файла:          protectionI2T.c
Автор:              Denis
Описание:
Модуль время-токовой защиты
======================================================================*/
#include "protectionI2T.h"

TAlarmI2T	i2tOverload;

// Иницилизация аварий
void ProtectionI2T_Init(TAlarmI2T *p, Uns Freq)
{
	p->enable = true;
	p->isFault = false;
	p->minHighCurrent = (Uns)_IQ15int( _IQmpy (_IQ15(*p->nominalCurr), _IQ(K_HIGH_I)) );
	p->maxLowCurrent  = (Uns)_IQ15int( _IQmpy (_IQ15(*p->nominalCurr), _IQ(K_LOW_I)) );
	p->scale =_IQ15div(1, Freq);	// Прирост времени таймера, зависящий от частоты, на которой функция вызывается
}
//--------------------------------------------------------
void ProtectionI2T_Update(TAlarmI2T *p)
{
	if (!p->enable)						// Если "разрешение работы"=0, то есть "выкл", то
	{
		p->timer = 0;
		return;							// Выходим из функции
	}

	if (!p->isFault)					// Если аварии нет 
	{
		if (*p->inputCurr < p->maxLowCurrent)	// если ток ниже величины, при которой время-токовая защита не ведется
		{	
			p->timer = 0;			// отключаем накопление
		}
		else
		{
			p->timeout =  I2T_CONV( *p->inputCurr, *p->nominalCurr, _IQ15(50), _IQ15(1.4), _IQ15(60));
			if (p->timer < p->timeout)	// Пока таймер не достиг времени срабатывания аварии
				p->timer += p->scale;// Наращиваем таймер
			else					// Как только таймер достиг времени срабатывания аварии
			{
				p->isFault = true;	// выставляем АВАРИЮ
				p->timer = 0;
			}
		}
	}
} 
//---------Конец файла------------------------------------ 

