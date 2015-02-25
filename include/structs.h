/*======================================================================
��� �����:          structs.h
�����:              ������ �.�.
������ �����:       01.08
���� ���������:		23/06/10
��������:
�������� ����� � ��������
========================================================================*/

#ifndef STRUCTS_
#define STRUCTS_

#include "std.h"

#ifdef __cplusplus
extern "C" {
#endif

// ������ ������
#define STATUS_RESET_MASK	0x2082
#define STATUS_EVLOG_MASK	0x56F
#define STATUS_MOVE_MASK	0xC
typedef union _TStatusReg {
	Uns all;
	struct {
		Uns Stop:1;			// 0     ����
		Uns Fault:1;		// 1     ������
     	Uns Closing:1;    	// 2     ���� ��������
     	Uns Opening:1;		// 3     ���� ��������
    	Uns Test:1;			// 4     ���� ����
     	Uns Closed:1;		// 5     �������
     	Uns Opened:1;		// 6     �������
		Uns Mufta:1;		// 7     �����
		Uns MuDu:1;			// 8     ������� ����������
     	Uns Ten:1;        	// 9     ������� ���
     	Uns Power:1;     	// 10    ��������� �������
		Uns BlkIndic:1;		// 11	 ������ �� �����
		Uns TsIndic:1;		// 12	 ������ �� ��
		Uns Defect:1;		// 13	 �������������
		Uns BlkDefect:1;	// 14 	 ������������� �� �����
		Uns TsDefect:1;		// 15 	 ������������� �� ��
	} bit;
} TStatusReg;

// ����������� ��������
#define PROC_NOMOVE_MASK	0x0001
#define PROC_PHENG_MASK		0x0002
#define PROC_CALIB_MASK		0x001C
#define PROC_OVERW_MASK		0x0020
#define PROC_DRV_T_MASK 	0x0040
#define PROC_RESET_MASK		0x00A1
#define PROCESS_EVLOG_MASK	0x00A1
typedef union _TProcessReg {
	Uns all;
	struct {
		Uns NoMove:1;		// 0     ��� ��������
		Uns PhOrd:1;		// 1     ����������� ��� ���������
     	Uns NoClose:1;		// 2     �� ������ ��������� �������
     	Uns NoOpen:1;	   	// 3     �� ������ ��������� �������
		Uns NoCalib:1;		// 4     ���������� �� ���������
     	Uns Overway:1;		// 5     ���������� �� ����������
		Uns Drv_T:1;		// 6	 �������� ���������
		Uns MuDuDef:1;		// 7     ������ �� ���������� ������ ��/��
		Uns Rsvd:8;			// 8-15  ������
	} bit;
} TProcessReg;

// ����������� ����
#define NET_UV_MASK			0x0007
#define NET_OV_MASK			0x0038
#define NET_PHO_MASK		0x0040
#define NET_VSK_MASK		0x0080
#define NET_BV_MASK			0x0700
#define NET_OV_MAX_MASK		0x3800
#if BUR_M
#define NET_EVLOG_MASK		0x7F7F
#define NET_RTSERR_MASK 	0x4000
#define NET_RESET_MASK      0x38FF
#else
#define NET_EVLOG_MASK		0x3F7F
#endif

typedef union _TNetReg {
	Uns all;
	struct {
		Uns UvR:1;			// 0     ��������� ���������� � ���� R
		Uns UvS:1;			// 1     ��������� ���������� � ���� S
		Uns UvT:1;			// 2     ��������� ���������� � ���� T
		Uns OvR:1;			// 3     ���������� ���������� � ���� R
		Uns OvS:1;			// 4     ���������� ���������� � ���� S
		Uns OvT:1;			// 5     ���������� ���������� � ���� T
		Uns PhOrd:1;		// 6     �������� ����������� ��� ����
		Uns VSk:1;			// 7     ���������� ���������� ������� ����
		Uns BvR:1;			// 8     ����� ���� R ������� ����
		Uns BvS:1;			// 9     ����� ���� S ������� ����
		Uns BvT:1;			// 10    ����� ���� T ������� ����
		Uns OvR_max:1;		// 11    ���������� ���������� � ���� R �� 47%
		Uns OvS_max:1;		// 12    ���������� ���������� � ���� S �� 47%
		Uns OvT_max:1;		// 13    ���������� ���������� � ���� T �� 47%
		#if BUR_M
		Uns RST_Err:1;		// 14   �������� ����������� ������� ��� ��� �
		Uns Rsvd:1;			// 15 ������
		#else
		Uns Rsvd:2;			// 14-15 ������
		#endif
	} bit;
} TNetReg;

// ����������� ��������
#define LOAD_PHL_MASK		0x0007
#define LOAD_I2T_MASK		0x0008
#define LOAD_SHC_MASK		0x00E0
#define LOAD_UNL_MASK		0x0400
#define LOAD_ISK_MASK		0x0800
#define LOAD_RESET_MASK		0x0FE7
#define LOAD_EVLOG_MASK		0x0FE7
typedef union _TLoadReg {
	Uns all;
	struct {
		Uns PhlU:1;			// 0     ����� ���� U
		Uns PhlV:1;			// 1     ����� ���� V
		Uns PhlW:1;			// 2     ����� ���� W
		Uns I2t:1;			// 3     �����-������� ����������
		Uns Rsvd1:1;		// 4     ������
		Uns ShCU:1;			// 5	 �������� ��������� � ���� U
		Uns ShCV:1;			// 6	 �������� ��������� � ���� V
		Uns ShCW:1;			// 7	 �������� ��������� � ���� W
		Uns Rsvd2:3;		// 8-9	 ������
		Uns ISkew:1;		// 11    ���������� ����
		Uns Rsvd:4;			// 12-15 ������
	} bit;
} TLoadReg;

// ����������� ����������
#define DEV_ERR_MASK		0x009F
#define DEV_TMP_MASK		0x0060
#define DEV_RSC_MASK		0x0800
#if BUR_M
#define DEV_EVLOG_MASK		0x0000
#else
#define DEV_EVLOG_MASK		0x0100
#endif
typedef union _TDeviceReg {
	Uns all;
  struct {
		Uns PosSens:1;		// 0     ���� ������� ���������
		Uns Memory1:1;		// 1     ���� ������ 1
		Uns Memory2:1;		// 2     ���� ������ 2
		Uns Rtc:1;			// 3     ���� ����� ��������� �������
		Uns TSens:1;		// 4     ���� ������� �����������
		Uns Th:1;			// 5     �������� �����
		Uns Tl:1;			// 6     �������������� �����
		Uns AVRcon:1;		// 7     ����� �� �����
	#if BUR_M
		Uns Rsvd1:3;		// 8-10  ������
	#else
		Uns LowPower:1;		// 8 	 ����� ���������� �������
		Uns Rsvd1:2;		// 9-10  ������
	#endif
		Uns Th_Err:1;		// 11 	 ����������� ����� ����� 110 ����
     	Uns Rsvd:4;       	// 12-15 ������
  } bit;
} TDeviceReg;


// ���������� ������������ ������
typedef struct {
	TProcessReg Proc;
	TNetReg     Net;
	TLoadReg    Load;
	TDeviceReg  Dev;
} TFltUnion;

// ������ ������
typedef enum {
	pgProcess = 0,			// ������ ������ ��������
	pgNet     = 1,			// ������ ������ ����
	pgLoad    = 2,			// ������ ������ ��������
	pgDevice  = 3,			// ������ ������ ����������
	pgCount   = 4			// ���������� �����
} TPrtGroup;

// ������� ���
typedef enum
{
	bcmNone = 0,				// ��� �������
	bcmStop,					// ����
	bcmClose,					// �������
	bcmOpen,					// �������
	bcmSetDefaultsUser,			// ���������������� ��������� �� ���������
	bcmCalibReset,				// ����� ����������
	bcmPrtReset,				// ����� �����
	bcmLogClear,				// ������� �������
	bcmCycleReset,				// ����� ������
	bcmSetDefaultsFact,			// ��������� ��������� ���������� �� ���������
	bcmDefStop,					// ������� ������� ���� ��� �������, ����� ��� ����������� ����� ����������������� ������
	bcmDiscrOutTest,			// ���� ���������� �������
	bcmDiscrInTest				// ���� ���������� ������	
} TBurCmd;

// ��������� ���������� ������
typedef union _TInputReg {
  Uns all;
	struct {
		Uns Open:1;       	// 0     �������
		Uns Close:1;      	// 1     �������
		Uns Stop:1;       	// 2     ����
		Uns Mu:1;			// 3	 ������� 
		Uns Du:1;			// 4	 ���������
		Uns Rsvd:11;      	// 5-15  ������
	} bit;
} TInputReg;

// ��� �������� �������
typedef enum {
	it24  =  0,		// 0 ��� ������� 24 �
	it220 = 1 		// 1 ��� ������� 220 �
}TInputType;

// ����� ��������� ��� (������� - �������� �����/���������� � ������������)
typedef enum {
	spLinAuto   = 0,		// 0 �������� �����/���������� � ������������
	spFire 		= 1 		// 1 �������
}TSettingPlace;

typedef enum {
	Empty 	  = 0,
	dt100_A25 = 1,
	dt100_A50 = 2,
	dt400_B20 = 3,
	dt400_B50 = 4,
	dt800_V40 = 5,
	dt1000_V20  = 6,
	dt4000_G9   = 7,
	dt4000_G18  = 8,
	dt10000_D6  = 9,
	dt10000_D12 = 10
}TDriveType;

// ����� ���������� ������
typedef union _TInputMask {
	Uns all;
	 struct {
		Uns Open:1;       	// 0     �������
		Uns Close:1;      	// 1     �������
		Uns Stop:1;       	// 2     ����
		Uns Mu:1;			// 3	 ��
		Uns Du:1;			// 4	 ��
		Uns Rsvd:11;      	// 5-15  ������
	 } bit;
} TInputMask;

// ��������� ���������� �������
typedef union _TOutputReg {
	Uns all;
	struct {
	 	Uns Dout0:1;		// 0 - ������
		Uns Dout1:1;		// 1 - �������
		Uns Dout2:1;		// 2 - �������
		Uns Dout3:1;		// 3 - ������
		Uns Dout4:1;		// 4 - �����������/ ���� �������
		Uns Dout5:1;		// 5 - �����������/ ���
		Uns Dout6:1;		// 6 - ������/	���
		Uns Dout7:1;		// 7 - ��/��/ ---
		Uns Dout8:1;		// 8 - --/�������������	
		Uns Dout9:1;		// 9   --/ ���
		Uns Dout10:1;		// 10  --/ ���	
		Uns Rsvd:5;      	// 11-15  ������
	} bit;
} TOutputReg;


// ����� ���������� �������
typedef union _TOutputMask {
	Uns all;
	 struct {
	 	Uns Dout0:1;		// 0 - �������
		Uns Dout1:1;		// 1 - �������
		Uns Dout2:1;		// 2 - �����
		Uns Dout3:1;		// 3 - ������
		Uns Dout4:1;		// 4 - �����������/ ���� �������
		Uns Dout5:1;		// 5 - �����������/ ���
		Uns Dout6:1;		// 6 - ��/�� /	���
		Uns Dout7:1;		// 7 - �������/ ---
		Uns Rsvd:8;      	// 8-15  ������
	 } bit;
} TOutputMask;

/*
// ������� Profibus DP
typedef union _TPbFuncs {
	Uns all;
	struct {
		Uns Rsvd1:1;		// 0		������
		Uns Sync:1;			// 1		Sync supported
		Uns Freeze:1;		// 2		Freeze supported
		Uns Rsvd:13;		// 3-15  ������
	} bit;
} TPbFuncs;*/


// ���������� ������� �����
typedef enum {
  pmOff       = 0,		// ������ ���������
  pmBlkSign   = 1,		// ������������ �� �����
  pmBlkTsSign = 2,		// ������������ �� ����� � ��
  pmSignStop  = 3,		// ������������ � �������
  pmCount     = 4		// ���������� �������
} TPrtMode;

// ������ ����������
typedef enum {
  csNone  = 0,				// ������ �� ����������
  csClose = 1,				// ������ ������ �������
  csOpen  = 2,				// ������ ������ �������
  csCalib = 3				// ������ ����������
} TCalibState;

// ��� �����
typedef enum {
  rdtNormal  = 0,			// ����������
  rdtInverse = 1			// ���������
} TRodType;

// ������� �������/������
typedef enum {
  trNone     = 0,			// ��� �������
  trTask     = 1,			// ������
  trReset    = 2 			// ��������
} TTaskReset;

// ���������� ��������� �����
typedef enum {
  nsOpened = 0,				// ���������-�������� �������
  nsClosed = 1				// ���������-�������� �������
} TNormState;

// �������� ����� ModBus
typedef enum {
  br2400   = 0,				// 2400 ���
  br4800   = 1,				// 4800 ���
  br9600   = 2,				// 9600 ���
  br19200  = 3,				// 19200 ���
  br38400  = 4,				// 38400 ���
  br57600  = 5,				// 57600 ���
  br115200 = 6				// 115200 ���
} TBaudRate;

// ����� �����
typedef enum {
  pmNone = 0,				// ��� �������� ��������, 2 ���� ����
  pmOdd  = 1,				// �������� �� ����������, 1 ���� ���
  pmEven = 2				// �������� �� ��������, 1 ���� ���
} TParityMode;


// �������� ����� Profibus DP
typedef enum {
	pbrAuto  = 0,			// ���� �����������
	pbr9_6   = 1,			// 9.6 ����/�
	pbr19_2  = 2,			// 19.2 ����/�
	pbr93_75 = 3,			// 93.75 ����/�
	pbr187_5 = 4,			// 187.5 ����/�
	pbr500   = 5,			// 500 ����/�
	pbr1500  = 6,			// 1500 ����/�
	pbr3000  = 7,			// 3000 ����/�
	pbr6000  = 8,			// 6000 ����/�
	pbr12000 = 9,			// 12000 ����/�
	pbr31_25 = 10,			// 31.25 ����/�
	pbr45_45 = 11			// 45.45 ����/�
} TPbBaudRate;

// ����� ��������� ����
typedef enum {
  imRms     = 0,		  	// ����������� ��������
  imPercent = 1			  	// � ��������� �� ������������ ���� ���������
} TIndicMode;

// ��� ����������
typedef enum {
	it_WINSTAR = 0,	// OLED WINSTAR
	it_FUTABA		// VAC FUTABA
} TIndicType;

// ��������� ����������� �����
typedef union _TLedsReg {	// ���
	Uns all;
	struct {
		Uns Opened:1;     	// 0     �������
		Uns Muff:1;      	// 1     �����
		Uns Defect:1;       	// 2     ���������������
		Uns Closed:1;     	// 4     �������
		Uns Fault:1;     	// 3     ������
		Uns Pdu:1;			// 5	 ���
		Uns MuDu:1;       	// 6     �����
		Uns DspOn:1;		// 7	 DSP_ON
		Uns Rsvd:8;      	// 6-15  ������
	} bit;
} TLedsReg;

// ��������� �������� ����� �����
typedef union _THallBlock {
	Uns all;
	struct {
		Uns Open:1;       // 0     �������
		Uns Close:1;      // 1     �������
		Uns StopMU:1;     // 2     ����/��
		Uns StopDU:1;     // 3     ����/��
		Uns Rsvd:12;      // 4-15  ������
	} bit;
} THallBlock;  

// ���������� ������������ ��� ��� �
typedef enum {
	cgOff = 0,
	cgStop = 1,
	cgOpen = 2,
	cgClose = 3
} TContactorGroup;

// ��� �������� ����/�����
typedef enum {
  vtKlin     = 0,		  	// �������� ��������
  vtShiber   = 1			// �������� ��������
} TValveType;

// ��� ������ ����
typedef enum {
	tmControl 		= 0, 	// ���������� �� ��������
	tmTimerControl	= 1, 	// ���������� �� �������� � ��������� �������
	tmAlwaysOn 		= 2,	// ������ �������
	tmHandControl 	= 3		// ������ ����������
}TTenMode;

// ������ ������ ��� �������� ���������� ��������
#define wmMuffEnable	(wmUporStart|wmMove|wmPause|wmUporFinish)
#define wmPhlDisable	(wmPause|wmTestPh)
#define wmShCDisable	(wmPause|wmKick)
typedef enum {
	wmStop       = 0x1,					// ����� ����
	wmTestPh     = 0x2,					// ����� ������������ ���, ���� �������� ���������� 115 ����
	wmUporStart  = 0x4,					// ����� ����������� ������� ��� ������ (����)
	wmMove       = 0x8,					// ����� ��������, ���� �������� ���������� 0 ����
	wmPause      = 0x10,				// ����� ����� ����� ��������, ���� �������� 140 ����
	wmUporFinish = 0x20,				// ����� ����������� ������� ����� ������, ����������� ������� � ����� KickMode
	wmKick       = 0x40,				// ����� �������� �������
	wmSpeedTest  = 0x80,				// ����� �����, ���� �������� �������� �������������
	wmDynBrake   = 0x100,				// ����� ������������� ����������
	wmTestThyr   = 0x200				// ����� ����� ����������
} TWorkMode;

// ��������� ��� ��������� ���������� ��������
typedef struct _TDmControl {
	TWorkMode WorkMode;					// ����� ������
	Int		 RequestDir;				// �������� ����������� ��������
	LgInt	 TargetPos;					// ������� ���������
	Uns		 TorqueSet;					// ������� �������
	Uns		 TorqueSetPr;				// ������� ������� � %
} TDmControl;

// ��������� ��� ������ � ���
typedef struct _TPduData {
	Uns Read;            // ������ ������
	Uns Write;           // ������ ������
	Uns Rsvd;            // ������
	Uns Type;            // ��� ����
	Uns Key;             // ��� �������
} TPduData;

// ��������� ��� ������ � �����
typedef struct {
	Int X;
	Int Y;
	Int Z;
} TCubPoint;

#define CUB_COUNT1		4
#define CUB_COUNT2		5
typedef struct {
	Int Data[CUB_COUNT1][CUB_COUNT2];
} TCubArray;

typedef struct {
	Int *X_Value;
	Int *X_Array;
	Int *Y_Value;
	Int *Y_Array;
} TCubConfig;

typedef struct {
	TCubPoint  Input;
	Int        Output;
	Uns        Num1;
	Uns        Num2;
	Int        PtR;
	Int        PtT;
	Int       *InputX;
	Int       *InputY;
	TCubPoint  Min;
	TCubPoint  Max;
	TCubPoint  Points[CUB_COUNT1][CUB_COUNT2];
} TCubStr;

// ��������� ��� ������� ������
typedef struct {
	Bool    ObsEnable;		// ���������� �������
	Uns     SetAngle;		// ������� ���� �� ����
	Uns     Indication;		// ��������� �������� �������
	TCubStr Cub1;
	TCubStr Cub2;
	TCubStr Cub3;
} TTorqObs;

// ��������� ��� ���������� �������

// �������� ����
// ��������������� �������
typedef union _TTEK_TechReg
{
	Uns all;
	struct 
	{
		Uns Opened:1;		// 0 		��������� "�������"
		Uns Closed:1;		// 1 		��������� "�������"
		Uns Mufta1:1;		// 2 		� ����� ������ ����� �� ����������� �� �������� ��� ��������
		Uns Mufta2:1;		// 3 		��������� �����
		Uns Rsvd2:3;		// 4-6 		������
		Uns MuDu:1;			// 7 		��/��
		Uns Opening:1;		// 8 		"�����������"
		Uns Closing:1;		// 9 		"�����������"
		Uns Stop:1;			// 10 		����
		Uns Rsvd3:2;		// 11-12 	������
		Uns Ten:1;			// 13; 		������� ���
		Uns Rsvd4:1;		// 14	 	������
		Uns Ready:1;		// 15 		����� � ������
	} bit;
} TTEK_TechReg;

// ������� ��������
typedef union _TTEK_DefReg
{
	Uns all;
	struct 
	{
		Uns I2t:1;		// 0 	�����-������� ������
		Uns ShC:1;		// 1 	�������� ���������
		Uns Drv_T:1;	// 2 	�������� ���������
		Uns Uv:1;		// 3 	���������� ���������� ��� ����
		Uns Phl:1;		// 4 	����� ���� ����������������
		Uns NoMove:1;	// 5 	��� ��������
		Uns Ov:1;		// 6 	�������������� �� �����
		Uns Bv:1;		// 7 	����������� �������� ���������� �� �����
		Uns Rsvd:1;		// 8 	������
		Uns Th:1;		// 9  	�������� �����
		Uns Tl:1;		// 10 	�������������� �����
		Uns Rsvd1:1;	// 11 	������
		Uns PhOrdU:1;	// 12   �������� ����������� ��� ����
		Uns PhOrdDrv:1;	// 13   ����������� ��� ���������
		Uns DevDef:1;	// 14 	������ ���������� (����� ��������� � ��������������)
		Uns Rsvd2:1;	// 15 	������
	} bit;
} TTEK_DefReg;

// ������� ������
typedef union _TTEK_ComReg
{
	Uns all;
	struct 
	{
		Uns Stop:1;				// 0 	 ����
		Uns Open:1;				// 1	 �������
		Uns Close:1;			// 2 	 �������
		Uns Rsvd:2;				// 3-4 	 ������
		Uns PrtReset:1;			// 5 	 ����� �����
		Uns Rsvd1:2;			// 6-7   ������
		Uns EnDiscrInTest:1; 	// 8 	 ��������� ����������� ���������� ������
		Uns DisDiscrInTest:1; 	// 9 	 ��������� ����������� ���������� ������
		Uns EnDiscrOutTest:1;	// 10 	 ��������� ����������� ���������� �������
		Uns DisDiscrOutTest:1; 	// 11 	 ��������� ����������� ���������� �������
		Uns Rsvd2:4;			// 12-15 ������
	} bit;
} TTEK_ComReg;

// ������� ��/��
typedef union _TTEK_Discrete
{
	Uns all;
	struct 
	{
		Uns InOpen:1;			// 0 	 ���� "�������"
		Uns InClose:1;			// 1 	 ���� "�������"
		Uns InStop:1;			// 2 	 ���� "����"
		Uns InMu:1;				// 3 	 ���� "��"
		Uns InDu:1;				// 4 	 ���� "��"
		Uns OutOpened:1;		// 5 	 ����� "�������"
		Uns OutClosed:1;		// 6 	 ����� "�������"
		Uns OutMufta:1;			// 7 	 ����� "�����"
		Uns OutFault:1;			// 8  	 ����� "������"
		Uns OutOpening:1;		// 9 	 ����� "�����������"
		Uns OutClosing:1;		// 10 	 ����� "�����������"
		Uns OutMuDu:1;			// 11    ����� "��/��"
		Uns OutNeispr:1;		// 12	����� "�������������"
		Uns Rsvd:1;				// 13 	������
		Uns IsDiscrOutActive:1;	// 14 	��������� ������ ������������ �������
		Uns IsDiscrInActive:1;	// 15 	��������� ������ ������������ ������
	} bit;
} TTEK_Discrete;



#ifdef __cplusplus
}
#endif // extern "C"

#endif

