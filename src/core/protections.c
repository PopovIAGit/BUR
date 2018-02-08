#include "config.h"
#include "protectionI2T.h"

DIFF_INPUT Muf = MUF_DEFAULT(&Revolution, 0); 	// ������ �� �����������

TPrtElem UvR = UV_DEFAULT(&Ram.GroupA.Ur, 0); 			// ���������� ���������� 
TPrtElem UvS = UV_DEFAULT(&Ram.GroupA.Us, 1);
TPrtElem UvT = UV_DEFAULT(&Ram.GroupA.Ut, 2);

TPrtElem UvR_Def = UVD_DEFAULT(&Ram.GroupA.Ur, 0); 		// ���������� ���������� �������������
TPrtElem UvS_Def = UVD_DEFAULT(&Ram.GroupA.Us, 1);
TPrtElem UvT_Def = UVD_DEFAULT(&Ram.GroupA.Ut, 2);

TPrtElem OvR = OV_DEFAULT(&Ram.GroupA.Ur, 3); 			// ���������� ����������
TPrtElem OvS = OV_DEFAULT(&Ram.GroupA.Us, 4);
TPrtElem OvT = OV_DEFAULT(&Ram.GroupA.Ut, 5);

TPrtElem OvR_Def = OVD_DEFAULT(&Ram.GroupA.Ur, 3); 		// ���������� ���������� �������������
TPrtElem OvS_Def = OVD_DEFAULT(&Ram.GroupA.Us, 4);
TPrtElem OvT_Def = OVD_DEFAULT(&Ram.GroupA.Ut, 5); 

TPrtElem OvR_max = OV_MAX_DEFAULT(&Ram.GroupA.Ur, 11); 	// ���������� ���������� 47%
TPrtElem OvS_max = OV_MAX_DEFAULT(&Ram.GroupA.Us, 12);
TPrtElem OvT_max = OV_MAX_DEFAULT(&Ram.GroupA.Ut, 13);

TPrtElem Vsk = VSK_DEFAULT(&Ram.GroupH.VSkValue, 7); 	// �������� ����������

TPrtElem BvR = BV_DEFAULT(&Ram.GroupA.Ur, 8); 			// ����� ���
TPrtElem BvS = BV_DEFAULT(&Ram.GroupA.Us, 9);
TPrtElem BvT = BV_DEFAULT(&Ram.GroupA.Ut, 10);

TPrtElem PhlU = LI_DEFAULT(&Ipr[0], 0);					// ���������� ��� 
TPrtElem PhlV = LI_DEFAULT(&Ipr[1], 1);
TPrtElem PhlW = LI_DEFAULT(&Ipr[2], 2);
/*
TPrtElem I2tMin = IT_DEFAULT(&Imidpr, 0, 3);			// ����� ������� ������ 1 �������
TPrtElem I2tMid = IT_DEFAULT(&Imidpr, 1, 3);			// 2 �������
TPrtElem I2tMax = IT_DEFAULT(&Imidpr, 2, 3);			// 3 �������*/

//TPrtElem IUnLoad = IUL_DEFAULT(&Imidpr, 10);			// ����������� ������� (������� ��������)
TPrtElem ISkew   = ISK_DEFAULT(&Ram.GroupH.ISkewValue, 11); 	// �������� �����

TPrtElem Th = OT_DEFAULT(&Ram.GroupA.Temper, 5);		// ������ �� ���������
TPrtElem Th_Err = OTE_DEFAULT(&Ram.GroupA.Temper, 11);	// ������ �� ���������
TPrtElem Tl = UT_DEFAULT(&Ram.GroupA.Temper, 6);		// ������ �� "����������"	


TPrtElem DrvT = DT_DEFAULT(&Ram.GroupC.DrvTInput , 6);
//LOG_INPUT DrvT = DT_DEFAULT(6, False);					// ������ �� ��������� ��������� - ���������� �������

TFltUnion Faults;
TFltUnion Defects;// �������������
 
Bool  onlyPosSens = false;		// ����, ����������, ��� ���� ������� ��������� - ������������ ������
Uns   ShCLevel       = 32767; 	// ������������ ��� ����� ���������� ������� ������������ ������ �� ��
Uns   MuffFlag       = 0;	  	// ���� ������������ �����
Int   EngPhOrdValue  = 0; 		// ���������� ����������� ��� ��� ����������� �������� (0 ������, 1 ������, -1 �����)

Uns   OverWayFlag 	 = 0; 		// ���� ��������������� ���������� 
Uns   OtTime         = 0;
Uns   UtTime         = 0;

Uns   DrvTTout   	 	= (Uns)DRV_TEMPER_TOUT;
Uns   Fault_Delay 	 	= (Uns)FLT_DEF_DELAY;
Int   HighTemper		= 110;
Uns   VskTimer 			= 0;
Uns   FlagEngPhOrd		= 0;
Uns   PhOrdTimer 		= 0;

Bool IsShcReset = false;			// ���� ������ ��

__inline void DefDriveFaults(void);	
__inline void ShCProtect(void);

void ProtectionsInit(void)	// ��������� ������������� ��� �����
{
	memset(&Faults, 		0,		sizeof(TFltUnion));	// ��������� ��� ��������� � �������� � 0 (��� ������)
	memset(&Defects, 		0,		sizeof(TFltUnion));	// ��������� ��� ��������� � �������� � 0 (��� ������)
	memset(&GrH->FaultsDev, 0,		sizeof(TDeviceReg));// ��������� ��� ��������� � �������� � 0 (��� ������)
	//!!!
	memset(&GrH->FaultsLoad, 0,		sizeof(TLoadReg));// ��������� ��� ��������� � �������� � 0 (��� ������)
	memset(&GrH->FaultsNet, 0,		sizeof(TNetReg));// ��������� ��� ��������� � �������� � 0 (��� ������)
 	PhOrdTimer = 0;
	EngPhOrdValue  = 0;



	//-------------������������ ����������-------------------------------------------
	i2tOverload.inputCurr 		= &Ram.GroupH.Imid;
	i2tOverload.nominalCurr 	= &Ram.GroupC.Inom;
	ProtectionI2T_Init(&i2tOverload, PRD_50HZ);
}

void ProtectionsUpdate(void)// ������������� ���������� � �������
{
	#if !BUR_M
	if(Sifu.Direction == SIFU_UP)
	{
		PhlU.Input = ToPtr(&Ipr[0]);	
		PhlW.Input = ToPtr(&Ipr[2]);
	}
	if(Sifu.Direction == SIFU_DOWN)
	{
		PhlU.Input = ToPtr(&Ipr[2]);	
		PhlW.Input = ToPtr(&Ipr[0]);
	}
	#endif

	ShCProtect();		//��������� ��
	DefDriveFaults();	//��������� ������� ������ ������

	GrC->DrvTInput = DRIVE_TEMPER - 10000;
	if(GrG->TestCamera && GrA->Faults.Proc.bit.Drv_T) GrA->Faults.Proc.bit.Drv_T = 0;
	if(GrG->TestCamera && GrA->Faults.Proc.bit.MuDuDef) GrA->Faults.Proc.bit.MuDuDef = 0;
	GrH->FaultsLoad.bit.I2t = i2tOverload.isFault;
}

void ProtectionsEnable(void)// �������� ��������� ����� ��
{
	static Byte State = 0;
	Bool Enable;

	if(Fault_Delay > 0) return;

	switch(++State)
	{
		case 1: 				// ��������� �� �����
				Muf.Enable = ((Dmc.WorkMode & wmMuffEnable) != 0) && !GrG->SimulSpeedMode && !ZazorTimer && !KickModeEnable && !GrG->TestCamera;
		break;
		case 2: 				// ��������� �� ����������
			
			#if BUR_M
				Enable =!IsStopped() && (GrC->Uv != pmOff) && !GrG->TestCamera;;
		//	else Enable = 0;
			#else
				Enable = (GrC->Uv != pmOff) && ((GrG->TestCamera && GrA->Status.bit.Power) == 0);
			#endif
				UvR.Cfg.bit.Enable = Enable;
				UvS.Cfg.bit.Enable = Enable;
				UvT.Cfg.bit.Enable = Enable;
				UvR_Def.Cfg.bit.Enable = Enable;
				UvS_Def.Cfg.bit.Enable = Enable;
				UvT_Def.Cfg.bit.Enable = Enable;

			#if BUR_M
				Enable =!IsStopped() && (GrC->Ov != pmOff);
			//	else Enable = 0;
			#else
				Enable = (GrC->Ov != pmOff);
			#endif
				OvR.Cfg.bit.Enable = Enable;
				OvS.Cfg.bit.Enable = Enable;
				OvT.Cfg.bit.Enable = Enable;
				OvR_Def.Cfg.bit.Enable = Enable;
				OvS_Def.Cfg.bit.Enable = Enable;
				OvT_Def.Cfg.bit.Enable = Enable;
			    OvR_max.Cfg.bit.Enable = Enable;
				OvS_max.Cfg.bit.Enable = Enable;
				OvT_max.Cfg.bit.Enable = Enable;
				
			#if BUR_M
		
				Vsk.Cfg.bit.Enable =!IsStopped() && (GrC->Ov != pmOff);
			//	else Vsk.Cfg.bit.Enable = 0;
			#else	
				Vsk.Cfg.bit.Enable = (GrC->VSk != pmOff);
			#endif
	
			#if BUR_M
				if(GrG->TestCamera)
				{
					Enable = 0;
				}
				else
				{
					if (BreakVoltFlag) Enable = (GrC->Bv != pmOff);
					else Enable = !IsStopped() && (GrC->Bv != pmOff);
				}
			#else
				Enable = (GrC->Bv != pmOff)&& ((GrG->TestCamera && GrA->Status.bit.Power) == 0);
			#endif
				BvR.Cfg.bit.Enable = Enable;
				BvS.Cfg.bit.Enable = Enable;
				BvT.Cfg.bit.Enable = Enable;
		break;
		case 3: 				// ��������� �� ����
				#if BUR_M

				if(GrG->TestCamera) Enable = 0;
				else
				{
					Enable = !IsStopped() && (GrC->Phl != pmOff);
				}
				#else
				Enable = !IsStopped() && (GrC->Phl != pmOff)&& !(GrG->TestCamera && (IsTestMode()));
				#endif
				PhlU.Cfg.bit.Enable = Enable;
				PhlV.Cfg.bit.Enable = Enable;
				PhlW.Cfg.bit.Enable = Enable;
		
				i2tOverload.enable 	   = !IsStopped() && (GrC->I2t != pmOff);
	//			IUnLoad.Cfg.bit.Enable = !IsStopped() && (GrC->IUnLoad != pmOff);
				ISkew.Cfg.bit.Enable   = !IsStopped() && (GrC->ISkew   != pmOff);

		break;
		case 4: 				// ��������� �� �����������
				Enable = (GrC->TemperTrack != pmOff);
				Th.Cfg.bit.Enable = Enable;
				Th_Err.Cfg.bit.Enable = Enable;
				Tl.Cfg.bit.Enable = Enable;

				Enable = (GrC->DriveTemper != pmOff) && !GrG->TestCamera;
				DrvT.Cfg.bit.Enable = Enable;
				State = 0;		// ��������� � �������
		break;
	}

	if (IsShcReset && IsMemParReady())
	{
		IsShcReset = false;
		GrH->ScFaults = 0;
		WritePar(REG_SC_FAULTS, &GrH->ScFaults, 1);
	}
}

void ProtectionsClear(void)	// ������ �������� ������ (������� ������ �������� � ���������)
{
	OverWayFlag = 0;
	MuffFlag = 0;

	GrA->Status.all &= ~STATUS_RESET_MASK;			//�������� ������ � �����

	GrA->Faults.Proc.all 	= 0;					//�������� ������ �������� 
	GrH->FaultsNet.all      = 0;					//�������� ������ ���� (U)
	GrH->FaultsLoad.all     = 0;					//�������� ������ �������� (I)
	GrH->FaultsDev.all     	= 0;					//�������� ������ ����������
	
	Encoder.Error  = False;
	enDPMA15.Error = false;
	Eeprom1.Error  = False;
	Eeprom2.Error  = False; 
	TempSens.Error = False;

	IsShcReset = true;
	i2tOverload.isFault = false;
	Encoder.skipDefectFl = 0;

}

void ProtectionsReset(void)	// ����� ������� �����(��� ���������� ����� ���������)
{
	OverWayFlag = 0;
	MuffFlag = 0;

	GrA->Status.all &= ~STATUS_RESET_MASK;	
	
	GrA->Faults.Proc.all &= ~PROC_RESET_MASK;	// �������� ������ �������� (��� ������, ������������ ����������� ���, ���������� �� ����������) 
	GrH->FaultsLoad.all  &= ~LOAD_RESET_MASK;	// �������� ������ �������� (���)
	#if BUR_M
	GrH->FaultsNet.all   &= ~NET_RESET_MASK;					
	#endif
}

void ProtectionsControl(void)	// ��� ������� ����������
{
	register Uns Dif1, Dif2, Dif3, Tmp;
	#if BUR_M
	if (IsStopped()){
		GrH->VSkValue = 0;
		VskTimer = 0;}
	else
	{
		if (VskTimer < VSK_TIME)
		{
			VskTimer++;
		}
		else
		{
			Dif1 = abs(GrA->Ur - GrH->Umid);			//������:   Ur = 222  Umid = 220 Dif1 = 2
			Dif2 = abs(GrA->Us - GrH->Umid);			//			Us = 218			 Dif2 = 2
			Dif3 = abs(GrA->Ut - GrH->Umid);			//			Ut = 220			 Dif3 = 0
			Tmp  = Max3UnsValue(Dif1, Dif2, Dif3);		//������� ������������ ��������, ��� ������ ����� ������. ��� = 2			
			GrH->VSkValue = ValueToPU0(Tmp, GrH->Umid);	// (2/220)*100 = 0.9% ��������
			
		}
	}
	#else
	// ������ �������� ����� ��� ������ �� ��������� ������� ����������
	Dif1 = abs(GrA->Ur - GrH->Umid);			//������:   Ur = 222  Umid = 220 Dif1 = 2
	Dif2 = abs(GrA->Us - GrH->Umid);			//			Us = 160			 Dif2 = 2
	Dif3 = abs(GrA->Ut - GrH->Umid);			//			Ut = 220			 Dif3 = 0
	Tmp  = Max3UnsValue(Dif1, Dif2, Dif3);		//������� ������������ ��������, ��� ������ ����� ������. ��� = 2			
	GrH->VSkValue = ValueToPU0(Tmp, GrH->Umid);	// (2/220)*100 = 0.9% ��������
	#endif

	if (IsStopped()) 							// ���� ��. �������� �� �� �������
		GrH->ISkewValue = 0;
	else										// �����
	{
		Dif1 = abs(Ipr[0] - Imidpr);			// ��� � �����������
		Dif2 = abs(Ipr[1] - Imidpr);
		Dif3 = abs(Ipr[2] - Imidpr);
		Tmp  = Max3UnsValue(Dif1, Dif2, Dif3);
		GrH->ISkewValue = ValueToPU0(Tmp, Imidpr);
	}
}

void FaultIndication(void)				// ��������� ������ ���������� � ���������������� ��������			
{
	static Uns 	tempMotorTimer = 0;

	if(Fault_Delay > 0) {Fault_Delay--; return;}

	GrA->Faults.Proc.bit.Overway = OverWayFlag;	// ���� ���������� ������������
	
	GrA->Faults.Proc.all &= ~PROC_CALIB_MASK;	// �������� ������ ����������
	if (GrC->CalibIndic != pmOff)				// ���� ������ �� ���. �������� �� ����������
	{
		GrA->Faults.Proc.bit.NoClose = IsNoClosePos();	// ��������� ��������� ���������� �� "�������"
		GrA->Faults.Proc.bit.NoOpen  = IsNoOpenPos();		// ��������� ��������� ���������� �� "�������"
		GrA->Faults.Proc.bit.NoCalib = IsNoCalib();		// ��������� ��������� ����������
	}
	
	#if BUR_M
	GrH->FaultsNet.bit.PhOrd = 0;	// ���� ��� � �� ��� ����� ������
	#else

//	GrH->FaultsNet.bit.PhOrd = !PhEl.Direction;				// ������ ����� ���� ����������� ��� ���� �� �����������
	GrH->FaultsNet.bit.PhOrd = 0;	
	#endif

	GrH->FaultsDev.all &= ~DEV_ERR_MASK;					// �������� ������ ����������

	#if BUR_90
	// ��������� ������ �� ��������� ��������� ��� ���_90
	if (!PiData.Connect)					// ���� ��� ����� � ��
	{
		PrtCalc(&DrvT);						// ������ �������� �� ������������� ������
	}										// �����
	else if ( (GrC->DriveTemper != pmOff) && !GrG->TestCamera )
	{										// ������� �� ���������� � �� ������
		if (ExtReg >> SBEXT_TEMPER_M)		// ���� � �� ������ ������ � ��������� ���������
		{
			if (tempMotorTimer++ > PRD_50HZ)
			{								// .. �� ���������� ������ ��� ������������ �������
				tempMotorTimer = 0;
				GrA->Faults.Proc.bit.Drv_T = 1;
			}
		}
		else tempMotorTimer = 0;
	}
	else GrA->Faults.Proc.bit.Drv_T = 0;
	#endif

	if (GrC->ErrIndic != pmOff)							// ���� ��������� ������ �� ���������
	{
		GrH->FaultsDev.bit.Memory1 = Eeprom1.Error;			// ������ 1 
		GrH->FaultsDev.bit.Memory2 = Eeprom2.Error;			// eeprom 2
		if(!GrC->RTCErrOff)
		{
			GrH->FaultsDev.bit.Rtc     = Ds3234.Error;			// ����
		}
		GrH->FaultsDev.bit.TSens   = TempSens.Error;		// �����������
		GrH->FaultsDev.bit.AVRcon  = !PiData.Connect;		// ������� ������� �� ���
		/*
		#if !BUR_M
		GrH->FaultsDev.bit.LowPower = !PowerEnable;			// ������� ����� ���������� �������
		#endif
		*/
	}
	if (GrC->PosSensEnable != pmOff)
	{
		if(GrC->EncoderType == 0)
		{
			GrH->FaultsDev.bit.PosSens = Encoder.Error;			// �������
		}
		else
		{
			GrH->FaultsDev.bit.PosSens = enDPMA15.Error;			// �������
		}
	}

//	#if !BUR_M
//	GrH->FaultsDev.bit.LowPower = !PowerEnable;				// ������� ����� ���������� �������
//	#endif
}

__inline void DefDriveFaults(void)		// ������� �� ������, ��������
{
	Uns MufEnable;
	
	GrA->Status.bit.Fault = IsFaultExist(pmSignStop);	// ���� ��������� ������������ ������ � ��� ��������
	GrA->Status.bit.Defect = IsDefectExist(pmSignStop); // ���������� � ������ ������ ������

	if (!IsStopped())					// ���� �� ������������
	{
		if (MuffFlag)					// ���� ������ ������ � �����
		{
			if (IsNoCalib()) MufEnable = !Calib.AutoStep;	// ���� �� �������������� (���� ���� �������������� �� ������� 0 � ������ �� ����� ����� ������ 1 � �������� ������)
			else if (!Dmc.RequestDir) MufEnable = True;		// ���� ��� �������� � �� ����������� �����������
			else if (Dmc.RequestDir > 0) MufEnable = !IsOpened();// ���� ����������� �������� �� ��������� � ��������
			else MufEnable = !IsClosed();	//	����������
			
		
		if (Mcu.Valve.BreakFlag && BreakFlag)// ���������� ����� � ������ �� ���� ������� ����� ���� �����������
			{
				MufEnable = 0;
			}

		GrA->Faults.Proc.bit.NoMove = MufEnable;// ���������� ������ �� ��������� �������� ��� ������������ ����������� ��������
		GrA->Status.bit.Mufta = MufEnable;// ���������� � ������ ��� ��������� �����
			
	
			ValveDriveStop(&Mcu, True);//���� ������� �� ��������� (��� ��������)

			if (Mcu.EvLog.Value) Mcu.EvLog.QueryValue = CMD_DEFSTOP;
			else Mcu.EvLog.Value = CMD_DEFSTOP;	//???  ������������� ���
		}

		if (IsFaulted())
		{
			if (!onlyPosSens || GrC->PosSensEnable == pmSignStop)
			{
				ValveDriveStop(&Mcu, True);	// ���� � ������� ��������� ������ �� ���� ������� �� ��������� 
				if (Mcu.EvLog.Value) Mcu.EvLog.QueryValue = CMD_DEFSTOP;
				else Mcu.EvLog.Value = CMD_DEFSTOP;
			}
		}

		if (IsDefected())
		{
		//	ValveDriveStop(&Mcu, True);	// ���� � ������� ��������� ������ �� ���� ������� �� ��������� 
		//	if (Mcu.EvLog.Value) Mcu.EvLog.QueryValue = CMD_DEFSTOP;
		//	else Mcu.EvLog.Value = CMD_DEFSTOP;
		}
		
	}
}

Bool IsDefectExist(TPrtMode Mode) // �������������
{
	Defects.Proc.all = GrA->Faults.Proc.all; 	//�������� �������� �������� � ��������   
	Defects.Load.all = GrH->FaultsLoad.all;  
	Defects.Dev.all  = GrH->FaultsDev.all; 

	Defects.Net.bit.UvR = GrH->DefectsNet.bit.UvR; 
	Defects.Net.bit.UvS = GrH->DefectsNet.bit.UvS; 
	Defects.Net.bit.UvT = GrH->DefectsNet.bit.UvT;
	Defects.Net.bit.OvR = GrH->DefectsNet.bit.OvR; 
	Defects.Net.bit.OvR = GrH->DefectsNet.bit.OvS; 
	Defects.Net.bit.OvR = GrH->DefectsNet.bit.OvT; 
	Defects.Net.bit.VSk = GrH->FaultsNet.bit.VSk; 


	if(Fault_Delay > 0) return false;
	// ������� �� ��� �������� �������
	if(Defects.Proc.bit.Drv_T)		Defects.Proc.bit.Drv_T 	 			 = 0;	// �������� ��
	if(Defects.Proc.bit.NoMove)		Defects.Proc.bit.NoMove  			 = 0;	// ��� ��������
	if(Defects.Proc.bit.CycleMode)  Defects.Proc.bit.CycleMode			 = 0;
	if(Defects.Proc.bit.PhOrd)		Defects.Proc.bit.PhOrd 	 			 = 0;
	if(Defects.Dev.bit.PosSens)		Defects.Dev.bit.PosSens 	 		 = 0;
	if(Defects.Net.all  & NET_BV_MASK)			Defects.Net.all 		&=~NET_BV_MASK;		// ����� �� ���
	if(Defects.Load.all & LOAD_SHC_MASK)		Defects.Load.all 		&=~LOAD_SHC_MASK;	// ��
	if(Defects.Load.all & LOAD_I2T_MASK)		Defects.Load.all 		&=~LOAD_I2T_MASK;	// ���
 
	if(GrC->CalibIndic 		< Mode)	Defects.Proc.all 		&=~PROC_CALIB_MASK;	// ������� ����������
	if(GrC->MuDuDef 		< Mode) Defects.Proc.bit.MuDuDef = 0;   // ������ ������� ������ ��/��

	if(pmBlkTsSign   		< Mode) Defects.Net.all 			&=~NET_UV_MASK;		// ���������� ���������� 
	if(pmBlkTsSign    		< Mode) Defects.Net.all 			&=~NET_OV_MASK;		// ���������� 31% - ������ ������������
	if(GrC->VSk     		< Mode) Defects.Net.all 			&=~NET_VSK_MASK;	// �������� ����������

	if(GrC->Phl				< Mode) Defects.Load.all 		&=~LOAD_PHL_MASK; 	// ����� �� 
  //	if(GrC->IUnLoad			< Mode) Defects.Load.all 		&=~LOAD_UNL_MASK;	// ���������� ���
	if(GrC->ISkew			< Mode) Defects.Load.all 		&=~LOAD_ISK_MASK;	// �������� �����

	if(GrC->TemperTrack		< Mode) Defects.Dev.all 			&=~DEV_TMP_MASK;	// ��������, ���������� ����� - �������

	if(GrC->ErrIndic		< Mode) Defects.Dev.all 			&=~DEV_ERR_MASK;	// ������������� ����� ����������

	if(Defects.Proc.all) 			 		return true;	// ���� �������� � ��������� ���������� ���
	if(Defects.Net.all)  			 		return true;
	if(Defects.Load.all) 			 		return true;
	if(Defects.Dev.all) 					return true;	// ���������

	return false;						// ���� ��������� ��� �� ��������� ���������� ����

}

Bool IsFaultExist(TPrtMode Mode) // ������������ � ���������� ���������
{
// Mode - ��� ��������, ������� ���������� ��� ����� ������ ��� ������������� �������
// �������� - ������ -0, �������� ��������� � �������� �� � ������� � ���� - 3 1-������ ��������� 2-��������� � ��
// �������� �� ������������ ����������� ��� ���� ����������� ������

	//Faults.Proc.all = GrA->Faults.Proc.all; 	//�������� �������� �������� � ��������
	Faults.Net.all  = GrH->FaultsNet.all;        
	Faults.Load.all = GrH->FaultsLoad.all;  

	Faults.Proc.bit.Drv_T  = 	GrA->Faults.Proc.bit.Drv_T;
	Faults.Proc.bit.NoMove =	GrA->Faults.Proc.bit.NoMove;
	Faults.Proc.bit.PhOrd  = 	GrA->Faults.Proc.bit.PhOrd;

	Faults.Dev.bit.Th_Err =  	GrH->FaultsDev.bit.Th_Err;
	Faults.Dev.bit.PosSens =  	GrH->FaultsDev.bit.PosSens;

	if(Fault_Delay > 0) return false;
 
switch(GrB->SettingPlace)
{
	case spLinAuto:

		if(GrC->DriveTemper		< Mode)	Faults.Proc.bit.Drv_T 	 = 0;	// �������� ������ �� ��������� ��
		if(GrC->PhOrd			< Mode)	Faults.Proc.bit.PhOrd 	 = 0;	// ����������� �������� �� - �������

		if(GrC->Bv   			< Mode) Faults.Net.all 			&=~NET_BV_MASK;
		if(GrC->Ov    			< Mode) Faults.Net.all 			&=~NET_OV_MASK;		// ���������� 31%
		if(GrC->Ov    			< Mode) Faults.Net.all 		    &=~NET_OV_MAX_MASK;	// ���������� 47% - �������
		if(GrC->Uv   			< Mode) Faults.Net.all 			&=~NET_UV_MASK;		// ���������� ���������� 
		#if BUR_M
		if(GrC->RST_Err			< Mode) Faults.Net.bit.RST_Err   = 0;
		#endif
		if(GrC->I2t				< Mode) Faults.Load.all 		&=~LOAD_I2T_MASK;
		if(GrC->ShC				< Mode) Faults.Load.all 		&=~LOAD_SHC_MASK;
		if(pmSignStop			< Mode) Faults.Dev.bit.Th_Err 	 = 0;	// ��������,  - �������
	
	 break;
	case spFire: 
	
		if(pmBlkTsSign			< Mode)	Faults.Proc.bit.Drv_T 	 = 0;	// �������� ������ �� ��������� ��
		if(pmBlkTsSign			< Mode)	Faults.Proc.bit.PhOrd 	 = 0;	// ����������� �������� �� - �������

		if(pmBlkTsSign   		< Mode) Faults.Net.all 			&=~NET_BV_MASK;
		if(pmBlkTsSign    		< Mode) Faults.Net.all 			&=~NET_OV_MASK;		// ���������� 31%
		if(pmBlkTsSign    		< Mode) Faults.Net.all 		    &=~NET_OV_MAX_MASK;	// ���������� 47% - �������
		if(pmBlkTsSign   		< Mode) Faults.Net.all 			&=~NET_UV_MASK;		// ���������� ���������� 
		#if BUR_M
		if(pmBlkTsSign			< Mode) Faults.Net.bit.RST_Err   = 0;
		#endif
		if(pmBlkTsSign			< Mode) Faults.Load.all 		&=~LOAD_I2T_MASK;
		if(GrC->ShC				< Mode) Faults.Load.all 		&=~LOAD_SHC_MASK;
		if(pmBlkTsSign			< Mode) Faults.Dev.bit.Th_Err 	 = 0;	// ��������,  - ������������	

	break;
}

	if(Faults.Proc.bit.Drv_T)				return true;	// �������� ��
	if(Faults.Proc.bit.PhOrd)				return true;	// �������� ����������� ��� ���������
	if(Faults.Proc.bit.NoMove)				return true;	// ��� ��������
	if(Faults.Net.all  & NET_BV_MASK)		return true;	// ����� �� ���
	if(Faults.Net.all  & NET_UV_MASK)		return true;	// ����������� -50%
	if(Faults.Net.all  & NET_OV_MASK)		return true;	// ������������ ���������� +31%
	if(Faults.Net.all  & NET_OV_MAX_MASK)	return true;	// ������������ +47%
	#if BUR_M
	if(Faults.Net.all  & NET_RTSERR_MASK)	return true;	// �������� ����������� ��� ����
	#endif
	if(Faults.Load.all & LOAD_SHC_MASK)		return true;	// ��
	if(Faults.Load.all & LOAD_I2T_MASK)		return true;	// ���
	if(Faults.Dev.all)
	{
		if (Faults.Dev.all == 1) onlyPosSens = true;
		return true;	// ����������� 110���� � ���� ������� ���������
	}

	return false;						// ���� ��������� ��� �� ��������� ���������� ����

}
__inline void ShCProtect(void)			// �������� �� �� 
{
	Uns adcU, adcV, adcW;

	if((Fault_Delay > 0)||(!GrC->ShC)) return;

	adcU = abs(ADC_IU - GrH->IU_Offset);
	adcV = abs(ADC_IV - GrH->IV_Offset);
	adcW = abs(ADC_IW - GrH->IW_Offset);
	
	if (adcU > GrC->ShC_Level)	GrA->Faults.Load.bit.ShCU = 1;
	if (adcV > GrC->ShC_Level)	GrA->Faults.Load.bit.ShCV = 1;
	if (adcW > GrC->ShC_Level)	GrA->Faults.Load.bit.ShCW = 1;

/*
	if(GrC->ShC)
	{
		if ((ADC_IU < GrC->ShC_Down)||(ADC_IU > GrC->ShC_Up))
			{
				GrH->FaultsLoad.bit.ShCU = 1;
			}
		if ((ADC_IV < GrC->ShC_Down)||(ADC_IV > GrC->ShC_Up))
			{
				GrH->FaultsLoad.bit.ShCV = 1;
			}
		if ((ADC_IW < GrC->ShC_Down)||(ADC_IW > GrC->ShC_Up))
			{
				GrH->FaultsLoad.bit.ShCW = 1;
			}	
	}	*/
}
			
void EngPhOrdPrt(void)					// �������� �� ������������ ����������� ��� (����������� ��� ���������)
{
	          // ���� ��� ��������� �������� ������������ ����������� ����
	static LgUns StartPos;
	static Uns   Timer = 0;
	static Uns   PhOrdZone = 10;
    LgInt data = (RevMax + 1)/2;
	LgInt  Delta = 0;

//	if(Fault_Delay>0) return;

 	 #if BUR_M
	GrC->PhOrd = pmSignStop;
	if (IsStopped())	// ���� ����� �� �������, ������ ��������
	{
		FlagEngPhOrd = False;					// �������� ���� ���� ��������� ������������� ��� ������������� �� ������ �����������
		StartPos = Revolution;	// ��������� ��������� ��� ������� ��������� � ��������
		Timer = 0;						// �������� ������
		PhOrdTimer = 0;
		return;							// �����
	}

	 #else 
	if ((GrC->PhOrd == pmOff) || IsStopped())	// ���� ������ �� ������. �����. ��� ��������� ��� ��� ������������ ��� �������������� 
	{
		FlagEngPhOrd = False;					// �������� ���� ���� ��������� ������������� ��� ������������� �� ������ �����������
		StartPos = Encoder.Revolution;	// ��������� ��������� ��� ������� ��������� � ��������
		Timer = 0;						// �������� ������
		return;							// �����
	}
	 #endif

	if (Timer < (GrC->PhOrdTime * (Uns)PRT_SCALE)) Timer++;	// ������� ������� � 0.1 � � ���� ������ ������ ����������������	
	else if (!FlagEngPhOrd && !ZazorTimer)								// ���� �� ���� � ������ ������ = 0 ��
	{												// ��� REV_MAX ������������ ��������� �������� ��������
		Delta = Revolution - StartPos;				// ��������� ������ �������� ��� ������� ��������� ����� ���������� ��������� ���������

		if (GrC->EncoderType == 0)
		{
			if (Delta > ((RevMax + 1) / 2))
				Delta -= (RevMax + 1);// ���� ������ ������ 2000 �� �������������� ������ �� 4000
			if (Delta < -((RevMax + 1) / 2))
				Delta += (RevMax + 1);// ���� ������ ������ -2000 �� �������������� ������ �� 4000
		}
		else if (GrC->EncoderType == 1)
		{
			if (Delta > data)
				Delta -= (RevMax + 1);// ���� ������ ������ 2000 �� �������������� ������ �� 4000
			if (Delta < -data)
				Delta += (RevMax + 1);// ���� ������ ������ -2000 �� �������������� ������ �� 4000
		}


		EngPhOrdValue = 0;	// ���������� ����������� ���, ��������� ����� ���� �������� ������
	
		if (Delta >=  ((LgInt)PhOrdZone)) EngPhOrdValue =  1; // ���� ������ ������ ���������� ����������� ��� �� ����������� ��� �� ������ ����������� ��������
		if (Delta <= -((LgInt)PhOrdZone)) EngPhOrdValue = -1; // ���� ������ ������ ���������� ����������� ��� �� ����������� ��� �� ��������� ����������� ��������
		
		if (GrB->RodType == rdtInverse) EngPhOrdValue = -EngPhOrdValue; // ���� ��� ����� ��������� �� �������������
		
		if ((EngPhOrdValue != 0) && (EngPhOrdValue != Dmc.RequestDir))		// ���� �������� ���� �� ���������� �������� �� ��������� � �������� �� 
			GrA->Faults.Proc.bit.PhOrd = 1;								// ���������� ������ ����������� ��� (���. �������)

		#if BUR_M

		if ((++PhOrdTimer >= 200) || (EngPhOrdValue != 0))
		{
			PhOrdTimer = 0;
			FlagEngPhOrd = 1;
		}

		#else
		if ((++PhOrdTimer >= 200) || (EngPhOrdValue != 0))
		{
			PhOrdTimer = 0;
			FlagEngPhOrd = 1;
		}			
	//	Flag = True;	// ���������� ���� �������� ������ � ���, ���������� ��� �������� ����������� ��� ����������� �� ����������
		#endif
	}
}

