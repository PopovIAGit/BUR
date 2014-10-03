/*======================================================================
Имя файла:          ds1305.h
Автор:              Саидов В.С.
Версия файла:       01.03
Дата изменения:		19/02/10
Применяемость:      Совместно с библиотекой CSL
Описание:
Драйвер часов реального времени (микросхема DS1305)
======================================================================*/

#ifndef DS1305_
#define DS1305_

#include "std.h"

#ifdef __cplusplus
extern "C" {
#endif

// Описание регистров микросхемы
#define DS1305_SEC           0x00 // Секунда
#define DS1305_MIN           0x01 // Минута
#define DS1305_HOUR          0x02 // Час
#define DS1305_DAY           0x03 // День недели
#define DS1305_DATE          0x04 // День месяца
#define DS1305_MONTH         0x05 // Месяц
#define DS1305_YEAR          0x06 // Год
#define DS1305_SEC_ALARM0    0x07 // Секунда для сигнала 0
#define DS1305_MIN_ALARM0    0x08 // Минута для сигнала 0
#define DS1305_HOUR_ALARM0   0x09 // Час для сигнала 0
#define DS1305_DAY_ALARM0    0x0A // День недели для сигнала 0
#define DS1305_SEC_ALARM1    0x0B // Секунда для сигнала 1
#define DS1305_MIN_ALARM1    0x0C // Минута для сигнала 1
#define DS1305_HOUR_ALARM1   0x0D // Час для сигнала 1
#define DS1305_DAY_ALARM1    0x0E // День недели для сигнала 1
#define DS1305_CONTROL_REG   0x0F // Регистр управления
#define DS1305_STATUS_REG    0x10 // Статусный регистр
#define DS1305_TRICK_REG     0x11 // Регистр настройки подзарядки
#define DS1305_RAM_BEGIN     0x20 // Начальный адрес внутреннего ОЗУ
#define DS1305_RAM_END       0x7F // Конечный адрес внутреннего ОЗУ

// Константы режима изменения часа (AM/PM)
#define DS1305_AM_PM         0x20 // Текущий режим (0 - AM, 1 - PM)
#define DS1305_AM_PM_MODE    0x40 // Наличие режима

// Маски адреса/кода микросхемы
#define DS1305_WRITE_MASK    0x80 // Запись данных
#define DS1305_ALARM_MASK    0x80 // Информация по сигналу

// Описание битов регистра управления
#define DS1305_AIE0          0x01 // Наличие прерывания по сигналу 0
#define DS1305_AIE1          0x02 // Наличие прерывания по сигналу 1
#define DS1305_INTCN         0x04 // Режим активация прерывания по сигналу
#define DS1305_WP            0x40 // Состояние защиты от записи
#define DS1305_EOSC          0x80 // Состояние запуска осциллятора

// Описание битов регистра статуса
#define DS1305_IRQF0         0x01 // Флаг прерывания по сигналу 0
#define DS1305_IRQF1         0x02 // Флаг прерывания по сигналу 1

// Описание битов регистра настройки подзарядки
#define DS1305_TC_DISABLE    0x00 // Отключение подзарядки
#define DS1305_TC_1D_2K      0xA5 // 1 Диод,  2кОм
#define DS1305_TC_1D_4K      0xA6 // 1 Диод,  4кОм
#define DS1305_TC_1D_8K      0xA7 // 1 Диод,  8кОм
#define DS1305_TC_2D_2K      0xA9 // 2 Диода, 2кОм
#define DS1305_TC_2D_4K      0xAA // 2 Диода, 4кОм
#define DS1305_TC_2D_8K      0xAB // 2 Диода, 8кОм

// Структура параметров для работы с ЧРВ
typedef struct DS1305 {
	Byte  SpiId;      		// Идентификатор SPI-порта (0-SPIA, 1-SPIB, ...)
	Uns   SpiBaud;    		// Частота синхроимпульсов (расчитанная для конктретного чипа)
	Byte *DataBuf;    		// Указатель на буфер данных времени и даты
	Byte  State;				// Состояние опроса часов
	Byte  Addr;					// Текущий адрес
	Byte  Data;					// Текущие данные
	Bool  Flag;       		// Флаг записи времени и даты
	Bool  Busy;					// Флаг ожидания записи
	Bool  Error;      		// Ошибка запуска осциллятора
	void (*CsFunc)(Byte ); 	// Функция выбора микросхемы
} DS1305;

void DS1305_Init(DS1305 *);
Byte DS1305_Read(DS1305 *, Byte Addr);
void DS1305_Write(DS1305 *, Byte Addr, Byte Data);
void DS1305_Update(DS1305 *);

#ifdef __cplusplus
}
#endif // extern "C"

#endif




