#ifndef DAC7513_
#define DAC7513_

#include "std.h"

#ifdef __cplusplus
extern "C" {
#endif

// Режим вывода
#define DAC7513_OFF     0     // Вывод отключен (на выходе 0 мА)
#define DAC7513_DIRECT  1     // Прямое задание значений
#define DAC7513_CURR    2     // Вывод по заданию тока
#define DAC7513_PR_0_25 3     // Вывод по заданию процента в диапазоне тока 0-25 мА
#define DAC7513_PR_4_20 4     // Вывод по заданию процента в диапазоне тока 4-20 мА

// Режимы энергосбережения
#define	DAC7513_NORMAL  0x00  // Нормальный режим
#define	DAC7513_MODE1   0x01  // Режим Power Down (подтяжка к земеле через 1кОм)
#define	DAC7513_MODE2   0x02  // Режим Power Down (подтяжка к земеле через 100кОм)
#define	DAC7513_MODE3   0x03  // Режим Power Down (выход в Z-состояние)

// Структура параметров для работы с ЦАП
typedef struct DAC7513 {
	Byte  SpiId;        		// Идентификатор SPI-порта (0-SPIA, 1-SPIB, ...)
	Uns   SpiBaud;      		// Частота синхроимпульсов (расчитанная для конктретного чипа)
	Byte  Mode;         		// Режим вывода
  	Byte  PowerMode;    		// Режим энергосбережения
	Int  *Value;        		// Задание выходной величины
	Bool *BusyCsFlag;			// Флаг ожидания chip-select'а, чтобы разрулить с ATMega
  	Uns   Gain;         		// Коэффициент усиления (Q12)
  	Int   Offset;       		// Коэффициент смещения
  	Uns   Data;         		// Значение передаваемое в микросхему
	void (*CsFunc)(Byte);		// Функция выбора микросхемы
} DAC7513;

void DAC7513_Update(DAC7513 *);

#ifdef __cplusplus
}
#endif // extern "C"

#endif



