/*======================================================================
Имя файла:          ApFilter3.h
Автор:              Саидов В.С.
Версия файла:       01.00
Дата изменения:		02/02/10
Применяемость:      Совместно с библиотекой IQmath
Описание:
Апериодический фильтр третьего порядка
======================================================================*/

#ifndef APFILTER3_
#define APFILTER3_

#include "std.h"

#ifdef __cplusplus
extern "C" {
#endif

// Структура для работы с фильтром
typedef struct APFILTER3 {
	LgInt Input;   // Входной сигнал
	LgInt Output;  // Выходной сигнал
	LgInt Output1; // Входной сигнал
	LgInt PrevOut1;// Выходной сигнал
	LgInt Output2; // Входной сигнал
	LgInt PrevOut2;// Выходной сигнал
	LgInt dt;      // Постоянная времени семплирования
	LgInt Tf;      // Постоянная времени фильтра
	LgInt K1;      // Коэффициент фильтра
	LgInt K2;      // Коэффициент фильтра
	LgInt PrevOut; // Предыдущее значение выходного сигнала
} APFILTER3;

void ApFilter3Init(APFILTER3 *);
void ApFilter3Calc(APFILTER3 *);
void ApFilter3Reset(APFILTER3 *);

#ifdef __cplusplus
}
#endif // extern "C"

#endif
