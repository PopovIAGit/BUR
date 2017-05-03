
#ifndef __ENCODER_DPMA15__
#define __ENCODER_DPMA15__

#include "std.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ENC_PRD							50						// Период вызова драйвера
#define ENC_STARTUP_LEVEL 				(2.000 * ENC_PRD)		// Задержка при включении энкодера
#define ENC_RESET_DELAY_LEVEL			(0.300 * ENC_PRD)		// Задержка перед сбросом энкодера
#define ENC_RESET_LEVEL					20						// Число возможных сбросов датчика положения
#define ENC_ATMEGA_SPI_DELAY_US			5						// Задержка между посылками по SPI
#define LIR_DA237_CLOCK_DELAY			8						// Время задержки (t1) после чип-селекта, перед началом генерации клоков (10,4 мкСек)

// Статус энкодера
/*typedef union {
	Uns all;
	struct {
		Uns stop:1;			// 0  Стоп
		Uns move:1;			// 1  Движение
		Uns closed:1;		// 2  Закрыто
		Uns opened:1;		// 3  Открыто
	    Uns direction:1;	// 4  0 - увеличение, 1 - уменьшение
	    Uns error:1;    	// 5  Авария
	    Uns calibrated:1;   // 6  0 - не откалиброван, 1 - откалиброван
    	Uns rsvd:9;		// 7-15  Резерв
	} bit;
} TEncStatus;*/

// Структура для работы с датчиком положения
typedef struct EN_DPMA15 {
	Byte   	   SpiId;           // Идентификатор SPI-порта (0-SPIA, 1-SPIB, ...)
	Uns        SpiBaud;         // Частота синхроимпульсов (расчитанная для конктретного чипа)
	Byte       bitCount;        // Количество байт/бит данных
	Uns      RevMax;			// Ограничение значения положения
	Uns      revolution;      // Значение положения с датчика в формате 16 разрядов
	Uns      EncoderData;		// Данные, получаемые от энкодера
	Uns       *RevErrValue;		// Значение процента ошибки/Количество ошибок
	Uns       *RevErrLevel;		// Уровень сбоя датчика
	Uns      prevRevolution;	// Предыдущее значение энкодера
	Byte       RevMisc;			// Контрольная сумма/Значение максимальной разницы
	Uns        RevErrCount;		// Счетчик ошибок
	Uns        RevTimer;		// Счетчик для расчета ошибки
	Uns        ResetCounter;	// Счетчик сброса энкодера
	Bool       ResetDelay;		// Флаг задержки сброса энкодера
	Bool       Error;           // Признак сбоя
	Bool       TmpError;		// Признак временного сбоя, который сбрасывается автоматически
	void (*CsFunc)(Byte);   // Функция выбора микросхемы
} EN_DPMA15;

// Прототипы функций
void encoder_DPMA15_GetData(EN_DPMA15 *);
//void LIR_DA237T_PositionCalc(ENCODER *);

#ifdef __cplusplus
}
#endif // extern "C"

#endif


