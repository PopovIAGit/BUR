/*======================================================================
Имя файла:          VlvDrvCtrl.h
Автор:              Саидов В.С. - (Обновил Попов И.А.)
Версия файла:       01.05
Дата изменения:		23/04/12
Применяемость:      Совместно с библиотекой Calibs (при наличии калибровок)
Описание:
Библиотека реализации алгоритмов управления приводом задвижки
======================================================================*/

#ifndef VLV_DRV_CNTRL_
#define VLV_DRV_CNTRL_

#include "std.h"

#ifdef __cplusplus
extern "C" {
#endif

// Константы для работы с положением
#define POS_UNDEF	0x7FFFFFFF
#define POS_ERR		10

// Состояния входов ТУ
#define TU_OPEN		0x1
#define TU_CLOSE		0x2
#define TU_STOP		0x4

// Команда управления
typedef enum {
  vcwNone=0,		// Нет команды
  vcwStop,			// Стоп
  vcwClose,			// Закрыть
  vcwOpen,			// Открыть
  vcwTestClose,		// Тестовое закрыть
  vcwTestOpen,		// Тестовое открыть
  vcwDemo,			// Демо режим
  vcwTestEng		// Тест двигателя
} TValveCmd;

// Тип уплотнения
typedef enum {
  vtNone=1,		// Без уплотнения
  vtClose,			// Уплотнение в закрыто
  vtOpen,			// Уплотнение в открыто
  vtBoth			// Уплотнение в закрыто и открыто
} TBreakMode;

// Настройка режима МУ/ДУ
typedef enum {
  mdOff=0,			// Выключен
  mdSelect,		// Выбор режима МУ/ДУ
  mdMuOnly,		// Только режим МУ
  mdDuOnly			// Только режим ДУ
} TMuDuSetup;

// Источник команд для режима ДУ
typedef enum {
	mdsAll=0,		// Все интерфейсы
	mdsDigital,		// Только дискретный
  mdsSerial		// Только последовательный
} TDuSource;

// Тип реверса
typedef enum {
  rvtStop=0,		// Переход в стоп
  rvtAuto,			// Автоматический реверс
  rvtNone			// Игнорирование команд при подаче команд на реверс
} TReverseType;

// Структура для работы с местным управлением
typedef struct {
	Bool				 Enable;				// Наличие местного управления
	Byte				 BtnKey;				// Команда с ручек/кнопок
	Byte				 PduKey;				// Команда с ПДУ
	Bool				 WaitConfirmFlag;		// Флаг того, что мы ожидаем подтверждение команды управления с ПДУ
	Bool				 CancelFlag;			// Флаг отмена команды
	Bool				 MpuBlockedFlag;		// Флаг отмены команды из-за блокировки МПУ
} TMpuControl;

// Структура для работы с телеуправлением
typedef struct {
	Bool				 Enable;			// Наличие телеуправления
	Bool				 LocalFlag;			// Флаг TRUE, если управление местное
	Uns				 	 State;				// Состояние входов телеуправления
	Bool				 Ready;				// Готовность обработки команд ТУ на движение
	Bool				 ReleStop;			// Стоп в релейном режиме
	Bool				 ReleStopEnable;	// Разрешение стопа в релейном режиме
	Uns					*ReleMode;			// Признак релейного режима
	Uns					*LockSeal;			// Блокировка залипания
} TTeleControl;

// Структура для работы с плавным регулированием
typedef struct {
	Bool				 Enable;			// Наличие плавного регулирования
	Bool				 Active;			// Признак работы плавного регулирования
} TSmoothControl;

// Структура для работы с демо-режимом
typedef struct {
	Bool				 Enable;			// Наличие демо-режима
	Bool				 Active;			// Признак работы демо-режима
	Uns				 	Timer;				// Таймер отсчета времени
	Uns				 	Timeout;			// Тайм-аут демо-режима
	Uns				 	TaskPos;			// Текущее задание положения
	Uns					*UpPos;				// Верхнее положение в %
	Uns					*DownPos;			// Нижнее положение в %
} TDemoControl;

// Структура при работе с задвижкой
typedef struct {
	Bool				PosRegEnable;		// Наличие режима позиционирования
	Bool            	BreakFlag;			// Флаг работы с уплотнением SVS.1
	LgInt				Position;			// Целевое положение
	Uns				 	BreakDelta;		// Максимальное смещение при работе с уплотнением
	TBreakMode			*BreakMode;			// Режим работы с уплотнением
	Ptr					CalibData;			// Данные по калибровке
} TValveControl;

// Структура для работы с журналом событий
typedef struct {
	Uns				 Value;				// Значение команды управления для журнала событий
	Uns				 Source;				// Источник команды управления для журнала событий
	Uns				 QueryValue;			// Значение команды управления, поставленной в очередь
} TEvLogControl;

// Структура статусного регистра
typedef union {
	Uns all;
	struct {
		Uns Stop:1;			// 0		Стоп
		Uns Fault:1;		// 1		Авария
		Uns Closing:1;		// 2		Идет закрытие
		Uns Opening:1;		// 3		Идет открытие
		Uns Rsvd1:1;		// 4		Резерв
		Uns Closed:1;		// 5		Закрыто
		Uns Opened:1;		// 6		Открыто
		Uns Rsvd2:1;		// 7		Резерв
		Uns MuDu:1;			// 8		Местное управление
		Uns Rsvd:7;			// 9-15 Резерв
	} bit;
} TVlvStatusReg;

// Коды команд управления
#define CMD_STOP				0x0001	// Стоп
#define CMD_CLOSE				0x0002	// Закрыть
#define CMD_OPEN				0x0004	// Открыть
#define CMD_MOVE				0x0008	// Переместить
#define CMD_DEFSTOP				0x0800	// Стоп при аварии(не является командой, добавлено для журнала)  : Добавил PIA 12.09.2012
#define CDM_DISCROUT_TEST		0x1000	// Тест дискретных выходов
#define CMD_DISCRIN_TEST		0x2000	// Тест дискретных входов

// Источник команды управления
#define CMD_SRC_BLOCK		0x0400	// подача блоком управления
#define CMD_SRC_PDU			0x0800	// Пульт дистанционного управления
#define CMD_SRC_MPU			0x1000	// Местный пост управления
#define CMD_SRC_DIGITAL		0x2000	// Дискретный интерфейс
#define CMD_SRC_SERIAL		0x4000	// Последовательный интерфейс
#define CMD_SRC_ANALOG		0x8000	// Аналогвый интерфейс

// Структура управления приводом
typedef struct {
	TVlvStatusReg	*Status;			// Статус работы
	TValveCmd		*ControlWord;		// Команда управления
	TMuDuSetup		*MuDuSetup;			// Настройка режима МУ/ДУ
	TDuSource		*DuSource;			// Источник команд для режима ДУ
	TReverseType	*ReverseType;		// Тип реверса
	TMpuControl		 Mpu;				// Местное управление
	TTeleControl	 Tu;				// Телеуправление
	TSmoothControl	 Smooth;			// Плавное регисрование
	TDemoControl	 Demo;				// Демо-режим
	TValveControl	 Valve;				// Управление задвижкой
	TEvLogControl	 EvLog;				// Журнал событий
	TValveCmd		 Command;			// Внутренняя команда
	Uns				 MuDuInput;			// Состояние входа МУ/ДУ
	Uns				 ActiveControls;	// Состояние активности интерфейсов управления
	Uns				 StartDelay;		// Пауза при обработке команды управления
	Uns 			 IgnorComFlag;		// Флаг игнорирования команды управления если уже находимся в крайнем положении
	void (*StopControl)(void);			// Функция останова управления
	void (*StartControl)(TValveCmd ControlWord); // Функция старта управления
} TVlvDrvCtrl;

void ValveDriveUpdate(TVlvDrvCtrl *);
void ValveDriveStop(TVlvDrvCtrl *, Bool Flag);
void ValveDriveMove(TVlvDrvCtrl *, Uns Percent);

#ifdef __cplusplus
}
#endif // extern "C"

#endif



