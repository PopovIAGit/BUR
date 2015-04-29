#ifndef _PULT_H_
#define _PULT_H_


#define PULT_TIME_FREQ				PRD_50HZ
#define PULT_CAP_RESET				(PULT_TIME_FREQ * 0.100)
#define SYNC_PULSES_NUM				11		// 11 ���
#define SYNC_PULSE_VALUE			85e3
#define BUR_IR_ADDRESS				0

typedef struct _TPult 
{
	Bool CapComplete;				// ���� ���������� �������
	Bool FirstPulse;
	Byte SyncPulsesCounter;			// ������� ��������������
	Uns  CapCounter;				// ������� ������� ������� ���������
	Uns  CapCode;					// �������� ���
	Byte KeyCode;					// ��� ������
	Byte Key;						// ������
} TPult;



void PultImpulseCapture(TPult *p);
void PultTimer(TPult *p);
void PultKeyExecute(TPult *p);




#endif
