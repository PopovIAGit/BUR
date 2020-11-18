#ifndef ENCODER_
#define ENCODER_

#include "std.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ENC_PRD							200						// Период вызова драйвера
#define ENC_STARTUP_LEVEL 				(2.000 * ENC_PRD)		// Задержка при включении энкодера
#define ENC_RESET_DELAY_LEVEL			(0.800 * ENC_PRD)		// Задержка перед сбросом энкодера
#define ENC_RESET_LEVEL					20						// Число возможных сбросов датчика положения
#define ENC_ATMEGA_SPI_DELAY_US			5						// Задержка между посылками по SPI


// Структура для работы с датчиком положения
typedef struct ENCODER {
	Byte   SpiId;           // Идентификатор SPI-порта (0-SPIA, 1-SPIB, ...)
	Uns    SpiBaud;         // Частота синхроимпульсов (расчитанная для конктретного чипа)
	Byte   Count;           // Количество байт/бит данных
	Uns    GrayCode;		// Положение в коде Грея
	LgUns  RevMax;			// Ограничение значения положения
	LgUns  Revolution;      // Значение положения с датчика
	Uns   *RevErrValue;		// Значение процента ошибки/Количество ошибок
	Uns   *RevErrLevel;		// Уровень сбоя датчика
	Byte   State;			// Состояние опроса датчика
	LgUns  RevData;			// Считываемое положение
	Byte   RevMisc;			// Контрольная сумма/Значение максимальной разницы
	Uns    errorCounter;	// Счетчик ошибок
	Uns    RevTimer;		// Счетчик для расчета ошибки
	Uns    ResetCounter;	// Счетчик сброса энкодера
	Uns    goodPosition;	// Последнее хорошее сохраненное положение
	Bool   ResetDelay;		// Флаг задержки сброса энкодера
	Bool   Error;           // Признак сбоя
	Byte   skipDefectFl;	// Флаг, означающий, что произошел "перескок" данных
	Bool   TmpError;		// Признак временного сбоя, который сбрасывается автоматически
	void (*CsFunc)(Byte);   // Функция выбора микросхемы
} ENCODER;

// Прототипы функций
void EncoderUpdate(void);
void FrabaEncoderCalc(ENCODER *);
void EleSyEncoderCalc(ENCODER *);
void AvagoEncoderCalc(ENCODER *);
void AtMegaAvagoEncoderUpdate(ENCODER *);	// Новый метод рассчета ошибки энкодера + обработка энкодера
void EncoderProtectionUpdate(ENCODER *);
void AtMegaAvagoEncoderCalc(ENCODER *);




extern LgUns Revolution;

#ifdef __cplusplus
}
#endif // extern "C"

#endif

