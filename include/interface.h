#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "LogEv.h"
#include "menu2x.h"			// ���� ��� ������������� ����������
#include "value_conv.h"		// ���������� ��� ��������������� �����/�����
#include "rtc.h"
#include "VlvDrvCtrl.h"		// ��������� ���������� ��������
#include "Calibs.h"			// ���������� ����������
#include "im.h"				// �������������� ������

#define LED_ACTIVE			1

// ������ A (����� = 0, ���������� = 40) - ����������� - ��������
typedef struct _TGroupA
{
	TStatusReg      Status;           	// 0.������ ������
	TFltUnion	    Faults;				   // 1-4.������
	TInputReg       Inputs;           	// 5.��������� ���������� ������
	TOutputReg      Outputs;          	// 6.��������� ���������� �������
	Uns             Position;        	// 7.���������
	Uns             Torque;           	// 8.������
	Int             Speed;				// 9.��������
	Uns             Ur;                 // 10.���������� ���� R
  	Uns             Us;                 // 11.���������� ���� S
 	Uns             Ut;                 // 12.���������� ���� T
	Uns             Iu;                 // 13.��� ���� U
	Uns             Iv;                 // 14.��� ���� V
	Uns             Iw;                 // 15.��� ���� W
	Int             AngleUI;            // 16.���� ��������
	TCalibState     CalibState;         // 17.������ ����������
	Int             PositionPr;         // 18.��������� %
	Uns             FullWay;            // 19.������ ���
	Int             CurWay;             // 20.������� ���
	Uns             CycleCnt;           // 21.������� ������
	Int             Temper;             // 22.����������� �����
	Uns             MkuPoVersion;       // 23.������ ��
	Uns Rsvd[16];
} TGroupA;

// ������ B (����� = 40, ���������� = 50) - ��������� ������������
typedef struct _TGroupB
{
	Uns				MoveCloseTorque;	// 0.������ ��������
	Uns				MoveOpenTorque;		// 1.������ ��������
	Uns				BreakCloseTorque;	// 2.������ ���������� �� ��������
	Uns				StartCloseTorque; 	// 3.������ �������� �� ��������
	Uns				BreakOpenTorque;  	// 4.������ ���������� �� ��������
	Uns				StartOpenTorque;  	// 5.������ �������� �� ��������
	TValveType		ValveType;			// 6.��� �������� (����/�����)
	TBreakMode      BreakMode;		   	// 7.��� ����������
	TRodType        RodType;            // 8.��� �����
	Uns				CloseZone;		 	// 9.���� �������
	Uns				OpenZone;		 	// 10.���� �������
	Uns             PositionAcc;        // 11.���� ��������
	Uns             MainCode;        	// 12.��� �������	
	TTimeVar 		DevTime;			// 13.�����
	TDateVar 		DevDate;			// 14.����
	Int		 		TimeCorrection;		// 15.������������� �������
	TIndicMode      IIndicMode;         // 16.����� ��������� ����
	TMuDuSetup      MuDuSetup;          // 17.��������� ������ ��/��
	TDuSource		DuSource;			// 18.�������� ������ ��
	Uns             TuLockSeal;         // 19.���������� ��������
	Uns             TuTime;             // 20.����� �������
	#if BUR_M
	Uns 			Rsvd1[2];
	#else
	TInputType 		InputType;			// 21.��� �������� ������� 24/220
	TInputMask	    InputMask;			// 22.����� ���������� ������
	#endif
	TOutputMask 	OutputMask;			// 23.����� ���������� �������
	TBaudRate       RsBaudRate;         // 24.�������� �����
	Uns             RsStation;          // 25.����� �������
	TParityMode		RsMode;				// 26.����� �����
	TSettingPlace	SettingPlace;		// 27.����� ��������� ���
	Uns 			Rsvd2[4];			// 28-31. 
	Uns				NoMoveTime;		   	// 32.����� ���������� ��������
	Uns				OverwayZone;		// 33.�����
	Uns 			Rsvd3;				// 34.
 	Uns             SleepTime;          // 35.�������� �����
	Uns 			Rsvd[14];
} TGroupB;

// ������ C (����� = 90, ���������� = 120) - ��������� ���������
typedef struct _TGroupC
{
	Uns             FactCode;           // 0.��� �������
	TDriveType      DriveType;          // 1.��� �������
    Uns        		ProductYear;        // 2.��� ������������ �����
  	Uns             FactoryNumber;      // 3.��������� ����� �����
  	Uns             MaxTorque;          // 4.������������ ������ �������
	Uns             Inom;               // 5.����������� ���
  	Uns             GearRatio;          // 6.������������ ����� ���������
	Int             BlockToGearRatio;   // 7.����������� �������� ����-�������� ����������� ��������
	Uns             PosPrecision;       // 8.�������� ���������
	Uns             PhOrdZone;      	// 9.���������� ����������� ��� ���������
	Uns             MuffZone;           // 10.���������� ������ ����� 
	Uns			    PosSensPow;			// 11.��� ������� ���������
	TPrtMode        CalibIndic;         // 12.��������� ����������
	Uns			    DisplResTout;		// 13.����� ������ ����������
	// ���������
	Uns             OpenAngle;          // 14.���� ���������� ����
	Int             AngleUI;            // 15.���� ��������
	TLedsReg        LedsReg;            // 16.��������� ����������� �����
	THallBlock      HallBlock;          // 17.��������� �������� ����� �����
	Uns             SetDefaults;        // 18.������� ���������� �� ���������
#if BUR_M
	TPrtMode       	RST_Err;           	// 19.�������� ����������� ��� ���� ��� ��� �
#else
	Uns       	    Rsvd1;           	// 19.������
#endif
	TPrtMode        DriveTemper;        // 20.������ �� ��������� ���������. (�������� ������ ��������� ����� � H)
	Uns             OvLevel_max;        // 21.������� ���������� ���������� ��� 47% ���������� (320�)
	Uns             OvTime_max;     	// 22.����� ���������� ���������� ��� 47% ���������� (1�)
	Uns             RevErrValue;       	// 23.���������� ����� ������� ���������
	Uns			    RevErrLevel;		// 24.������� ���� ������� ���������
	//-��������� �����--
	Uns             VoltAcc;            // 25.������������� �������
	Uns             BrakeAngle;         // 26.���� ����������
	Uns             BrakeTime;          // 27.����� ����������
	Uns				KickCount;			// 28.����������� ������
	Uns             OvLevel;    		// 29.������� ���������� ����������
	Uns             OvTime;     		// 30.����� ���������� ����������
	Uns             UvLevel;   			// 31.������� ��������� ����������
	Uns             UvTime;    			// 32.����� ��������� ����������
	Uns             VSkLevel;          	// 33.������� ��������� ����������
	Uns             VSkTime;           	// 34.����� ��������� ����������
	Uns             BvLevel;   			// 35.������� ������ ������� ���
	Uns             BvTime;    			// 36.����� ������ ������� ���
	TPrtMode        Phl;           		// 37.������ �� ������ ��� ��������
	Uns             PhlLevel;      		// 38.������� ������ ���
	Uns             PhlTime;       		// 39.����� ����������� ������ ���
	TPrtMode        I2t;                // 40.�����-������� ������
	Uns             I2tMinCurrent;      // 41.������� ������������ ����
	Uns             I2tMidCurrent;      // 42.������� �������� ����
	Uns             I2tMaxCurrent;      // 43.������� ������������� ����
	Uns             I2tMinTime;         // 44.����� �� ����������� ����
	Uns             I2tMidTime;         // 45.����� �� ������� ����
	Uns             I2tMaxTime;         // 46.����� �� ������������ ����
	Uns				PhOrdTime;			// 47.����� ����������� ��� ���������
	Uns				DrvTLevel;			// 48. ������� ������������ ������ �� ��������� ���������	
	Uns				DrvTInput;			// 49.
	TPrtMode        Ov;					// 50.������ �� ���������� ����������
	TPrtMode        Uv;					// 51.������ �� ���������� ����������
	TPrtMode        VSk;               	// 52.������ �� ��������� ����������
	TPrtMode        Bv;        			// 53.������ �� ������ ������� ���
	TPrtMode        PhOrd;          	// 54.������ �� ��������� ����������� ��� ���������	
	Uns             OvDTime;     		// 55.����� ���������� ����������
	Uns             UvDTime;     		// 56.����� ���������� ����������
	Uns             MkuPoSubVersion;    // 24.��������� ��
	TPrtMode        IUnLoad;            // 58.������ �� ����� ��������
	Uns             IUnLevel;           // 59.��� ����� ��������
	Uns             IUnTime;            // 60.����� ����� ��������
	TPrtMode        ISkew;              // 61.������ �� ��������� ����
	Uns             ISkewLevel;         // 62.������� ��������� ����
	Uns             ISkewTime;          // 63.����� ��������� ����
	TPrtMode        ShC;        		// 64.������ �� ��������� ���������
	Uns				SelectDynBr;		// 65.����� ������������� ���������� (������� ������� ��� ������ �������)
	Uns				IndicatorType;		// 66.��� ����������
	Uns				RTCErrOff;			// 67 ���������� ��������� ������ ����� ��������� �������
	TPrtMode        ErrIndic;           // 68.��������� ������ ����������
	TPrtMode        TemperTrack;        // 69.������ �� ���������/�������������� �����
	Int             TemperHigh;         // 70.������� ��������� �����
	Int             TemperLow;          // 71.������� �������������� �����
	Int             TenOnValue;         // 72.������� ��������� ����
	Int             TenOffValue;        // 73.������� ���������� ����
	Int             CorrTemper;         // 74.������������� ����������� �����
	TPrtMode        MuDuDef;        	// 75.������ ������ ������ ��/��
	Int				IU_Input;			// 76.�������� ��� ���������� �������� ���� IU
	Int				IV_Input;			// 77.�������� ��� ���������� �������� ���� IV
	Int				IW_Input;			// 78.�������� ��� ���������� �������� ���� IW
	Uns             IU_Mpy;             // 79.������������� ���� ���� U
	Uns             IV_Mpy;             // 80.������������� ���� ���� V
	Uns             IW_Mpy;             // 81.������������� ���� ���� W
	Uns             IU_Offset;          // 82.�������� ���� ���� U
	Uns             IV_Offset;          // 83.�������� ���� ���� V
	Uns             IW_Offset;          // 84.�������� ���� ���� W
	Uns             UR_Mpy;             // 85.������������� ���������� ���� R
	Uns             US_Mpy;             // 86.������������� ���������� ���� S
	Uns             UT_Mpy;             // 87.������������� ���������� ���� T
	Uns             UR_Offset;          // 88.�������� ���������� ���� R
	Uns             US_Offset;          // 89.�������� ���������� ���� S
	Uns             UT_Offset;          // 90.�������� ���������� ���� T
	Uns			    CoefVoltFltr;		// 91.����������� ���������� �������� ����������
	Uns			    CoefCurrFltr;		// 92.����������� ���������� ���� ��������
	Uns			    Dac_Mpy;			// 93.������������� ���
	Int			    Dac_Offset;			// 94.�������� ���
	Uns             ClosePosition;      // 95.��������� �������
	Uns             OpenPosition;       // 96.��������� �������
	TTenMode		Rsvd6[3];			// 97 - 99 ������
	Uns				ShC_Up; 			// 100 ������
	Uns				ShC_Down;			// 101 ������� � H
	Uns             Position;           // 102.���������
	Uns				LowPowerLvl;		// 103 �������� ���������� � �������� ��� 3� ������
	Int				Upor25;				// 104 �������� ��� ���������/��������� �������� ����� ��� 220� 25% �� �����
	Int				Upor35;				// 105 �������� ��� ���������/��������� �������� ����� ��� 220� 35% �� �����
	Int				Upor50;				// 106 �������� ��� ���������/��������� �������� ����� ��� 220� 50% �� �����
	Int				Upor75;				// 107 �������� ��� ���������/��������� �������� ����� ��� 220� 75% �� �����
	Int				Upor100;			// 108 �������� ��� ���������/��������� �������� ����� ��� 220� 100% �� �����
	Int				Corr40Trq;			// 109 �������� ��� ������������� ��������� ����� �������� (������ 60%)
	Int				Corr60Trq;			// 110 �������� ��� ������������� ��������� ������� �������� (������ 60%)
	Int				Corr80Trq;			// 111
	Int				Corr110Trq;			// 112
	Uns			    Rsvd2[7];			// 113 - 119 ������ 
} TGroupC;

// ������ D (����� = 210, ���������� = 20 )  	- �������
typedef struct _TGroupD
{
	TTaskReset     TaskClose;           // 0.������� �������
	TTaskReset     TaskOpen;            // 1.������� �������
	Uns            RevOpen;             // 2.������� �� ��������
	Uns            RevClose;            // 3.������� �� ��������
	Uns            AutoCalib;           // 4.�������������� ����������
	Uns            CalibReset;          // 5.����� ����������
	TValveCmd	   ControlWord;         // 6.������� ����������	
	Uns			   PrtReset;      		// 7.����� �����
	Uns            SetDefaults;         // 8.������� ���������� �� ���������
	Uns Rsvd1;//�������� ������� ������� 
	Uns            CycleReset;			// 10.����� �������� ������
	Uns			   RsReset;				// 11. ����� �����
	Uns 		   Rsvd[8];				// 12-19.������
} TGroupD;


// ������ G (����� 230, ���������� 30)  - ���������������� ��������� ����
typedef struct _TGroupG
{
	Uns            Mode;                // 0.����� �����
	TLedsReg       LedsReg;             // 1.���� ����������� �����
	TOutputReg     OutputReg;			// 2.���� ���������� �������
	Int			   DacValue;			// 3.�������� ���
	Uns			   DisplShow;			// 4.����� �������
	Uns			   TestCamera;			// 5.���� � ������
	Uns			   ThyrGroup;			// 6.����� ������ ����������
	Uns            SifuEnable;          // 7.���������� ������ ����
	Uns			   Rsvd2[10];			// 8-17.������
	Uns			   ThyrOpenAngle;		// 18.�������� ���� ��������
	Uns			   DiscrInTest;			// 19. ���� ���������� ������
	Uns			   DiscrOutTest;		// 20. ���� ���������� �������
	Bool		   IsDiscrTestActive;	// 21. ������� �� ���� ��/��
	Uns 		   Rsvd[8];			    // 22-29.������
} TGroupG;

// ������ H (����� 260, ���������� 140)
typedef struct _TGroupH
{
	TCalibState     CalibState;          // 0.��������� ����������
	Uns             CalibRsvd;           // 1.������ ��� ����������
	Uns             ClosePosition[2];    // 2-3.��������� �������
	Uns             OpenPosition[2];     // 4-5.��������� �������
										 // ������ ����� +2
	Uns             Password1;           // 6.�������� ������
	Uns             Password2;           // 7.��������� ������
	Uns             ScFaults;            // 8.������ ��
	Uns				UporOnly;			 // 9.�������� ���������� ������ ������ �� �����
	Uns             CycleCnt;            // 10.������� ������
	Uns				Rsvd1[5];			 // 11-15.������
	Uns				TransCurr;			 // 16.��� ��������
	TCubArray		TqCurr;				 // 17-36.��� �����������
	TCubArray		TqAngUI;			 // 37-56.���� ��������
	TCubArray		TqAngSf;			 // 57-76.���� ����
	Uns				ZazorTime;			 // 77.����� ������� ������
	TNetReg         FaultsNet;           // 78.����������� ����
  	Uns             Ur;                  // 79.���������� ���� R
  	Uns             Us;                  // 80.���������� ���� S
  	Uns             Ut;                  // 81.���������� ���� T
	Uns             Umid;             	 // 82.������� ����������
	Uns             VSkValue;          	 // 83.��������� ��� �������� ����
  	Uns     	    PhOrdValue;        	 // 84.����������� ��� ����
	TNetReg         DefectsNet;          // 85.����������� ���� (��� ��������������)
	Uns             Rsvd7;         	 	 // 86. ������
	TLoadReg        FaultsLoad;          // 87.����������� ��������
	Uns             Iu;                  // 88.��� ���� U
	Uns             Iv;                  // 89.��� ���� V
	Uns             Iw;                  // 90.��� ���� W
	Uns             Imid;				 // 91.������� ���
  	Uns             ISkewValue;          // 92.��������� ����� ��������
	Uns             Position;            // 93.���������
	Uns				Rsvd2[2];			 // 94 - 95 ������
	TReverseType	ReverseType;         // 96.��� �������
	Uns				DemoPosition1;       // 97.���� ��������� 1
	Uns				DemoPosition2;       // 98.���� ��������� 2
	TInputReg       Inputs;           	 // 99.��������� ���������� ������
	TOutputReg      Outputs;          	 // 100.��������� ���������� �������
	Uns             TuReleMode;          // 101.�������� �����
	TNormState	    NormState[3];		 // 102-104.���������� ��������� ������    - �� ������������ �� ���� �� ���������� �����
	TNormState 	    NormOut[8];			 // 105-112.���������� ��������� ������� - �� �������� ���
	TDeviceReg      FaultsDev;           // 113.����������� ����������
	Uns             StartIndic;			 // 114.��������� � ������
 	Uns             SleepTime;           // 115.�������� �����  mb to C
	Uns             BusyValue;       	 // 116.������� ����������
	Uns 			LogEvAddr;			 // 117. ������� ����� ������� �������
	Uns 			LogCmdAddr;			 // 118. ������� ����� ������� ������
	Uns 			LogParamAddr;		 // 119. ������� ����� ������� ��������� ����������
	Uns				LogEvCount;			 // 120. ���������� ���������� ����� ������� �������
	Uns				LogCmdCount;		 // 121. ���������� ���������� ����� ������� ������
	Uns				LogParamCount;		 // 122. ���������� ���������� ����� ������� ��������� ����������
	Uns             Torque;           	 // 123.������
	Int             Speed;				 // 124.��������
	Uns				Seconds;			 // 125.�������
	TContactorGroup ContGroup;			 // 126. ���������� ������������
	TBurCmd 		LogControlWord;		 // 127. ������� ���
	Uns				LogReset;			 // 128. ����� ��������
	Uns 			Rsvd3[11];			 // 129-139.������
} TGroupH;

// ������ E (����� 400, ���������� 32)
typedef struct _TGroupE
{
	TTimeVar       LogTime;				// T.�����
	TDateVar       LogDate;				// D.����
	TStatusReg     LogStatus;           // 0.������ ������
	TFltUnion	   LogFaults;			// 1-4.������	
	Int            LogPositionPr;       // 5.��������� %
	Uns            LogTorque;           // 6.������
	Uns            LogUr;               // 7.���������� ���� R
  	Uns            LogUs;               // 8.���������� ���� S
  	Uns            LogUt;               // 9.���������� ���� T
	Uns            LogIu;               // 10.��� ���� U
	Uns            LogIv;               // 11.��� ���� V
	Uns            LogIw;               // 12.��� ���� W
	Int            LogTemper;           // 13.����������� �����
	TInputReg      LogInputs;           // 14.��������� ���������� ������
	TOutputReg     LogOutputs;          // 15.��������� ���������� �������
	Uns 		   Rsvd[14];			// 16-29.������
} TGroupE;


typedef struct _TLogEvBuffer
{
	TStatusReg     	LogStatus;          // 0.������ ������
	Int            	LogPositionPr;		// 1.��������� %
	Uns            	LogTorque;          // 2.������
	Uns            	LogUr;              // 3.���������� ���� R
  	Uns            	LogUs;              // 4.���������� ���� S
  	Uns            	LogUt;              // 5.���������� ���� T
	Uns            	LogIu;              // 6.��� ���� U
	Uns            	LogIv;              // 7.��� ���� V
	Uns            	LogIw;              // 8.��� ���� W
	Int            	LogTemper;          // 9.����������� �����
	Uns      		LogInputs;        	// 10.��������� ���������� ������
	Uns				LogOutputs;			// 11.��������� ���������� �������
} TLogEvBuffer;

// ��������� ����� 4000
typedef struct _TGroupT
{
	TTEK_TechReg 	TechReg;			// 1 ��������������� �������
	TTEK_DefReg  	DefReg;				// 2 ������� ��������
	Uns 		 	PositionPr;			// 3 ������� ��������� %
	TTEK_ComReg  	ComReg;				// 4 ������� ������
	Uns 		 	CycleCnt;			// 5 ������� ������
	Uns			 	Rsvd1;				// 6 ������
	Uns 		 	Iu;					// 7 ��� ���� U
	Uns 		 	Rsvd2[9];			// 8 - 16 ������
	Uns 			Ur;					// 17 ���������� ������� ����
	Uns				Rsvd3;				// 18 ������
	Uns 		 	   Speed;				// 19 ������� ��������
	Uns			 	Rsvd4;				// 20 ������
	Uns 		 	Torque;				// 21 ������� ������ �������� �*�
	TTEK_Discrete 	TsTu;				// 22 ��/��
	Uns				Rsvd6[4];			// 23 - 26 ������
	Uns				RsStation;			// 27 ����� ������� (������ ��� ������)
	Uns				Rsvd7;				// 28 ������
	Uns				Rsvd8[12];			// 29 ������
} TGroupT;



// ��������� ���������� ����������
typedef struct _TDriveData
{
	TGroupT			GroupT;				// TechReg
	TGroupB 		GroupB;				// 1.���������������� ���������
	TGroupC			GroupC;				// 2.��������� ���������
	TGroupD			GroupD;				// 3.������� ����������
	TGroupG			GroupG;				// 4.���������� ����
	TGroupH			GroupH;				// 5.������� ���������
	TGroupA 		GroupA;				// 0.����������� �������
	TGroupE			GroupE;				// 6.������ �������

} TDriveData;


// ��������� ���������� ����������
/*typedef struct _TTEKDriveData
{
	TTEK_MB_GROUP	MainGroup;
} TTEKDriveData;*/

#ifdef CREATE_DRIVE_DATA					// ����� ��������� ������ ������ � ����� �����, ����� ����� ������ "define multiple times"
TDriveData Ram;

TGroupA *GrA = &Ram.GroupA;
TGroupB *GrB = &Ram.GroupB;
TGroupC *GrC = &Ram.GroupC;
TGroupD *GrD = &Ram.GroupD;
TGroupE *GrE = &Ram.GroupE;
TGroupG *GrG = &Ram.GroupG;
TGroupH *GrH = &Ram.GroupH;
TGroupT *GrT = &Ram.GroupT;

#else
extern TDriveData Ram;
extern TGroupA *GrA;
extern TGroupB *GrB;
extern TGroupC *GrC;
extern TGroupD *GrD;
extern TGroupE *GrE;
extern TGroupG *GrG;
extern TGroupH *GrH;
extern TGroupT *GrT;

#endif

#if BUR_M
#define TEK_DEVICE_FAULT_MASK	0xF910				// ����� �� ������� �������� ���������� ��� ������� ���
#else
#define TEK_DEVICE_FAULT_MASK	0xF990
#endif

#define RAM_ADR					0									// ����� ������ ������� ������ �� ������
#define RAM_SIZE				sizeof(TDriveData)					// ������ ������� ������ ����������
#define RAM_DATA_SIZE			(RAM_SIZE - sizeof(TGroupE))		// ���������� ���������� ��� ������

#define MENU_GROUPS_COUNT		5
#define MENU_EVLOG_GROUP		4

#define LOGS_CNT				3									// ���������� �������� ����������

#define LOG_EV_START_ADDR		12000								// ��������� ����� ������� ������ �������� ������ �������
#define LOG_EV_DATA_CNT			20		//sizeof(TGroupE) + 2		// ���������� ����� � 1 ������ �������� ������ �������
#define LOG_EV_CNT				500									// ���������� ������� � ������ �������
#define LOG_EV_DATA_CELL		1									// ���������� �������� � �������� ������ �������

#define LOG_EV_BUF_START_ADDR	0									// ��������� ����� ������ �������
#define LOG_EV_BUF_DATA_CNT		sizeof(TLogEvBuffer)				// ���������� ����� � ����� ������ ������
#define LOG_EV_BUF_DATA_CELL	5									// ���������� ����� ������ � ����� ������

#define LOG_EV_BUF_CELL_COUNT	5									// ���������� �������� � ������ ������� �������

#define PREV_LEV_INDEX		0										//������� ��� ����������� ����������� ������ ��������
#define PREV_LCMD_INDEX		1
#define PREV_LPAR_INDEX		2



#define LOG_EV_RAM_DATA_ADR		GetAdr(GroupE)						// ����� ������ ������� � Ram
#define LOG_EV_RAM_DATA_LADR	(LOG_EV_RAM_DATA_ADR * LOG_EV_DATA_CNT)
#define LOG_EV_SIZE				(LOG_EV_CNT * LOG_EV_DATA_CNT * LOG_EV_DATA_CELL)
//!!!!

#define LOG_CMD_START_ADDR		1000								// ��������� ����� ������� ������
#define LOG_CMD_DATA_CNT		5									// ���������� ������������ � ������ �����
#define LOG_CMD_CNT				1500								// ������� ������� ������ (���������� �������)

#define LOG_PARAM_START_ADDR	9000								// ��������� ����� �������� ��������� ����������
#define LOG_PARAM_DATA_CNT		5									// ���������� ������������ ����� (���� ��������� �������: ����, ����� � �.�.)
#define LOG_PARAM_CNT			500									// ������� ������� ��������� ����������

#define IM_READ_BUF_SIZE		((LOG_EV_DATA_CNT * LOG_EV_DATA_CELL) + (LOG_EV_BUF_DATA_CNT * LOG_EV_BUF_DATA_CELL))

#define REG_TORQUE_ADDR		GetAdr(GroupA.Torque)
#define REG_START_IND			GetAdr(GroupH.StartIndic)
//#define REG_START_IND			GetAdr(GroupA.Torque)
#define REG_LOG_ADDR			GetAdr(GroupH.LogEvAddr)
#define REG_LOG_TIME			GetAdr(GroupE.LogTime)
#define REG_LOG_DATE			GetAdr(GroupE.LogDate)

#define REG_CODE				GetAdr(GroupB.MainCode)
#define REG_FCODE				GetAdr(GroupC.FactCode)
#define REG_PASSW1				GetAdr(GroupH.Password1)
#define REG_PASSW2				GetAdr(GroupH.Password2)

#define REG_CYCLE_CNT			GetAdr(GroupH.CycleCnt)
#define REG_CONTROL				GetAdr(GroupD.ControlWord)
#define REG_SC_FAULTS			GetAdr(GroupH.ScFaults)
#define REG_CALIB_STATE			GetAdr(GroupH.CalibState)

#define CMD_DEFAULTS_USER	0x0010	// ���������������� ��������� �� ���������
#define CMD_RES_CLB			0x0020	// ����� ���������� ������� ���������
#define CMD_RES_FAULT		0x0040	// ����� �����
#define CMD_CLR_LOG			0x0080	// ������� �������
#define CMD_RES_CYCLE		0x0100	// ����� �������� ������
#define CMD_PAR_CHANGE		0x0200	// ��������� ����������
#define CMD_DEFAULTS_FACT 	0x0400	// ��������� ��������� �� ���������

#define TS_SIGNAL_MASK_M 	511		// ����� ��� ��������� ������ �������� �� ���������� ����������� ���� ��������
#define TS_SIGNAL_MASK_T 	255		// 

#if BUR_M
#define LOG_EV_DEFAULTS { \
		false, false, false, false, true, 4, \
		0, 0, 0, 0, \
		0, 0, 0, 0, \
		0, 0, 0, 0, \
		&Ram.GroupB.DevTime.all, &Ram.GroupB.DevDate.all,\
		&Ram.GroupA.Status.all, &Ram.GroupA.PositionPr, &Ram.GroupA.Torque,				\
		&Ram.GroupA.Ur, &Ram.GroupA.Us, &Ram.GroupA.Ut,			\
		&Ram.GroupA.Iu, &Ram.GroupA.Iv, &Ram.GroupA.Iw,			\
		&Ram.GroupA.Temper, 0, &Ram.GroupA.Outputs.all,	\
		&Ram.GroupH.Seconds,		\
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0  \
		}
#else
#define LOG_EV_DEFAULTS { \
		false, false, false, false, true, 4, \
		0, 0, 0, 0, \
		0, 0, 0, 0, \
		0, 0, 0, 0, \
		&Ram.GroupB.DevTime.all, &Ram.GroupB.DevDate.all,\
		&Ram.GroupA.Status.all, &Ram.GroupA.PositionPr, &Ram.GroupA.Torque,				\
		&Ram.GroupA.Ur, &Ram.GroupA.Us, &Ram.GroupA.Ut,			\
		&Ram.GroupA.Iu, &Ram.GroupA.Iv, &Ram.GroupA.Iw,			\
		&Ram.GroupA.Temper, &Ram.GroupA.Inputs.all, &Ram.GroupA.Outputs.all,	\
		&Ram.GroupH.Seconds,		\
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0  \
		}
#endif

#define LOG_CMD_DEFAULTS	{	\
		FALSE, FALSE, FALSE,	\
		&Ram.GroupB.DevTime.all, &Ram.GroupB.DevDate.all, &Ram.GroupH.Seconds, \
		&Ram.GroupA.Status.all, 0, 0,\
		0,0,0,0,0				\
		}


#define LOG_PARAM_DEFAULTS	{	\
		false, false, false,	\
		&Ram.GroupB.DevTime.all, &Ram.GroupB.DevDate.all, &Ram.GroupH.Seconds, \
		0, 0,					\
		0,						\
		0, 0, 0, 0, 0,			\
		0, 0, 0, 0, 0			\
		}

// Ram.GroupH.LogEvCount
#define MENU_DEFAULT { \
	MS_START, 0, 0, 0, 1, True, True, False, \
	&Ram.GroupC.MaxTorque, \
	0, 0, 0, False, 0, BLINK_HI_TOUT, BLINK_LO_TOUT, 0xFF, 0, BIT_TOUT, \
	False, 0, 0, \
	ToPtr(&Ram), Display.HiStr, Display.LoStr, \
	REG_START_IND, RAM_ADR, "", "", \
	False, False, MENU_EVLOG_GROUP, REG_LOG_ADDR, REG_LOG_TIME, REG_LOG_DATE, &Ram.GroupH.LogEvCount, \
	0, 0, MENU_GROUPS_COUNT, \
	0, 0, 0, \
	0, 0, LOG_EV_CNT, \
	0, 0, 0, \
	0, 0, 0, 0, \
	FALSE, FALSE, 0, 0, FALSE, 0, \
	4, {GetAdr(GroupB.DevTime),GetAdr(GroupB.DevDate),GetAdr(GroupB.RodType),GetAdr(GroupB.ValveType),0},	\
	2, {GetAdr(GroupB.MoveCloseTorque),GetAdr(GroupB.MoveOpenTorque),0,0,0},	\
	1, {GetAdr(GroupD.TaskClose),0,0,0,0},	\
	1, {GetAdr(GroupD.TaskOpen),0,0,0,0},	\
	1, {GetAdr(GroupD.RevOpen),0,0,0,0},	\
	1, {GetAdr(GroupD.RevClose),0,0,0,0},	\
	1, {GetAdr(GroupB.BreakCloseTorque),0,0,0,0},	\
	1, {GetAdr(GroupB.StartCloseTorque),0,0,0,0},	\
	1, {GetAdr(GroupB.BreakOpenTorque),0,0,0,0},	\
	1, {GetAdr(GroupB.StartOpenTorque),0,0,0,0},	\
	1, {0,0,0,0,0},	\
	1, {GetAdr(GroupB.MuDuSetup),0,0,0,0},	\
	groups, Null, params, values, "������", \
	StartDispl, EnableEdit, WriteValue, GetExpressText, GetExpressState\
}
//#define MENU_GROUPS_COUNT		5
//#define MENU_EVLOG_GROUP		4


#define DRIVE_CTRL_DEFAULT { \
	ToPtr(&Ram.GroupA.Status), \
	&Ram.GroupD.ControlWord, \
	&Ram.GroupB.MuDuSetup, \
	&Ram.GroupB.DuSource, \
	&Ram.GroupH.ReverseType, \
	True, 0, 0, False, \
	True, False, 0, True, False, True,\
	&Ram.GroupH.TuReleMode, \
	&Ram.GroupB.TuLockSeal, \
	False, False, \
	False, False, 0, DEMO_TOUT, 0, \
	&Ram.GroupH.DemoPosition2, \
	&Ram.GroupH.DemoPosition1, \
	True, False, POS_UNDEF, 0, \
	&Ram.GroupB.BreakMode, \
	&Calib, \
	0, 0, vcwNone, 0, 0, 0, 0,\
	&StopPowerControl, \
	&StartPowerControl \
}

#define CALIBS_DEFAULT { \
	False, False, 0, 0, REV_MAX, 0, 0, 0, 0, \
	MUFF_CLB_TIME, 0, 0, 0, 0, 0, 0, 0, \
	&Ram.GroupC.PosSensPow, \
	&Ram.GroupB.PositionAcc, \
	ToPtr(&Ram.GroupB.RodType), \
	&Encoder.Revolution, \
	&Ram.GroupA.PositionPr, \
	&Ram.GroupA.FullWay, \
	&Ram.GroupA.CurWay, \
	&Ram.GroupH.CycleCnt, \
	&MuffFlag, \
	ToPtr(&Ram.GroupD.TaskClose), \
	ToPtr(&Ram.GroupH.CalibState) \
}



#define IM_DEFAULT { \
	RECEIVE_FUNC_MODE, imInit, 0, 0, \
	False, &Bluetooth.TxBusy, 0, \
	IM_TIMEOUT, \
	False, False, False, False, \
	(Uns *)&Ram, &ImReadBuf[0] \
}


// ���������� ���������� ������
extern MENU        	Menu;
extern TLogCmd		LogCmd;
extern TLogParam 	LogParam;
extern CALIBS       Calib;
extern TVlvDrvCtrl  Mcu;
extern TOutputReg   Dout;

extern Uns IrImpCount;
extern Uns LcdEnable;
extern Bool PowerEnable;
extern Bool DisplayRestartFlag;

extern TIM Im;
extern Uns TempMuDu;

// ��������� �������
void InterfaceInit(void);
void InterfIndication(void);
void DataBufferPre(void);
void LogEvControl(void);
void GetCurrentCmd(void);
void LogCmdControl(void);
void LogParamControl(void);
void LogParamMbExec(void);
Bool InterfaceRefresh(void);
void StartDispl(String Str);
Bool EnableEdit(Uns Password, Uns Param);
Bool WriteValue(Uns Memory, Uns Param, Uns *Value);
void GetExpressText(Uns State);
Byte GetExpressState(Byte CurrentState, Uns State);
void RefreshData(void);
void DataSetting(void);
void AddControl(void);
void LocalControl(void);
void RemoteControl(void);
void AlgControl(void);
void ClbControl(void);
void BlkSignalization(void);
void TsSignalization(void);
void ShowDriveType(void);
void DisplayStart(void);
#if IR_IMP_TEST
void IrImpCalc(void);
#endif
void ImReadRecordsToBuffer(void);

#endif
