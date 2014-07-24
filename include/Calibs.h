/*======================================================================
Имя файла:          Calibs.h
Автор:              Саидов В.С.
Версия файла:       01.02
Дата изменения:		24/07/12
Применяемость:      Совместно с библиотекой IQmath (для 28xx)
Описание:
Библиотека работы с калибровками
======================================================================*/

#ifndef CALIBS_
#define CALIBS_

#include "std.h"

#ifdef __cplusplus
extern "C" {
#endif

// Статусы калибровки
#define	CLB_CLOSE   0x01  // Задано Закрыто/находимся в зоне Закрыто
#define	CLB_OPEN    0x02  // Задано Открыто/находимся в зоне Открыто
#define	CLB_FLAG    0x03  // Калибровка выполнена

// Структура управления калибровками
typedef struct {
	Uns TaskClose;          // Задание/сброс Закрыто
	Uns TaskOpen;           // Задание/сброс Открыто
	Uns RevOpen;            // Задание количества оборотов на открытие
	Uns RevClose;           // Задание количества оборотов на закрытие
	Uns RevAuto;            // Автоматическая калибровка
} ClbCommand;

// Структура индикации калибровок
typedef struct {
	Uns    Status;          // Статус калибровки
	Uns    Reserved;        // Резерв
	LgUns  ClosePos;        // Положение Закрыто
	LgUns  OpenPos;         // Положение Открыто
} ClbIndication;

// Структура для работы с калибровками
typedef struct CALIBS {
	Bool   ResetFlag;       // Флаг сброса
	Bool   CancelFlag;      // Флаг отмены команды
	Uns    GearRatio;       // Передаточное число редуктора * 100
	Uns    Zone;            // Зона калибровки
	LgUns  RevMax;          // Максимальное количество оборотов - 1
	LgInt  LinePos;         // Текущее линейное положение
	LgUns  FullStep;        // Полный ход
	LgInt  BasePos;         // Базовое положение
	Uns    CycleData;       // Параметр для расчета циклов
	Uns    MuffTime;        // Таймаут успокоения муфты
	Uns    MuffTimer;       // Таймер успокоения(ожидания срабатывания) муфты
	Uns    AutoCmd;         // Команда при автоматической калибровке
	Uns    RevAuto;         // Количество оборотов при автоматической калибровке
	Uns    AutoStep;        // Шаг автоматической калибровки
	Uns    Reserved;        // Резерв
	LgInt  GearInv;         // Коэффициент обратный передаточныму числу редуктора
	LgInt  FstepInv;        // Коэффициент обратный полному ходу
	Uns   *PosSensPow;    	// Указатель на тип датчика положения
	Uns   *PositionAcc;     // Указатель на смещение конечных положений
	Uns   *RodType;         // Указатель на тип штока
	LgUns *AbsPosition;     // Указатель на текущее положение
	Int   *PositionPr;      // Указатель на текущее положение в %
	Uns   *FullWay;         // Указатель на полный ход выходного звена
	Int   *CurWay;          // Указатель на текущий ход выходного звена
	Uns   *CycleCnt;        // Указатель на количество расчитанных циклов
	Uns   *MuffFlag;        // Сигнал муфты
	ClbCommand    *Command;    // Указатель на структуру управления калибровками
	ClbIndication *Indication; // Указатель на структуру индикации
} CALIBS;

void  Calibration(CALIBS *);
void  DefineCalibParams(CALIBS *);
LgUns CalcClbGearInv(CALIBS *);
LgUns CalcClbAbsRev(CALIBS *, Uns GearRev);
Int   CalcClbGearRev(CALIBS *, LgInt AbsRev);
LgUns CalcClbPercentToAbs(CALIBS *, Uns Percent);
void  CalcClbCycle(CALIBS *);

#ifdef __cplusplus
}
#endif // extern "C"

#endif



