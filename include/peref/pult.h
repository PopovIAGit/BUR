#ifndef _PULT_H_
#define _PULT_H_


#define PULT_TIME_FREQ				PRD_50HZ
#define PULT_CAP_RESET				(PULT_TIME_FREQ * 0.100)
#define SYNC_PULSES_NUM				11		// 11 бит
#define SYNC_PULSE_VALUE			85e3
#define BUR_IR_ADDRESS				0
#define POST_FRAME_TIMEOUT			899		// 100 мс (18 кГц)
#define PRE_FRAME_TIMEOUT			450		// 50 мс

typedef struct _TPult 
{
	Bool CapComplete;				// Флаг завершения захвата пакета данных
	Bool isFrameRecieved;			// Флаг захвата 11 битов данных
	Bool FirstPulse;
	Bool ReadyToRecieve;			// Флаг готовности к приему сигнала
	Byte SyncPulsesCounter;			// Счетчик синхроимульсов
	Uns  CapCounter;				// Счетчик времени захвата импульсов
	Uns  CapCode;					// Принятый код
	Uns  postFrameTimer;			// Таймер выдержки фронта после получения пакета
	Uns  preFrameTimer;				// Таймер выдержки перед приемом данных
	Byte KeyCode;					// Код кнопки
	Byte Key;						// Кнопка
} TPult;



void PultImpulseCapture(TPult *p);
void PultTimer(TPult *p);
void PultKeyExecute(TPult *p);

void IrDA_NoiseFilter(TPult *p);	// Функция фильтрации шумов ИК-приемника




#endif
