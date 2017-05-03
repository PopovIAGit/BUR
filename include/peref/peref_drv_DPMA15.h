
#ifndef __ENCODER_DPMA15__
#define __ENCODER_DPMA15__

#include "std.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ENC_PRD							50						// ������ ������ ��������
#define ENC_STARTUP_LEVEL 				(2.000 * ENC_PRD)		// �������� ��� ��������� ��������
#define ENC_RESET_DELAY_LEVEL			(0.300 * ENC_PRD)		// �������� ����� ������� ��������
#define ENC_RESET_LEVEL					20						// ����� ��������� ������� ������� ���������
#define ENC_ATMEGA_SPI_DELAY_US			5						// �������� ����� ��������� �� SPI
#define LIR_DA237_CLOCK_DELAY			8						// ����� �������� (t1) ����� ���-�������, ����� ������� ��������� ������ (10,4 �����)

// ������ ��������
/*typedef union {
	Uns all;
	struct {
		Uns stop:1;			// 0  ����
		Uns move:1;			// 1  ��������
		Uns closed:1;		// 2  �������
		Uns opened:1;		// 3  �������
	    Uns direction:1;	// 4  0 - ����������, 1 - ����������
	    Uns error:1;    	// 5  ������
	    Uns calibrated:1;   // 6  0 - �� ������������, 1 - ������������
    	Uns rsvd:9;		// 7-15  ������
	} bit;
} TEncStatus;*/

// ��������� ��� ������ � �������� ���������
typedef struct EN_DPMA15 {
	Byte   	   SpiId;           // ������������� SPI-����� (0-SPIA, 1-SPIB, ...)
	Uns        SpiBaud;         // ������� ��������������� (����������� ��� ������������ ����)
	Byte       bitCount;        // ���������� ����/��� ������
	Uns      RevMax;			// ����������� �������� ���������
	Uns      revolution;      // �������� ��������� � ������� � ������� 16 ��������
	Uns      EncoderData;		// ������, ���������� �� ��������
	Uns       *RevErrValue;		// �������� �������� ������/���������� ������
	Uns       *RevErrLevel;		// ������� ���� �������
	Uns      prevRevolution;	// ���������� �������� ��������
	Byte       RevMisc;			// ����������� �����/�������� ������������ �������
	Uns        RevErrCount;		// ������� ������
	Uns        RevTimer;		// ������� ��� ������� ������
	Uns        ResetCounter;	// ������� ������ ��������
	Bool       ResetDelay;		// ���� �������� ������ ��������
	Bool       Error;           // ������� ����
	Bool       TmpError;		// ������� ���������� ����, ������� ������������ �������������
	void (*CsFunc)(Byte);   // ������� ������ ����������
} EN_DPMA15;

// ��������� �������
void encoder_DPMA15_GetData(EN_DPMA15 *);
//void LIR_DA237T_PositionCalc(ENCODER *);

#ifdef __cplusplus
}
#endif // extern "C"

#endif


