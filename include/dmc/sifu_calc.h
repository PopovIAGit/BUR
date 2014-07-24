/*======================================================================
��� �����:         	sifu_calc.h
�����:             	������ �.�.
������ �����:      	01.02
���� ���������:		12/03/10
��������:
���������� ������� ���������-�������� ���������� ��� ���
======================================================================*/

#ifndef SIFU_CALC_
#define SIFU_CALC_

#ifdef __cplusplus
extern "C" {
#endif

#include "std.h"

#define SIFU_NONE		0
#define SIFU_DOWN		1
#define SIFU_UP			2
#define SIFU_DYN_BREAK	3
/*
#define SIFU_UR			2
#define SIFU_US			1
#define SIFU_UT			0
#define SIFU_UT_REV		3
#define SIFU_US_REV		4
#define SIFU_EN_TRN		5

#define SIFU_UR_REV		4
*/
#define SIFU_UR			0
#define SIFU_US			1
#define SIFU_UT			2
#define SIFU_UR_REV		3
#define SIFU_UT_REV		4


#define SIFU_EN_TRN		5


#define SIFU_OPEN_ANG	0
#define SIFU_CLOSE_ANG	180
#define SIFU_MAX_ANG	180

typedef struct {
	Uns  *UrAngle;			// ����:       ��������� �� ���� ���������� ���� R
	Uns  *UsAngle;			// ����:       ��������� �� ���� ���������� ���� S
	Uns  *UtAngle;			// ����:       ��������� �� ���� ���������� ���� T
	Int  *Polarity;			// ����:       ��������� �� ���������� ����������
	Uns   Direction;		// ����:			����������� ���������
	Uns   SetAngle;			// ����:       �������� ���� ��������
	Uns	  Status;			// �����:		��������� ����������� �����
	Uns   MaxAngle;			// ��������:   ������������ ���� ��������
	Uns	  OpenAngle;		// ����������: ������� ���� ��������
	Uns   AccelTime;		// ��������:   �������� ������������� ���� ��������
	Uns   AccelTimer;		// ����������: C������ ���������
} SIFU;

void sifu_calc(SIFU *);
void sifu_calc2(SIFU *);

#ifdef __cplusplus
}
#endif // extern "C"

#endif


