#ifndef _AT25XXX_H_
#define _AT25XXX_H_

#include "std.h"

// Биты статусного регистра
#define AT25XXX_RDY    0x01
#define AT25XXX_WEN    0x02
#define AT25XXX_BP0    0x04
#define AT25XXX_BP1    0x08
#define AT25XXX_WPEN   0x80


typedef struct AT25XXX {
	Byte  SpiId;					// Идентификатор SPI канала (SPIA, SPIB, ...)
	Uns   SpiBaud;					// Частота синхроимпульсов
	Uns   Addr;						// Адрес
	Byte  Func;						// Функция
	Uns   Count;					// Количество байт данных
	Byte *Buffer;					// Указатель на буфер данных
	Byte  RdData;					// Текущий считанный байт
	Byte  WrData;					// Текущий записанный байт
	Byte  State;					// Текущий шаг выполнения
	Bool  Error;					// Флаг ошибки в работе памяти
	Byte  RetryTimer;				// Таймер повторов перезаписи
	Byte  RetryCount;				// Количество повторов перезаписи
	Uns   BusyTimer;				// Таймер окончания записи
	Uns   BusyTime;					// Максимальное время записи
	void (*CsFunc)(Byte);			// Функция chip select'а
} AT25XXX;

void AT25XXX_Init(AT25XXX *);							// Инициализация
void AT25XXX_Update(AT25XXX *);							// 
Byte AT25XXX_ReadStatus(AT25XXX *);						// Чтение статусного регистра
void AT25XXX_WriteStatus(AT25XXX *, Byte Status);		// Запись статусного регистра
void AT25XXX_WaitBusy(AT25XXX *);						// Ожидание готовности памяти
Byte AT25XXX_ReadByte(AT25XXX *, Uns Addr);				// Чтение байта
void AT25XXX_WriteByte(AT25XXX *, Uns Addr, Byte Data);	// Запись байта
void AT25XXX_WriteEnable(AT25XXX *);					// Разрешение записи
void AT25XXX_WriteDisable(AT25XXX *);					// Запрещение записи


#endif
