/*======================================================================
Имя файла:          Inputs.h
Автор:              Саидов В.С.
Версия библиотеки:  01.01
Версия файла:       01.01
Применяемость:      Все типы чипов
Описание:
Библиотека обработки входных сигналов

========================================================================
История:
------------------------------------------------------------------------
	10/09/09	Версия 01.00

	18/09/09	Версия 01.01
1.Оптимизация времени выполнения
----------------------------------------------------------------------*/

#ifndef INPUTS_
#define INPUTS_

#include "std.h"

#ifdef __cplusplus
extern "C" {
#endif

// Типы логик обработки
typedef enum {
	ltDigital = 0,	// Дискретный вход
	ltAnMax,			// Аналоговый по максимуму
	ltAnMin,			// Аналоговый по минимуму
	ltAbs,    		// Аналоговый по по модулю
	ltZone,       	// Аналоговый с мертвой зоной
	ltMaxSign,		// Аналоговый по максимуму знаковый
	ltMinSign		// Аналоговый по минимуму знаковый
} TLogType;

// Макрос для определения значения уровня в структуре для дискретных сигналов
#define DIG_LEVEL(Index, Level)	(Ptr)(((Index) << 1) | ((Level) & 0x1))
#define DIN_LEVEL(Bit, Level)	DIG_LEVEL(IO_BIT_NUM_INIT(Bit), Level)

// Структура для уровневой обработки сигнала
typedef struct LOG_INPUT {
	TLogType LogType;		// Тип логики обработки
	Bool     Enable;		// Разрешение работы	
	Uns     *Input;		// Вход
	Uns     *Output;		// Выход
	Uns     *Level;		// Уровень срабатывания
	Uns     *Timeout;		// Тайм-аут срабатывания
	Uns      BitMask;		// Маска бита выхода
	Uns      Scale;		// Коэффициент масштабирования времени
	Uns      Timer;		// Таймер срабатывания
	Bool     Match;		// Режим установки сигнала
	Bool     Flag;			// Состояние установки сигнала
} LOG_INPUT;

// Структура для дифференциальной обработки сигнала
typedef struct DIFF_INPUT {
	Bool    Enable;      // Разрешение работы
	LgUns  *Input;       // Вход
	Uns    *Output;      // Выход
	Uns    *Level;       // Уровень срабатывания
	Uns    *Timeout;     // Тайм-аут срабатывания
	Uns     BitMask;   	// Бит выхода
	Uns     Scale;			// Коэффициент масштабирования времени
	Uns     Timer;       // Таймер срабатывания
	LgUns   InputPrev;   // Предыдущее состояние входа
} DIFF_INPUT;

#define AV_POW_LIMIT	15	// Степень коэффициента усиления

// Структура для расчета значения аналогового сигнала по интегралу
typedef struct AVERAGE {
	Uns    *Input;       // Вход
	Uns    *Output;      // Выход
	Uns    *Gain;        // Коэффициент усиления
	Int    *Offset;      // Коэффициент смещения
	Int     Signal;      // Смещенный сигнал
	LgUns   Sum;         // Интеграл
	Uns     GainRms;     // Коэффициент расчета среднего значения
} AVERAGE;

typedef struct AVERAGE_DIG {
	Uns    *Input;       // Вход
	Uns    *Output;      // Выход
	Uns    *Gain;        // Коэффициент усиления
	Int    *Offset;		// Коэффициент смещения
	Uns     Level;			// Активный уровень
	Uns     InputBit;    // Номер бита порта
	LgUns   Sum;         // Интеграл
} AVERAGE_DIG;

// Прототипы функций
void LogInputCalc(LOG_INPUT *);
void DiffInputCalc(DIFF_INPUT *);
void AverageCalc(AVERAGE *);
void AverageIntIntegral(AVERAGE *);
void AverageUnsIntegral(AVERAGE *);
void Average2PhaseIntegral(AVERAGE *, AVERAGE *p1, AVERAGE *p2);
void AverageDigInt(AVERAGE_DIG *);
void AverageDigCalc(AVERAGE_DIG *);

#ifdef __cplusplus
}
#endif // extern "C"

#endif





