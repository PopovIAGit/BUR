#ifndef ENCODER_
#define ENCODER_

#include "std.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ENC_PRD							200						// ������ ������ ��������
#define ENC_STARTUP_LEVEL 				(2.000 * ENC_PRD)		// �������� ��� ��������� ��������
#define ENC_RESET_DELAY_LEVEL			(0.800 * ENC_PRD)		// �������� ����� ������� ��������
#define ENC_RESET_LEVEL					20						// ����� ��������� ������� ������� ���������
#define ENC_ATMEGA_SPI_DELAY_US			5						// �������� ����� ��������� �� SPI


// ��������� ��� ������ � �������� ���������
typedef struct ENCODER {
	Byte   SpiId;           // ������������� SPI-����� (0-SPIA, 1-SPIB, ...)
	Uns    SpiBaud;         // ������� ��������������� (����������� ��� ������������ ����)
	Byte   Count;           // ���������� ����/��� ������
	Uns    GrayCode;		// ��������� � ���� ����
	LgUns  RevMax;			// ����������� �������� ���������
	LgUns  Revolution;      // �������� ��������� � �������
	Uns   *RevErrValue;		// �������� �������� ������/���������� ������
	Uns   *RevErrLevel;		// ������� ���� �������
	Byte   State;			// ��������� ������ �������
	LgUns  RevData;			// ����������� ���������
	Byte   RevMisc;			// ����������� �����/�������� ������������ �������
	Uns    errorCounter;	// ������� ������
	Uns    RevTimer;		// ������� ��� ������� ������
	Uns    ResetCounter;	// ������� ������ ��������
	Uns    goodPosition;	// ��������� ������� ����������� ���������
	Bool   ResetDelay;		// ���� �������� ������ ��������
	Bool   Error;           // ������� ����
	Byte   skipDefectFl;	// ����, ����������, ��� ��������� "��������" ������
	Bool   TmpError;		// ������� ���������� ����, ������� ������������ �������������
	void (*CsFunc)(Byte);   // ������� ������ ����������
} ENCODER;

// ��������� �������
void EncoderUpdate(void);
void FrabaEncoderCalc(ENCODER *);
void EleSyEncoderCalc(ENCODER *);
void AvagoEncoderCalc(ENCODER *);
void AtMegaAvagoEncoderUpdate(ENCODER *);	// ����� ����� �������� ������ �������� + ��������� ��������
void EncoderProtectionUpdate(ENCODER *);
void AtMegaAvagoEncoderCalc(ENCODER *);




extern LgUns Revolution;

#ifdef __cplusplus
}
#endif // extern "C"

#endif

