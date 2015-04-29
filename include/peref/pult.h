#ifndef _PULT_H_
#define _PULT_H_


#define PULT_TIME_FREQ				PRD_50HZ
#define PULT_CAP_RESET				(PULT_TIME_FREQ * 0.100)
#define SYNC_PULSES_NUM				11		// 11 бит
#define SYNC_PULSE_VALUE			85e3
#define BUR_IR_ADDRESS				0

typedef struct _TPult 
{
	Bool CapComplete;				// Флаг завершения захвата
	Bool FirstPulse;
	Byte SyncPulsesCounter;			// Счетчик синхроимульсов
	Uns  CapCounter;				// Счетчик времени захвата импульсов
	Uns  CapCode;					// Принятый код
	Byte KeyCode;					// Код кнопки
	Byte Key;						// Кнопка
} TPult;



void PultImpulseCapture(TPult *p);
void PultTimer(TPult *p);
void PultKeyExecute(TPult *p);




#endif
