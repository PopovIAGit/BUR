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
	TFltUnion	    Faults;				// 1-4.������
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
	#if BUR_M
	Uns 			Rsvd68;				// B28. ������
	#else
	Uns				ReversMove;			// B28. 68 ������ ����. 0 - ������ ��������; 1 - ������ �������.
	#endif
	Uns 			Rsvd2[3];			// 29-31.
	Uns				NoMoveTime;		   	// 32.����� ���������� ��������
	Uns				OverwayZone;		// 33.�����
	TDriveType      DriveType;          // 34.��� �������
 	Uns             SleepTime;          // 35.�������� �����
	Uns 			Rsvd[14];
} TGroupB;

// ������ C (����� = 90, ���������� = 120) - ��������� ���������
typedef struct _TGroupC
{
//-----------------------��������� �������------------------------
	Uns             FactCode;           // 0.��� �������
	Uns				Rsvd3;				// 1.Reserv
    Uns        		ProductYear;        // 2.��� ������������ �����
  	Uns             FactoryNumber;      // 3.��������� ����� �����
  	Uns             MaxTorque;          // 4.������������ ������ �������
	Uns             Inom;               // 5.����������� ���
  	Uns             GearRatio;          // 6.������������ ����� ���������
	Uns             RevErrValue;       	// 7.���������� ����� ������� ���������
	Uns			    RevErrLevel;		// 8.������� ���� ������� ���������
	THallBlock      HallBlock;          // 9.��������� �������� ����� �����
	Uns             MkuPoSubVersion;    // 10.��������� ��
//-----------------------�������������� ���������---------------------
	Int             BlockToGearRatio;   // C11.����������� �������� ����-�������� ����������� ��������
	Uns			    progonCycles;		// C12. 102 ���������� ������ ������ ������� (���� != 0, ���������� ����� �������)
	Uns             MuffZone;           // 13.���������� ������ ����� 
	Uns			    PosSensPow;			// 14.��� ������� ���������
	Uns			    DisplResTout;		// 15.����� ������ ����������
	Uns             SetDefaults;        // 16.������� ���������� �� ���������
	Uns             VoltAcc;            // 17.������������� �������
	Uns       	    selectBraking;      // 18.����� ������������� ���������� (0-�����������, 1-�����������)
	Uns             BrakeAngle;         // 19.���� ����������
	Uns             BrakeTime;          // 20.����� ���������� 
	Uns 			NoCurrPause;		// 21.����� ���������� ����� ����� �����������
	Uns 			BrakeZone;			// 22.������������ ����������
	Uns       	    Rsvd5[12];          // 23-34.������
//-----------------------��������� ��������-------------------------------
	Int             CorrTemper;         // 35.������������� ����������� �����
	Uns             Rsvd6;       		// 36.������
	Uns             Rsvd7;       		// 37.������
	Uns			    CoefVoltFltr;		// 38.����������� ���������� �������� ����������
	Uns			    CoefCurrFltr;		// 39.����������� ���������� ���� �������� 
	Uns				TrqViewTime;		// 40.����������� ���������� �������
	Uns             Rsvd8[4];       	// 41-44.������
	Uns				MbOffsetMode;		// 45.����� ������ ������ ��� ����� ��������� �� 40000
	Uns				LedTestMode;		// 46.��������� ���� ����������� �� 2 �������
	Uns			    Dac_Mpy;			// 47.������������� ���
	Int			    Dac_Offset;			// 48.�������� ���
	Uns				IndicatorType;		// 49.��� ����������
	Uns             Rsvd16[4];       	// 50-53.������
//----------------------������������ �����(����������)----------------------
	TPrtMode        Ov;					// 54.������ �� ���������� ����������
	Uns             OvLevel_max;        // 55.������� ���������� ���������� ��� 47% ���������� (320�)
	Uns             OvTime_max;     	// 56.����� ���������� ���������� ��� 47% ���������� (1�) 
	Uns             OvLevel;    		// 57.������� ���������� ����������
	Uns             OvDTime;     		// 58.����� ���������� ����������
	Uns             OvTime;     		// 59.����� ���������� ���������� 
	TPrtMode        Uv;					// 60.������ �� ���������� ����������
	Uns             UvLevel;   			// 61.������� ��������� ����������
	Uns             UvDTime;     		// 62.����� ���������� ����������
	Uns             UvTime;    			// 63.����� ��������� ���������� 
 	TPrtMode        VSk;               	// 64.������ �� ��������� ����������
	Uns             VSkLevel;          	// 65.������� ��������� ����������
	Uns             VSkTime;           	// 66.����� ��������� ����������
	TPrtMode        Bv;        			// 67.������ �� ������ ������� ���
	Uns             BvLevel;   			// 68.������� ������ ������� ���
	Uns       	    Rsvd9;           	// 69.������
	Uns             BvTime;    			// 70.����� ������ ������� ��� 
#if BUR_M
	TPrtMode       	RST_Err;           	// 71.�������� ����������� ��� ���� ��� ��� �
#else
	Uns       	    Rsvd10;           	// 71.������
#endif
//----------------------������������ �����(���)-----------------------------
	TPrtMode        Phl;           		// 72.������ �� ������ ��� ��������
	Uns             PhlLevel;      		// 73.������� ������ ���
	Uns             PhlTime;       		// 74.����� ����������� ������ ��� 
	TPrtMode        I2t;                // 75.�����-������� ������
	Uns       	    Rsvd11[9];          // 76-84.������
	TPrtMode        ISkew;              // 85.������ �� ��������� ����
	Uns             ISkewLevel;         // 86.������� ��������� ����
	Uns             ISkewTime;          // 87.����� ��������� ���� 
	TPrtMode        ShC;        		// 88.������ �� ��������� ���������
	Uns				ShC_Level; 			// 89 
	Uns       	    EncoderCalcMethod;  // 90.����� ������ ��������� ������ �������� (������������ ����� / ���������� �������)
	//----------------------������������ �����(�������/����������)-----------
	TPrtMode        CalibIndic;         // 91.��������� ����������
	TPrtMode        ErrIndic;           // 92.��������� ������ ����������
	TPrtMode        TemperTrack;        // 93.������ �� ���������/�������������� �����
	Uns       	    Rsvd13;          	// 94.������
	Int             TemperHigh;         // 95.������� ��������� �����
	Int             TemperLow;          // 96.������� �������������� ����� 
 	Int             TenOnValue;         // 97.������� ��������� ����
	Int             TenOffValue;        // 98.������� ���������� ����
	TPrtMode        DriveTemper;        // 99.������ �� ��������� ���������. (�������� ������ ��������� ����� � H)
	Uns				DrvTLevel;			// 100. ������� ������������ ������ �� ��������� ���������	
	Uns				DrvTInput;			// 101. 
	Uns       	    Rsvd14;          	// 102.������
	TPrtMode        PhOrd;          	// 103.������ �� ��������� ����������� ��� ���������	
	Uns				PhOrdTime;			// 104.����� ����������� ��� ���������
	TPrtMode        MuDuDef;        	// 105.������ ������ ������ ��/��
	Uns				RTCErrOff;			// 106 ���������� ��������� ������ ����� ��������� �������
	Uns       	    PosSensEnable;      // C107. 197 ���������� �����, ���� ����� ������ "���� ������� ���������"
	//--------------------��������� ��������------------------------------------
	Uns       	    Rsvd108;          	// 108.������
	Int				Upor25;				// 109.�������� ��� ���������/��������� �������� ����� ��� 220� 25% �� �����
	Int				Upor35;				// 110 �������� ��� ���������/��������� �������� ����� ��� 220� 35% �� �����
	Int				Upor50;				// 111 �������� ��� ���������/��������� �������� ����� ��� 220� 50% �� �����
	Int				Upor75;				// 112 �������� ��� ���������/��������� �������� ����� ��� 220� 75% �� �����
	Int				Upor100;			// 113 �������� ��� ���������/��������� �������� ����� ��� 220� 100% �� �����
	Int				Corr40Trq;			// 114 �������� ��� ������������� ��������� ����� �������� (������ 60%)
	Int				Corr60Trq;			// 115 �������� ��� ������������� ��������� ������� �������� (������ 60%)
	Int				Corr80Trq;			// 116
	Int				Corr110Trq;			// 117
	Int				Corr25Trq;			// 113
	Uns       	    UporAddOpen;        // 118. �������������� ������������� ��� ����� � "��������"
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
	Uns 		   Rsvd1;//�������� ������� ������� 
	Uns            CycleReset;			// 10.����� �������� ������
	Uns			   RsReset;				// 11. ����� �����
	Uns 		   Rsvd[8];				// 12-19.������
} TGroupD;


// ������ G (����� 230, ���������� 30)  - ���������������� ��������� ����
typedef struct _TGroupG
{
	Uns            Mode;                // G0. 230 ����� �����
	TLedsReg       LedsReg;             // G1. 231 ���� ����������� �����
	TOutputReg     OutputReg;			// G2. 232 ���� ���������� �������
	Int			   DacValue;			// G3. 233 �������� ���
	Uns			   DisplShow;			// G4. 234 ����� �������
	Uns			   TestCamera;			// G5. 235 ���� � ������
	Uns			   ThyrGroup;			// G6. 236 ����� ������ ����������
	Uns            SifuEnable;          // G7. 237 ���������� ������ ����
	Uns            SimulSpeedMode;		// G8. 238 ����� ��������� ��������
	Uns			   Rsvd2[9];			// G9-17. 239-247 ������
	Uns			   ThyrOpenAngle;		// G18. 248 �������� ���� ��������
	Uns			   DiscrInTest;			// G19. 249 ���� ���������� ������
	Uns			   DiscrOutTest;		// G20. 250 ���� ���������� �������
	Bool		   IsDiscrTestActive;	// G21. 251 ������� �� ���� ��/��
	Uns 		   Rsvd[8];			    // G22-29. 252-259 ������
} TGroupG;

// ������ H (����� 260, ���������� 140)
typedef struct _TGroupH
{
	TCalibState     CalibState;          // 0.��������� ����������
	Uns             CalibRsvd;           // 1.������ ��� ����������
	Uns             ClosePosition[2];    // 2-3.��������� �������
	Uns             OpenPosition[2];     // 4-5.��������� �������
	Uns             Password1;           // 6.�������� ������
	Uns             Password2;           // 7.��������� ������
	Uns             ScFaults;            // 8.������ ��
	Uns				UporOnly;			 // 9.�������� ���������� ������ ������ �� �����
	Uns             CycleCnt;            // 10.������� ������
	Uns             reservH11;       	 // 11.������
	Uns			    reservH12;		 	 // 12.������
	Uns				KickCount;			 // 13.����������� ������
	TLedsReg        LedsReg;             // 14.��������� ����������� �����
	Uns				CmdSource;			 // 15.�������� ������� ��� �����
	Uns				TransCurr;			 // 16.��� ��������
	TCubArray		TqCurr;				 // 17-36.��� �����������
	TCubArray		TqAngUI;			 // 37-56.���� ��������
	TCubArray		TqAngSf;			 // 57-76.���� ����
	Uns				ZazorTime;			 // 89.����� ������� ������
	TNetReg         FaultsNet;           // 90.����������� ����
	Uns             Umid;             	 // 91.������� ����������
	Uns             VSkValue;          	 // 92.��������� ��� �������� ����
  	Uns     	    PhOrdValue;        	 // 93.����������� ��� ����
	TInputReg       Inputs;           	 // 94.��������� ���������� ������
	TOutputReg      Outputs;          	 // 95.��������� ���������� �������
	TReverseType	ReverseType;         // 96.��� �������
	Uns             TuReleMode;          // 97.�������� �����
	Uns             IU_Mpy;              // H98.  ������������� ���� ���� U
	Uns             IV_Mpy;              // H99.  ������������� ���� ���� V
	Uns             IW_Mpy;              // H100. ������������� ���� ���� W
	Uns             IU_Offset;           // H101. �������� ���� ���� U
	Uns             IV_Offset;           // H102. �������� ���� ���� V
	Uns             IW_Offset;           // H103. �������� ���� ���� W
	Uns             UR_Mpy;              // H104. ������������� ���������� ����
	Uns             US_Mpy;              // H105. ������������� ���������� ����
	Uns             UT_Mpy;              // H106. ������������� ���������� ����
	Uns             UR_Offset;           // H107. �������� ���������� ���� R
	Uns             US_Offset;           // H108. �������� ���������� ���� S
	Uns             UT_Offset;           // H109. �������� ���������� ���� T
	Uns				Rsvd110;
	Uns				Rsvd111;
	Uns				Rsvd112;
	TDeviceReg      FaultsDev;           // 113.����������� ����������
	Uns             StartIndic;			 // 114.��������� � ������
 	Uns             SleepTime;           // 115.�������� �����  
	Uns             BusyValue;       	 // 116.������� ����������
	Uns 			LogEvAddr;			 // 117. ������� ����� ������� �������
	Uns 			LogCmdAddr;			 // 118. ������� ����� ������� ������
	Uns 			LogParamAddr;		 // 119. ������� ����� ������� ��������� ����������
	Uns				LogEvCount;			 // 120. ���������� ���������� ����� ������� �������
	Uns				LogCmdCount;		 // 121. ���������� ���������� ����� ������� ������
	Uns				LogParamCount;		 // 122. ���������� ���������� ����� ������� ��������� ����������
	Uns             Torque;           	 // 123. ������
	Int             Speed;				 // 124. ��������
	Uns				Seconds;			 // 125. �������
	TContactorGroup ContGroup;			 // 126. ���������� ������������
	TBurCmd 		LogControlWord;		 // 127. ������� ���
	Uns				LogReset;			 // 128. ����� ��������
	TNetReg         DefectsNet;          // 129.����������� ���� (��� ��������������)
	Uns             Rsvd7;         	 	 // 130. ������
	TLoadReg        FaultsLoad;          // 131.����������� ��������
	Uns				ADC_iu;				 // 132.��� IU
	Uns				ADC_iv;				 // 133.��� IV
	Uns				ADC_iw;				 // 134.��� IW
	Uns             Imid;				 // 135.������� ���
  	Uns             ISkewValue;          // 136.��������� ����� ��������
	Uns             Position;            // 137.���������
	Uns 			Rsvd3[2];			 // 138-139.������
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
	Uns 		 	Speed;				// 19 ������� ��������
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
#define REG_TASK_CLOSE			GetAdr(GroupD.TaskClose)
#define REG_RS_RESET			GetAdr(GroupD.RsReset)

#define CMD_DEFAULTS_USER	0x0010	// ���������������� ��������� �� ���������
#define CMD_RES_CLB			0x0020	// ����� ���������� ������� ���������
#define CMD_RES_FAULT		0x0040	// ����� �����
#define CMD_CLR_LOG			0x0080	// ������� �������
#define CMD_RES_CYCLE		0x0100	// ����� �������� ������
#define CMD_PAR_CHANGE		0x0200	// ��������� ����������
#define CMD_DEFAULTS_FACT 	0x0400	// ��������� ��������� �� ���������

#define TS_SIGNAL_MASK_M 	2047		// ����� ��� ��������� ������ �������� �� ���������� ����������� ���� ��������
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
	NULL, \
	NULL, \
	True, False, POS_UNDEF, 0, \
	&Ram.GroupB.BreakMode, \
	&Calib, \
	0, 0, 0, vcwNone, 0, 0, 0, 0,\
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
