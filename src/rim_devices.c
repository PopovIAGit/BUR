/*--------------------------
	rim_devices.c

	������ ���������
--------------------------*/

#include "config.h"

AT25XXX  Eeprom1   = AT25XXX_DEFAULT1;
AT25XXX  Eeprom2   = AT25XXX_DEFAULT2;
TDisplay Display   = DISPLAY_DEFAULT;
DAC7513  Dac       = DAC7513_DEFAULT;
ENCODER  Encoder   = ENCODER_DEFAULT;
ADT7301  TempSens  = ADT7301_DEFAULT;
DS1305   Ds1305    = DS1305_DEFAULT;
RTC_Obj  Rtc       = RTC_DEFAULT;
TPult 	 Pult 	   = PULT_DEFAULTS;
LOG_INPUT BtnOpen   	= BTN_DEFAULT(0, False);	// ���� true
LOG_INPUT BtnClose  	= BTN_DEFAULT(1, False);	// ���� true
LOG_INPUT BtnStop_MU    = BTN_DEFAULT(2, False);// �������� ���� �������� �� ���
LOG_INPUT BtnStop_DU    = BTN_DEFAULT(3, False);
LOG_INPUT TuOpen  = TU_DEFAULT(&ExtReg,  SBEXT_OPEN,   0);
LOG_INPUT TuClose = TU_DEFAULT(&ExtReg,  SBEXT_CLOSE,  1);
LOG_INPUT TuStop  = TU_DEFAULT(&ExtReg,  SBEXT_STOP,   2);
LOG_INPUT TuMu    = TU_DEFAULT2(&ExtReg, SBEXT_MU,     3);
LOG_INPUT TuDu    = TU_DEFAULT2(&ExtReg, SBEXT_DU,     4);


//����������
Uns BtnStatus = 0;
Uns BtnTout   = (Uns)BTN_TIME;
Uns BtnLevel  = BTN_LEVEL;
Uns ExtReg    = 0;
//--------
Bool RtcStart    = False;
Int  BlockTemper;
Uns  TemperTimer = 0;
bool ReadFlag = false;
Bool RamUpdFlag = false;
Bool TenEnable = False;
Byte RimRefrState = 0;
Uns ContTest = 0;
Uns ContTestTimer = 0;
Uns ContStopErrTimer = 0;

Byte DisplTesNum = 0;

__inline void DisplTest(void);
__inline void TenControl(void);
void SetPlisAddr(Uns Addr);


void RimDevicesInit(void)
{
	SetPlisAddr(CS_RESET);
	DelayUs(100);
	SetPlisAddr(CS_NONE);

	AT25XXX_Init(&Eeprom1);
	AT25XXX_Init(&Eeprom2);
	DS1305_Init(&Ds1305);

	DISPL_AddSymb(&Display,0,ToPtr(Icons), NUM_ICONS);

	GrG->DiscrInTest = 0;
	GrG->DiscrOutTest = 0;
	GrG->IsDiscrTestActive = 0;
	GrG->Mode = 0;
}

Bool RimDevicesRefresh(void)
{
	if (!RimRefrState) return true;

	Display.ResTout = GrC->DisplResTout * (Uns)DISPL_SCALE;			// ����� �������

	BtnOpen.Input   = ToPtr(&HALL_SENS1);	// �������� ������ ������������ �����
	BtnClose.Input  = ToPtr(&HALL_SENS2);
	BtnStop_MU.Input= ToPtr(&HALL_SENS4);
	BtnStop_DU.Input= ToPtr(&HALL_SENS3);

#if !BUR_M
	// ��� ����� �� �����������
	TuOpen.Level  = DIN_LEVEL(SBEXT_OPEN,  (Uns)GrB->InputMask.bit.Open);	// �������� ������ ��� ��������� ������ ��, 220/24 �� ������ 
	TuClose.Level = DIN_LEVEL(SBEXT_CLOSE, (Uns)GrB->InputMask.bit.Close);	// �������� 0/1. ������� ����������� � ����������� �� ���� ����� - ����������/�������������
	TuStop.Level  = DIN_LEVEL(SBEXT_STOP,  (Uns)GrB->InputMask.bit.Stop);   // ��������� ������ �������� �� ����� ����� SPI
	TuMu.Level    = DIN_LEVEL(SBEXT_MU,    (Uns)GrB->InputMask.bit.Mu);
	TuDu.Level    = DIN_LEVEL(SBEXT_DU,    (Uns)GrB->InputMask.bit.Du);
#endif
	Dac.Gain   = GrC->Dac_Mpy;
	Dac.Offset = GrC->Dac_Offset;

	RimRefrState = 0;
	return (!RimRefrState);
}

void ReadParams(void)
{
	ReadAllParams();
	while (!IsMemParReady()) { AT25XXX_Update(&MemPar);}				// ���� ���� ������� ��� ��������� �� ������																	// ������� ���������� ��� �������������, ������� ���������� ���������	
}

void EEPROM_Update(AT25XXX *Eeprom)
{
	AT25XXX_Update(Eeprom);
}


void EEPROM_Func(Byte Memory, Byte Func, 
	Uns Addr, Uns *Data, Uns Count)
{
	AT25XXX *Eeprom;
	
	switch (Memory)
	{
		case MEM_PAR: Eeprom = &Eeprom1; break;
		case MEM_LOG: Eeprom = &Eeprom2; break;
		default: return;
	}
	
	if (Eeprom->Error) return;									// ��� ������, ��������� ����������
	
	Eeprom->Addr   = Addr << 1;									// �������� ����� (�������� �� 2), �.�. ������ � ��������� 2 �����, � � ������ ������� ��������
	Eeprom->Buffer = ToPtr(Data);								// ��������� �� ������
	Eeprom->Count  = Count << 1;
	Eeprom->Func   = Func;
}

void SetPlisData(void)
{
	SPI_init(SPIA, SPI_MASTER, 0, PLIS_BRR, 8);

	SetPlisAddr(CS_LED);
	SPI_send(PLIS_SPI, Ram.GroupC.LedsReg.all);
	SetPlisAddr(CS_NONE);

	SetPlisAddr(CS_LCD_ON);
	SPI_send(PLIS_SPI, !LcdEnable);
	SetPlisAddr(CS_NONE);

}

void RimIndication(void)//	��������� ��� ���
{
		//if (Encoder.Error && !IsNoCalib()) // ���� ������ �������� � ��������������
	//	{Mcu.EvLog.Source = 0; GrD->CalibReset = 1;} // �������� �������� ������ ��� ������� � ���������� ����� ����������
	
	Dac.Mode = DAC7513_PR_4_20;
	if (IsTestMode())
		Dac.Value = &GrG->DacValue; // ���� ���
	else
	{
		Dac.Value = &GrA->PositionPr;		// ���� � ��� �������� � % �������� ���������
		if (IsNoCalib()) Dac.Mode = DAC7513_OFF;// ���� �� �������������� �� �� ������ �������� � ��� ��������
	}

	if (IsTestMode()) DisplTest();				// ���� ���� �� ���� �������
	
	GrA->Temper = BlockTemper + GrC->CorrTemper; // �������� ����������� � ������������ ��

	TenControl();	// ���������� �����
	
	if (RamUpdFlag && IsMemParReady())
	{
		WriteAllParams();
		RefreshData();
		RamUpdFlag = false;
	}

//	OUT_SET(RELAY_ENB, 0); ��� � ��� ������
}

void RtcControl(void)
{
	static Uns PrevTime;
	static Uns PrevDate;
	TTimeVar *DevTime = &GrB->DevTime;
	TDateVar *DevDate = &GrB->DevDate;
	
	if (Ds1305.Busy) return;
	
	if (RtcStart)
	{
		if (DevTime->all != PrevTime) 
			{RTC_SetTime(&Rtc, DevTime, 0); Ds1305.Flag = True;}
		if (DevDate->all != PrevDate) 
			{RTC_SetDate(&Rtc, DevDate, 1); Ds1305.Flag = True;}
		if (RTC_TimeCorr(&Rtc, GrB->TimeCorrection)) Ds1305.Flag = True;
	}

	if (!Ds1305.Flag) 
	{
		RTC_GetTime(&Rtc, DevTime);
		RTC_GetDate(&Rtc, DevDate);
		GrH->Seconds = Rtc.Sec;
	}

	PrevTime = DevTime->all;
	PrevDate = DevDate->all;
	RtcStart = True;
}


__inline void DisplTest(void)
{
	switch(DisplTesNum)
	{
		case 1:
			strcpy(Display.HiStr, "A����Ũ���������");
			strcpy(Display.LoStr, "���������������");
			break;
		case 2:
			strcpy(Display.HiStr, "����������������");
			strcpy(Display.LoStr, "����������������");
			break;
		case 3:
			strcpy(Display.HiStr, "�0123456789()!?&");
			strcpy(Display.LoStr, "��#~%^*_+-\\/<>'$");
			break;
		default:
			strcpy(Display.HiStr, "                ");
			strcpy(Display.LoStr, "                ");
			break;
	}
}
__inline void TenControl(void)	//  ���������� ����� (����������� ����)
{	
	if (TempSens.Error) TenEnable = False; // ���� ������ ������� ����������� �� �� �������� ���
	else if (GrA->Temper >= GrC->TenOffValue) 	TenEnable = False;
	else if (GrA->Temper <= GrC->TenOnValue)	TenEnable = True;

	if (TempSens.Error) TenEnable = False;

	GrA->Status.bit.Ten = TenEnable;

	if (PowerEnable) OUT_SET(TEN_OFF, TenEnable); //  ��� ����������� 0
}


void SetPlisAddr(Uns Addr)
{
	Uint32 Mask = 0;

	// ��� �������� Avago � ������ ATMega
	if (Addr == CS_ENC)
		GpioDataRegs.GPADAT.bit.GPIO8 = 0;
	else
		GpioDataRegs.GPADAT.bit.GPIO8 = 1;
  
	if (Addr & 0x01) Mask |= CS0;
	if (Addr & 0x02) Mask |= CS4;
	if (Addr & 0x04) Mask |= CS2;
	if (Addr & 0x08) Mask |= CS3;

	CS_PORT = (CS_PORT & (~CS_MASK)) | Mask;
}

void DiscrInOutTestObserver(void)
{
// ������������� ������ ������ ����� ���������
	if ((GrG->DiscrInTest == 1) && (GrG->DiscrOutTest == 1))
	{
		GrG->DiscrInTest = 0;
		GrG->DiscrOutTest = 0;

		return;
	}

	if (!GrG->IsDiscrTestActive)
	{
		if (GrG->DiscrInTest || GrG->DiscrOutTest)
		{
			GrG->Mode = 1;
			// �������� ������� ��/�� ����� ������
			GrH->Outputs.all = 0;
			RamTek.MainGroup.TsTu.all = 0;
		}
	}
	else
	{
		if (GrG->Mode == 0)
		{
			GrG->DiscrInTest = 0;
			GrG->DiscrOutTest = 0;
		}
	}
	
	GrG->IsDiscrTestActive = (GrG->DiscrInTest || GrG->DiscrOutTest);
}

void TekModbusParamsUpdate(void)
{
	TTEK_MB_GROUP *tek = &RamTek.MainGroup;
	
	// ��������� ��������������� �������
	tek->TechReg.bit.Opened  = GrA->Status.bit.Opened;
	tek->TechReg.bit.Closed  = GrA->Status.bit.Closed;
	tek->TechReg.bit.Mufta1  = GrA->Status.bit.Mufta;
	tek->TechReg.bit.Mufta2  = GrA->Status.bit.Mufta;
	tek->TechReg.bit.MuDu    = !GrA->Status.bit.MuDu;
	tek->TechReg.bit.Opening = GrA->Status.bit.Opening;
	tek->TechReg.bit.Closing = GrA->Status.bit.Closing;
	tek->TechReg.bit.Stop    = GrA->Status.bit.Stop;
	tek->TechReg.bit.Ten     = GrA->Status.bit.Ten;
	
	// ��������� ������� ��������
	tek->DefReg.bit.I2t = GrA->Faults.Load.bit.I2t;
	tek->DefReg.bit.ShC = (GrA->Faults.Load.bit.ShCU || GrA->Faults.Load.bit.ShCV || GrA->Faults.Load.bit.ShCW); 
	tek->DefReg.bit.Drv_T = GrA->Faults.Proc.bit.Drv_T;
	tek->DefReg.bit.Uv = (GrA->Faults.Net.bit.UvR || GrA->Faults.Net.bit.UvS || GrA->Faults.Net.bit.UvT);
	tek->DefReg.bit.Phl = (GrA->Faults.Load.bit.PhlU || GrA->Faults.Load.bit.PhlV || GrA->Faults.Load.bit.PhlW);
	tek->DefReg.bit.NoMove = GrA->Faults.Proc.bit.NoMove;
	tek->DefReg.bit.Ov = (GrA->Faults.Net.bit.OvR || GrA->Faults.Net.bit.OvS || GrA->Faults.Net.bit.OvT);
	tek->DefReg.bit.Bv = (GrA->Faults.Net.bit.BvR || GrA->Faults.Net.bit.BvS || GrA->Faults.Net.bit.BvT);
	tek->DefReg.bit.Th = GrA->Faults.Dev.bit.Th;
	tek->DefReg.bit.Tl = GrA->Faults.Dev.bit.Tl;
	tek->DefReg.bit.PhOrdU = GrA->Faults.Net.bit.PhOrd;
	tek->DefReg.bit.PhOrdDrv = GrA->Faults.Proc.bit.PhOrd;
	tek->DefReg.bit.DevDef 	 = ((GrA->Faults.Dev.all & TEK_DEVICE_FAULT_MASK) != 0);

	// ������� ������
	// ��� ������������ ����� �������, ���������� ���
	if (Mcu.ActiveControls & CMD_SRC_SERIAL)
	{
		if(tek->ComReg.bit.Stop)
		{
			GrD->ControlWord = vcwStop;

			tek->ComReg.bit.Stop = 0;
			tek->ComReg.bit.Open = 0;
			tek->ComReg.bit.Close = 0;
		}
		else if(tek->ComReg.bit.Open)
		{
			GrD->ControlWord = vcwOpen;

			tek->ComReg.bit.Stop = 0;
			tek->ComReg.bit.Open = 0;
			tek->ComReg.bit.Close = 0;
		}
		else if(tek->ComReg.bit.Close)
		{
			GrD->ControlWord = vcwClose;

			tek->ComReg.bit.Stop = 0;
			tek->ComReg.bit.Open = 0;
			tek->ComReg.bit.Close = 0;
		}
	}
	else
	{
		tek->ComReg.bit.Stop = 0;
		tek->ComReg.bit.Open = 0;
		tek->ComReg.bit.Close = 0;
	}

	if (tek->ComReg.bit.PrtReset)
	{
		GrD->PrtReset = 1;
		tek->ComReg.bit.PrtReset = 0;
	}

	if (tek->ComReg.bit.EnDiscrOutTest)
	{
		GrG->DiscrOutTest = 1;
	}
	else if (tek->ComReg.bit.DisDiscrOutTest)
	{
		if (GrG->DiscrOutTest == 1)
		{
			GrG->Mode = 0;	
			GrG->DiscrOutTest = 0;
			#if BUR_M
				ContTest = 0;
				ContTestTimer = 0;
				GrH->Outputs.bit.Dout3 = 0;
			#endif
		}
	}
	else if (tek->ComReg.bit.EnDiscrInTest)
	{
		GrG->DiscrInTest = 1;
	}
	else if (tek->ComReg.bit.DisDiscrInTest)
	{
		if (GrG->DiscrInTest == 1)
		{
			GrG->Mode = 0;	
			GrG->DiscrInTest = 0;
		}
	}

	// �� ������ ������ ���������� ������� ������
	if (tek->ComReg.all != 0)
		tek->ComReg.all = 0;

// ������ ������������ ��/��
/*
	if (tek->ComReg.bit.SetDu && IsStopped())
		{	
			Mcu.MuDuInput = 0;
			tek->ComReg.bit.SetDu = 0;
		}
	else if (tek->ComReg.bit.SetMu && IsStopped())
		{	
			Mcu.MuDuInput = 1;
			tek->ComReg.bit.SetMu = 0;	
		}
*/
	tek->PositionPr 	 = GrA->PositionPr;
	tek->CycleCnt 		 = GrH->CycleCnt;
	tek->Iu				 = GrA->Iu;
	tek->Ur				 = GrA->Ur;
	tek->Torque			 = GrA->Torque;
	tek->Speed			 = GrA->Speed;
	tek->RsStation		 = GrB->RsStation;

	//��������� ���������� ������ � �������
	tek->TsTu.bit.IsDiscrOutActive = (GrG->DiscrOutTest);
	tek->TsTu.bit.IsDiscrInActive = (GrG->DiscrInTest);

	#if !BUR_M
	tek->TsTu.bit.InOpen 	 = GrH->Inputs.bit.Open;
	tek->TsTu.bit.InClose	 = GrH->Inputs.bit.Close;
	tek->TsTu.bit.InStop 	 = GrH->Inputs.bit.Stop;
	tek->TsTu.bit.InMu 		 = GrH->Inputs.bit.Mu;
	tek->TsTu.bit.InDu 		 = GrH->Inputs.bit.Du;
	#else

		if((GrG->DiscrInTest)&&(IsTestMode()))
		{		
			if (tek->TsTu.bit.InOpen && tek->TsTu.bit.InClose)
			{
				tek->TsTu.bit.InOpen = 0;
				tek->TsTu.bit.InClose = 0;	
			}
			if (IsPowerOff() && tek->TsTu.bit.InOpen && !ContTest)
			{
				tek->TsTu.bit.InOpen = 0;
				Dmc.RequestDir = 1;
				ContTest = 1;
				if(!PhEl.Direction)
					{
						GrH->ContGroup = cgOpen;
					}
			}
		   if (IsPowerOff() && tek->TsTu.bit.InClose && !ContTest)
			{
				tek->TsTu.bit.InClose = 0;
				Dmc.RequestDir = -1;
				ContTest = 1;
				if(!PhEl.Direction)
					{
						GrH->ContGroup = cgClose;
					}
			} 
			
			if (ContTest && (ContTestTimer < CONT_TEST_TIME)) ContTestTimer++;
			else if (ContTestTimer >= CONT_TEST_TIME)
			 {
					if(IsPowerLost() || GrH->FaultsNet.bit.RST_Err)
					{		
						GrH->Outputs.bit.Dout3 = 1;	
					}
					GrH->ContGroup = cgStop;
						tek->TsTu.bit.InOpen = 0;
							tek->TsTu.bit.InClose = 0;
					ContTest = 0;
					Dmc.RequestDir = 0;	
					ContTestTimer = 0;
			 }
			 // ���� ��� ����� ���� ����������.
			 if (IsPowerOn() && !ContTest) ContStopErrTimer++;
			 else if (ContStopErrTimer >= CONT_TEST_TIME) 
			 {
			 		GrH->Outputs.bit.Dout3 = 1;	
					ContStopErrTimer = 0;
			 }
		}
	#endif

	if (!GrG->DiscrOutTest)
	{
		#if BUR_M
		tek->TsTu.bit.OutOpened = GrH->Outputs.bit.Dout7;
		tek->TsTu.bit.OutClosed = GrH->Outputs.bit.Dout6;
	 	tek->TsTu.bit.OutMufta  = GrH->Outputs.bit.Dout2;
		tek->TsTu.bit.OutFault  = GrH->Outputs.bit.Dout3;
		tek->TsTu.bit.OutNeispr = GrH->Outputs.bit.Dout8;
		#else
		tek->TsTu.bit.OutOpened  = GrH->Outputs.bit.Dout2;
		tek->TsTu.bit.OutClosed  = GrH->Outputs.bit.Dout1;
	 	tek->TsTu.bit.OutMufta   = GrH->Outputs.bit.Dout3;
		tek->TsTu.bit.OutFault   = GrH->Outputs.bit.Dout0;
		tek->TsTu.bit.OutOpening = GrH->Outputs.bit.Dout5;
		tek->TsTu.bit.OutClosing = GrH->Outputs.bit.Dout4;
		tek->TsTu.bit.OutMuDu 	 = GrH->Outputs.bit.Dout6;
		tek->TsTu.bit.OutNeispr  = GrH->Outputs.bit.Dout7;
		#endif
	}
	else
	{
		#if BUR_M
		GrH->Outputs.bit.Dout7 = tek->TsTu.bit.OutOpened; 
		GrH->Outputs.bit.Dout6 = tek->TsTu.bit.OutClosed;
	 	GrH->Outputs.bit.Dout2 = tek->TsTu.bit.OutMufta;
		GrH->Outputs.bit.Dout3 = tek->TsTu.bit.OutFault;
		GrH->Outputs.bit.Dout8 = tek->TsTu.bit.OutNeispr;
		GrH->Outputs.bit.Dout5 = 1;								// ������� �� �������� ������
		#else
		GrH->Outputs.bit.Dout2 = tek->TsTu.bit.OutOpened;
		GrH->Outputs.bit.Dout1 = tek->TsTu.bit.OutClosed;
	 	GrH->Outputs.bit.Dout3 = tek->TsTu.bit.OutMufta;
		GrH->Outputs.bit.Dout0 = tek->TsTu.bit.OutFault;
		GrH->Outputs.bit.Dout5 = tek->TsTu.bit.OutOpening; 
		GrH->Outputs.bit.Dout4 = tek->TsTu.bit.OutClosing;
		GrH->Outputs.bit.Dout6 = tek->TsTu.bit.OutMuDu;
		GrH->Outputs.bit.Dout7 = tek->TsTu.bit.OutNeispr;
		#endif
	}
}

