/*======================================================================
Имя файла:          ProgOutput.h
Автор:              Саидов В.С.
Версия файла:       01.01
Дата изменения:		03/03/10
Применяемость:      Все типы чипов
Описание:
Библиотека обрабки программируемых выходов
======================================================================*/

#ifndef PROG_OUTPUT_
#define PROG_OUTPUT_

#include "std.h"

#ifdef __cplusplus
extern "C" {
#endif

// Условия для программируемого дискретного выхода
typedef enum {
  ocEqual=0,           // Равно
  ocNotEqual,          // Не равно
  ocGreat,             // Больше
  ocLess,              // Меньше
  ocGreatOrEqual,      // Больше или равно
  ocLessOrEqual,       // Меньше или равно
  ocBitSet,            // Бит установлен
  ocBitClear,          // Бит сброшен
  ocAllBitsSet,        // Все биты установлены
  ocAllBitsClear       // Все биты сброшены
} TProgOutCondition;

// Конфигурация программируемого дискретного выхода
typedef struct _TProgOutConfig {
  Uns Addr;       					// Адрес
  Uns Value;      					// Значение
  TProgOutCondition Condition;  // Условие
} TProgOutConfig;

// Структура для обработки выходов 
typedef struct PROG_OUT {
	Bool Enable;				// Разрешение работы
	TProgOutConfig	*Config;	// Указатель на конфигурацию
	Uns *Data;					// Указатель на буфер данных
	Uns *Output;				// Указатель на регистр состояния
	Uns  OutputBit;			// Номер бита для выхода
	Uns  ActiveLev;			// Активный уровень выхода
} PROG_OUT;

void ProgOutCalc(PROG_OUT *);

#ifdef __cplusplus
}
#endif // extern "C"

#endif





