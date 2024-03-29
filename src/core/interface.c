/*------------------------
interface.c

������������ ������
--------------------------*/
#define CREATE_DRIVE_DATA
#define CREATE_STRUCTS
//#define USE_DEFPAR_VALUES

#include "config.h"



TLogEv			LogEv = LOG_EV_DEFAULTS;				// ��������� ��������� ������� �������
TLogEvBuffer	LogEvBuffer[LOG_EV_BUF_CELL_COUNT];		// ����� ����������, ������� ������������ �� ����������� �������
TLogCmd			LogCmd = LOG_CMD_DEFAULTS;				// ��������� ��������� ������� ������
TLogParam 		LogParam = LOG_PARAM_DEFAULTS;			// ��������� ��������� ������� ��������� ����������

Uns 		PositionAccTmp = 0;

CALIBS      Calib = CALIBS_DEFAULT;
TVlvDrvCtrl Mcu   = DRIVE_CTRL_DEFAULT;
//TOutputReg  Dout  = {0x00};

MENU        Menu  = MENU_DEFAULT;					// ��������� ��������� ����

Uns ImReadBuf[IM_READ_BUF_SIZE * 2];			// ����� ��������������� ������
TIM			Im = IM_DEFAULT;

Byte InterfRefrState = 0;
Bool DataError = false;

char LogEvBufIndex = 0;
bool LogEvMainDataFlag = false;
char LogEvBufCurIndex = 0;
Byte LogEvBufSeconds = 0;
Uns  PrevLogAddr[3] = {0,0,0};
Uns  PrevLogCount[3] = {0,0,0};
Bool LogEvSecondsRead = False;

Bool ReadLogFlag = False;
Uns  PultActiveTimer = 0;

Uint16 IrImpCount = 0;

Uns  LcdEnable = 1;

Uns  BlinkTimer = 0;
Uns  BtnOldStatus;
Uns  PrevCycle;

Bool PowerEnable = True;
Uns  DisplayTimer = (PRD_10HZ * 1);
Uns  DisplayRestartTimer = 0;
Bool DisplayRestartFlag = false;

Bool ClearLogFlag = False;	// ������� ������� ???
Uns  ClearLogSource = 0;
Uns  DefFlag = 0;			// ����������� � ���������� ���������� 
Uns  DefCode;
Uns  DefAddr;
Uns  CancelTimer = 0;		// ����� �� ������

Bool IsDriveTypeShow = true;
Uns ShowDriveTypeTimer = 0;
Uns PduKeyFlag = 0;

Uns DbgLog = 0;
Uns TempMuDu = 0;
Uns MuDuDefTimer = 0;
Uns LedTestTimer = 0;

TDigitalInput 	dinDeblok,
				dinStop,
				dinClose,
				dinOpen,
				dinDU,
				dinMU;
Uns		startTimer = PRD_10HZ*1;	// ������ �������������. ���� �� �� ����� 0, ���� �������������. 1 �������.
extern  Int  	lastDirection;
extern	Bool	calibStopFlag;

 Uns mudustatedefect = 0;
 Uns mudustatefault = 0;

 Byte PduKeysControls (Byte );
__inline void CheckParams(void);
__inline void DefParamsSet(Uns Code);
static   void PutAddData(Uns Addr, Uns *Value);
static   void UpdateCode(Uns Addr, Uns *Code, Uns Def);
Uns	DigitalInputUpdate (TDigitalInput *);

Uns PowerOffCnt = 0;
Uns PowerOnCnt = 0;

Bool PI_CalibEnable = false; 			// ���� ���������� ���������� ����� ��

void InterfaceInit(void)
{
	Uns i;

	ReadParams();
//!!!!!!!!!!!!!!!!!!!!!!	
	CheckParams();
//!!!!!!!!!!!!!!!!
	GrD->SetDefaults = 0;
	GrC->SetDefaults = 0;
	GrG->DisplShow = 0;
	GrT->ComReg.all = 0;

	GrH->LogReset = 0;
//!!!!!!!!!!!!!!!!

	PrevLogAddr[PREV_LEV_INDEX]   = GrH->LogEvAddr;
	PrevLogAddr[PREV_LCMD_INDEX]  = GrH->LogCmdAddr;
	PrevLogAddr[PREV_LPAR_INDEX]  = GrH->LogParamAddr;
	PrevLogCount[PREV_LEV_INDEX]  = GrH->LogEvCount;
	PrevLogCount[PREV_LCMD_INDEX] = GrH->LogCmdCount;
	PrevLogCount[PREV_LPAR_INDEX] = GrH->LogParamCount;

	memset(LogParam.MbBuffer, 0, MB_PARAM_BUF_COUNT);
	memset(ImReadBuf, 0, (IM_READ_BUF_SIZE * 2));

	for (i= 0; i < LOG_EV_BUF_CELL_COUNT; i++)
	{
		LogEvBuffer[i].LogStatus.all = 0;
		LogEvBuffer[i].LogPositionPr = 0;
		LogEvBuffer[i].LogTorque	 = 0;
		LogEvBuffer[i].LogUr		 = 0;
		LogEvBuffer[i].LogUs		 = 0;
		LogEvBuffer[i].LogUt		 = 0;
		LogEvBuffer[i].LogIu		 = 0;
		LogEvBuffer[i].LogIv		 = 0;
		LogEvBuffer[i].LogIw		 = 0;
		LogEvBuffer[i].LogTemper	 = 0;
		LogEvBuffer[i].LogInputs	 = 0;
		LogEvBuffer[i].LogOutputs 	 = 0;
	}
	

	GrA->Status.all = 0x01;
	GrA->CalibState = GrH->CalibState;
	GrA->CycleCnt = GrH->CycleCnt;
	PrevCycle = GrH->CycleCnt;
	GrA->MkuPoVersion = (DEVICE_GROUP * 1000) + VERSION; // 1*1000+114 = 1114
#if PLAT_VERSION_7
	GrC->MkuPoSubVersion = (MODULE_VERSION * 100) + SUBVERSION;
#else
	GrC->MkuPoSubVersion = (MODULE_VERSION_OLD * 100) + SUBVERSION;
#endif

	GrG->SimulSpeedMode = 0;	// � ������ ������ ��������� ����� ��������� ��������
	GrH->initComplete = false;	// ������� ���� ���������� �������������
	GrH->extraPassword = 0;

	if (GrC->ModbusPauseStart == 0) GrC->ModbusPauseStart = 3; 		// 3 = 0,3 ���

	if (GrH->ScFaults) LowPowerReset |= BIT0;

	RefreshData(0);

	// ������� ������
	Ram.GroupH.StartIndic =REG_TORQUE_ADDR; //8;

	// �������� �������� ���������
	if (IsNoCalib()) Menu.Express.Enable = TRUE;

	#if BUR_M
	GrB->DuSource = mdsSerial;
	GrB->KvoKvzOffOnStop = 0;
	#endif

	//memset(&RamTek, 0, sizeof(TTEKDriveData));

	// ���������� ������� Bluetooth � ��������������� ������
	Im.EnableReceive = Bluetooth.EnableRx;
	Im.EnableTransmit = Bluetooth.EnableTx;
	Im.TransmitByte = Bluetooth.TransmitByte;
#if BUR_90
	// ���� ��� ��������� ���������� ��������� ������������� ���� ����� ����, �� ���������� �������� ������ ����������
	if (!GrC->IU_Mpy && !GrC->IV_Mpy && !GrC->IW_Mpy)
	{
		GrC->IU_Mpy = GrC->IV_Mpy = GrC->IW_Mpy = 4332;
		WritePar(GetAdr(GroupC.IU_Mpy), &GrC->IU_Mpy, 3);
		GrD->SetDefaults = 1;
	}
#endif
}

void DisplayStart(void)
{
	if (DisplayTimer > 0)
	{
		DisplayTimer--;
		if (!DisplayTimer) Display.Enable = true;
	}

	if (DisplayRestartTimer > 0)
	{
		Display.ResTout = 5;
		DisplayRestartTimer--;
	}
	else
	{
		Display.ResTout = GrC->DisplResTout * (Uns)DISPL_SCALE;
	}
}

void InterfIndication(void)
{
	GrA->Faults.Net.all  = (GrH->FaultsNet.all | GrH->DefectsNet.all);
	GrA->Faults.Load.all = GrH->FaultsLoad.all;
	GrA->Faults.Dev.all  = GrH->FaultsDev.all;
	GrA->Inputs.all      = GrH->Inputs.all;
	GrA->Outputs.all     = GrH->Outputs.all;
	GrA->CycleCnt 	     = GrH->CycleCnt;

	// ����������� ����� "���������� �������������"
	if (startTimer)					// ���� ������ ������ 0
	{
		startTimer--;
		GrH->initComplete = false;	// �������, ��� ������������� ��� �� ���������
	}
	else							// ���� ������ ��������� �� ����
		GrH->initComplete = true;	// �������, ��� ������������� ���������
}

void RefreshData(Uns addr)
{
	static Bool Flag = false;
	
	RimRefrState    = 1;
	DmcRefrState    = 1; 	
	InterfRefrState = 1;

	SerialCommRefresh();
	
	if (addr == REG_INDICATOR_TYPE)
	{
		DisplayReset(&Display, (Uns)Ram.GroupC.IndicatorType);
	}

	if (!Flag)
	{
		while(!InterfaceRefresh())   {};
		while(!RimDevicesRefresh())  {};
		while(!DmcControlRefresh())  {};
	//	while(!ProtectionsRefresh()) {};
		Flag = true;
	}
}

void DataSetting(void)	//???
{
	struct MENU_DCR Dcr;

	if (DefFlag != 1) return;

	if (DefAddr < RAM_DATA_SIZE)
	{
		GetDcr(DefAddr, &Dcr);
		if (((Dcr.Config.all & DefCode) == DefCode)
			&&(DefAddr != REG_PRODUCT_YEAR)		// �� ��� ������������
			&&(DefAddr != REG_FACT_NUM)			// �� ��������� �����
			&&(DefAddr != REG_MAX_TORQUE)		// �� ������������ ������
			&&(DefAddr != REG_I_NOM)			// �� ����������� ���
			&&(DefAddr != REG_DRIVE_TYPE)		// �� ��� ��������������
			&&(DefAddr != REG_ENCODER_TYPE)		// �� ��� ������� ���������
#if !BUR_90
			&&(DefAddr != REG_versionPO_PUE)	// �� ������ �� ���
#endif
#if BUR_90
			&&(DefAddr != REG_CUR_IU)     // �� ������������� ���� U
			&&(DefAddr != REG_CUR_IV)     // �� ������������� ���� V
			&&(DefAddr != REG_CUR_IW)     // �� ������������� ���� W
#endif
			&&(DefAddr != REG_GEAR_RATIO) )		// �� �� ���������
		{
	 		*(ToUnsPtr(&Ram) + DefAddr) = Dcr.Def;
		}
		DefAddr++;
		return;
	}
	
	if (IsMemParReady())
	{
		WriteAllParams();
		RefreshData(DefAddr);
		DefFlag = 2;
	}
}

void PowerOn(void)
{
	static Bool DisplShcFlag = true;

#if BUR_90
	Ram.GroupH.PP90Reg.bit.LcdEnable = 0; // �������� 0
#else
	LcdEnable = 1;
#endif
// 1) Display.Restart = 5;		
// 2) ���������
// 3) Display.Restart = 0;

	if (DisplayRestartFlag)
	{
		DisplayTimer = (PRD_10HZ * 1);
	//	DisplayRestartTimer = (PRD_10HZ * 2);
		DisplayRestartFlag = false;
	}
	
	// ������� ��� ���������, ���� ���� ��
	if (DisplShcFlag && (GrH->FaultsLoad.bit.ShCU || GrH->FaultsLoad.bit.ShCV || GrH->FaultsLoad.bit.ShCW))
	{
		DisplayRestartTimer = (PRD_10HZ * 1);
		DisplayRestartFlag = true;
		DisplShcFlag = false;
	}

	if (!DisplayTimer) 
		Display.Enable = True;

	PowerEnable = True;	

	if (!GrG->TestCamera)
	{
		SerialCommRefresh();
	}

	PowerOnCnt++;
}

void PowerOff(void)
{
#if BUR_90
	Ram.GroupH.PP90Reg.bit.LcdEnable = 1; // �������� 0
#else
	LcdEnable = 0;
#endif
 	Display.Enable = False;
	GrH->LedsReg.all = 0x00;
	PowerEnable = False;

	#if BUR_M

	if (!(GrH->FaultsLoad.all & LOAD_SHC_MASK))
	{
		PiData.DiscrOut = 0x00; //???
		PiData.DiscrOut2 = 0x00;
	}
	#else
	PiData.DiscrOut = 0x00;
	PiData.DiscrOut2 = 0x00;
	#endif

	OUT_SET(TEN_OFF, 1);

	PowerOffCnt++;
}

Bool InterfaceRefresh(void)
{
	if (!InterfRefrState) return true;

	Calib.GearRatio = GrC->GearRatio;
	Calib.GearInv   = CalcClbGearInv(&Calib);

	Mcu.Valve.BreakDelta = CalcClbAbsRev(&Calib, GrB->OverwayZone);

	Menu.Update = True;
	Menu.SleepTime = GrB->SleepTime * (Uns)SLEEP_SCALE;

	InterfRefrState = 0;

	return (!InterfRefrState);
}

void DataBufferPre(void)
{
	static Uns PreTimer = 0;
//	static Uns Timer = LOG_BUF_START_TOUT;

	if (GrH->LogReset > 0)							// �������� ������ � ������ ��������
	{
		GrH->LogReset 	   = 0;
		
		GrH->LogEvAddr 	   = 0;
		GrH->LogCmdAddr    = 0;
		GrH->LogParamAddr  = 0;
		GrH->LogEvCount    = 0;
		GrH->LogCmdCount   = 0;
		GrH->LogParamCount = 0;
		Mcu.EvLog.Value    = CMD_CLR_LOG;
	}



//	if (Timer > 0) {	Timer--;	return;		}

	if ((++PreTimer >= PRD_200HZ) && !LogEv.EventFlag)						// ������ �� �������	
	{			
		PreTimer = 0;

		LogEvBuffer[0] = LogEvBuffer[1];
		LogEvBuffer[1] = LogEvBuffer[2];
		LogEvBuffer[2] = LogEvBuffer[3];
		LogEvBuffer[3] = LogEvBuffer[4];

		LogEvBuffer[4].LogStatus 	 = GrA->Status;
		LogEvBuffer[4].LogPositionPr = GrA->PositionPr;
		LogEvBuffer[4].LogTorque	 = GrA->Torque;
		LogEvBuffer[4].LogUr		 = GrA->Ur;
		LogEvBuffer[4].LogUs		 = GrA->Us;
		LogEvBuffer[4].LogUt		 = GrA->Ut;
		LogEvBuffer[4].LogIu		 = GrA->Iu;
		LogEvBuffer[4].LogIv		 = GrA->Iv;
		LogEvBuffer[4].LogIw		 = GrA->Iw;
		LogEvBuffer[4].LogTemper	 = GrA->Temper;
		LogEvBuffer[4].LogInputs	 = GrA->Inputs.all;
		LogEvBuffer[4].LogOutputs 	 = GrA->Outputs.all;
	}
}

void LogEvControl(void)
{
	static Uns Timer = (Uns)LOG_START_TOUT;
	static Uns Addr;

	if (Timer > 0) Timer--;													// �������� ������ �������

	LogEv.Enable = !Timer;

	GrA->Faults.Dev.bit.LowPower = !PowerEnable;								// ���������� ��� �����, �.�. �� ����� ������� �� ������, � �� ���� �� ����� ������ ��� ������

	LogEv.FaultsState[0] = GrA->Faults.Proc.all & PROCESS_EVLOG_MASK;		// �������� �������� ��������� �������
	LogEv.FaultsState[1] = GrA->Faults.Net.all  & NET_EVLOG_MASK;
	LogEv.FaultsState[2] = GrA->Faults.Load.all & LOAD_EVLOG_MASK;
	LogEv.FaultsState[3] = GrA->Faults.Dev.all  & DEV_EVLOG_MASK;

//--------------------------------------------------------------------------------
	LogEvUpdate(&LogEv);													// �������� ������� ������������ ������ ������ ������� �������

	GrA->Faults.Dev.bit.LowPower = 0;											// ���������� ���, ����� �� ������������ ��� �����
//--------------------------------------------------------------------------------
	if (IsMemParReady())
	{
		if (GrH->LogEvAddr != PrevLogAddr[PREV_LEV_INDEX])					// ��������� ������� ����� ������ ������ � ����������
		{
			WritePar(GetAdr(GroupH.LogEvAddr), &GrH->LogEvAddr, 1);			// ���������� ����� ����� ������ ������ � ������
			PrevLogAddr[PREV_LEV_INDEX] = GrH->LogEvAddr;					// ���������� �����
		}
		else if (GrH->LogEvCount != PrevLogCount[PREV_LEV_INDEX])
		{
			WritePar(GetAdr(GroupH.LogEvCount), &GrH->LogEvCount, 1);
			PrevLogCount[PREV_LEV_INDEX] = GrH->LogEvCount;
		}
		else if (LogEv.WriteFlag)													// ��������� ���������� �������� ������ � ���� ���������� ������
		{
			Addr = LOG_EV_START_ADDR + GrH->LogEvAddr * LOG_EV_DATA_CNT * LOG_EV_DATA_CELL;		// ��������� ����� 
			WritePar(Addr, LogEv.Data, LOG_EV_DATA_CNT);										// ���������� ��������� � ������ ������������� �������

			LogEvMainDataFlag = true;
			LogEvBufCurIndex = LOG_EV_BUF_CELL_COUNT;											// �������� �������
			LogEv.WriteFlag = false;															// ���������� ���� ���������� ������. �� ����� ����� ������������ ������ ��������� � ������ �������

			// ����� ������ �������� ������, ��������� ��������� ����� ������
			Addr = LOG_EV_BUF_START_ADDR + GrH->LogEvAddr * LOG_EV_BUF_DATA_CNT * LOG_EV_BUF_DATA_CELL;

			return;
		}
		else if (Menu.EvLogFlag)									// ������ ������� �� ����
		{
			if (ReadLogFlag)
			{
				Menu.EvLogFlag = False;
				ReadLogFlag = False;
			}
			else
			{
				// ��������� ����� �� ������ �� ��� � ����������� �� �������� ��������� � ����
				Addr = LOG_EV_START_ADDR + Menu.EvLog.Position * LOG_EV_DATA_CNT * LOG_EV_DATA_CELL;
				// ���������� ������ �������� �� ������. � �������� ������ - ������ E Ram
				ReadPar(Addr, ToUnsPtr(&Ram) + LOG_EV_RAM_DATA_ADR, LOG_EV_DATA_CNT);

				ReadLogFlag = True;
			}
		}
	}

	if ((IsMemLogReady()) && (LogEvMainDataFlag))
	{
		if (LogEvBufCurIndex > 0)
		{

			WriteLog(Addr, ToUnsPtr(&LogEvBuffer[LogEvBufCurIndex - 1]), LOG_EV_BUF_DATA_CNT);		// ����� � ������

			LogEvBufCurIndex--;

			Addr += LOG_EV_BUF_DATA_CNT;							// ����������� ����� �� ���������� ���������� �� ������ ������
		}

		if (LogEvBufCurIndex == 0)					// ���� ������� ����������, ������ �� ����� �� ������ ���������� (5-� �������)
		{
			LogEvMainDataFlag = false;							// �������� ��� ������ �� ������ ���������� ���� ������ ��������� ������� (0-� �������)
			if (++GrH->LogEvAddr >= LOG_EV_CNT)					// �������������� ��������� ����� ������ � ��������� ��� ����� �� ������� ������������ ������� 
				GrH->LogEvAddr = 0;								// ���� ����� �� ������������ �������, �� ����������� ����� ������ �����

			if (GrH->LogEvCount < LOG_EV_CNT)					// ���� �� ��������� ������������ ����� �������
				GrH->LogEvCount++;								// �������������� ����� ������� �������� ������

			LogEv.EventFlag = false;							// ���������� �������� ���� �������, ����� �������� ������ ������ ��������� 5-���. �����
		}
	}
}

void GetCurrentCmd(void)	// 50 Hz
{
	Uns LogControlWord = bcmNone;
	static Uns PrevEvLogValue = 0;
	static Bool FirstCmd = true;
	static Uns DelayTimer = 0;

	if (Mcu.EvLog.Value != 0)
		LogControlWord = bcmNone;
	else if (Mcu.EvLog.QueryValue)
	{
		if (DelayTimer++ > 15)
		{
			Mcu.EvLog.Value = Mcu.EvLog.QueryValue;
			Mcu.EvLog.Source = Mcu.EvLog.QuerySource;
			Mcu.EvLog.QueryValue = 0;
			Mcu.EvLog.QuerySource = 0;
			LogControlWord = bcmNone;
			DelayTimer = 0;
		}
	}

	// �������� ������������ ������� ����, �������, �������
	if (Mcu.EvLog.Value <= CMD_OPEN)	// CMD_OPEN = 0x4, CMD_CLOSE = 0x2, CMD_STOP = 0x1, ��� ��� ������ ��� ����� 0x4 
	{
		if (Mcu.EvLog.Value == PrevEvLogValue)
		{
			Mcu.EvLog.Value = 0;
			return;
		}
	}

	switch(Mcu.EvLog.Value)
	{
		case CMD_STOP: 			LogControlWord = bcmStop;				break;
		case CMD_CLOSE: 		LogControlWord = bcmClose;				break;
		case CMD_OPEN: 			LogControlWord = bcmOpen;				break;
		case CMD_DEFAULTS_USER: LogControlWord = bcmSetDefaultsUser; 	break;
		case CMD_RES_CLB:		LogControlWord = bcmCalibReset;			break;
		case CMD_RES_FAULT: 	LogControlWord = bcmPrtReset;			break;
		case CMD_CLR_LOG: 		LogControlWord = bcmLogClear; 			break;
		case CMD_RES_CYCLE:		LogControlWord = bcmCycleReset;			break;
		case CMD_DEFAULTS_FACT: LogControlWord = bcmSetDefaultsFact;	break;
		case CMD_DEFSTOP:		LogControlWord = bcmDefStop;			break;
		case CDM_DISCROUT_TEST: LogControlWord = bcmDiscrOutTest;		break;
		case CMD_DISCRIN_TEST: 	LogControlWord = bcmDiscrInTest; 		break;
		case CMD_FIX_POS: 		LogControlWord = bcmFixPos; 			break;
	}

	if (Mcu.EvLog.Value != 0)
	{
		PrevEvLogValue = Mcu.EvLog.Value;

		if (FirstCmd)
		{
			FirstCmd = false;

			// ���� ����� ������ �� ������ - ����, �� ������� ��
			if (Mcu.EvLog.Value == CMD_STOP)
				LogControlWord = bcmNone;
		}
	}
	if (Mcu.EvLog.Value)
	{
		LogControlWord = LogControlWord | Mcu.EvLog.Source;
		Ram.GroupH.CmdSource  = Mcu.EvLog.Source;
		Mcu.EvLog.Source = 0;
	}
	Mcu.EvLog.Value = 0;

	LogCmd.CmdReg = LogControlWord;
}

void LogCmdControl(void)
{
	static Uns Timer = (Uns)LOG_START_TOUT;
	Uns Addr;

	if (Timer > 0)	Timer--;

	LogCmd.Enable = !Timer;
//--- �������� ��� ���������� ������ ������� "�������" � "�������" ��� ����� � ���������� ---
	if (LogCmd.CmdReg && calibStopFlag)													// ���� ���� ������������ �������� ��� ������ � ������ � ��� ���������� ������� �� ���������
	{
		GrA->Status.bit.Opened = (calibStopFlag == CLB_OPEN);							// ���������� ������ "�������" � "�������"
		GrA->Status.bit.Closed = (calibStopFlag == CLB_CLOSE);							// � ����������� �� �������� �����

		calibStopFlag = 0;																// ������� ����
	}
//--------------------------------------------------------------------------------
	LogCmdUpdate(&LogCmd);																// �������� ������� ������������ �������
//--------------------------------------------------------------------------------
	// ���������� ����� ������ �������
	if (IsMemParReady())
	{
		if (GrH->LogCmdAddr != PrevLogAddr[PREV_LCMD_INDEX])							// ��������� ���������� ���
		{																				// ��������� ��������� ������ ������� ������ �������
			WritePar(GetAdr(GroupH.LogCmdAddr), &GrH->LogCmdAddr, 1);					// ���������� ����� ����� � ���
			PrevLogAddr[PREV_LCMD_INDEX] = GrH->LogCmdAddr;								// ��������� ���������� �����
		}
	//--------------------------------------------------------------------------------
		else if (LogCmd.WriteFlag)														// ��������� ���������� ���
		{																				// ��������� ������� ����� ���������� ������ �������
			Addr = LOG_CMD_START_ADDR + GrH->LogCmdAddr * LOG_CMD_DATA_CNT;				// ��������� ��������� ����� ������
			WritePar(Addr, LogCmd.Data, LOG_CMD_DATA_CNT);								// ���������� �� ������ �������� Eeprom1

			LogCmd.WriteFlag = false;													// ���������� ���� ���������� ������ �������

			if (GrH->LogCmdCount < LOG_CMD_CNT)											// ���� �� ��������� ������������ ����� �������
				GrH->LogCmdCount++;														// �������������� ����� ������� �������� ������

			if (++GrH->LogCmdAddr >= LOG_CMD_CNT)										// �������������� ����� ������� ������ �������
				GrH->LogCmdAddr = 0;													// ��������� ����� �� ������� (��������� ���������� ���������� �������). ���� ��������� - ��������
		}
		else if (GrH->LogCmdCount != PrevLogCount[PREV_LCMD_INDEX])
		{
			WritePar(GetAdr(GroupH.LogCmdCount), &GrH->LogCmdCount, 1);
			PrevLogCount[PREV_LCMD_INDEX] = GrH->LogCmdCount;
		}
	}
}

void LogParamMbExec(void)
{
	Uns *Data = (Uns *)&Ram;
	static Uns LogParamIndex = 0;

// ��������� ������, ����� ��� ������ ������������ � ������, ����� �� ���� ��������� � �������
// ����������� ��������� ����������. ��� ��, ���� ������ �� �������, �.�. ����� ��������� �� ������ ����
	if ((LogParam.ExecFlag) || (!LogParam.MbIndex) || (LogParam.WriteFlag))
		{	return;	}

	LogParam.Addr 	  = LogParam.MbBuffer[LogParamIndex];
	LogParam.NewValue = Data[LogParam.Addr];
	LogParam.ExecFlag = true;

	if (++LogParamIndex == LogParam.MbIndex)
	{
		LogParamIndex = 0;
		LogParam.MbIndex = 0;
	}
}

void LogParamControl(void)
{
	static Uns Timer = (Uns)LOG_START_TOUT;
	Uns Addr;

	if (Timer > 0)	Timer--;

	LogParam.Enable = !Timer;

//--------------------------------------------------------------------------------
	LogParamUpdate(&LogParam);															// �������� ������� ������������ �������
//--------------------------------------------------------------------------------
	if (IsMemParReady())
	{
		if (GrH->LogParamAddr != PrevLogAddr[PREV_LPAR_INDEX])							// ��������� �������� ���������� ���
		{																				// ��������� ��������� ������ ������� ������
			WritePar(GetAdr(GroupH.LogParamAddr), &GrH->LogParamAddr, 1);				// ���������� ����� ����� � ���
			PrevLogAddr[PREV_LPAR_INDEX] = GrH->LogParamAddr; 							// ��������� ���������� ����� ������ �������
		}
	//--------------------------------------------------------------------------------
		else if (LogParam.WriteFlag)													// ��������� ���������� ������
		{																				// ��������� ������� ����� ���������� ������ �������
			Addr = LOG_PARAM_START_ADDR + GrH->LogParamAddr * LOG_PARAM_DATA_CNT;		// ��������� ��������� ����� ������ � ���
			WritePar(Addr, LogParam.Data, LOG_PARAM_DATA_CNT);							// ���������� �������� ��� ������
			LogParam.WriteFlag = false; 												// ���������� ���� ���������� ������

			if (GrH->LogParamCount < LOG_PARAM_CNT)										// ���� �� ��������� ������������ ����� �������
				GrH->LogParamCount++;													// �������������� ����� ������� �������� ��������� ����������

			if (++GrH->LogParamAddr >= LOG_PARAM_CNT)									// �������������� ����� ������ �������
				GrH->LogParamAddr = 0;													// ���� �� ����� �� ������� (��������� ���������� ���������� �������), ��������
		}
	//--------------------------------------------------------------------------------
		else if (GrH->LogParamCount != PrevLogCount[PREV_LPAR_INDEX])
		{
			WritePar(GetAdr(GroupH.LogParamCount), &GrH->LogParamCount, 1);
			PrevLogCount[PREV_LPAR_INDEX] = GrH->LogParamCount;
		}
	}
}

void ShowDriveType(void)
{
	if (IsDriveTypeShow)
	{
		if ((ShowDriveTypeTimer < DRV_TYPE_SHOW_TIME)&&(PduKeyFlag == 0)&&(!(GrC->HallBlock.bit.StopMU && GrC->HallBlock.bit.StopDU)))//||(Pult.Key == 0))
		{
			ShowDriveTypeTimer++;
			Menu.State = MS_DRIVE_TYPE;
			ReadValuesStr(Menu.HiString, 229 + GrB->DriveType);
			Menu.LoString[11]=' ';
			if (BUR_M) Menu.LoString[10]='M';
			else Menu.LoString[10]='T';
			DecToStr(GrA->MkuPoVersion, &Menu.LoString[9], 3, 4, True, False);
		}
		else
		{
			Menu.State = MS_START;
			IsDriveTypeShow = false;
			ShowDriveTypeTimer = 0;
			PduKeyFlag = 0;
		}
	}
} 

// ���������� ����������, ������� ������������ �� ������� ������ ����
void StartDispl(String Str)
{
	Uns displPosition = 0;
	displPosition = !GrA->Faults.Dev.bit.PosSens ? GrA->PositionPr : 9999; // ���� "���� ������� ���������", ������� 999.9
	DecToStr(displPosition, &Menu.HiString[8], 1, 4, True, True);
	Menu.HiString[9] = '%';

	//ReadAddStr(Menu.HiString, 3);

	if (IsPassword1())			Menu.HiString[11] = CODE_ICO;
	if (MbConnect)				Menu.HiString[13] = CONN_ICO;
	if (Bluetooth.IsConnected)	Menu.HiString[15] = BT_ICO;

#if IR_IMP_TEST
	DecToStr(IrImpCount, &Menu.LoString[12], 0, 4, FALSE, FALSE);
#endif	
}

// ���������� �������������� ��������� ����
Bool EnableEdit(Uns Password, Uns Param)
{
	switch (Password)
	{
		case 1: if (IsPassword1()) return False; break; 
		case 2: if (IsPassword2()) return False; break; 
	}

	return True;
}
 
// ������ ����������� ��������� � ���
Bool WriteValue(Uns Memory, Uns Param, Uns *Value)
{
	if (Memory && !IsMemParReady()) return False;
	
	if (Param == REG_CODE)
	{
		if (!IsMemParReady()) return False;
		UpdateCode(REG_PASSW1, Value, DEF_USER_PASS);
		return True;
	}
	else if (Param == REG_FCODE)
	{
		if (!IsMemParReady()) return False;
#if BUR_90
		if (*Value == DEF_CALIB_PASS)			// ���� � �������� C1. ��� ������� ������ "������ ������"
		{
			if (!PI_CalibEnable)				// ��������� ���������� ����� ��, ���� ��� ���� ���������
			{
				PI_CalibEnable = true;
				Mcu.Mpu.CalibEnableFlag = true;
			}
			else if (PI_CalibEnable)			// ��������� ���������� ����� ��, ���� ��� ���� ���������
			{
				PI_CalibEnable = false;
				Mcu.Mpu.CalibDisableFlag = true;
			}
			return True;
		}
#endif
		UpdateCode(REG_PASSW2, Value, DEF_FACT_PASS);
		return True;
	}
	/*
	else if (Param == REG_CONTROL)
	{
		if (!(Mcu.ActiveControls & Mcu.EvLog.Source)) return False;
	}
	*/

	*(ToUnsPtr(&Ram) + Param) = *Value;
	if (Memory)
	{
		//TempValue = *Value;
		WritePar(Param, Value, 1);
		RefreshData(Param);
																	// ���������� ������ � ������ ��������� ����������
		LogParam.Addr 	  = Param;									// ����� ���������
		LogParam.NewValue = *Value;									// ����� �������� ���������
		LogParam.ExecFlag = true;									// ���������� ���� ������������ �������
	}
	else if ( (Param == REG_DEV_DATE)||(Param == REG_DEV_TIME) )	// ���� ���������� �������� ������� ��� ����
	{
		if (GrB->DevDate.bit.Year != 0)								// ���� ��� �� ����� ����
		{
			GrH->FaultsDev.bit.TimeNotSet = 0;						// �� ������� ������ "����� �� ������"
			NoTimeSetTimer = 0;
		}
		// ���������� ������ � ������ ��������� ����������
		LogParam.Addr 	  = Param;									// ����� ���������
		LogParam.NewValue = *Value;									// ����� �������� ���������
		LogParam.ExecFlag = true;									// ���������� ���� ������������ �������
	}
	else if ( (Param >= REG_TASK_CLOSE)&&(Param <= REG_REV_CLOSE) )	// ���� ���� ������ � �������� ����������
	{
		// ���������� ������ � ������ ��������� ����������
		LogParam.Addr 	  = Param;									// ����� ���������
		LogParam.NewValue = *Value;									// ����� �������� ���������
		LogParam.ExecFlag = true;									// ���������� ���� ������������ �������
	}

	return True;
}

void GetExpressText(Uns State)
{
	if (State != 9)
	{
		ReadAddStr(Menu.HiString, EXPR_STR_ADDR + (State<<1) + 0);
		ReadAddStr(Menu.LoString, EXPR_STR_ADDR + (State<<1) + 1);
	}
	else
	{
		// ����������� �������� ���� ��� �������� ����������
		ReadAddStr(Menu.HiString, EXPR_STR_ADDR + (State<<1) + 0);
		DecToStr(GrA->PositionPr, &Menu.LoString[8], 1, 4, True, True);
		Menu.LoString[9] = '%';
	}
}

Byte GetExpressState(Byte CurrentState, Uns State)
{
	// �� ������ ��������!
	static Byte ExCycleCount = 0;

	switch (State)
	{
		// ��������� ������� ���������: ������� ��� �������
		case 0: if 		(IsClosed())	CurrentState = 14;		// ���� �������, �� ��������� ���������	
				else if (IsOpened())	CurrentState = 20;		// ���� �������, �� ����� ��� ���������
				break;
		// ��������� �� ��������� "�������"
		case 1: if ((IsOpening()) || (IsClosing()))			CurrentState = 15;		// ���� �������������� ������� "�������", �� ��������� �� ��������� ���
//		case 1: if (IsOpening())			CurrentState = 15;		// ���� �������������� ������� "�������", �� ��������� �� ��������� ���
				break;
		// ������� �� ����� ��������
		case 2: if ((IsStopped()) && (!IsOpened()))	CurrentState = 16;	// �������������� ����
				else if (IsOpened())				CurrentState = 19;	// ������� �� �������
				break;
		// ����� ����������
		case 3: GrD->CalibReset = 1;	ExCycleCount = 0;				CurrentState = 3;	// ������������ � ������ ���� ����������
				break;
		// ������� �� "�������". ��������� ������� ������� �����
		case 4: if (++ExCycleCount >= 2)			// ���� ��� �������� ������ ����
					{	ExCycleCount = 0;	CurrentState = 26;	}
				else						CurrentState = 20;		// ����� ��������� � ��������� "�������"
				break;
				
		// ��������� �� ��������� "�������"
		case 5: if ((IsClosing()) || (IsOpening()))			CurrentState = 21;		// ���� �������������� ������� "�������", �� ��������� �� ��������� ���
//		case 5: if (IsClosing())			CurrentState = 21;		// ���� �������������� ������� "�������", �� ��������� �� ��������� ���
				break;
		// ������� �� ����� ��������
		case 6: if ((IsStopped()) && (!IsClosed()))	CurrentState = 22;	// �������������� ����
				else if (IsClosed())				CurrentState = 25;	// ������� �� �������
				break;
		// ������� �� "�������". ��������� ������� ������� �����
		case 7: if (++ExCycleCount >= 2)			// ���� ��� �������� ������ ����
					{	ExCycleCount = 0;	CurrentState = 26;	}
				else						CurrentState = 14;		// ����� ��������� � ��������� "�������"
				break;
		// ��������� ����� ��� ������ ��� ��������
		case 8: if (GrB->ValveType == vtKlin)	CurrentState = 29;
				else							CurrentState = 30;
				break;
		// ��������� ����� ��� ������ ��� ��������
		case 9: if (GrB->ValveType == vtKlin)	CurrentState = 31;
				else							CurrentState = 32;
				break;
		// ��������� ��� ����������� �����
		case 10: if (BUR_M)					
							CurrentState = 34;
				 else	{
				 			CurrentState = 33;
							#if !BUR_M
							Menu.Express.List11.Buf[0] = GetAdr(GroupB.InputType);
							#endif
						}
				 break;					
	}
	return CurrentState;
}

__inline void CheckParams(void)
{
	struct MENU_DCR Dcr;
	register Int  i;
	Uns *Data = ToUnsPtr(&Ram);
	
	for (i=RAM_DATA_SIZE-1; i >= 0; i--)
	{
		GetDcr(i, &Dcr);
		if (!Dcr.Config.Val.Memory) Data[i] = 0;
		else
		#ifndef USE_DEFPAR_VALUES
		if (!CheckRange(Data[i], Dcr.Min, Dcr.Max))
		#endif
		{
			Data[i] = Dcr.Def;
			DataError = True;
		}
	}
	
	if (DataError) WriteAllParams();
}

static void PutAddData(Uns Addr, Uns *Value)
{
	PutAddHiStr(Addr);
	
	if (!Value) 
		PutAddLoStr(Addr + 1);
	else
	{
		DISPL_ClrLoStr(&Display);
		DecToStr(*Value, &Display.LoStr[7], 0, 2, True, False);
		Display.LoStr[9] = '%';
	}
}

static void UpdateCode(Uns Addr, Uns *Code, Uns Def)
{
	Uns *Password = ToUnsPtr(&Ram) + Addr;
	Bool WriteFlag = False;
	
	if (*Password != 0)
	{
		if (*Code == *Password)
		{
			*Code = 0; *Password = 0;
			Mcu.Mpu.CorrPasswordFlag = true; // ������� ���������, ��� ������ ������ �����
			//WriteFlag = True;
		}
		else if (*Code == Def)
		{
			*Code = 0; *Password = 0;
			Mcu.Mpu.PasswordResetFlag = true; // ������� ���������, ��� ������ �������
			WriteFlag = True;
		}
		else
		{
			Mcu.Mpu.WrongPasswordFlag = true; // ������� ���������, ��� ������ ������ �������
		}
	}
	else if (*Code != 0)					// ���� �������� ����� �������� ������
	{
		if (!GrH->extraPassword) // ���� �������������� ������ ��� �� ������
		{
			GrH->extraPassword = *Code;
			*Code = 0;
		}
		else
		{
			if (GrH->extraPassword == *Code)
			{
				*Password = *Code; *Code = 0;
				Mcu.Mpu.EditDisableFlag = true; 	// ������� ���������, ��� ������ ����� ������
				WriteFlag = True;
				GrH->extraPassword = 0;
			}
			else
			{
				GrH->extraPassword = 0;
				*Code = 0;
				Mcu.Mpu.CodesDontMatch = true; 	// ������� ���������, ��� ������ �� ���������!!
			}
		}

	}
	else if (*Code == 0)
	{
		*Code = 0; *Password = 0;
		Mcu.Mpu.PasswordResetFlag = true; // ������� ���������, ��� ������ �������
		WriteFlag = True;
	}
	
	if (WriteFlag) WritePar(Addr, Password, 1);
}

void AddControl(void)
{
	if (!IsTestMode())
	{
		if (!PowerEnable)				// ���� ����������
		{
			GrH->BusyValue = 0;	// ������� ����������
			Menu.State = MS_START;		// ���� � ��������� �����
		}
		else if (ClearLogFlag)			
		{
			GrH->BusyValue = GetWriteLogPercent();
			PutAddData(BUSY_STR_ADR, &GrH->BusyValue);
			if (IsMemLogReady())
			{
				Mcu.EvLog.Source = ClearLogSource;
				ClearLogFlag = False;	
			}
		}
		else if (DefFlag)	// ���� ���������� ����� ���������� � ��������
		{
			if (DefFlag == 1) 									// 
				GrH->BusyValue = GetSetParPercent(DefAddr);
			else												//
			{
				GrH->BusyValue = GetWriteParPercent();
				if (IsMemParReady()) DefFlag = 0;
			}
			PutAddData(BUSY_STR_ADR, &GrH->BusyValue);
		}
		else if (Mcu.Mpu.WaitConfirmFlag) // ������������� ������� �� ����
		{
			PutAddData(CONFIRM_ADDR, Null);
			if (++CancelTimer >= 50)		// 50 - ��� 5 ������ �� ������� 10 ��
			{
				Mcu.Mpu.WaitConfirmFlag = False;
				CancelTimer = 0;
			}
		}
		else if (Mcu.Mpu.CancelFlag) //������ ������� ���
		{
			PutAddData(CMD_CANC_ADR, Null);
			if (++CancelTimer >= (Uns)CANCEL_TOUT)
			{
				Mcu.Mpu.CancelFlag = False;
				CancelTimer = 0;
			}
		}
		else if (Mcu.Mpu.CorrPasswordFlag) //��� ������� ������ ���������
		{
			PutAddData(CORR_PASS_ADDR, Null);
			if (++CancelTimer >= (Uns)CANCEL_TOUT)
			{
				Mcu.Mpu.CorrPasswordFlag = False;
				CancelTimer = 0;
			}
		}
		else if (Mcu.Mpu.WrongPasswordFlag) //��� ������� ������ �� ���������
		{
			PutAddData(WRONG_PASS_ADDR, Null);
			if (++CancelTimer >= (Uns)CANCEL_TOUT)
			{
				Mcu.Mpu.WrongPasswordFlag = False;
				CancelTimer = 0;
			}
		}
		else if (Mcu.Mpu.PasswordResetFlag) // ��� ������� �������
		{
			PutAddData(PASS_RESET_ADDR, Null);
			if (++CancelTimer >= (Uns)CANCEL_TOUT)
			{
				Mcu.Mpu.PasswordResetFlag = False;
				CancelTimer = 0;
			}
		}
		else if (Mcu.Mpu.EditDisableFlag) // ����� ����� ��� �������
		{
			PutAddData(EDIT_DISABLE_ADDR, Null);
			if (++CancelTimer >= (Uns)CANCEL_TOUT)
			{
				Mcu.Mpu.EditDisableFlag = False;
				CancelTimer = 0;
			}
		}
		else if (Mcu.Mpu.CodesDontMatch) // ���� ������� �� ���������
		{
			PutAddData(CODE_DONT_MATCH_ADDR, Null);
			if (++CancelTimer >= (Uns)CANCEL_TOUT)
			{
				Mcu.Mpu.CodesDontMatch = False;
				CancelTimer = 0;
			}
		}
#if BUR_90
		else if (Mcu.Mpu.CalibEnableFlag) // ����� ���������� ����� �� �������
		{
			PutAddData(CALIB_EN_ADDR, Null);
			if (++CancelTimer >= (Uns)CANCEL_TOUT)
			{
				Mcu.Mpu.CalibEnableFlag = False;
				CancelTimer = 0;
			}
		}
		else if (Mcu.Mpu.CalibDisableFlag) // ����� ���������� ����� �� �������
		{
			PutAddData(CALIB_DIS_ADDR, Null);
			if (++CancelTimer >= (Uns)CANCEL_TOUT)
			{
				Mcu.Mpu.CalibDisableFlag = False;
				CancelTimer = 0;
			}
		}
		else if (Mcu.Mpu.HiLvlSetFlag)	// ��������� ������� ���������� 1 ��� ���������� ������
		{
			PutAddData(SET_LVLS_ADDR, Null);
			Menu.LoString[8]='1';
#if BUR_M
			Menu.LoString[11]='2'; Menu.LoString[13]='0'; // ������� ����� 220
#else
			if (GrB->InputType == it24)	// � ����������� �� ���� �������� ������� ������� 220 ��� 24
			{
				Menu.LoString[11]=' '; Menu.LoString[13]='4'; // ������� ����� 24
			}
			else
			{
				Menu.LoString[11]='2'; Menu.LoString[13]='0'; // ������� ����� 220
			}
#endif
			if (++CancelTimer >= (Uns)CANCEL_TOUT)
			{
				Mcu.Mpu.HiLvlSetFlag = False;
				CancelTimer = 0;
			}
		}
		else if (Mcu.Mpu.LowLvlSetFlag)	//  ��������� ������� ����������� 0 ��� ���������� ������
		{
			PutAddData(SET_LVLS_ADDR, Null);
			Menu.LoString[8]='0';
#if BUR_M
			Menu.LoString[11]='2'; Menu.LoString[13]='0'; // ������� ����� 220
#else
			if (GrB->InputType == it24)	// � ����������� �� ���� �������� ������� ������� 220 ��� 24
			{
				Menu.LoString[11]=' '; Menu.LoString[13]='4'; // ������� ����� 24
			}
			else
			{
				Menu.LoString[11]='2'; Menu.LoString[13]='0'; // ������� ����� 220
			}
#endif
			if (++CancelTimer >= (Uns)CANCEL_TOUT)
			{
				Mcu.Mpu.LowLvlSetFlag = False;
				CancelTimer = 0;
			}
		}
		else if (Mcu.Mpu.DataSavedFlag) // ��������� ���������� ������ �������� �� flash
		{
			PutAddData(DATA_SAVED_ADDR, Null);
			if (++CancelTimer >= (Uns)CANCEL_TOUT)
			{
				Mcu.Mpu.DataSavedFlag = False;
				CancelTimer = 0;
			}
		}
#endif	// #if BUR_90
		else if (Mcu.Mpu.MpuBlockedFlag)	// ������� �� �������������
		{
			CancelTimer++;
			if (CancelTimer <= 20)			// 2 ������� ���������� "������� ��������"
			{
				PutAddData(CMD_CANC_ADR, Null);
			}
			else
			{
				PutAddData(MPU_BLOCKED_ADR, Null);
			}
			if (CancelTimer >= 40)		// 2 ������� ���������� "��� �������������"
			{
				Mcu.Mpu.MpuBlockedFlag = False;
				CancelTimer = 0;
			}
		}
		else
		{
			GrH->BusyValue = 0;
			Menu.Indication = True;
			CancelTimer = 0;
			return;
		}
	}
	else if (!GrG->DisplShow)	// ������� �� �����
	{
		GrH->BusyValue = 0;
		Menu.Indication = True;
		return;
	}
	
	BtnStatus &= BTN_STOP;	// ���� ��� ���� �� �������� ����, ���� ���� ����� ������ �� ��������� ���� � ������� �� ��������������
	Pult.Key = 0;			// �������� ������� � ���
	Menu.Indication = False;// �� ����� � ����
}

void LocalControl(void) // ������� � �� ��������
{
		THallBlock *Halls = &GrC->HallBlock;			// ��������� ��� ���������
		Mcu.Mpu.Enable = (!IsParamEditing());

	if (!IsTestMode())
	{
		switch(BtnStatus & (~BTN_STOP))
			{
				case BTN_OPEN: 
						Mcu.Mpu.BtnKey = KEY_OPEN;
					break;
				case BTN_CLOSE:
						Mcu.Mpu.BtnKey = KEY_CLOSE;
					break;
				case (BTN_OPEN & BTN_CLOSE):
				
					break;
			}	

		switch(BtnStatus & BTN_STOP)
	    {
		#if BUR_90
		case BTN_STOP_MU:
		#if BUR_M
		    Mcu.Mpu.BtnKey = KEY_STOP;
		#else
		    if (IsLocalControl() || GrB->MuDuSetup == mdOff)
		    Mcu.Mpu.BtnKey = KEY_STOP;
		#endif
		    break;
		case BTN_STOP_DU:
		#if BUR_M
		    Mcu.Mpu.BtnKey = KEY_STOP;
		#else
		    if (IsLocalControl() || GrB->MuDuSetup == mdOff)
		    Mcu.Mpu.BtnKey = KEY_STOP;
		#endif
		    break;
		#else
		    case BTN_STOP:
		#if BUR_M
		    Mcu.Mpu.BtnKey = KEY_STOP;
		#else
		    if (IsLocalControl() || GrB->MuDuSetup == mdOff)
		    Mcu.Mpu.BtnKey = KEY_STOP;
		#endif
		#endif
		    break;
	    }

		if (BtnStatus) BlinkTimer = (Uns)BLINK_TOUT;
	}

	if (BtnStatus == BTN_STOP)			// ���� ����� ��������� � ��������� "����"
	{
		GrT->StopButtonState = true;	// ���������� ���� "����� ���� ������"
	}
	else
		GrT->StopButtonState = false;	// ������� ���� "����� ���� ������"

	Halls->bit.Open    = BtnOpen.Flag;							// �������� ������ � �������� ����� �� ���������
	Halls->bit.Close   = BtnClose.Flag;
	Halls->bit.StopMU  = BtnStop_MU.Flag;						// ���� �������� ��������� ��� ����������
	Halls->bit.StopDU  = BtnStop_DU.Flag;

	BtnStatus &= BTN_STOP;										// ���� ������ ���  �� ���� �� ������ ��������� � �� ����� �������������									// ���������� ������� ������� �� ����
	
	//��������� ������ � ���
	if (Pult.Key != 0)											// ���� ������ ������� �� ���� �� ���			
	{
		Mcu.Mpu.PduKey = PduKeysControls(Pult.Key);				// ���������� ������� � ���������� ��������, ��� ��������� ���������
		Menu.Key = Mcu.Mpu.PduKey;
		PduKeyFlag = 1;
		PultActiveTimer = PULT_LED_TOUT;
		BlinkTimer = (Uns)BLINK_TOUT;							// ������ ������������� ��� ������ �������� :)
		Pult.Key = 0;											// ���������� ���� � ���
	}

	// ����� �� ���������� ������ ��������-��������� ������ "����"
#if BUR_90
	if ((Menu.State == MS_EXPRESS) && Menu.Express.Enable && 	\
		(Halls->bit.StopMU || Halls->bit.StopDU) && (Menu.Key == 0))
#else
	if ((Menu.State == MS_EXPRESS) && Menu.Express.Enable && 	\
		(Halls->bit.StopMU && Halls->bit.StopDU) && (Menu.Key == 0))
#endif
	{
		Menu.Express.Select = false;
		Menu.Key = KEY_ENTER;
	}

	if(Menu.State == MS_SELPAR && Menu.Key == KEY_ENTER && !IsStopped()) // ���� �� � ����� � ������ ������ �� �������������� �� ���������� ���
	{
		Menu.Key = 0;	
	}

// ��������� �������� ����� ������
	if (IsSleeping())
	{
		if (Mcu.Mpu.BtnKey != 0)
		{
			Menu.Key = KEY_ESCAPE;
		}
	}

	// ����� ����, �� �� �������� ����� �� ���������
	if (!IsStopped())
	{
		Menu.SleepTimer = 0;
	}

	// ���������� ������ ��������� ������������ ����� �� ���������� ��� � ��� ��� �����
	// ���� ������� OffKVOKVZ_Control(&KvoKvzOff) ���������� � TsSignalization
	KvoKvzOff.TuState |= Mcu.Tu.State;			// ����: ��������� ��
	KvoKvzOff.ButtonsState |= Mcu.Mpu.BtnKey;	// ����: ��������� ������
	KvoKvzOff.PduKeyState |= Mcu.Mpu.PduKey;		// ����: ��������� ������ ���
}

// ������� ��������� ������ ��� � ����������� �� �������� ��������� GrB->EnableControlPDU
Byte PduKeysControls (Byte pduKey)
{
	Byte result;
	static char savedCommand = 0;

	result = pduKey;

	// ���� ���������� �������� � ��� �������������
	if (GrB->EnableControlPDU == ctrlDisabled)
	{
		if (pduKey == KEY_OPEN  || pduKey == KEY_CLOSE)			// ���� ��� ������� "�������" ��� "�������"
		{
			result = 0;
			Mcu.Mpu.CancelFlag = true;							// ��������� ��� �������
		}
	}
	// ���� ����� ����� ������������� ������ � ���
	else if (GrB->EnableControlPDU == ctrlNeedConfirm)
	{
		if (!Mcu.Status->bit.MuDu && (GrB->MuDuSetup != mdOff))	// ���� ���������� � �� �������������
		{
			if (pduKey == KEY_OPEN  || pduKey == KEY_CLOSE)		// � ��������� ������� "�������" ��� �������
			{
				result = 0;
				Mcu.Mpu.CancelFlag = true;						// ��������� ��� �������
			}
		}
		else if (Mcu.Mpu.WaitConfirmFlag == false)				// ���� ��� �� ��������� � ������ �������� �������������� �������
		{
			if (pduKey == KEY_OPEN  || pduKey == KEY_CLOSE)		// � ��������� ������� "�������" ��� �������
			{
				savedCommand = pduKey;
				result = 0;										// ��������� ����������� �������
				Mcu.Mpu.WaitConfirmFlag = true;					// � ��������� � ����� �������� �������
			}
		}
		else													// ���� ������� ����� �������� ������������� �������
		{
			if (pduKey == KEY_ENTER)							// ���� ������ ������� "����"
				result = savedCommand;							// ��������������� ������� ����������
			else												// ��� ������ ������� ���������
				result = 0;

			Mcu.Mpu.WaitConfirmFlag = false;					// �, ��������������, ���������� ����
		}
	}

	return result;
}

void RemoteControl(void) //24 - 220 + �����, 
{
	#if !BUR_M					    // ���� ��� � �� ������������ ���������� �����
	if (!PiData.Connect) return;	// ���� ��� ����� � ��� �� �� ������������ ������� ��
	Mcu.Tu.Enable = (!IsTestMode()) && (!IsParamEditing() && !GrG->TestCamera);  // ���� ���� �� �� �������� ������ � ��

	#if !BUR_90
	if(GrB->InputType == it24)		
	{
		ExtReg = ((Uns)(PiData.DiscrIn24 & 0x1F));
	}
	else if(GrB->InputType == it220)	
	{  	
		ExtReg = ((Uns)(PiData.DiscrIn220 & 0x1F));
	}

	GrH->Inputs.bit.Open  	= (Uns)TuOpen.Flag  ^ (Uns)GrB->InputMask.bit.Open;
	GrH->Inputs.bit.Close 	= (Uns)TuClose.Flag ^ (Uns)GrB->InputMask.bit.Close;
	GrH->Inputs.bit.Stop  	= (Uns)TuStop.Flag  ^ (Uns)GrB->InputMask.bit.Stop;
	GrH->Inputs.bit.Mu  	= (Uns)TuMu.Flag    ^ (Uns)GrB->InputMask.bit.Mu;
	GrH->Inputs.bit.Du  	= (Uns)TuDu.Flag    ^ (Uns)GrB->InputMask.bit.Du;
	#else
	ExtReg = ((Uns)(PiData.DiscrIn220 & 0x7F));

	dinDeblok.inputBit 	= ((ExtReg>>SBEXT_DEBLOK)&0x1) 	^ (Uns)GrB->InputMask.bit.Deblok;
	dinStop.inputBit 	= ((ExtReg>>SBEXT_STOP)&0x1) 	^ (Uns)GrB->InputMask.bit.Stop;
	dinClose.inputBit 	= ((ExtReg>>SBEXT_CLOSE)&0x1) 	^ (Uns)GrB->InputMask.bit.Close;
	dinOpen.inputBit 	= ((ExtReg>>SBEXT_OPEN)&0x1) 	^ (Uns)GrB->InputMask.bit.Open;
	dinDU.inputBit 		= ((ExtReg>>SBEXT_DU)&0x1) 		^ (Uns)GrB->InputMask.bit.Du;
	dinMU.inputBit 		= ((ExtReg>>SBEXT_MU)&0x1) 		^ (Uns)GrB->InputMask.bit.Mu;

	GrH->Inputs.bit.Deblok 	= DigitalInputUpdate (&dinDeblok);
	GrH->Inputs.bit.Open 	= DigitalInputUpdate (&dinOpen);
	GrH->Inputs.bit.Close 	= DigitalInputUpdate (&dinClose);
	GrH->Inputs.bit.Stop 	= DigitalInputUpdate (&dinStop);
	GrH->Inputs.bit.Mu 		= DigitalInputUpdate (&dinMU);
	GrH->Inputs.bit.Du 		= DigitalInputUpdate (&dinDU);
	#endif


	switch(GrB->MuDuSetup)
	{
		case mdOff:		GrA->Faults.Proc.bit.MuDuDef = 0; mudustatefault = 0; mudustatedefect = 0;			break;
		case mdMuOnly:	GrA->Faults.Proc.bit.MuDuDef = 0; mudustatefault = 0; mudustatedefect = 0;			break;
		case mdDuOnly:	GrA->Faults.Proc.bit.MuDuDef = 0; mudustatefault = 0; mudustatedefect = 0;			break;
		case mdSelect:		
					if(IsStopped() && !GrG->TestCamera) 
					{
						if(!GrH->Inputs.bit.Mu && !GrH->Inputs.bit.Du)
						{	 	
								if (++MuDuDefTimer > (3 * PRD_50HZ))
								{
									Mcu.MuDuInput = 0;
									GrA->Faults.Proc.bit.MuDuDef = 1;
									mudustatedefect = 0;
									mudustatefault=1;
									MuDuDefTimer = 0;
								}		
						}
						else if(GrH->Inputs.bit.Mu  && !GrH->Inputs.bit.Du)
						{		
								MuDuDefTimer  = 0;
						 		Mcu.MuDuInput = 1;
						 		if (!mudustatefault) GrA->Faults.Proc.bit.MuDuDef = 0;
								  mudustatedefect = 0;
								  //mudustatefault = 0;
						}
						else if(!GrH->Inputs.bit.Mu && GrH->Inputs.bit.Du) 
						{ 		
								MuDuDefTimer  = 0;
						 		Mcu.MuDuInput = 0;
								if (!mudustatefault) GrA->Faults.Proc.bit.MuDuDef = 0;
								  mudustatedefect = 0;
								  //mudustatefault = 0;
						}
						else if(GrH->Inputs.bit.Mu  && GrH->Inputs.bit.Du)  
						{		
								if (++MuDuDefTimer > (3 * PRD_50HZ))
								{
									Mcu.MuDuInput = 0;
									GrA->Faults.Proc.bit.MuDuDef = 1;
									mudustatedefect = 1;
									//mudustatefault=0;
									MuDuDefTimer = 0;
								}
						}
					}
					else if (GrG->TestCamera)
					{
					    mudustatedefect = 0;
					                                        mudustatefault=0;
					}
					break; 
	}
	#else
	switch(GrB->MuDuSetup)
	{
		case mdSelect:
		case mdMuOnly:
					if (IsMemParReady())
					{
						GrB->MuDuSetup = mdOff;
						WritePar(GetAdr(GroupB.MuDuSetup), GrB->MuDuSetup, 1);
					}
					break;
	}

	#if BUR_90
		// ��������� ����������� ����� "������������"
		ExtReg = ((Uns)(PiData.DiscrIn220 & 0x7F));
		dinDeblok.inputBit = ((ExtReg>>SBEXT_DEBLOK)&0x1) ^ (Uns)GrB->InputMask.bit.Deblok;
		dinDeblok.timeout = (PRD_50HZ * GrB->TuTime) / 10;
		GrH->Inputs.bit.Deblok = DigitalInputUpdate (&dinDeblok);
	#endif

	#endif
}

void BlkSignalization(void)	// ������������ �� �����
{
	static Uns DspTimer = 0;

	TLedsReg *Reg = &GrH->LedsReg;	// ����� 
	
	if (++DspTimer >= DSP_LED_TIMER)
	{
		DspTimer = 0;
		if (PowerEnable)
		{
		#if BUR_90
			CPU_LED = !CPU_LED;
		#else
			Reg->bit.DspOn = !Reg->bit.DspOn;
			Reg->bit.Pdu = !Reg->bit.Pdu;
		#endif
		}
	}
	
	// ���� ������������� ��� �� ���������, �� ��������� ������������
	if (!GrH->initComplete)
	{
		Reg->all = 0;
		return;
	}

	GrA->Status.bit.BlkIndic = IsFaultExist(pmBlkSign); // ��������� �� ����� ���� ���� ������ �����, ������ ����  ������
	GrA->Status.bit.BlkDefect = IsDefectExist(pmBlkSign); // ��������� �� ����� ���� ���� ������������� �����, ������ ����  ������

	if (!PowerEnable) return;	// ���� �������� ���� �� ������ ������

	if (IsTestMode()) {Reg->all = GrG->LedsReg.all; return;} // ���� ���� �� �������� �������� ������� ����������� ��������� � �������
	
	if (GrC->LedTestMode)
	{
		Reg->all = 255;
		if(++LedTestTimer >= LED_TEST_TIMEOUT)
		{
			GrC->LedTestMode = 0;
			LedTestTimer = 0;
		}
		return;
	}

	if (IsClosed()) Reg->bit.Closed = LED_ACTIVE;	// 
	else if (IsClosing()) Reg->bit.Closed = !Reg->bit.Closed;
	else Reg->bit.Closed = !LED_ACTIVE;

	if (IsOpened()) Reg->bit.Opened = LED_ACTIVE;
	else if (IsOpening()) Reg->bit.Opened = !Reg->bit.Opened;
	else Reg->bit.Opened = !LED_ACTIVE;

	if(!IsSleeping())
	{
		if (PultActiveTimer > 0)	
		{	
			PultActiveTimer--;
			Reg->bit.Pdu = LED_ACTIVE;	
		}
		else{Reg->bit.Pdu = !LED_ACTIVE;}
	}

	Reg->bit.Defect = IsBlockDefect();   // ������� �������������

	Reg->bit.MuDu  = IsLocalControl();	// ��� � �� �� ���������

	Reg->bit.Fault = IsBlockFault();	// ���� ���� ������ �� ����� (��� ���������� ��. ��� � �� ����������)

	Reg->bit.Muff  = IsMuffActive();	// �����
}

void TsSignalization(void) //��
{
	GrH->BtnStopFlag = OffKVOKVZ_Control(&KvoKvzOff);		// ����� ������� ������������ ����� �� ���������� ��� � ��� ��� �����

	#if !BUR_M
	    if(PauseModbus > 0) return;
	#endif

	TOutputReg *Reg = &GrH->Outputs;
	
	GrA->Status.bit.TsIndic  = IsFaultExist(pmBlkTsSign);// � ������ � ������ ��������� ��
	GrA->Status.bit.TsDefect  = IsDefectExist(pmBlkTsSign);// � ������ � ������ ��������� ��

	// 30/06/21 ---------- PositionAcc ����������� ������ ����������� � �����
	if (IsStopped())
	{
		PositionAccTmp = GrB->PositionAcc;
	}
	else
	{
		PositionAccTmp = 0;
	}


	// ���������� �� ����� ������������.

	#if BUR_M
	Reg->all &= TS_SIGNAL_MASK_M;
	#else
	Reg->all &= TS_SIGNAL_MASK_T;
	#endif

	if(IsTestMode() || GrG->TestCamera)
	{
		if (!GrG->IsDiscrTestActive)
			Reg->all = GrG->OutputReg.all;
	#if BUR_M
		if (GrC->ReversKVOKVZ == 1)
		{
		    Reg->bit.Dout9 = 1;			//  ���
		    Reg->bit.Dout10 = 1;		//  ���
		}
	#endif
	}
	else	
	{ 
#if BUR_M
	#if !BUR_90
		if(PauseModbus > 0)
		{
		    Reg->bit.Dout2 = 0;					//  �����
		    Reg->bit.Dout3 = 0;					//  �� �����
		    Reg->bit.Dout5 = 0;					//  �������
		    if (GrC->NoControlKVOKVZ == 0)
		    {
			Reg->bit.Dout6 = 0;				//  �������
			Reg->bit.Dout7 = 0;				//  �������
		    }
		    else if (GrC->NoControlKVOKVZ == 1)
		    {
			Reg->bit.Dout6 = 1;				//  �������
			Reg->bit.Dout7 = 1;				//  �������
		    }
		    Reg->bit.Dout8 = 0;					//  �������������
		    if (GrC->ReversKVOKVZ == 0)
		    {
		    	Reg->bit.Dout9  = 1;		//  ���
		    	Reg->bit.Dout10 = 1;		//  ���
		    }
		    else if (GrC->ReversKVOKVZ == 1)
		    {
		    	Reg->bit.Dout9  = 0;		//  ���
		    	Reg->bit.Dout10 = 0;		//  ���
		    }

		}
		else
		{

			Reg->bit.Dout2 = IsMuffActive()	 ^ 		(Uns)GrB->OutputMask.bit.mufta;		//  �����
			Reg->bit.Dout3 = IsTsFault()	 ^ 		(Uns)GrB->OutputMask.bit.fault;		//	�� �����
			Reg->bit.Dout5 = 1 		     	 ^ 		(Uns)GrB->OutputMask.bit.powerOn;	//  �������
			Reg->bit.Dout6 = IsClosed()		 ^ 		(Uns)GrB->OutputMask.bit.closed;	//  �������
			Reg->bit.Dout7 = IsOpened()		 ^ 		(Uns)GrB->OutputMask.bit.opened;	//  �������
			Reg->bit.Dout8 = IsTsDefect()	 ^ 		(Uns)GrB->OutputMask.bit.defect;	//  �������������
			if (GrC->ReversKVOKVZ == 0)
			{
			    Reg->bit.Dout9 =  !(IsOpened() 	||	(!IsOpened()&& !IsClosed()));		//  ���
			    Reg->bit.Dout10 = !(IsClosed() 	||	(!IsOpened()&& !IsClosed()));		//  ���
			}
			else if (GrC->ReversKVOKVZ == 1)
			{
			    Reg->bit.Dout9 =  (IsOpened() 	||	(!IsOpened()&& !IsClosed()));		//  ���
			    Reg->bit.Dout10 = (IsClosed() 	||	(!IsOpened()&& !IsClosed()));		//  ���
			}

		}
	#else
		Reg->bit.Dout2 = IsMVOactive()	 ^ 		(Uns)GrB->OutputMask.bit.mufta;		//  ����� � ��������
		Reg->bit.Dout3 = IsTsFault()	 ^ 		(Uns)GrB->OutputMask.bit.fault;		//	�� �����
		Reg->bit.Dout5 = IsMVZactive()	 ^ 		(Uns)GrB->OutputMask.bit.mufta;		//  ����� � ��������
		Reg->bit.Dout6 = IsClosed()		 ^ 		(Uns)GrB->OutputMask.bit.closed;	//  �������
		Reg->bit.Dout7 = IsOpened()		 ^ 		(Uns)GrB->OutputMask.bit.opened;	//  �������


		if(GrB->BurM90to60 == 1)
		{
			Reg->bit.Dout8 = IsMuffActive()	 ^ 		(Uns)GrB->OutputMask.bit.mufta;	//  �����!!!!!
		}
		else
		{
			Reg->bit.Dout8 = IsTsDefect()	 ^ 		(Uns)GrB->OutputMask.bit.defect;	//  �������������
		}

		if ((GrC->ReversKVOKVZ == 0) && (GrH->ContGroup != cgStopKvoKvz))
		{
		    Reg->bit.Dout9 =  !(IsOpened() 	||	(!IsOpened()&& !IsClosed()));		//  ���
		    Reg->bit.Dout10 = !(IsClosed() 	||	(!IsOpened()&& !IsClosed()));		//  ���
		}
		else if ((GrC->ReversKVOKVZ == 1) && (GrH->ContGroup != cgStopKvoKvz))
		{
		    Reg->bit.Dout9 =  (IsOpened() 	||	(!IsOpened()&& !IsClosed()));		//  ���
		    Reg->bit.Dout10 = (IsClosed() 	||	(!IsOpened()&& !IsClosed()));		//  ���
		}
	#endif

#else
		Reg->bit.Dout0 = IsTsFault()	 ^ 		(Uns)GrB->OutputMask.bit.fault;		//	�� �����

	#if BUR_90
		if (GrB->OutputMask.bit.closed)			// ���� ����� ������� ���������������
		{										// ���������� ������ �����, ����� �� ����
			Reg->bit.Dout1 = IsClosed() ^ (Uns)GrB->OutputMask.bit.closed;	//	�������
		}
		else									// ���� ����� �� ��������������
		{										// ���������� ������ � ������ BtnStopFlag
			Reg->bit.Dout1 = GrH->BtnStopFlag ? 1 : IsClosed();		//	�������
		}

		if (GrB->OutputMask.bit.opened)			// ���� ����� ������� ���������������
		{										// ���������� ������ �����, ����� �� ����
			Reg->bit.Dout2 = IsOpened() ^ (Uns)GrB->OutputMask.bit.opened;	//	�������
		}
		else									// ���� ����� �� ��������������
		{										// ���������� ������ � ������ BtnStopFlag
			Reg->bit.Dout2 = GrH->BtnStopFlag ? 1 : IsOpened(); 	//	�������
		}

		Reg->bit.Dout3 = IsMVOactive()	 ^ 		(Uns)GrB->OutputMask.bit.mufta;		//  ����� � ��������

	#else

		if (GrB->OutputMask.bit.closed)			// ���� ����� ������� ���������������
		{										// ���������� ������ � ������ BtnStopFlag
			Reg->bit.Dout1 = GrH->BtnStopFlag ? 0 : IsClosed() ^ (Uns)GrB->OutputMask.bit.closed;	//	�������
		}
		else									// ���� ����� �� ��������������
		{										// ���������� ������ �����, ����� �� ����
			Reg->bit.Dout1 = IsClosed();		//	�������
		}

		if (GrB->OutputMask.bit.opened)			// ���� ����� ������� ���������������
		{										// ���������� ������ � ������ BtnStopFlag
			Reg->bit.Dout2 = GrH->BtnStopFlag ? 0 : IsOpened() ^ (Uns)GrB->OutputMask.bit.opened;	//	�������
		}
		else									// ���� ����� �� ��������������
		{										// ���������� ������ �����, ����� �� ����
			Reg->bit.Dout2 = IsOpened();
		}

		Reg->bit.Dout3 = IsMuffActive()	 ^ 		(Uns)GrB->OutputMask.bit.mufta;		//	�����
	#endif

		Reg->bit.Dout4 = IsClosing()	 ^ 		(Uns)GrB->OutputMask.bit.closing;	//	�����������
		Reg->bit.Dout5 = IsOpening()	 ^ 		(Uns)GrB->OutputMask.bit.opening;	//	�����������
		Reg->bit.Dout6 = !IsLocalControl()^ 	(Uns)GrB->OutputMask.bit.muDu; 		//	��/��
		Reg->bit.Dout7 = IsTsDefect()	 ^ 		(Uns)GrB->OutputMask.bit.defect;	//  �������������
	#if BUR_90
		Reg->bit.Dout8 = IsMVZactive()	 ^ 		(Uns)GrB->OutputMask.bit.mufta;		//  ����� � ��������
		Reg->bit.Dout9 = !GrB->InputType;											// ��� �������� �������
	#endif
#endif
	}

	#if BUR_M
	// ������ �� serial communication
	if (PowerEnable || (!PowerEnable && (GrH->FaultsLoad.all & LOAD_SHC_MASK)))
	{
		PiData.DiscrOut = (Byte)(Reg->all & 0xFF);
		PiData.DiscrOut2 = (Byte)((Reg->all >> 8) & 0x7);
	}
	#else
	if (PowerEnable)
	{
		PiData.DiscrOut = (Byte)(Reg->all & 0xFF);
		#if BUR_90
		PiData.DiscrOut2 = (Byte)((Reg->all >> 8) & 0x3);
		#else
		PiData.DiscrOut2 = (Byte)((Reg->all >> 8) & 0x1);
		#endif
	}
	#endif

#if BUR_90
	// ���������� ����� ��
	if (GrC->TaskPiCalib)
	{
		if (GrH->FaultsDev.bit.AVRcon || !PI_CalibEnable) 	// ���� ���������� �� ��������� ��� ���������� ����� � ��...
		{
			Mcu.Mpu.CancelFlag = True;						// ... �� ���������� ���� ������ � ������ �� ������
		}
		else												// ���� ��� ����, �� ������������
		{
			// ����������� ���������� TaskPiCalib � ������, ������������ � ����� ��
			PiData.DiscrOut2 |= (Byte)((GrC->TaskPiCalib & 0x3) << 3);
		}
		GrC->TaskPiCalib = 0;
	}
#endif

}

void AlgControl(void)
{
	static Uns prevDeblokState = 0;			// ���������� ��������� ���� "������������"
	if (!DefFlag)
	{
		if (GrD->SetDefaults != 0)
		{
			if (!IsStopped()) Mcu.Mpu.CancelFlag = True;
			else {DefParamsSet(M_EDIT_PAR); Mcu.EvLog.Value = CMD_DEFAULTS_USER;}
			GrD->SetDefaults = 0;
		}
		else if (GrC->SetDefaults != 0)
		{
			if (!IsStopped()) Mcu.Mpu.CancelFlag = True;
			else {DefParamsSet(M_FACT_PAR); Mcu.EvLog.Value = CMD_DEFAULTS_FACT; }
			GrC->SetDefaults = 0;
		}
	}
#if BUR_90
	// ���� ��������� ����� ����������� ����� ������������
	if ((prevDeblokState != GrH->Inputs.bit.Deblok)&&(GrH->Inputs.bit.Deblok == 1))
	{
		GrD->PrtReset = 1;						// �� ������������ ������ ������� Deblok ������ ������� "����� �����"
	}
	prevDeblokState = GrH->Inputs.bit.Deblok;
#endif
	// ����� �����
	if (GrD->PrtReset != 0)
	{	
		if (!IsStopped()) Mcu.Mpu.CancelFlag = True;
		else {ProtectionsClear(); Mcu.EvLog.Value = CMD_RES_FAULT;}
		GrD->PrtReset = 0;
	}
}

void ClbControl(void)	// 200 Hz ���������� ������������
{
	Calib.ResetFlag = !IsStopped();	// ���� ������������ �� ���������� 1 � ����� 
	if (Calib.CancelFlag)			// ���� ��������� ���� ������ ������� (���� �������������� , ��� ��������� ������ ���� ��� ���� ��������������)
	{
		Mcu.Mpu.CancelFlag = True;	// ���������� ������ ������� ���
		Calib.CancelFlag = False;	// ���������� ���� � �����������
	}
	
	switch (Calib.AutoCmd)			// ��������� �������������� � ���� ������� ���� ��... ���� ���� ������� �� ����������
	{
		case CLB_CLOSE: *Mcu.ControlWord = vcwClose; break;	// ���� ������� �������� �� ���������� ����������� ����� ��������
		case CLB_OPEN:  *Mcu.ControlWord = vcwOpen;  break;	// ���������� �� ��������
	}
	Calib.AutoCmd = 0;				// ���������� ������� ��������������

	GrA->Status.bit.Closed = /* IsStopped() && */ ((Calib.Zone & CLB_CLOSE) != 0);	// �������� - ���� � ����� � �������������� "�������"
	GrA->Status.bit.Opened = /* IsStopped() && */ ((Calib.Zone & CLB_OPEN)  != 0);	// �������� - ���� � ����� � �������������� "��������"
						
    GrH->Position      = Revolution;								// �������� ������� ��������� (���� � �����)
//	GrC->ClosePosition = IndicPos(Calib.Indication->ClosePos);				// ��������	��������� �������
//	GrC->OpenPosition  = IndicPos(Calib.Indication->OpenPos);				// �������� ��������� �������
//	GrC->Position 	   = GrH->Position;										// �������� ������� ��������� � ��.�

	if (!IsNoCalib() && (Menu.State != MS_EXPRESS) && Menu.Express.Enable)
		Menu.Express.Enable = FALSE;

	if (GrD->CalibReset != 0)							// ���� ������ ������� �� ����� ����������
	{
		if (!IsStopped()) 									// ���� �� ������������ �� 
			Mcu.Mpu.CancelFlag = True;						// ���������� ������� � ���
		else												// ����� ���� ������������
		{
			GrD->TaskClose = trReset;
			GrD->TaskOpen  = trReset;
		
			Mcu.EvLog.Value = CMD_RES_CLB;					// ������ � ������ ���������� �������� ������� ������ ������� ���������
		}
		GrD->CalibReset = 0;								// �������� ������� �� ����� ����������

		if (Menu.State != MS_EXPRESS)						// ������ ���� �� ��������� � ��������-���������
			Menu.Express.Enable = TRUE;
	}

	if (GrG->TestCamera)
	{
		BT_ON_OFF = 1;
		if(GrC->EncoderType == 0)
		{
			GrA->Position      = !Encoder.Error ? GrH->Position : 65535;
		}
		else
		{
			GrA->Position      = !enDPMA15.Error ? GrH->Position : 65535;
		}

	}
	else
	{
		// ���������� bluetooth: 0 - ��������; 1 - ��������
		BT_ON_OFF = (GrB->BtEnable) ? 1 : 0;

		GrA->Position      = Revolution;
	}

	if (GrD->CycleReset != 0)							// ���� ������ ������� �� ����� ������� ����������� ������ ������
	{
		if (!IsStopped()) 									// ���� �� ������������ �� 
			Mcu.Mpu.CancelFlag = True;						// ���������� ������� � ���
		else												// ����� ���� ������������
		{
			GrH->CycleCnt = 0;							// ���������� ������� ������
			Calib.CycleData = 0;							// �������� ������� (���� ������ ���� ������� �����, ��� ���� ��������)
			Mcu.EvLog.Value = CMD_RES_CYCLE;				// ������ � ������ ���������� �������� ������� ������ ����������� ������
		}
		GrD->CycleReset = 0;							// ���������� ������� ������ ������
	}

	if (IsMemParReady())									// ���� ���� ���������� � ������ ����������
	{
		if (GrH->CycleCnt != PrevCycle)										// ���� ������� ������ ���������
		{	
			WritePar(REG_CYCLE_CNT, &GrH->CycleCnt, 1);						// �������� �������� ������� ������
			PrevCycle = GrH->CycleCnt;										// ��������� ���������� ��������, ��� ����������� ��������
		}
		else if (GrH->CalibState != GrA->CalibState)						// ���� ��������� ���������� ����������
		{
			WritePar(REG_CALIB_STATE, &GrH->CalibState, sizeof(ClbIndication));// �� �������� ��������� ����������
			GrA->CalibState = GrH->CalibState;								// ���������� ���������� ��������, ��� ����������� ��������
		}
	}


}

__inline void DefParamsSet(Uns Code)	// � ����� ???
{
	DefFlag = 1;
	DefCode = Code;
	DefAddr = 0;
}

#if IR_IMP_TEST
void IrImpCalc(void)
{
	static Uint16 State = 1;
	if (State != IR_IN) {IrImpCount++; State = IR_IN;}
}
#endif

void ImBufferReader(Byte LogType, Uns RecordNum)
{
	static Uns PrevRecNum = -1U;
	static Bool EvLogMainCellRead = false;
	static Bool EvLogBufCellRead = false;
	Uns	*ImBuf, Addr;

	if (!Im.CanReadNextRec)
		return;

	Im.IsReadRecBusy = !(IsMemParReady() && IsMemLogReady());
							
	if (!Im.IsReadRecBusy)
	{
		if ((LogType == IM_LOGEV_TYPE) && EvLogBufCellRead)
		{
			if (PrevRecNum == RecordNum)
			{
				EvLogBufCellRead = false;
				Im.CanReadNextRec = false;
				Im.IsBufReady = true;

				return;
			}
		}
		else if ((LogType != IM_LOGEV_TYPE) && (PrevRecNum == RecordNum))
		{
			Im.CanReadNextRec = false;
			Im.IsBufReady = true;
		}
		else
		{
			PrevRecNum = RecordNum;
		
			switch (LogType)
			{
				case IM_LOGEV_TYPE:

				if (!EvLogMainCellRead)
				{
					if (!(RecordNum & 0x01))
						{	ImBuf = &ImReadBuf[0];	}
					else
						{	ImBuf = &ImReadBuf[IM_READ_BUF_SIZE];	}
						
					Addr = LOG_EV_START_ADDR + RecordNum * LOG_EV_DATA_CNT * LOG_EV_DATA_CELL; 
					ReadPar(Addr, ImBuf, LOG_EV_DATA_CNT);

					EvLogMainCellRead = true;
				}
				else
				{
					Addr = LOG_EV_BUF_START_ADDR + RecordNum * LOG_EV_BUF_DATA_CNT * LOG_EV_BUF_DATA_CELL;

					if (!(RecordNum & 0x01))
						{	ImBuf = &ImReadBuf[LOG_EV_DATA_CNT];	}
					else
						{	ImBuf = &ImReadBuf[LOG_EV_DATA_CNT + IM_READ_BUF_SIZE];		}

					ReadLog(Addr, ImBuf, (LOG_EV_BUF_DATA_CNT * LOG_EV_BUF_DATA_CELL));
					EvLogMainCellRead = false;
					EvLogBufCellRead = true;
				}

				break;

				case IM_LOGCMD_TYPE:
				
					if (!(RecordNum & 0x01))
					{	ImBuf = &ImReadBuf[0];	}
					else
					{	ImBuf = &ImReadBuf[LOG_CMD_DATA_CNT];	}
					
					Addr = LOG_CMD_START_ADDR + RecordNum * LOG_CMD_DATA_CNT;
					ReadPar(Addr, ImBuf, LOG_CMD_DATA_CNT);

				break;

				case IM_LOGPARAMS_TYPE:

					if (!(RecordNum & 0x01))
					{	ImBuf = &ImReadBuf[0];	}
					else
					{	ImBuf = &ImReadBuf[LOG_PARAM_DATA_CNT];	}
					
					Addr = LOG_PARAM_START_ADDR + RecordNum * LOG_PARAM_DATA_CNT;
					ReadPar(Addr, ImBuf, LOG_PARAM_DATA_CNT);

				break;
			}
		}
	}
}

// ������� ��������� ����������� �������
Uns	DigitalInputUpdate (TDigitalInput *p)
{
	if (p->inputBit)		// ����
	{
		if (p->timer++ >= GrB->TuTime*5)
		{
			p->timer = p->timeout;
			p->output = TRUE;
		}
	}
	else
	{
		p->timer = 0;
		p->output = FALSE;
	}

	return p->output;
}


#define DELAY_TIMEOUT	10 	// 10 = 1 ��� �� 10 ��
#define ON_TIMEOUT	 	10	// ����� ������� ��� ��� ��� �������� ����� ���� 10 = 1 ��� �� 10 ��

#define KVO_KVZ_OFF_DEFAULT { \
		0, 0, 0, \
		ON_TIMEOUT, \
		DELAY_TIMEOUT, \
		0, 0, 0 }

TKVOKVZoff KvoKvzOff = KVO_KVZ_OFF_DEFAULT;

// ������� ������������ ����� �� ���������� ��� � ��� ��� �����
// ���� - ���� �������� ����� ���� � ���� ����������� ������� ���� �� ��
// ����� - ���� ���������� ��� � ���
Bool OffKVOKVZ_Control (TKVOKVZoff *p)	// 10 Hz
{
	if (!GrB->KvoKvzOffOnStop)
	{
		return 0;
	}

	if (!PowerEnable)					// ���� ��� ������� �������
	{
		return 1;						// �� ���������� ���� ����������
	}

	if (p->delayFlag)						// ���� ����� ���� ��������
	{										// �� ������ ���������� ������ � ���������� ��������� ����� ���������� � ��
		p->offFlag = false;
		if (p->timer++ >= p->delayTimeout)	// ����� ������ ����� ������ �������� ���� ��������
		{
			p->timer = 0;
			p->delayFlag = false;
		}
	}
	else if (!p->offFlag) 						// ���� ���� ���������� ��� � ��� ����
	{											// �� ������� �� ��������� ������ � ��
		if ((p->ButtonsState == KEY_STOP)||	// ���� ������ ������ ���� �� ��� �� ���������� ���� ��������� ��� � ���
			((p->TuState & TU_STOP) && (Mcu.ActiveControls & CMD_SRC_DIGITAL))||	// ���� ������ ���� �� �� � ������ ��������� - ����������
			(p->PduKeyState == KEY_STOP) )		// ���� ������� � ��� - ����������
		{
			if (!GrA->Status.bit.Closing && !GrA->Status.bit.Opening)
			{
				p->offFlag = true;
			}
		}
	}
	else										// ���� ���� ���������� ��� � ��� �������
	{
		if(p->timer++ >= p->onTimeout)		// ����������� ������.
		{										// �� ��������� �������, ���������, ���� �� ����
			if ((p->ButtonsState == KEY_STOP)||
				((p->TuState & TU_STOP) && (Mcu.ActiveControls & CMD_SRC_DIGITAL))||
				(p->PduKeyState == KEY_STOP) )
			{									// ���� ���� �� ����, �� ���������� ������
				p->timer = p->onTimeout;
			}
			else								// ���� ���� ����, �� ������� ��� �����
			{
				p->timer = 10;					// ������� ���� ������� ��� � ���
				p->offFlag = false;				// � ���������� ���� ��������, ����� ������� �����, ������ ��� ����� �������� �� ��������� �� � ������
				p->delayFlag = true;
			}
		}
	}

	p->TuState = p->ButtonsState = p->PduKeyState = 0;

	return p->offFlag;
}

// ������� �������� � ������� ������ ������� �� ������� ��������� ��� ��� ��������� ��������� ���������� �������
void PosFixControl(void)
{
	static Uns PrevPosition = 0;	// ���������� �������� ��������� ��������
	static Uns DeltaPos = 0;		// ������� ����� ������� � ���������� ��������� ���������
	static Uns PrevTsState = 0;		// ���������� ��������� ��������������

	if (PauseModbus)				// �� ����� �������� �� ������ ������������ �������� � ������� �� �������
	{
		PrevPosition = Revolution;
		PrevTsState = GrA->Outputs.all;
		GrA->PosFix = GrH->Position;
		return;
	}

	if (GrA->Status.bit.Stop)
	{
		DeltaPos = abs(GrH->Position - PrevPosition);
		// ���� � ��������� "����" ��������� ��������� ��������� �������� ����� ��� �� 5 �����, �� ���������� ��� ������� � ������ ������
		if ( (DeltaPos > 5 ) && ( DeltaPos < RevMax - 5 ) )
		{
			GrA->PosFix = GrH->Position;
			Mcu.EvLog.QueryValue = CMD_FIX_POS;
			Mcu.EvLog.QuerySource = CMD_SRC_BLOCK;
		}
	}
	else
	{
		GrA->PosFix = GrH->Position;
	}
	PrevPosition = GrH->Position;

	// ���� ��������� �� ���������� - ����� � ������
	if (GrA->Outputs.all != PrevTsState)
	{
		Mcu.EvLog.QueryValue = CMD_FIX_POS;
		Mcu.EvLog.QuerySource = CMD_SRC_BLOCK;
		GrA->PosFix = GrH->Position;
	}
	PrevTsState = GrA->Outputs.all;
}
