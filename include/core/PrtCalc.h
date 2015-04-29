/*======================================================================
��� �����:          PrtCalc.h
�����:              ������ �.�.
������ �����:       01.00
���� ���������:		15/04/10
�������������:      ��� ���� �����
��������:
���������� ���������� ��������� ������ �����
======================================================================*/

#ifndef PRTCALC_
#define PRTCALC_

#include "std.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PRT_CFG_SET(Mode, Level, Bit, Hyst) \
	((Mode<<1)|(Level<<2)|(Bit<<3)|(Hyst<<7))

typedef union {
	Uns all;
	struct
	{
		Uns Enable:1;			// 0		���������� ������
		Uns Mode:1;				// 1		����� ������
		Uns Level:1;			// 2		�������� �������
		Uns Num:4;				//	3-6	����� ���� � �������� ������
		Uns Hyst:9;				// 7-15  �������� �����������
	} bit;
} TPrtCfg;

typedef struct {
	TPrtCfg Cfg;			// ������������ ������
	Int    *Input;			// ����
	Uns    *Output;		// �����
	Int    *Level;			// ������� ������������
	Uns    *Timeout;		// ����-��� ������������
	Uns     Scale;			// ����������� ��������������� �������
	Uns     Timer;			// ������ ������������
} TPrtElem;

void PrtCalc(TPrtElem *);

#ifdef __cplusplus
}
#endif // extern "C"

#endif





