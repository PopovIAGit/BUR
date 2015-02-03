#ifndef DS3234_
#define DS3234_

#include "std.h"

#ifdef __cplusplus
extern "C" {
#endif

// Описание регистров микросхемы
#define DS3234_SEC           0x00 // Секунда
#define DS3234_MIN           0x01 // Минута
#define DS3234_HOUR          0x02 // Час
#define DS3234_DAY           0x03 // День недели
#define DS3234_DATE          0x04 // День месяца
#define DS3234_MONTH         0x05 // Месяц
#define DS3234_YEAR          0x06 // Год

#define DS3234_CONTROL_REG   		0x0E // Регистр управления
#define DS3234_CONTROL_STATUS_REG	0x0F // Статусный регистр
#define DS3234_TEMP_MSB				0x11
#define DS3234_TEMP_LSB				0x12
#define DS3234_DISTEMPCONV			0x13

// Константы режима изменения часа (AM/PM)
#define DS3234_AM_PM         0x20 // Текущий режим (0 - AM, 1 - PM)
#define DS3234_AM_PM_MODE    0x40 // Наличие режима

// Маски адреса/кода микросхемы
#define DS3234_WRITE_MASK    0x80 // Запись данных
#define DS3234_ALARM_MASK    0x80 // Информация по сигналу

// Описание битов регистра управления
/*
#define DS1305_AIE0          0x01 // Наличие прерывания по сигналу 0
#define DS1305_AIE1          0x02 // Наличие прерывания по сигналу 1
#define DS1305_INTCN         0x04 // Режим активация прерывания по сигналу
*/
#define DS3234_EOSC          0x80 // Состояние запуска осциллятора
#define DS3234_BSY			 0x04
#define DS3234_BBTD			 0x01
#define DS3234_CONV			 0x20


// Структура параметров для работы с ЧРВ
typedef struct DS3234 {
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
} DS3234;

void DS3234_Init(DS3234 *);
Byte DS3234_Read(DS3234 *, Byte Addr);
void DS3234_Write(DS3234 *, Byte Addr, Byte Data);
void DS3234_Update(DS3234 *);

void DS3234_TempConv(DS3234 *p);
void DS3234_ReadTemp(DS3234 *p);


#ifdef __cplusplus
}
#endif // extern "C"

#endif




