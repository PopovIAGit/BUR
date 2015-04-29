/*======================================================================
Имя файла:          ApFilter1.h
Автор:              Саидов В.С.
Версия файла:       01.00
Дата изменения:		02/02/10
Применяемость:      Совместно с библиотекой IQmath
Описание:
Апериодический фильтр первого порядка
======================================================================*/

#ifndef APFILTER1_
#define APFILTER1_

#include "std.h"

#ifdef __cplusplus
extern "C" {
#endif

// Структура для работы с фильтром
typedef struct APFILTER1 {
	LgInt Input;   // Входной сигнал
	LgInt Output;  // Выходной сигнал
	LgInt PrevOut; // Выходной сигнал
	LgInt dt;      // Постоянная времени семплирования
	LgInt Tf;      // Постоянная времени фильтра
	LgInt K1;      // Коэффициент фильтра
	LgInt K2;      // Коэффициент фильтра
} APFILTER1;

void ApFilter1Init(APFILTER1 *);
void ApFilter1Calc(APFILTER1 *);
void ApFilter1Reset(APFILTER1 *);

#ifdef __cplusplus
}
#endif // extern "C"

#endif
