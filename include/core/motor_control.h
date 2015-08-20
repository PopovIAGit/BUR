/*======================================================================
��� �����:          motor_control.h
�����:              ������ �.�.
������ �����:       01.06
���� ���������:		23/06/10
��������:
������ ���������� ����������
======================================================================*/

#ifndef MOTOR_CONTROL_
#define MOTOR_CONTROL_

// ����������� ���������� ������
#include "Inputs.h"				// ��������� ������� ��������
#include "ileg.h"				// ������� ��������� �������� ���������� � ����
#include "sifu_calc.h"			// ������� ����
#include "ApFilter1.h"			// �������������� ������ ������� �������
#include "ApFilter3.h"			// �������������� ������ �������� �������
#include "protectionI2T.h"

// ��������� ���������� � ���������
#define MIN_FREQ_RPM	600	// ����������� �������� � ��/��� �������� ����� ��������
#define POWER_LOST		100	// ����������� ����������
#define SP_POS_LIMIT	400	// ����������� �� ��������� ��������� ��� ������� ��������
#define TORQ_MIN_PR		10	// ����������� �������� � ���������
#define TORQ_MAX_PR		110	// ������������ �������� � ���������
#define TQ_VOLT_MIN		120	// ����������� ���������� ��� ������� �������
#define TQ_VOLT_MAX		260	// ������������ ���������� ��� ������� �������
#define LENGTH_TRQ		72	// ������ ������ ������������� ������� �������

#define OPEN_MASK  0x2
#define CLOSE_MASK 0x8
#define STOP_MASK  0x20 
#define NOMOVE_MASK 0xA

// �������� ������������� ��� ��������
#define PH_EL_DEFAULT { 0, 0, 5, 0, &UR, &US, &UT }

#define DMC_DEFAULT { wmStop, 0, POS_UNDEF, 0, 0 }

#define SIFU_DEFAULT { \
	&UR.CurAngle, \
	&US.CurAngle, \
	&UT.CurAngle, \
	&US.Polarity,  \
	SIFU_NONE, SIFU_CLOSE_ANG, 0, SIFU_MAX_ANG, SIFU_MAX_ANG, 0, 0 \
}

// �������
#define ADC_CONV(Value, Gain, Offset) 	(((LgInt)(Value - Offset) * Gain) >> 13) 
// �������� ������������� ��� ��������
#define URMSF_DEFAULT 	  { 0,0,0,_IQ(1.0/PRD_200HZ),_IQ(0.05),0,0 }		// tf 0.02
#define RMS_FLTR_DEFAULT  { 0, 0, 0, _IQ(DMC_FLTR_TS), _IQ(0.05), 0, 0 }
#define TORQ_FLTR_DEFAULT { 0, 0, 0, 0, 0, 0, _IQ(DMC_TORQF_TS), _IQ(0.02), 0, 0, 0}
#define IMID_FLTR_DEFAULT { 0, 0, 0, 0, 0, 0, _IQ(DMC_TORQF_TS), _IQ(0.02), 0, 0, 0}

// ���������� ���������� ������
extern APFILTER3  URfltr;
extern APFILTER3  USfltr;
extern APFILTER3  UTfltr;
extern APFILTER3  IUfltr;
extern APFILTER3  IVfltr;
extern APFILTER3  IWfltr;
extern APFILTER1  Umfltr;
extern APFILTER3  Imfltr;
extern APFILTER1  Phifltr;
extern APFILTER3  Trqfltr;
extern ILEG_TRN   UR;
extern ILEG_TRN   US;
extern ILEG_TRN   UT;
extern ILEG_TRN   IU;
extern ILEG_TRN   IV;
extern ILEG_TRN   IW;
extern PH_ORDER   PhEl;
extern SIFU       Sifu;
extern TDmControl Dmc;
extern TTorqObs   Torq;

extern Byte DmcRefrState;
extern Uns  Ipr[3];
extern Uns  Imid;
extern Uns  Imidpr;
extern Uns  TorqMaxNm;
extern Uns  LowPowerReset;
extern Uns  ZazorTimer;
extern Uns  TqUmid;
extern Uns  TqImidPr;
extern Uns  TqAnUI;
extern Bool KickModeEnable;
extern Uns BreakFlag;
extern Uns BreakVoltFlag;

void MotorControlInit(void);
void MotorControlUpdate(void);
void DmcIndication1(void);
void DmcIndication2(void);
void DefineCtrlParams(void);
void StopPowerControl(void);
void StartPowerControl(TValveCmd ControlWord);
void ControlMode(void);
void CalibStop(void);
void TorqueCalc(void);
void SpeedCalc(void);
void LowPowerControl(void);
Bool DmcControlRefresh(void);
void CubInit(TCubStr *, TCubConfig *Cfg);
void CubRefresh(TCubStr *, TCubArray *Array);
void CubCalc(TCubStr *);
void PowerCheck(void);
void ProgonModeUpdate (void);

void TqHandControl(void);

#if BUR_M
void NetMomitor(void);
void ContactorControl(TContactorGroup i);
#endif
void MonitorUpdate1(void);

#endif

