/*======================================================================
��� �����:          VlvDrvCtrl.h
�����:              ������ �.�. - (������� ����� �.�.)
������ �����:       01.05
���� ���������:		23/04/12
�������������:      ��������� � ����������� Calibs (��� ������� ����������)
��������:
���������� ���������� ���������� ���������� �������� ��������
======================================================================*/

#ifndef VLV_DRV_CNTRL_
#define VLV_DRV_CNTRL_

#include "std.h"

#ifdef __cplusplus
extern "C" {
#endif

// ��������� ��� ������ � ����������
#define POS_UNDEF	0x7FFFFFFF
#define POS_ERR		10

// ��������� ������ ��
#define TU_OPEN		0x1
#define TU_CLOSE		0x2
#define TU_STOP		0x4

// ������� ����������
typedef enum {
  vcwNone=0,		// ��� �������
  vcwStop,			// ����
  vcwClose,			// �������
  vcwOpen,			// �������
  vcwTestClose,		// �������� �������
  vcwTestOpen,		// �������� �������
  vcwDemo,			// ���� �����
  vcwTestEng		// ���� ���������
} TValveCmd;

// ��� ����������
typedef enum {
  vtNone=1,		// ��� ����������
  vtClose,			// ���������� � �������
  vtOpen,			// ���������� � �������
  vtBoth			// ���������� � ������� � �������
} TBreakMode;

// ��������� ������ ��/��
typedef enum {
  mdOff=0,			// ��������
  mdSelect,		// ����� ������ ��/��
  mdMuOnly,		// ������ ����� ��
  mdDuOnly			// ������ ����� ��
} TMuDuSetup;

// �������� ������ ��� ������ ��
typedef enum {
	mdsAll=0,		// ��� ����������
	mdsDigital,		// ������ ����������
  mdsSerial		// ������ ����������������
} TDuSource;

// ��� �������
typedef enum {
  rvtStop=0,		// ������� � ����
  rvtAuto,			// �������������� ������
  rvtNone			// ������������� ������ ��� ������ ������ �� ������
} TReverseType;

// ��������� ��� ������ � ������� �����������
typedef struct {
	Bool				 Enable;				// ������� �������� ����������
	Byte				 BtnKey;				// ������� � �����/������
	Byte				 PduKey;				// ������� � ���
	Bool				 WaitConfirmFlag;		// ���� ����, ��� �� ������� ������������� ������� ���������� � ���
	Bool				 CancelFlag;			// ���� ������ �������
	Bool				 MpuBlockedFlag;		// ���� ������ ������� ��-�� ���������� ���
} TMpuControl;

// ��������� ��� ������ � ���������������
typedef struct {
	Bool				 Enable;			// ������� ��������������
	Bool				 LocalFlag;			// ���� TRUE, ���� ���������� �������
	Uns				 	 State;				// ��������� ������ ��������������
	Bool				 Ready;				// ���������� ��������� ������ �� �� ��������
	Bool				 ReleStop;			// ���� � �������� ������
	Bool				 ReleStopEnable;	// ���������� ����� � �������� ������
	Uns					*ReleMode;			// ������� ��������� ������
	Uns					*LockSeal;			// ���������� ���������
} TTeleControl;

// ��������� ��� ������ � ������� ��������������
typedef struct {
	Bool				 Enable;			// ������� �������� �������������
	Bool				 Active;			// ������� ������ �������� �������������
} TSmoothControl;

// ��������� ��� ������ � ����-�������
typedef struct {
	Bool				 Enable;			// ������� ����-������
	Bool				 Active;			// ������� ������ ����-������
	Uns				 	Timer;				// ������ ������� �������
	Uns				 	Timeout;			// ����-��� ����-������
	Uns				 	TaskPos;			// ������� ������� ���������
	Uns					*UpPos;				// ������� ��������� � %
	Uns					*DownPos;			// ������ ��������� � %
} TDemoControl;

// ��������� ��� ������ � ���������
typedef struct {
	Bool				PosRegEnable;		// ������� ������ ����������������
	Bool            	BreakFlag;			// ���� ������ � ����������� SVS.1
	LgInt				Position;			// ������� ���������
	Uns				 	BreakDelta;		// ������������ �������� ��� ������ � �����������
	TBreakMode			*BreakMode;			// ����� ������ � �����������
	Ptr					CalibData;			// ������ �� ����������
} TValveControl;

// ��������� ��� ������ � �������� �������
typedef struct {
	Uns				 Value;				// �������� ������� ���������� ��� ������� �������
	Uns				 Source;				// �������� ������� ���������� ��� ������� �������
	Uns				 QueryValue;			// �������� ������� ����������, ������������ � �������
} TEvLogControl;

// ��������� ���������� ��������
typedef union {
	Uns all;
	struct {
		Uns Stop:1;			// 0		����
		Uns Fault:1;		// 1		������
		Uns Closing:1;		// 2		���� ��������
		Uns Opening:1;		// 3		���� ��������
		Uns Rsvd1:1;		// 4		������
		Uns Closed:1;		// 5		�������
		Uns Opened:1;		// 6		�������
		Uns Rsvd2:1;		// 7		������
		Uns MuDu:1;			// 8		������� ����������
		Uns Rsvd:7;			// 9-15 ������
	} bit;
} TVlvStatusReg;

// ���� ������ ����������
#define CMD_STOP				0x0001	// ����
#define CMD_CLOSE				0x0002	// �������
#define CMD_OPEN				0x0004	// �������
#define CMD_MOVE				0x0008	// �����������
#define CMD_DEFSTOP				0x0800	// ���� ��� ������(�� �������� ��������, ��������� ��� �������)  : ������� PIA 12.09.2012
#define CDM_DISCROUT_TEST		0x1000	// ���� ���������� �������
#define CMD_DISCRIN_TEST		0x2000	// ���� ���������� ������

// �������� ������� ����������
#define CMD_SRC_BLOCK		0x0400	// ������ ������ ����������
#define CMD_SRC_PDU			0x0800	// ����� �������������� ����������
#define CMD_SRC_MPU			0x1000	// ������� ���� ����������
#define CMD_SRC_DIGITAL		0x2000	// ���������� ���������
#define CMD_SRC_SERIAL		0x4000	// ���������������� ���������
#define CMD_SRC_ANALOG		0x8000	// ��������� ���������

// ��������� ���������� ��������
typedef struct {
	TVlvStatusReg	*Status;			// ������ ������
	TValveCmd		*ControlWord;		// ������� ����������
	TMuDuSetup		*MuDuSetup;			// ��������� ������ ��/��
	TDuSource		*DuSource;			// �������� ������ ��� ������ ��
	TReverseType	*ReverseType;		// ��� �������
	TMpuControl		 Mpu;				// ������� ����������
	TTeleControl	 Tu;				// ��������������
	TSmoothControl	 Smooth;			// ������� ������������
	TDemoControl	 Demo;				// ����-�����
	TValveControl	 Valve;				// ���������� ���������
	TEvLogControl	 EvLog;				// ������ �������
	TValveCmd		 Command;			// ���������� �������
	Uns				 MuDuInput;			// ��������� ����� ��/��
	Uns				 ActiveControls;	// ��������� ���������� ����������� ����������
	Uns				 StartDelay;		// ����� ��� ��������� ������� ����������
	Uns 			 IgnorComFlag;		// ���� ������������� ������� ���������� ���� ��� ��������� � ������� ���������
	void (*StopControl)(void);			// ������� �������� ����������
	void (*StartControl)(TValveCmd ControlWord); // ������� ������ ����������
} TVlvDrvCtrl;

void ValveDriveUpdate(TVlvDrvCtrl *);
void ValveDriveStop(TVlvDrvCtrl *, Bool Flag);
void ValveDriveMove(TVlvDrvCtrl *, Uns Percent);

#ifdef __cplusplus
}
#endif // extern "C"

#endif



