/*======================================================================
Имя файла:          PrtCalc.h
Автор:              Саидов В.С.
Версия файла:       01.00
Дата изменения:		15/04/10
Применяемость:      Все типы чипов
Описание:
Библиотека реализации алгоритма работы защит
======================================================================*/

#ifndef PRTCALC_
#define PRTCALC_

#include "std.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PRT_CFG_SET(Mode, Level, Bit, Hyst) \
	((Mode<<1)|(Level<<2)|(Bit<<3)|(Hyst<<7))

typedef union {
	Uns all;
	struct
	{
		Uns Enable:1;			// 0		Разрешение работы
		Uns Mode:1;				// 1		Режим работы
		Uns Level:1;			// 2		Активный уровень
		Uns Num:4;				//	3-6	Номер бита в регистре аварий
		Uns Hyst:9;				// 7-15  Значение гистерезиса
	} bit;
} TPrtCfg;

typedef struct {
	TPrtCfg Cfg;			// Конфигурация защиты
	Int    *Input;			// Вход
	Uns    *Output;		// Выход
	Int    *Level;			// Уровень срабатывания
	Uns    *Timeout;		// Тайм-аут срабатывания
	Uns     Scale;			// Коэффициент масштабирования времени
	Uns     Timer;			// Таймер срабатывания
} TPrtElem;

void PrtCalc(TPrtElem *);

#ifdef __cplusplus
}
#endif // extern "C"

#endif





