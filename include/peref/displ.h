/*======================================================================
Имя файла:          displ.h
Автор:              Саидов В.С.
Версия файла:       01.03
Дата изменения:		03/03/10
Применяемость:      Совместно с библиотекой CSL
Описание:
Драйвер двухстрочного индикатора
======================================================================*/

#ifndef DISPL_
#define DISPL_

#include "std.h"

#ifdef __cplusplus
extern "C" {
#endif

// Структура для работы с индикатором
typedef struct DISPL {
	Byte   SpiId;        // Идентификатор SPI-порта (0-SPIA, 1-SPIB, ...)
	Uns    SpiBaud;      // Частота синхроимпульсов (расчитанная для конктретного чипа)
	Bool   Enable;       // Включение/выключение индикатора
	Bool   Status;			// Текущее состояние работы
	Bool   Restart;      // Сброс индикатора
	Uns    ResTimer; 		// Таймер для сброса
	Uns    ResTout;  		// Тайм-аут сброса
	Uns    PauseTimer;	// Таймер для формирования паузы
	Byte   State;        // Текущее состояние обновления данных
	Byte   CursorPos;    // Позиция для курсора
	Byte   CursorAddr;	// Адрес для добавления символов
	Char   HiStr[17];    // Буфер верхней строчки индикатора
	Char   LoStr[17];    // Буфер нижней строчки индикатора
	Char  *Data;         // Указатель на выводимый байт данных
	IOpin  EN;           // Ножка для формирования строба
	IOpin  RS;           // Ножка выбора команда/данные
	IOpin  RW;           // Ножка выбора чтение/запись
	IOport Port;         // Порт данных
	void (*CsFunc)(Byte);// Функция выбора микросхемы
} DISPL;

__inline void DISPL_ClrHiStr(DISPL *p) {memset(p->HiStr, ' ', 16); p->HiStr[16] = '\0';}
__inline void DISPL_ClrLoStr(DISPL *p) {memset(p->LoStr, ' ', 16); p->LoStr[16] = '\0';}

__inline void DISPL_PutHiStr(DISPL *p, Ptr Adr) {PFUNC_blkRead(Adr, (Ptr)p->HiStr, 16);}
__inline void DISPL_PutLoStr(DISPL *p, Ptr Adr) {PFUNC_blkRead(Adr, (Ptr)p->LoStr, 16);}

void DISPL_Update(DISPL *);
void DISPL_AddSymb(DISPL *, Byte Addr, Ptr Data, Byte Count);

#ifdef __cplusplus
}
#endif // extern "C"

#endif





