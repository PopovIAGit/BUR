/*======================================================================
Имя файла:          rtc.h
Автор:              Саидов В.С.
Версия библиотеки:  01.00
Версия файла:       01.00
Применяемость:      Все типы чипов
Описание:
Библиотека симуляции часов реального времени и 
функций управления временем и датой

========================================================================
История:
------------------------------------------------------------------------
	17/12/09	Версия 01.00
----------------------------------------------------------------------*/

#ifndef RTC_
#define RTC_

#include "std.h"

#ifdef __cplusplus
extern "C" {
#endif

// Константы режима изменения часа (AM/PM)
#define RTC_AM_PM         0x20 // Текущий режим (0 - AM, 1 - PM)
#define RTC_AM_PM_MODE    0x40 // Наличие режима

// Структуры для работы с временем и датой
typedef union _TTimeVar {
  Uns all;
  struct {
     Uns Min:6;        // 0-5   Минута
     Uns Hour:10;      // 6-15  Час
  } bit;
} TTimeVar;

typedef union _TDateVar {
  Uns all;
  struct {
  	Uns Day:5;        // 0-4   День месяца
	   Uns Month:4;      // 5-8   Месяц
  	Uns Year:7;       // 9-15  Год
  } bit;
} TDateVar;

typedef union {
  Uns      all;     // Код времени/даты
  TTimeVar Time;    // Структура запакованного времени
  TDateVar Date;    // Структура запакованной даты
} TTimeDate;

// Структуры для работы с часами реального времени
typedef struct RTC_Obj {
	Byte Sec;         // Секунда
	Byte Min;         // Минута
	Byte Hour;        // Час
	Byte WeekDay;     // День недели
	Byte Day;         // День месяца
	Byte Month;       // Месяц
	Byte Year;        // Год
	Byte PrevHour;    // Предыдущий час для корректировки
} RTC_Obj;

typedef struct RTC_Lite {
	Byte Sec;         // Секунда
	Byte Min;         // Минута
	Byte Hour;        // Час
	Byte Day;         // День
} RTC_Lite;

void RTC_Update(void *);
void RTC_Tick(void *);
Byte RTC_DefMaxDay(Byte Year, Byte Month);
void RTC_GetTime(void *p, TTimeVar *Time);
void RTC_GetDate(void *, TDateVar *Date);
void RTC_SetTime(void *, TTimeVar *Time, Byte Sec);
void RTC_SetDate(void *, TDateVar *Date, Byte WeekDay);
Byte RTC_TimeCorr(void *, Int Corr);

#ifdef __cplusplus
}
#endif // extern "C"

#endif



