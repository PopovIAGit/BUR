
#include "config.h"

APFILTER3  URfltr;
APFILTER3  USfltr;
APFILTER3  UTfltr;
APFILTER3  IUfltr;
APFILTER3  IVfltr;
APFILTER3  IWfltr;

APFILTER1  Umfltr  	= RMS_FLTR_DEFAULT;
APFILTER3  Imfltr  	= IMID_FLTR_DEFAULT;
APFILTER1  Phifltr  = URMSF_DEFAULT;
APFILTER3  Trqfltr 	= TORQ_FLTR_DEFAULT;

ILEG_TRN   UR;
ILEG_TRN   US;
ILEG_TRN   UT;
ILEG_TRN   IU;
ILEG_TRN   IV;
ILEG_TRN   IW;
SIFU 	   Sifu 	= SIFU_DEFAULT;
PH_ORDER   PhEl 	= PH_EL_DEFAULT;
TDmControl Dmc  	= DMC_DEFAULT;
TTorqObs   Torq;


Uns  ZazorTimer 	= 0;
Uns  Iload[3] 		= {0,0,0};	//����������� ���� � � (���� ��������)
Uns  Ipr[3]   		= {0,0,0};	//����������� ���� � % �� ������������
Uns  Imid	  		= 0;		//������� ����������� ���	
Uns  Imidpr	  		= 0;		//������� ����������� ��� � ��������� �� I�
Uns  Inom 	  		= 0;		//����������� ��� ���������, ������ � ��������� Net
Uns  Umid     		= 0;	    //������� ����������� ����������
Uns  AngleUI  		= 0;		//���� ����� ����� � �����������
Bool DynBrakeEnable = False;	// ������ ������������� ���������� �� ������
Uns  KickCounter    = 0;		// ������� ���������� ������
Uns  KickModeTimer  = 0;		// ������� ��� ������ �������� �������
Uns  TestModeTimer  = 0;		// ����� � �������� ������
Uns  UporModeTimer  = 0;		// ������� ��� �����
Uns  PreventModeTimer = 0;		// ����� ��� ������ �� �������� � �������
Uns  PauseModeTimer = 0;		// ������� ��� ������ �����
Uns  DynModeTimer   = 0;		// ������� ��� ������������� ����������
Uns  RunStatus 		= 0;
Byte DmcRefrState   = 0;
Uns  AbsSpeed 		= 0;
Uns  KickSetAngle   = 0;		// ������� ���� �������� �� �������� ������
Uns  LowPowerTimer  = 0;		// ������ ������ ������� ����������
Uns  LowPowerReset  = 0;		// ������ ������ ������� ����������
Uns  SaveDirection  = SIFU_NONE;
Uns  SaveContactorDirection = 0;
Uns  TorqueMax 		= 0;
Uns  TorqueUser     = 0;
Bool KickModeEnable = False;	// ���������� �������� �������
#if BUR_M
Uns NetMonitorFlag  = 0;
Uns Net_Mon_Timer   = 0;		//������ ����� ��� ��� ������� ������� ���������� ��� ������� �� ����
Uns StopSetTimer 	= 0;
Uns PowerLostTimer  = 0;
Uns PowerLostTimer2 = 0;
#endif
Uns Ang 			= 2;
Uns BreakFlag 		= 0;
Uns PowerSupplyEnable 	= 0; 	// ������� ������� ��������� 
Uns PowerSupplyCnt 		= 0;	// �������� �� ���������� �������

Uns DebugStartDelayCnt = 0;
																			// ������� ���������� ������ � table_tomzel.asm
Int InomDef[10]  	 = {13,11,18,52,52,47,56,110,85,148};						// default �������� ��� Inom ��� ������ ��������
Int MomMaxDef[10]  	 = {10,10,40,40,80,100,400,400,1000,1000};				//					��� Mmax 
Int TransCurrDef[10] = {1000,1000,1000,1000,1000,1000,1100,1100,1100,1100};	//					��� TransCur �������
Int GearRatioDef[5]	 = {5250,7360,16720,16016,16016};						//��� ������������� ����� ��������� 

extern Int	//������� ���������
	drive1,  drive2,  drive3,	drive4,	drive5,	
	drive6,  drive7,  drive8,  drive9,	drive10, 
	drive11, drive12, drive13, drive14,	drive15, 
	drive16, drive17, drive18, drive19, drive20;

//-------------�������------------------
Uns program   = 1;
Uns mon_index = 0;
Uns dot       = 0;
Uns dot_max   = 10;
Int buffer1[256];
Int buffer2[256];

Uns DbgEncoder = 0;
Uns DbgStop =0;


__inline void ADC_Aquisition(void);
__inline void DmcPrepare(void);
__inline void TorqueObsInit(void);
__inline void TestThyrControl(void);
__inline void PulsePhaseControl(void);
__inline void StopMode(void);
__inline void TestPhMode(void);
__inline void UporStartMode(void);
__inline void MoveMode(void);
__inline void PauseMode(void);
__inline void UporFinishMode(void);
__inline void KickMode(void);
__inline void SpeedTestMode(void);
__inline void DynBrakeMode(void);
__inline void DmcTest(void);

static  void  ClkThyrControl(Uns State);

//---- ���������� ����������-----
void MotorControlInit(void)
{
	URfltr.dt = _IQ(DMC_TS);
	USfltr.dt = _IQ(DMC_TS);
	UTfltr.dt = _IQ(DMC_TS);
	IUfltr.dt = _IQ(DMC_TS);
	IVfltr.dt = _IQ(DMC_TS);
	IWfltr.dt = _IQ(DMC_TS);

	ApFilter1Init(&Umfltr);
	ApFilter3Init(&Imfltr);
	ApFilter1Init(&Phifltr);
	ApFilter3Init(&Trqfltr);

	memset(&UR, 0, sizeof(ILEG_TRN));	// �������� ��������� ��������� ����� � ����������
	memset(&US, 0, sizeof(ILEG_TRN));	// ���� ����� ���������� � �������� ��� ����� ������
	memset(&UT, 0, sizeof(ILEG_TRN));
	memset(&IU, 0, sizeof(ILEG_TRN));
	memset(&IV, 0, sizeof(ILEG_TRN));
	memset(&IW, 0, sizeof(ILEG_TRN));

	memset(&Torq,  0, sizeof(TTorqObs)); // �������� ��������� ������� �������� ��� ���� ��
	
	CubInit(&Torq.Cub1, &TqCurr);		 // ������������� ��������� ���� ��� ������� �������
	CubInit(&Torq.Cub2, &TqAngUI);
	CubInit(&Torq.Cub3, &TqAngSf);


}
void MotorControlUpdate(void)
{
#if BUR_M
	if (!SaveContactorDirection)
	{
		GrD->ControlWord = vcwStop;
		SaveContactorDirection = 1;
	}
#endif
	ADC_Aquisition();					// ���������� ���
	DmcPrepare();						// ���������� �������� U I � ������� ����������� � ����� ���
	ph_order_trn(&PhEl);				// ����������� ���
	if (UR.Output < 70) PhEl.Direction = 0;
	TestThyrControl();					// ���� ���������� - ���� �������� 0
	PulsePhaseControl();				// ����
	MonitorUpdate1();					// ������� - ������ �����
}

__inline void ADC_Aquisition(void){		//18kHz

	URfltr.Input = _IQ16toIQ(ADC_UR);
	USfltr.Input = _IQ16toIQ(ADC_US);
	UTfltr.Input = _IQ16toIQ(ADC_UT);
	IUfltr.Input = _IQ16toIQ(ADC_IU);
	IVfltr.Input = _IQ16toIQ(ADC_IV);
	IWfltr.Input = _IQ16toIQ(ADC_IW);

	ApFilter3Calc(&URfltr);
	ApFilter3Calc(&USfltr);
	ApFilter3Calc(&UTfltr);
	ApFilter3Calc(&IUfltr);
	ApFilter3Calc(&IVfltr);
	ApFilter3Calc(&IWfltr);
}

__inline void DmcPrepare(void){	//18kHz

		UR.Input = ADC_CONV(_IQtoIQ16(URfltr.Output), GrC->UR_Mpy, GrC->UR_Offset);
		US.Input = ADC_CONV(_IQtoIQ16(USfltr.Output), GrC->US_Mpy, GrC->US_Offset);
		UT.Input = ADC_CONV(_IQtoIQ16(UTfltr.Output), GrC->UT_Mpy, GrC->UT_Offset);
		IU.Input = ADC_CONV(_IQtoIQ16(IUfltr.Output), GrC->IU_Mpy, GrC->IU_Offset);
		IV.Input = ADC_CONV(_IQtoIQ16(IVfltr.Output), GrC->IV_Mpy, GrC->IV_Offset);
		IW.Input = ADC_CONV(_IQtoIQ16(IWfltr.Output), GrC->IW_Mpy, GrC->IW_Offset);

		// �������������� �������� ����

		if (IsStopped())
		{
			if (!InRange(IU.Input, -15, 15))
			{
				if(IU.Input > 0) GrC->IU_Offset++;
				if(IU.Input < 0) GrC->IU_Offset--;
			}
				if (!InRange(IV.Input, -15, 15))
			{
				if(IV.Input > 0) GrC->IV_Offset++;
				if(IV.Input < 0) GrC->IV_Offset--;
			}
				if (!InRange(IW.Input, -15, 15))
			{
				if(IW.Input > 0) GrC->IW_Offset++;
				if(IW.Input < 0) GrC->IW_Offset--;
			}
		}

		ileg_trn_calc(&UR);
		ileg_trn_calc(&US);
		ileg_trn_calc(&UT);
		ileg_trn_calc(&IU);
		ileg_trn_calc(&IV);
		ileg_trn_calc(&IW);

	if (!IV.CurAngle) Phifltr.Input = _IQ16toIQ(US.CurAngle); 
}

__inline void PulsePhaseControl(void){	//18kHz	

	sifu_calc2(&Sifu);	
	
	THYR_PORT |= ((LgUns)Sifu.Status & ((LgUns)THYR_MASK));   //
	THYR_PORT &= ((LgUns)Sifu.Status | (~((LgUns)THYR_MASK)));//

	OUT_SET(ENB_TRN, (Sifu.Status >> SIFU_EN_TRN) & 0x1);
}

void DmcIndication1(void)
{
	// ���� �������� � �
	Iload[0] = I_RMS_CALC(IU.Output);
	Iload[1] = I_RMS_CALC(IV.Output);
	Iload[2] = I_RMS_CALC(IW.Output);
	if (IsStopped() && !IsTestMode()) memset(Iload, 0, 3);		// ���� ���� � �� ���� �� 0

	// ���� �������� � %
	Ipr[0] = ValueToPU1(Iload[0], GrC->Inom);
	Ipr[1] = ValueToPU1(Iload[1], GrC->Inom);
	Ipr[2] = ValueToPU1(Iload[2], GrC->Inom);

	// ��������� ����� �������� � ���� �������� ����������
	if (GrB->IIndicMode == imRms)     memcpy(&GrH->Iu, Iload, 3);
	if (GrB->IIndicMode == imPercent) memcpy(&GrH->Iu, Ipr,   3);
	GrC->OpenAngle = Sifu.OpenAngle;

	// ������� ����������� ���
	if (GrB->IIndicMode == imRms)     GrH->Imid = Imid;//	����� ��������� �������� ����, � � ��� %
	if (GrB->IIndicMode == imPercent) GrH->Imid = Imidpr;
	
	// ���� ����� ����� � �����������
	ApFilter1Calc(&Phifltr);
	GrC->AngleUI = _IQtoIQ16(Phifltr.Output);
	if (IsStopped() && !IsTestMode()) GrC->AngleUI = 0; // ���� �� ���� � �� ����, �� 0
}

void DmcIndication2(void)
{
	// ������� ����������
	GrH->Ur = UR.Output;
	GrH->Us = US.Output;
	GrH->Ut = UT.Output;

	// ������� ����������� ���������� � ����������� ��� ����
	Umfltr.Input = _IQ16toIQ(Mid3UnsValue(GrH->Ur, GrH->Us, GrH->Ut));
	ApFilter1Calc(&Umfltr);
	GrH->Umid = _IQtoIQ16(Umfltr.Output);

		#if BUR_M
		GrA->Status.bit.Power = 1;
		#else
		if (IsPowerOn())
			GrA->Status.bit.Power = 0;
		else {memset(&GrH->Ur, 1, 3); 
			}
		#endif

			
	if (!PhEl.Direction) GrH->PhOrdValue = 0;					// ����������� ��� ����
	else if (PhEl.Direction > 0) GrH->PhOrdValue = 2;
	else GrH->PhOrdValue = 1;
	
	// ����� � �����������
	GrA->Torque  = GrH->Torque;
	GrA->Speed   = GrH->Speed;
	GrA->Ur		 = GrH->Ur;
	GrA->Us		 = GrH->Us;
	GrA->Ut		 = GrH->Ut;
	GrA->Iu		 = GrH->Iu;
	GrA->Iv		 = GrH->Iv;
	GrA->Iw		 = GrH->Iw;
	GrA->AngleUI = GrC->AngleUI;

	GrC->IU_Input = IU.Input;
	GrC->IV_Input = IV.Input;
	GrC->IW_Input = IW.Input;
}

void DefineCtrlParams(void) // ������ �������������� ���������� � 
{
	Int MaxZone, CloseZone, OpenZone;	// ������������ ����, ���� ��������, ���� ��������
	
	if (GrB->ValveType == vtShiber){GrB->BreakMode = vtNone;} // ���� �������� �������� �������� �� ���������� ������������� ��� ���������� � "���������"

	MaxZone = (GrA->FullWay >> 1) - 1; // ������������ ���� ��� (������ ����/2)-1 
	
	CloseZone = GrB->CloseZone;			// �������� �������� ���� ��������
	if (CloseZone > MaxZone) CloseZone = MaxZone; // ���� ������ ������ ��������� ������ ��������
	
	OpenZone = GrB->OpenZone;			// ���� ����� ��� � ��������
	if (OpenZone > MaxZone) OpenZone = MaxZone;
	OpenZone = GrA->FullWay - OpenZone;	// ���� �������� ��� ������ ���� - ���� �������� ��������
	
	if (!Dmc.RequestDir)  // ���� �� ����������� ����������� �������� 
		Dmc.TorqueSet = TorqueUser;// ������ ������� = TorqueUser
	else if (Dmc.RequestDir > 0) // ���� ������ ������� �� ��������
	{
		Dmc.TorqueSet = GrB->MoveOpenTorque; // ������ ������� ��������s
		BreakFlag = 0;
		if (!IsNoCalib())		 // ���� ������������� ����� ���������
		{
			if (GrA->CurWay <= CloseZone)    { Dmc.TorqueSet = GrB->StartCloseTorque;  BreakFlag = 0;}// ���� ������� ��������� ��������� � ���� ������� �� ������ ������ ��� ������ �������� �� ��������
			else if (GrA->CurWay >= OpenZone){ Dmc.TorqueSet = GrB->BreakOpenTorque; BreakFlag = 1;} // ���� ��������� � ���� �������� ��  ������ ������ ��� ������ ���������� �� ��������
		}
	}
	else	// ���� ������� ����������� �� ��� �� �� �������� � ������� ���������� �� �������
	{
		Dmc.TorqueSet = GrB->MoveCloseTorque;
		BreakFlag = 0;
		if (!IsNoCalib())
		{
			if      (GrA->CurWay <= CloseZone)    	{ Dmc.TorqueSet = GrB->BreakCloseTorque; BreakFlag = 1;}
			else if (GrA->CurWay >= OpenZone) 		{ Dmc.TorqueSet = GrB->StartOpenTorque; BreakFlag = 0;}
		}
	}

#if !TORQ_TEST
	Dmc.TorqueSetPr = ValueToPU0(Dmc.TorqueSet, TorqueMax); // ������� ������ � % �� �������������
#endif
}

void StopPowerControl(void) // ���������� ��� �����
{
	GrA->Status.bit.Stop	  	= 1;	// ��������� � ������ ���������� ���� � ��� ��������� ��������
	GrA->Status.bit.Closing 	= 0;
	GrA->Status.bit.Opening 	= 0;
	GrA->Status.bit.Test	  	= 0;

	#if BUR_M
		GrH->ContGroup   = cgStop;
		PhEl.Direction 	 = 0;
		PowerLostTimer   = 0;
		PowerLostTimer2  = 0;
		Net_Mon_Timer = 0;
		DbgStop++;
	#endif 

	// ���� ������� �� �� ������ � �� �� �����, ��
	// �������� ����� ������������� ����������
	if (MuffFlag || IsFaulted() || IsDefected())
	 Dmc.WorkMode = wmStop;
	#if BUR_M
	else  Dmc.WorkMode = wmStop;// � �����.� ��� ������������� ����������
	#else 
	else if (DynBrakeEnable) Dmc.WorkMode = wmDynBrake;// � �����.� ���������� ������������ ����������
	#endif
	// �������� ����� ��������� ������
	Mcu.StartDelay = (Uns)START_DELAY_TIME;	
}


#if BUR_M
void NetMomitor(void)
{
	if ((GrH->FaultsLoad.all & LOAD_SHC_MASK) && IsPowerOn())	
		GrD->ControlWord = vcwStop;	

	if ((!IsPowerOff()) && (!IsTestMode())&& (Dmc.WorkMode == wmStop) && (GrH->CalibState == csCalib) && (!(GrH->FaultsLoad.all & LOAD_SHC_MASK))) // ���� ���� ����������, ���� ������ �� ���������� � ��������� � �����

	{ 
		if (IsPowerOff())
		{
			 Net_Mon_Timer	= 0;
			 PhEl.Direction = 0;
		}
		

		if (Net_Mon_Timer < NET_MON_START_TIME)	// ���� 
		{ 
			Net_Mon_Timer++;		
		}
		else 
		{
			Net_Mon_Timer = 0;

			switch(PhEl.Direction)
			{
				case  -1:GrD->ControlWord = vcwClose;  break;
				case  1 :GrD->ControlWord = vcwOpen;   break;	
			}	
		}
	}
	if (IsPowerOff())
	{
		 PhEl.Direction = 0;
	}
	if (IsPowerOff()&&(Dmc.WorkMode != wmStop)) //  ���� ���  ���������� � �� ���� �� ���������
	{
		if(PowerLostTimer2 < NET_MON_STOP_TIME2) //���� 1 �   (Uns)(GrC->BURM_Timer2 * PRD2) NET_MON_STOP_TIME2
		{
			PowerLostTimer2++;
		}
		else 
		{
			GrD->ControlWord = vcwStop;	
			PowerLostTimer2   = 0;
		}		
	}
	if ((PhEl.Direction == 0)&&(IsPowerOn()))  //  ���� ���� ���� ���� ����������, � ������������ ��� �� ����������
	{
		if(PowerLostTimer < NET_MON_STOP_TIME)// (Uns)(GrC->BURM_Timer1 * PRD2) NET_MON_STOP_TIME
		{		
			PowerLostTimer++;
		}
		else
		{
			GrD->ControlWord = vcwStop;	
			PowerLostTimer   = 0;
			GrH->FaultsNet.bit.VSk = 1; 

		}
	}
	
	if (Mcu.IgnorComFlag != 0) GrD->ControlWord = vcwStop;

	if ((Dmc.RequestDir != PhEl.Direction)&&(Dmc.RequestDir !=0)&&(PhEl.Direction != 0))
		GrH->FaultsNet.bit.RST_Err = 1;	

	if(GrH->FaultsNet.bit.RST_Err)
	{
		PhEl.Direction = 0;
	}
}

void ContactorControl(TContactorGroup i) // ���� 0 �� 
{
   switch(i)
   {
		case cgStop: 	
					GrH->Outputs.bit.Dout1 = 0;	//  ���.�������
					GrH->Outputs.bit.Dout0 = 0;	//  ���.�������
					GrH->Outputs.bit.Dout4 = 1;	//  ���.���� ���� ��������� ���� 0 �� ���� ���� 1 �� ������
					
				  if (++StopSetTimer > CONECTOR_STOP_TIME)
	   				{
	   					StopSetTimer   = 0;
						GrH->ContGroup = cgOff;
	   				}		 
					
				break;
		case cgOpen: 	
					GrH->Outputs.bit.Dout1 = 1;	//  ���.�������
					GrH->Outputs.bit.Dout0 = 0;	//  ���.�������
					GrH->Outputs.bit.Dout4 = 0;	//  ���.����

					if (++StopSetTimer > CONECTOR_TIME)
	   				{
	   					StopSetTimer   = 0;
						GrH->ContGroup = cgOff;
	   				}
					
				break;  
		case cgClose: 	
					GrH->Outputs.bit.Dout1 = 0;	//  ���.�������
					GrH->Outputs.bit.Dout0 = 1;	//  ���.�������
					GrH->Outputs.bit.Dout4 = 0;	//  ���.����
					
				if (++StopSetTimer > CONECTOR_TIME)
	   				{
	   					StopSetTimer   = 0;
						GrH->ContGroup = cgOff;
	   				}
					
				break;
		case cgOff: //��������� ���
				GrH->Outputs.bit.Dout1 = 0;	//  ���.�������
				GrH->Outputs.bit.Dout0 = 0;	//  ���.�������
				GrH->Outputs.bit.Dout4 = 0;	//  ���.����		
				break;
   }

}
#endif

void StartPowerControl(TValveCmd ControlWord)	// ���������� ��� �����
{
register Uns Tmp;

	if (GrH->FaultsLoad.all & LOAD_SHC_MASK) return;		// ���� ���� �� �� �������

#if !BUR_M
	if (!PhEl.Direction) return;							// ���� �� ���������� ��� ����������� ���, �� ������� ��� ����
#endif							// ���� �� ���������� ��� ����������� ���, �� �������
	
	switch(ControlWord)	// ������� ����������� �����
	{
		case vcwClose:					// ���� ��������
			Dmc.WorkMode   = wmTestPh;	// ���. ���� ���
			Dmc.RequestDir = -1;		// ����������� ��������
			#if BUR_M
			if(!PhEl.Direction)
			{
				GrH->ContGroup = cgClose;
			}
			#endif 
			break;
		case vcwOpen:					 
			Dmc.WorkMode   = wmTestPh;	 
			Dmc.RequestDir = 1;
			#if BUR_M
			if(!PhEl.Direction)
			{
				GrH->ContGroup = cgOpen;
			}
			#endif 
			break;
		case vcwTestClose:				 // ���� ���� �������� 
			Dmc.WorkMode   = wmSpeedTest;// �������� �������� ��������
			Dmc.RequestDir = -1;		 // ����������� ��������
			GrA->Status.bit.Test = 1;  // ���������� ��� �����
			#if BUR_M
			if(!PhEl.Direction)
			{
			GrH->ContGroup = cgClose;
			}
			#endif 
			break;
		case vcwTestOpen:
			Dmc.WorkMode   = wmSpeedTest;
			Dmc.RequestDir = 1;
			GrA->Status.bit.Test = 1;
			#if BUR_M
			if(!PhEl.Direction)
			{
				GrH->ContGroup = cgOpen;
			}
			#endif 
			break;
		default:
			return;						 // �� ������� ���� �� ������ ������� ������� 
	}
	
	// ��������� ������ "��������� �������"
	KickModeEnable = GrC->KickCount && (Dmc.RequestDir > 0); // ���� ������� ���������� ������ � ����������� �����������
	Torq.ObsEnable = True;//  ��������� ������ �������
	
	// ����� ������ ����������� ��� ����� (����� ����������)
	ProtectionsReset(); 

	// ���������� ������������ ����������
	ClkThyrControl(1);

	// ����� ��������� �������
	KickCounter      = 0;
	KickModeTimer    = 0;
	TestModeTimer    = 0;
	UporModeTimer    = 0;
	PreventModeTimer = 0;
	PauseModeTimer   = 0;
	DynModeTimer     = 0;
	#if BUR_M
	Net_Mon_Timer    = 0;

		PowerLostTimer   = 0;
		PowerLostTimer2  = 0;
	#endif

	// ����������� ����������� ������ ����������
	#if BUR_M
	Sifu.Direction = SIFU_UP;
	#else
	if (Dmc.RequestDir != PhEl.Direction) Sifu.Direction = SIFU_DOWN;	// ���� ����������� ����������� ��� �� ������������ ������� �� ������ ������� �� ����
	else Sifu.Direction = SIFU_UP;
	#endif

	// ������������ � �������� � ��������� ����������
	GrA->Status.bit.Stop = 0; //
	Dmc.TorqueSet = 0xFFFF;     // ���������� ������������ ������
	if (Dmc.RequestDir < 0) GrA->Status.bit.Closing = 1; // ��������� �������� "��������"
	if (Dmc.RequestDir > 0) GrA->Status.bit.Opening = 1; // ��������� �������� "��������"

	Tmp = (GrA->Status.all & STATUS_MOVE_MASK);  // ���������� � ���� 2 ���� (����������� � �����������, ������� ���� ��� � �����)
	if (RunStatus == Tmp) ZazorTimer = 0;		   // ���� ����������� �� ���������� ������
	else {ZazorTimer = GrH->ZazorTime * (Uns)ZAZOR_SCALE; RunStatus = Tmp;} // ����� ���� �������� ������ ����������� �� ������� ����� ������ ��� 0
	
	// ���������� ������������� ����������
	#if BUR_M
	DynBrakeEnable = False;
	#else
	DynBrakeEnable = True;	
	#endif
}

//---state machine---------

void ControlMode(void)
{
	PowerCheck(); // ��������� ��������� ������� ������� ���������

#if BUR_M
	ContactorControl(GrH->ContGroup);
#endif

	if (IsTestMode()) {DmcTest(); return;}	// ���� ���� ��������� ���� � �������

	TorqueCalc();


	switch (Dmc.WorkMode) // ����� ������ �� ������ ������������ �����
	{
		case wmStop:       StopMode();       break;
		case wmTestPh:     TestPhMode();     break;
		case wmUporStart:  UporStartMode();  break;
		case wmMove:       MoveMode();       break;
		case wmPause:      PauseMode();		 break;
		case wmUporFinish: UporFinishMode(); break;
		case wmKick:       KickMode();       break;
		case wmSpeedTest:  SpeedTestMode();  break;
		case wmDynBrake:   DynBrakeMode();   break;
	}
	
	if (ZazorTimer > 0) ZazorTimer--;	// ���� ���� ��������� ������
}

__inline void StopMode(void)		// ���. ����
{
	Sifu.SetAngle   = SIFU_MAX_ANG; // ��������� ���������
	Sifu.AccelTime  = 0;			// �������� �������� 0(����� �������)
	Sifu.Direction  = SIFU_NONE;    // ��������� ����
	GrH->Torque = 0;				// ������ ����
	Dmc.RequestDir = 0;

	
	ClkThyrControl(0);				// ��������� ������������ ����������
}

__inline void TestPhMode(void)		// ���. ���� ���
{									// �������� �� �� � protection.c
	#if BUR_M	
	if (!PhEl.Direction)	return;
	#endif	 
	Sifu.SetAngle   = 115;			// 
	Sifu.AccelTime  = 100;
	GrH->Torque = 0;			// ������ �� ����������

	if (++TestModeTimer >= (Uns)TEST_STATE_TIME)// ������� ����� ����� 0.04 ���
	{
		Dmc.WorkMode = wmUporStart;				// ��������� �� ���. ���� (����� ����������� ������� �� ������)
		TestModeTimer = 0;						// ���������� ������ ��������� ������
	}
}

__inline void UporStartMode(void)	// ���. ���� �����
{
	Sifu.SetAngle   = Torq.SetAngle;// ���� �������� �� ����� �������
	Sifu.AccelTime  = GrC->VoltAcc; // ������������� 100 
	GrH->Torque 	= Dmc.TorqueSet - 1; // ���������� ������ = ������� �������
	
//	#if 0 // ???
	if(GrH->UporOnly == 1) return;
	if (++UporModeTimer >= (Uns)UPOR_STATE_TIME) // ������� ����� ����� 2 ���.
	{
		if (AbsSpeed > MIN_FREQ_RPM) // ���� �������� ���������� ���� 600 ��/���, �� 
		{
			Dmc.WorkMode = wmMove;	 // ���. ��������
			KickModeEnable = False;  // ����� ����� �� ���������
		}
		else if (KickModeEnable)	 // ����� ���� ������� ����� �����
		{
			if (KickCounter < GrC->KickCount) Dmc.WorkMode = wmKick; // ���� ������� ������ �� �������� �� ��������� ����� �����
			else KickModeEnable = False;								 // ����� ��������� ����� �����
		}
		UporModeTimer = 0;	// ���������� ������ �����, ���� ����� �� ���������� �������� ������
	}
//	#endif
}

__inline void MoveMode(void)	// ���. ��������
{
	Sifu.SetAngle   = 0;	// ������ �������� ���������� (������� �� ���� �������� ����������)
	Sifu.AccelTime  = GrC->VoltAcc;	// 100
	GrH->Torque 	= Torq.Indication; // ���������� ������ ��� �� ����

	if (!Sifu.OpenAngle) PreventModeTimer++;		// ���� ���� �������� ���������� 0 �� ������� ������
	if (PreventModeTimer >= (Uns)MOVE_STATE_TIME)	// ���� ��������� � ������ �������� ������ 1 �
	{
		if ((Torq.Indication >= Dmc.TorqueSet) || (AbsSpeed < MIN_FREQ_RPM)) // ���� ������� ������� ������ ��� ������� ��������� ��������
			Dmc.WorkMode = wmPause;	// ���. �����
		PreventModeTimer = 0;		// �������� ������ 
	}
}

__inline void PauseMode(void) // ���. �����
{
	Sifu.OpenAngle  = SIFU_MAX_ANG; // ��������� ���������
	Sifu.AccelTime  = GrC->VoltAcc; //100
	GrH->Torque = Dmc.TorqueSet - 1;	   // ����� ����� �� ������� 

	if (++PauseModeTimer >= (Uns)PAUSE_STATE_TIME) // ����� 0.04 �������. 
	{
		Dmc.WorkMode = wmUporFinish;	// ��������� � ����� ���. ���� �����
		PauseModeTimer = 0;				// �������� ������
	}
}

__inline void UporFinishMode(void)	// ���. ���� �����
{
	Sifu.SetAngle   = Torq.SetAngle;// ���� �� �������
	Sifu.AccelTime  = GrC->VoltAcc; //100
	GrH->Torque 	= Dmc.TorqueSet - 1; // ������ �� �������
	
//	#if 0 // ???
	if(GrH->UporOnly == 1) return;
	if (++UporModeTimer >= (Uns)UPOR_STATE_TIME)	// 2 ���
	{
		if (AbsSpeed > MIN_FREQ_RPM) Dmc.WorkMode = wmMove; // ���� �������� ������ 600 ��/��� �� ������ ������� � ���. �������
		UporModeTimer = 0;									// �������� ������
	}
//	#endif
}

__inline void KickMode(void)		// ���. ����
{
	Sifu.SetAngle   = KickSetAngle;	// ������ ��������
	Sifu.AccelTime  = 0;			// ����. ������
	GrH->Torque = Dmc.TorqueSet - 1;// ������ ��� ������ �������
	
	KickModeTimer++;		// ������� ������ �����
	KickSetAngle = 150;		// ������� ��������� �� 150 ����
	if (KickModeTimer >= (Uns)KICK_ST0_TIME) KickSetAngle = 0; // ���� ������ ������ 0.04 �� ���������
	if (KickModeTimer >= (Uns)KICK_ST1_TIME)				   // ���� ������ �������� �� 0.08 �� 
	{
		Dmc.WorkMode = wmUporStart;	// ��������� �� ���� ������
		KickCounter++;				// �������� ������� ������ ������
		KickModeTimer = 0;			// �������� ������
	}
}

__inline void SpeedTestMode(void)	// ���. ����
{
	#if BUR_M
	if (!PhEl.Direction)	return;
	#endif	
	
	Sifu.SetAngle   = GrG->ThyrOpenAngle; // 0
	Sifu.AccelTime  = GrC->VoltAcc;     // 100
	GrH->Torque = Torq.Indication; // ��� ����
}

__inline void DynBrakeMode(void) // ����� ������������� ����������
{
	Sifu.AccelTime  = 0;		// ������������ �������� ��������
	GrH->Torque = 0;		// �� ���������� ������
	
	if (++DynModeTimer < (GrC->BrakeTime * (Uns)CTRLMODE_SCALE))	 // ���� ����� ���������� �� ��������� ��������� �����
	{
		Mcu.StartDelay = 0xFFFF; // ��������� ��������� ������ ����������

		if (DynModeTimer < (Uns)DYN_PAUSE_TIME) //  ���� ������ ������ 0.04� ��
		{
			Sifu.SetAngle  = SIFU_MAX_ANG;		// ���� ������������
			Sifu.Direction = SIFU_NONE;			// ���� ����������
		}
		else // ����� ���� � ����������� �� ����������� ���
		{
			if (!PhEl.Direction)	// ���� ����������� ��� �� �����������
				Sifu.SetAngle = SIFU_CLOSE_ANG;	// ���� ������� 180
			else if (PhEl.Direction > 0)	// ������ ������������������
			{
				Sifu.MaxAngle = 140;		// 140
				Sifu.SetAngle = GrC->BrakeAngle - 30;//���� ������� � ������ 60 - 30
			}
			else							// ��������
			{
				Sifu.MaxAngle = SIFU_MAX_ANG;// 180
				Sifu.SetAngle = GrC->BrakeAngle + 30; // ���� ������� 60 + 30 
			}
			Sifu.Direction = SIFU_DYN_BREAK; // ���������� ����� ���� ������������ ����������
		}
	}
	else // ���� ����� ���������� ����� �� ������ � ���. ����
	{
		Sifu.MaxAngle  = SIFU_MAX_ANG;			// ������������ ����  
		Dmc.WorkMode   = wmStop;
		DynBrakeEnable = False;	// ��������� ����� ������������� ����������
		DynModeTimer   = 0;		// ���������� ������ ���.����.
		Mcu.StartDelay = (Uns)START_DELAY_TIME; // ������ ����� ����� �������
	}
}

__inline void DmcTest(void)				// ���� ������ ����������
{
	if (!GrG->SifuEnable)		// ���� ��������� ������ ���� � �������� ������
	{									// ��������� ��������� ���� � ��� ��� ���
		Dmc.WorkMode   = wmStop;		// ����� ��� � ����� 
		Sifu.SetAngle  = Sifu.MaxAngle; // ��������� ���������
		Sifu.AccelTime = 0;				// �������� �������� ������������
		Sifu.Direction = SIFU_NONE;     // ��������� ����
		GrA->Status.bit.Test = 0;     // ���������� ��� ����� ����� � ��������� ����� �������� ��� �� ���������� ����������
		ClkThyrControl(0);				// ��������� ������������ ����������
	}
	else
	{
		Dmc.WorkMode    = wmTestThyr;	// ����� ��� ��������� ���������
		Sifu.SetAngle   = SIFU_OPEN_ANG;// ���� �������� 0 ��������
		Sifu.AccelTime  = 0;			// ������������ �������� ��������
		#if BUR_M
		Sifu.Direction = SIFU_UP;
		#else
		Sifu.Direction  = (!GrG->ThyrGroup) ? SIFU_UP : SIFU_DOWN; // ����� ������ ���������� ��� �������� - ������ ��� �����������
		#endif
		GrA->Status.bit.Test = 1;     // ���������� ��� ��������� � �����
		ClkThyrControl(1);				// ��������� ������������ ���������� 
	}
}

__inline void TestThyrControl(void)     //���. ���� ����������
{
	if (Dmc.WorkMode != wmTestThyr) return;	// ���� �� ���� �� ������� �� ���� �� �����
	
	UR.CurAngle = SIFU_OPEN_ANG;			// ��������� ��� ���������
	US.CurAngle = SIFU_OPEN_ANG;
	UT.CurAngle = SIFU_OPEN_ANG;
}

void CalibStop(void)	// ��������� �� ������ ����������
{
	Bool StopFlag = False; // ��������� ���� ���������

	if (Mcu.Valve.Position == POS_UNDEF) {Dmc.TargetPos = POS_UNDEF; return;}  // ���� �� ����� ������� ��������� �� ������ ������� � ����������� � �������
	Dmc.TargetPos = Calib.LinePos - Mcu.Valve.Position; // ���� ������ ���������, ��  ������� ������ :)
	
	if (IsStopped() || GrG->TestCamera) return; // ���� ������������ �� �������
	
	if ((Dmc.RequestDir < 0) && (Dmc.TargetPos <= 0)) StopFlag = True; // ���� ����������� �������� �������� ������� ��������� ���������� ��� ��������� �� ������ ���� �����
	if ((Dmc.RequestDir > 0) && (Dmc.TargetPos >= 0)) StopFlag = True; // ���� ����������� �������� ��������
	
	if (StopFlag)	// ���� ����� ���������������
	{
		DbgEncoder = Encoder.Revolution;
		if (Mcu.Valve.BreakFlag) OverWayFlag = 1; // ���� �������� � ����������� �� ���������� ��� ���������� �� ����������
		else 
			{
			ValveDriveStop(&Mcu, False);		  // ���� ��� ���������� �� ���� ������� �� ���� � �������
				Mcu.EvLog.Value = CMD_STOP;			// ���� �� ���������
			}
	}
}

void TorqueCalc(void)	// ������ ������� �� ����
{
	register TCubStr *Cub;	//
	register Int Tmp;
	register Int Add;		// ������ �������� ��������� �������
	register Int UporAdd;	// ������ �������� ������� �����

	Imfltr.Input = _IQ16toIQ(Mid3UnsValue(Iload[0], Iload[1], Iload[2]));
	ApFilter3Calc(&Imfltr);
	Imid = _IQtoIQ16(Imfltr.Output);
	Imidpr = ValueToPU1(Imid, GrC->Inom);

#if !TORQ_TEST	// ���� �� ����
//	if (GrC->DriveType != 0) return; // ���� ��������� ��� ������� �� �������
	if (!Torq.ObsEnable) {Torq.Indication = 0; Torq.SetAngle = Sifu.MaxAngle; return;} //���� �������� ������ ������� �� ��������� 0 � ��������� ��������� � �������

	Cub = (Imidpr >= GrH->TransCurr) ? &Torq.Cub1 : &Torq.Cub2;  // �������� �� ������ ���� �������� ��� ��������� ��� ������� �����
#else
	Cub = (TqImidPr >= GrH->TransCurr) ? &Torq.Cub1 : &Torq.Cub2; // �������� ������ �������
#endif
	
	CubCalc(Cub);	// ������� ��������� ���
	
#if !TORQ_TEST

	if (Dmc.TorqueSetPr < 40) Add = GrC->Corr40Trq;		// ������� PIA 09.10.2012
	else if (Dmc.TorqueSetPr < 60)	  Add = GrC->Corr60Trq;		// ������������� ��������� ������� ��� +- ��������� ������� �������� �� ����������� �����			
	else if (Dmc.TorqueSetPr < 80)	  Add = GrC->Corr80Trq;
	else if (Dmc.TorqueSetPr < 110)	  Add = GrC->Corr110Trq;

	Trqfltr.Input = _IQ16toIQ(Cub->Output); // ��������� �������� �������
	ApFilter3Calc(&Trqfltr);				// 
	Tmp =(_IQtoIQ16(Trqfltr.Output)) + Add; 


//	Tmp = Cub->Output;


	if (Tmp < TORQ_MIN_PR) Tmp = TORQ_MIN_PR;	// ��������� �� ��������� � ���� �� 
	if (Tmp > TORQ_MAX_PR) Tmp = TORQ_MAX_PR;   // 10 �� 110 %
	
	Torq.Indication = PU0ToValue(Tmp, TorqueMax);// ��������� �������� � �� ������������ ������������� �
#else
	Torq.Indication = Cub->Output;	// ���� ���� �������� ��� ������� ������ �����
#endif

if 		(Dmc.TorqueSetPr < 30)	
			UporAdd = GrC->Upor25;
else if (Dmc.TorqueSetPr < 40)	
			UporAdd = GrC->Upor35;
else if (Dmc.TorqueSetPr < 60)	
			UporAdd = GrC->Upor50;
else if (Dmc.TorqueSetPr < 80)	
			UporAdd = GrC->Upor75;
else if	(Dmc.TorqueSetPr < 110)	
			UporAdd = GrC->Upor100;

	Torq.SetAngle   = Torq.Cub3.Output - UporAdd; // �������� ������� ��� ���� (����)
}


__inline void TorqueObsInit(void)
{	
	switch (GrC->DriveType) 
	{
		case dt100_A25:   PFUNC_blkRead(&drive1,   			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
						  PFUNC_blkRead(&TransCurrDef[0], 	(Int *)(&Ram.GroupH.TransCurr),		  1);
						  GrH->UporOnly = 1;

						  if ((GrC->GearRatio != GearRatioDef[0])||(GrC->Inom != InomDef[0])||(GrC->MaxTorque != MomMaxDef[0]))
						  {
						  	if (IsMemParReady())
						  	{
								GrC->GearRatio = GearRatioDef[0];
								GrC->Inom = InomDef[0];
								GrC->MaxTorque = MomMaxDef[0];
						  		WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3); 
						  	}	
						  }						  
		 		break;//1
		case dt100_A50:   PFUNC_blkRead(&drive2,   			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
						  GrH->UporOnly = 0;
						  PFUNC_blkRead(&TransCurrDef[1], 	(Int *)(&Ram.GroupH.TransCurr),		  1);

						  if ((GrC->GearRatio != GearRatioDef[0])||(GrC->Inom != InomDef[1])||(GrC->MaxTorque != MomMaxDef[1]))
						  {
						  	if (IsMemParReady())
						  	{
								GrC->GearRatio = GearRatioDef[0];
								GrC->Inom = InomDef[1];
								GrC->MaxTorque = MomMaxDef[1];
						  		WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3); 
						  	}	
						  }							  
				break;//2
		case dt400_B20:   PFUNC_blkRead(&drive3,   			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
					 		GrH->UporOnly = 0;
						  PFUNC_blkRead(&TransCurrDef[2], 	(Int *)(&Ram.GroupH.TransCurr),		  1);

						  if ((GrC->GearRatio != GearRatioDef[1])||(GrC->Inom != InomDef[2])||(GrC->MaxTorque != MomMaxDef[2]))
						  {
						  	if (IsMemParReady())
						  	{
								GrC->GearRatio = GearRatioDef[1];
								GrC->Inom = InomDef[2];
								GrC->MaxTorque = MomMaxDef[2];
						  		WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3); 
						  	}	
						  }							 
		 		break;//3
		case dt400_B50:   PFUNC_blkRead(&drive4,   			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
					 		GrH->UporOnly = 0;
						  PFUNC_blkRead(&TransCurrDef[3], 	(Int *)(&Ram.GroupH.TransCurr),		  1);

						  if ((GrC->GearRatio != GearRatioDef[1])||(GrC->Inom != InomDef[3])||(GrC->MaxTorque != MomMaxDef[3]))
						  {
						  	if (IsMemParReady())
						  	{
								GrC->GearRatio = GearRatioDef[1];
								GrC->Inom = InomDef[3];
								GrC->MaxTorque = MomMaxDef[3];
						  		WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3); 
						  	}	
						  }	
				break;//4
		case dt800_V40:   PFUNC_blkRead(&drive5,   			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
						  GrH->UporOnly = 0;
						  PFUNC_blkRead(&TransCurrDef[4], 	(Int *)(&Ram.GroupH.TransCurr),		  1);

						  if ((GrC->GearRatio != GearRatioDef[1])||(GrC->Inom != InomDef[4])||(GrC->MaxTorque != MomMaxDef[4]))
						  {
						  	if (IsMemParReady())
						  	{
								GrC->GearRatio = GearRatioDef[1];
								GrC->Inom = InomDef[4];
								GrC->MaxTorque = MomMaxDef[4];
						  		WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3); 
						  	}	
						  }			  
				break;//5
		case dt1000_V20 : PFUNC_blkRead(&drive6,   			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
					GrH->UporOnly = 0;
						  PFUNC_blkRead(&TransCurrDef[5], 	(Int *)(&Ram.GroupH.TransCurr),		  1);

						  if ((GrC->GearRatio != GearRatioDef[1])||(GrC->Inom != InomDef[5])||(GrC->MaxTorque != MomMaxDef[5]))
						  {
						  	if (IsMemParReady()) 
						  	{
								GrC->GearRatio = GearRatioDef[1];
								GrC->Inom = InomDef[5];
								GrC->MaxTorque = MomMaxDef[5];
						  		WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3); 
						  	}	
						  }	  
		 		break;//6
		case dt4000_G9  : PFUNC_blkRead(&drive7,   			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
					GrH->UporOnly = 0;
						  PFUNC_blkRead(&TransCurrDef[6], 	(Int *)(&Ram.GroupH.TransCurr),		  1);

						  if ((GrC->GearRatio != GearRatioDef[2])||(GrC->Inom != InomDef[6])||(GrC->MaxTorque != MomMaxDef[6]))
						  {
						  	if (IsMemParReady())
						  	{
								GrC->GearRatio = GearRatioDef[2];
								GrC->Inom = InomDef[6];
								GrC->MaxTorque = MomMaxDef[6];
						  		WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3); 
						  	}	
						  }						  
				break;//7
		case dt4000_G18 : PFUNC_blkRead(&drive8,   			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
					 	GrH->UporOnly = 0;
						  PFUNC_blkRead(&TransCurrDef[7], 	(Int *)(&Ram.GroupH.TransCurr),		  1);

						  if ((GrC->GearRatio != GearRatioDef[2])||(GrC->Inom != InomDef[7])||(GrC->MaxTorque != MomMaxDef[7]))
						  {
						  	if (IsMemParReady())
						  	{
								GrC->GearRatio = GearRatioDef[2];
								GrC->Inom = InomDef[7];
								GrC->MaxTorque = MomMaxDef[7];
						  		WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3); 
						  	}	
						  }			 				 
				break;//8
		case dt10000_D6 : PFUNC_blkRead(&drive9,   			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
					 	  GrH->UporOnly = 0;
						  PFUNC_blkRead(&TransCurrDef[8], 	(Int *)(&Ram.GroupH.TransCurr),		  1);

						  if ((GrC->GearRatio != GearRatioDef[3])||(GrC->Inom != InomDef[8])||(GrC->MaxTorque != MomMaxDef[8]))
						  {
						  	if (IsMemParReady())
						  	{
								GrC->GearRatio = GearRatioDef[3];
								GrC->Inom = InomDef[8];
								GrC->MaxTorque = MomMaxDef[8];
						  		WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3); 
						  	}	
						  }	
				break;//9
		case dt10000_D12: PFUNC_blkRead(&drive10,  			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
						  GrH->UporOnly = 0;
						  PFUNC_blkRead(&TransCurrDef[9], 	(Int *)(&Ram.GroupH.TransCurr),		  1);

						  if ((GrC->GearRatio != GearRatioDef[4])||(GrC->Inom != InomDef[9])||(GrC->MaxTorque != MomMaxDef[9]))
						  {
						  	if (IsMemParReady())
						  	{
								GrC->GearRatio = GearRatioDef[4];
								GrC->Inom = InomDef[9];
								GrC->MaxTorque = MomMaxDef[9];
						  		WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3); 
						  	}	
						  }							  
				break;//10
	}
} 

void SpeedCalc(void) // ������ ��������
{
	static   Uns   Timer = 0;
	static   LgUns RevPrev = -1UL;
	register LgInt Delta;
	
	if (++Timer >= (Uns)SP_CALC_TOUT) // �������� ������� �������� 1 ���
	{
		if (RevPrev == -1UL) 		  // 1 ��������
			Delta = 0;				 // ������ � 0
		else
		{
			Delta = Encoder.Revolution - RevPrev; // ������ = ����� �������� - ����������
			if (Delta >  SP_POS_LIMIT) Delta -= (REV_MAX+1); // ���� ������� ����� ���� �������� �� ��������
			if (Delta < -SP_POS_LIMIT) Delta += (REV_MAX+1);
		}
		RevPrev = Encoder.Revolution; // ���������� ���������� ��������

		GrH->Speed = (Delta * (LgInt)GrC->BlockToGearRatio) >> 2; // ������� ��������. ��������� ��������� �� ������� * �� ������������ ����� = �������� ��������� �����
		AbsSpeed = abs(GrH->Speed);	//���������� ��������

		Timer = 0;						// �������� ������
	}
}

void LowPowerControl(void)		// ���������� ��� ������� ���������� ??? 
{
#if !LOWPOW_TEST
	static Bool ShCFlag = False;
	Uns ShCState = 0;
	
	ShCState = GrH->FaultsLoad.all & LOAD_SHC_MASK;	// ������� ��������� ��
	if (!ShCState) ShCFlag = False;					// ���� ��� �� �� �������� ����
	
	if (IsTestMode()) PowerOn();					// ���� �������� ����� �� ��� ��������
	else if (LowPowerTimer > 0) LowPowerTimer--;	// ���� 
	else if (LowPowerReset & BIT0)					// ���� ��������� ����� ������ ������� ����������
	{
		PowerOn();									// �������� �������
		if (IsMemParReady())						// ���� ����� ����������
		{
			GrH->FaultsLoad.all |= GrH->ScFaults;	//  
			GrH->ScFaults = 0;						// 
//!!!		WritePar(REG_SC_FAULTS, &GrH->ScFaults, 1);
			LowPowerReset &= ~BIT0;					// ������� ����� ������
		}
	}
	else if (!ShCFlag && ShCState)					// ���� ��
	{
		DisplayRestartFlag = true;
		PowerOff();									// ��������� 
		if (IsMemParReady())
		{
			GrH->ScFaults = ShCState;				// ��������� ��
			WritePar(REG_SC_FAULTS, &GrH->ScFaults, 1);
			LowPowerReset |= BIT0;					// ������ � ��������
			LowPowerTimer = (Uns)POWER_TOUT;		// ����� � 3 ���
			ShCFlag = True;						    // ���� �� ������
		}
		
		#if BUR_M
			GrH->ContGroup = cgStop;				// ��������� ���������� �� �
		//GrD->ControlWord = vcwStop;			
		#endif
	}
	#if BUR_M
	else if (GrA->Status.bit.Power)				// 
	#else
	else if (!GrA->Status.bit.Power)			// 
	#endif		
	{	
		if (!PowerSupplyEnable)	// ���� �������� ������� ������� �����������
		{
			ClkThyrControl(0);				
			Sifu.Direction = SIFU_NONE; // ��������� ����
			DisplayRestartFlag = true;
			PowerOff();
			LowPowerReset |= BIT1;		// ������ � �������
			LowPowerTimer = 10;
			InitSysCtrl(DSP28_PLLCR(SYSCLK, CLKIN*4), 0);					// ��������� ������� � ��� ����
		}
		else if (LowPowerReset & BIT1)	// ���� ������ � ����������� ������
		{
			InitSysCtrl(DSP28_PLLCR(SYSCLK, CLKIN), 0);						// ������� ������� �� ������� �������
			PowerEnable = True;			
			Sifu.Direction = SaveDirection; //SIFU_NONE; // // ���� � ��� �� ����������� ��� � ����
			ClkThyrControl(1);
			DisplayRestartFlag = true;
			PowerOn();					// ��������	
			LowPowerReset &= ~BIT1;     // ���������� ��� ���������� � �������
		}
		else SaveDirection = Sifu.Direction; // ��������� � ����� ����������� �����
	}
	else	// ���� ��������� ������� 
	{
	//	DisplayRestartFlag = true;
		PowerOn();	// �������� ���
		LowPowerReset &= ~BIT1;//������ ������ 
	}
#endif
}

Bool DmcControlRefresh(void) // ��� � RefreshData
{
	if (!DmcRefrState) return true;	

	switch(++DmcRefrState)
	{
		case 2:
			TorqueObsInit();		
			break;
		case 3:
			URfltr.Tf = _IQdiv(GrC->CoefVoltFltr, 1E08); 		  			ApFilter3Init(&URfltr); //
			USfltr.Tf = URfltr.Tf; 								  			ApFilter3Init(&USfltr);
			UTfltr.Tf = URfltr.Tf; 								  			ApFilter3Init(&UTfltr);
			break;
		case 4:
			IUfltr.Tf = _IQdiv(GrC->CoefCurrFltr, 1E08); 					ApFilter3Init(&IUfltr);
			IVfltr.Tf = IUfltr.Tf; 											ApFilter3Init(&IVfltr);
			IWfltr.Tf = IUfltr.Tf; 											ApFilter3Init(&IWfltr);
			break;
		case 5:	TorqueMax = GrC->MaxTorque * 10; // ������� ������������� ������� � *10
		 		break;
		case 6: CubRefresh(&Torq.Cub1, &GrH->TqCurr);  break;
		case 7: CubRefresh(&Torq.Cub2, &GrH->TqAngUI); break;
		case 8: CubRefresh(&Torq.Cub3, &GrH->TqAngSf);
				DmcRefrState = 0; 
				break;

	}
	
	return !DmcRefrState;
}

void CubInit(TCubStr *v, TCubConfig *Cfg)	//������������� ����
{
	register TCubPoint *Pt;
	register Uns i, j, Tmp;

	v->InputX = Cfg->X_Value;
	v->InputY = Cfg->Y_Value;
	
	v->Min.X  = 0x7FFF;
	v->Max.X  = 0x8000;
	v->Min.Y  = 0x7FFF;
	v->Max.Y  = 0x8000;
	
	for (i=0; i < CUB_COUNT1; i++)
	{
		Tmp = Cfg->X_Array[i];
		for (j=0; j < CUB_COUNT2; j++)
		{
			Pt = &v->Points[i][j];
			Pt->X = Tmp;
			Pt->Y = Cfg->Y_Array[j];

			if (Pt->X < v->Min.X) v->Min.X = Pt->X;
			if (Pt->X > v->Max.X) v->Max.X = Pt->X;
			if (Pt->Y < v->Min.Y) v->Min.Y = Pt->Y;
			if (Pt->Y > v->Max.Y) v->Max.Y = Pt->Y;
		}
	}
}

void CubRefresh(TCubStr *v, TCubArray *Array)	//
{
	register TCubPoint *Pt;
	register Uns i, j;
	
	for (i=0; i < CUB_COUNT1; i++)
	{
		for (j=0; j < CUB_COUNT2; j++)
		{
			Pt = &v->Points[i][j];
			Pt->Z = Array->Data[i][j];
		}
	}
}

void CubCalc(TCubStr *v)
{
	Int A0, A1, B0, B1;
	
	// �������� ������� ������ �� ������ ���� ��� ������ �� ������� ����
	v->Input.X = *v->InputX;
	v->Input.Y = *v->InputY;
	
	if (v->Input.X < v->Min.X) v->Input.X = v->Min.X;
	if (v->Input.X > v->Max.X) v->Input.X = v->Max.X;
	if (v->Input.Y < v->Min.Y) v->Input.Y = v->Min.Y;
	if (v->Input.Y > v->Max.Y) v->Input.Y = v->Max.Y;
	
	// ����� ��������
	for (v->Num1=0; v->Num1 < CUB_COUNT1-1; v->Num1++)
		if (v->Input.X < v->Points[v->Num1+1][0].X) break;
	for (v->Num2=0; v->Num2 < CUB_COUNT2-1; v->Num2++)
		if (v->Input.Y < v->Points[v->Num1][v->Num2+1].Y) break;

	// ������ ����� R
	A0 = v->Points[v->Num1][v->Num2].X;
	A1 = v->Points[v->Num1+1][v->Num2].X;
	B0 = v->Points[v->Num1][v->Num2].Z;
	B1 = v->Points[v->Num1+1][v->Num2].Z;
	
	v->PtR  = B0 + _IQ1div((LgInt)(v->Input.X - A0) * (B1 - B0), (A1 - A0) << 1);
	
	// ������ ����� T
	A0 = v->Points[v->Num1][v->Num2+1].X;
	A1 = v->Points[v->Num1+1][v->Num2+1].X;
	B0 = v->Points[v->Num1][v->Num2+1].Z;
	B1 = v->Points[v->Num1+1][v->Num2+1].Z;

	v->PtT  = B0 + _IQ1div((LgInt)(v->Input.X - A0) * (B1 - B0), (A1 - A0) << 1);

	// ��������� ������
	A0 = v->Points[v->Num1][v->Num2].Y;
	A1 = v->Points[v->Num1][v->Num2+1].Y;
	B0 = v->PtR;
	B1 = v->PtT;
	
	v->Output = B0 + _IQ1div((LgInt)(v->Input.Y - A0) * (B1 - B0), (A1 - A0) << 1);
}


// ������������ ����������
static void ClkThyrControl(Uns State)	
{
	if (State) 
	     EPwm4Regs.AQCTLA.bit.CAU = AQ_CLEAR;
	else EPwm4Regs.AQCTLA.bit.CAU = AQ_SET;
}

//---Monitor--------------//	18kHz
void MonitorUpdate1(void){

	if (!dot) switch (program)
	{
		 case 1:  
				 buffer1[mon_index] = US.Input;
		         buffer2[mon_index] = IU.Input;		break;
		 case 2:  
				 buffer1[mon_index] = US.Input;
			     buffer2[mon_index] = IV.Input;		break;
		 case 3:  
				 buffer1[mon_index] = US.Input; 
			     buffer2[mon_index] = IW.Input;		break;
		 case 4:  
				 buffer1[mon_index] = IU.Input;
			     buffer2[mon_index] = IV.Input;			break;

		 case 5:  buffer1[mon_index] = IU.Input;
			      buffer2[mon_index] = IW.Input;			break;
		 case 6:  buffer1[mon_index] = GpioDataRegs.GPADAT.bit.GPIO13;
			      buffer2[mon_index] = PowerSupplyEnable;			break;
		 case 7:  buffer1[mon_index] = AdcRegs.ADCRESULT0;
			      buffer2[mon_index] = AdcRegs.ADCRESULT2;			break;

		 case 10:  
				 buffer1[mon_index] = US.CurAngle;
		         buffer2[mon_index] = IU.CurAngle;		break;
		 case 11:  
				 buffer1[mon_index] = UR.Input;
			     buffer2[mon_index] = UR.Output;		break;
		 case 12:  
				 buffer1[mon_index] = AdcRegs.ADCRESULT3; 
			     buffer2[mon_index] =  AdcRegs.ADCRESULT3 - 32767;		break;
		 
		 case 13:
				 buffer1[mon_index] =  Torq.Indication; 
			     buffer2[mon_index] =  Dmc.WorkMode;		break;				
	}

	if (++dot > dot_max)
	{
		if (++mon_index > 255) mon_index = 0;
		dot = 0;
	}
}

void sifu_calc2(SIFU *v)				//������� ���� - �������� - ���� S � R ���������� �������, �.�. 
{
	v->Status = 0x3F;				// ���������� � ������� �������� �� ��� ������������ ����� � ���������� ������	������ 0011 1111 - ����� �������� ���������� ���������� �� ������ ��� ����� ������ �������
	if (v->Direction == SIFU_NONE)	// ���� �� ������ ����������� ������ - ������ ���� ���������� ��
	{
		v->AccelTimer = 0;			// �������� ������ ��� ������������ �������������
		v->OpenAngle = v->SetAngle; // � ������ ������� �� ���� �������� �� ������� ���� ��������
	}
	else							// ���� ���� ��������� � ���� �������
	{
		v->Status &= ~(1<<SIFU_EN_TRN);	// ���������� ��� ��������� ����� �� ������� ������ ������� Status = 0001 1111

		if (!v->AccelTime) v->OpenAngle = v->SetAngle;	// ���� �������� ���������� �� ������� ������ ������� ������ �� ������ �������� �������� �� ������ ���� �������� = ���� �������
		else if (++v->AccelTimer >= v->AccelTime) // ���� �������� �������� ������� �� ��������� ������  ������������ �������� � ����� �� ����������
		{
			if (v->OpenAngle < v->SetAngle) v->OpenAngle++;	// ���� ������� ���� �������� ������ ���� ������� �� ����������� ���� �������� �� 1
			if (v->OpenAngle > v->SetAngle) v->OpenAngle--; // ���� ���� �������� ������ ���� ������� �� ��������� ������� ���� �� 1
			v->AccelTimer = 0;								// ���������� ������ ��� ��������� ��������
		}
																			// ��� ���������� ������������� ���� � ������� �����
		if ((*v->UrAngle >= v->OpenAngle) && (*v->UrAngle < v->MaxAngle))	// ���� ���� ���� ���� ������ ���� ��������� � ���� ���� ��������� - ������ �� ������ 170 ��������
			 v->Status &= ~((1<<SIFU_UR)|(1<<SIFU_UR_REV));								// ���������� ��� ������� ���������� �� ����� ���������� ��������� ���� UR. ���������� �.�. �������� ����������� ����� Status = 0001 1110

		if ((*v->UsAngle >= v->OpenAngle) && (*v->UsAngle < v->MaxAngle))	// ����������
		 	 v->Status &= ~(1<<SIFU_US);									// ��� �������� 2 ���� ��� ������ � ����������� ������ ���� S Status = 0000 1101

	  	if ((*v->UtAngle >= v->OpenAngle) && (*v->UtAngle < v->MaxAngle))	// ����������
			 v->Status &= ~((1<<SIFU_UT)|(1<<SIFU_UT_REV));					// ��� �������� 2 ���� ��� ������ � ����������� ������ ���� S Status = 0001 0011

		switch(v->Direction)												// � ����������� �� ���������� ����������� ������ ���������� �� ��������� ����� ��������� �������
		{
			case SIFU_DOWN: v->Status |= ((1<<SIFU_UR)|(1<<SIFU_UT)); break;// ���� ����������� �������� �� ���������� 1 ��� ����� ���� R S ���� ��� ���� ��������
			case SIFU_UP:   v->Status |= ((1<<SIFU_UR_REV)|(1<<SIFU_UT_REV)); break; // ���������� ������ �������� ������������ ��� ����� ����������� ������
			case SIFU_DYN_BREAK:	// ���� ������� ������� ����� ������������� ���������� �� (�������� �������������  ����������)
				if (*v->Polarity <= 0) v->Status |= (1<<SIFU_US); 	//  
				v->Status |=  ((1<<SIFU_UT)|(1<<SIFU_UT_REV));		//  
				v->Status &= ~((1<<SIFU_UR)|(1<<SIFU_UR_REV));		// 
			break;																
		}
	}
}

void PowerCheck(void)			// 200 Hz 
{
	if(POWER_CONTROL)			// ���� ������� ���������
		{
			PowerSupplyCnt++;			// �������� �� ����������
			
			if (DebugStartDelayCnt < DEBUG_START_TIME) // ���� ������ ������ 5 ���, ������ � ����� ���������
			{
				DebugStartDelayCnt++;

				if (PowerSupplyCnt >= 5)
				{
					PowerSupplyEnable = 0;
					PowerSupplyCnt = 5;
				}
			}
			else // ���� ������ ������ 5 ��� � �� ����������� �� ����� � ����� ������
			{
				PowerSupplyEnable = 1;	// ������ ����������
				PowerSupplyCnt = 0;		// �������� ������
				DebugStartDelayCnt = DEBUG_START_TIME;
			}
		}
	else 	// ���� ������� ��������
		{
			PowerSupplyEnable = 1;
			PowerSupplyCnt = 0;
			DebugStartDelayCnt = 0;
		}					
}
