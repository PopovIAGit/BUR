/*======================================================================
��� �����:          ProgOutput.h
�����:              ������ �.�.
������ �����:       01.01
���� ���������:		03/03/10
�������������:      ��� ���� �����
��������:
���������� ������� ��������������� �������
======================================================================*/

#ifndef PROG_OUTPUT_
#define PROG_OUTPUT_

#include "std.h"

#ifdef __cplusplus
extern "C" {
#endif

// ������� ��� ���������������� ����������� ������
typedef enum {
  ocEqual=0,           // �����
  ocNotEqual,          // �� �����
  ocGreat,             // ������
  ocLess,              // ������
  ocGreatOrEqual,      // ������ ��� �����
  ocLessOrEqual,       // ������ ��� �����
  ocBitSet,            // ��� ����������
  ocBitClear,          // ��� �������
  ocAllBitsSet,        // ��� ���� �����������
  ocAllBitsClear       // ��� ���� ��������
} TProgOutCondition;

// ������������ ���������������� ����������� ������
typedef struct _TProgOutConfig {
  Uns Addr;       					// �����
  Uns Value;      					// ��������
  TProgOutCondition Condition;  // �������
} TProgOutConfig;

// ��������� ��� ��������� ������� 
typedef struct PROG_OUT {
	Bool Enable;				// ���������� ������
	TProgOutConfig	*Config;	// ��������� �� ������������
	Uns *Data;					// ��������� �� ����� ������
	Uns *Output;				// ��������� �� ������� ���������
	Uns  OutputBit;			// ����� ���� ��� ������
	Uns  ActiveLev;			// �������� ������� ������
} PROG_OUT;

void ProgOutCalc(PROG_OUT *);

#ifdef __cplusplus
}
#endif // extern "C"

#endif





