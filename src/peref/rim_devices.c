/*--------------------------
	rim_devices.c

	Модуль переферии
--------------------------*/

#include "config.h"

AT25XXX  Eeprom1   = AT25XXX_DEFAULT1;
AT25XXX  Eeprom2   = AT25XXX_DEFAULT2;
TDisplay Display   = DISPLAY_DEFAULT;
DAC7513  Dac       = DAC7513_DEFAULT;
ENCODER  Encoder   = ENCODER_DEFAULT;
ADT7301  TempSens  = ADT7301_DEFAULT;
//DS1305   Ds1305    = DS1305_DEFAULT;
DS3234   Ds3234    = DS3234_DEFAULT;
RTC_Obj  Rtc       = RTC_DEFAULT;
TPult 	 Pult 	   = PULT_DEFAULTS;
LOG_INPUT BtnOpen   	= BTN_DEFAULT(0, False);	// было true
LOG_INPUT BtnClose  	= BTN_DEFAULT(1, False);	// было true
LOG_INPUT BtnStop_MU    = BTN_DEFAULT(2, False);// возможно надо поменять на тру
LOG_INPUT BtnStop_DU    = BTN_DEFAULT(3, False);
LOG_INPUT TuOpen  = TU_DEFAULT(&ExtReg,  SBEXT_OPEN,   0);
LOG_INPUT TuClose = TU_DEFAULT(&ExtReg,  SBEXT_CLOSE,  1);
LOG_INPUT TuStop  = TU_DEFAULT(&ExtReg,  SBEXT_STOP,   2);
LOG_INPUT TuMu    = TU_DEFAULT2(&ExtReg, SBEXT_MU,     3);
LOG_INPUT TuDu    = TU_DEFAULT2(&ExtReg, SBEXT_DU,     4);


//управление
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
//	DS1305_Init(&Ds1305);
	DS3234_Init(&Ds3234);

	DISPL_AddSymb(&Display,0,ToPtr(Icons), NUM_ICONS);

	GrG->DiscrInTest = 0;
	GrG->DiscrOutTest = 0;
	GrG->IsDiscrTestActive = 0;
	GrG->Mode = 0;
}

Bool RimDevicesRefresh(void)
{
	if (!RimRefrState) return true;

	Display.ResTout = GrC->DisplResTout * (Uns)DISPL_SCALE;			// Сброс дисплея

	BtnOpen.Input   = ToPtr(&HALL_SENS1);	// забираем данные дляобработки ручек
	BtnClose.Input  = ToPtr(&HALL_SENS2);
	BtnStop_MU.Input= ToPtr(&HALL_SENS4);
	BtnStop_DU.Input= ToPtr(&HALL_SENS3);

#if !BUR_M
	// Все входа не реверсивные
	TuOpen.Level  = DIN_LEVEL(SBEXT_OPEN,  (Uns)GrB->InputMask.bit.Open);	// забираем данные для обработки входов ТУ, 220/24 не влияет 
	TuClose.Level = DIN_LEVEL(SBEXT_CLOSE, (Uns)GrB->InputMask.bit.Close);	// приходит 0/1. уровень формируется в зависимости от типа входа - реверсивнй/нереверсивный
	TuStop.Level  = DIN_LEVEL(SBEXT_STOP,  (Uns)GrB->InputMask.bit.Stop);   // состояние входов получаем от АВРки через SPI
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
	while (!IsMemParReady()) { AT25XXX_Update(&MemPar);}				// Ждем пока прочтет все параметры из памяти																	// Функция вызывается при инициализации, поэтому необходимо зациклить	
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
	
	if (Eeprom->Error) return;									// При ошибке, завершаем выполнение
	
	Eeprom->Addr   = Addr << 1;									// Сдвигаем адрес (умножаем на 2), т.к. данные в структуре 2 байта, а в память пишется побайтно
	Eeprom->Buffer = ToPtr(Data);								// Указатель на данные
	Eeprom->Count  = Count << 1;
	Eeprom->Func   = Func;
}

void SetPlisData(void)
{
	SPI_init(SPIA, SPI_MASTER, 0, PLIS_BRR, 8);

	SetPlisAddr(CS_LED);
	SPI_send(PLIS_SPI, Ram.GroupH.LedsReg.all);
	SetPlisAddr(CS_NONE);

	SetPlisAddr(CS_LCD_ON);
	SPI_send(PLIS_SPI, !LcdEnable);
	SetPlisAddr(CS_NONE);

}

void RimIndication(void)//	Проверить для нас
{
	//if (Encoder.Error && !IsNoCalib()) // если ошибка энкодера и откалиброванно
	//	{Mcu.EvLog.Source = 0; GrD->CalibReset = 1;} // сбросили источник команд для журнала и выставляем сброс калибровок
	
	Dac.Mode = DAC7513_PR_4_20;
	if (IsTestMode())
		Dac.Value = &GrG->DacValue; // тест ЦАП
	else
	{
		Dac.Value = &GrA->PositionPr;		// Даем в цап значение в % текущего положения
		if (IsNoCalib()) Dac.Mode = DAC7513_OFF;// Если не откалиброванно то не выдаем значение и ЦАП выключен
	}

	if (IsTestMode()) DisplTest();				// если тест то тест дисплея
	
	GrA->Temper = BlockTemper + GrC->CorrTemper; // Забираем температуру и корректируем ее

	TenControl();	// управление теном
	
	if (RamUpdFlag && IsMemParReady())
	{
		WriteAllParams();
		RefreshData();
		RamUpdFlag = false;
	}

//	OUT_SET(RELAY_ENB, 0); нет у нас такого
}

void RtcControl(void)
{
	static Uns PrevTime;
	static Uns PrevDate;
	TTimeVar *DevTime = &GrB->DevTime;
	TDateVar *DevDate = &GrB->DevDate;
	
//	if (Ds1305.Busy) return;
	if (Ds3234.Busy) return;
	
	if (RtcStart)
	{
		if (DevTime->all != PrevTime)
		{
			RTC_SetTime(&Rtc, DevTime, 0);
			Ds3234.Flag = True;
		}
		if (DevDate->all != PrevDate)
		{
			RTC_SetDate(&Rtc, DevDate, 1);
			Ds3234.Flag = True;
		}
		if (RTC_TimeCorr(&Rtc, GrB->TimeCorrection))
		{
			Ds3234.Flag = True;
		}
	}

//	if (!Ds1305.Flag) 
	if (!Ds3234.Flag)
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
			strcpy(Display.HiStr, "AБВГДЕЁЖЗИЙКЛМНО");
			strcpy(Display.LoStr, "абвгдеёжзийклмно");
			break;
		case 2:
			strcpy(Display.HiStr, "ПРСТУФХЦЧШЩЪЫЬЭЮ");
			strcpy(Display.LoStr, "прстуфхцчшщъыьэю");
			break;
		case 3:
			strcpy(Display.HiStr, "Я0123456789()!?&");
			strcpy(Display.LoStr, "я№#~%^*_+-\\/<>'$");
			break;
		default:
			strcpy(Display.HiStr, "                ");
			strcpy(Display.LoStr, "                ");
			break;
	}
}
__inline void TenControl(void)	//  управление теном (подогреваем блок)
{	
	if (TempSens.Error) TenEnable = False; // если ошибка датчика температуры то не включаем тен
	else if (GrA->Temper >= GrC->TenOffValue) 	TenEnable = False;
	else if (GrA->Temper <= GrC->TenOnValue)	TenEnable = True;

	if (TempSens.Error) TenEnable = False;

	GrA->Status.bit.Ten = TenEnable;

	if (PowerEnable) OUT_SET(TEN_OFF, TenEnable); //  тен выключается 0
}


void SetPlisAddr(Uns Addr)
{
	Uint32 Mask = 0;

	// Для энкодера Avago с платой ATMega
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
// Одновременная подача команд теста запрещена
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
			// обнуляем регистр ТС/ТУ перед тестом
			GrH->Outputs.all = 0;
			GrT->TsTu.all = 0;
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
	//TGroupT *tek = &Ram. Tek.MainGroup;
	
	// Заполняем технологический регистр
	GrT->TechReg.bit.Opened  = GrA->Status.bit.Opened;
	GrT->TechReg.bit.Closed  = GrA->Status.bit.Closed;
	GrT->TechReg.bit.Mufta1  = GrA->Status.bit.Mufta;
	GrT->TechReg.bit.Mufta2  = GrA->Status.bit.Mufta;
	GrT->TechReg.bit.MuDu    = !GrA->Status.bit.MuDu;
	GrT->TechReg.bit.Opening = GrA->Status.bit.Opening;
	GrT->TechReg.bit.Closing = GrA->Status.bit.Closing;
	GrT->TechReg.bit.Stop    = GrA->Status.bit.Stop;
	GrT->TechReg.bit.Ten     = GrA->Status.bit.Ten;
	GrT->TechReg.bit.Ready   = !GrA->Status.bit.Fault;
	
	// Заполняем регистр дефектов
	GrT->DefReg.bit.I2t = GrA->Faults.Load.bit.I2t;
	GrT->DefReg.bit.ShC = (GrA->Faults.Load.bit.ShCU || GrA->Faults.Load.bit.ShCV || GrA->Faults.Load.bit.ShCW); 
	GrT->DefReg.bit.Drv_T = GrA->Faults.Proc.bit.Drv_T;
	GrT->DefReg.bit.Uv = (GrA->Faults.Net.bit.UvR || GrA->Faults.Net.bit.UvS || GrA->Faults.Net.bit.UvT);
	GrT->DefReg.bit.Phl = (GrA->Faults.Load.bit.PhlU || GrA->Faults.Load.bit.PhlV || GrA->Faults.Load.bit.PhlW);
	GrT->DefReg.bit.NoMove = GrA->Faults.Proc.bit.NoMove;
	GrT->DefReg.bit.Ov = (GrA->Faults.Net.bit.OvR || GrA->Faults.Net.bit.OvS || GrA->Faults.Net.bit.OvT);
	GrT->DefReg.bit.Bv = (GrA->Faults.Net.bit.BvR || GrA->Faults.Net.bit.BvS || GrA->Faults.Net.bit.BvT);
	GrT->DefReg.bit.Th = GrA->Faults.Dev.bit.Th;
	GrT->DefReg.bit.Tl = GrA->Faults.Dev.bit.Tl;
	GrT->DefReg.bit.PhOrdU = GrA->Faults.Net.bit.PhOrd;
	GrT->DefReg.bit.PhOrdDrv = GrA->Faults.Proc.bit.PhOrd;
	GrT->DefReg.bit.DevDef 	 = ((GrA->Faults.Dev.all & TEK_DEVICE_FAULT_MASK) != 0);

	// Регистр команд
	// При срабатывании одной команды, сбрасываем все
	if (Mcu.ActiveControls & CMD_SRC_SERIAL)
	{
		if(GrT->ComReg.bit.Stop)
		{
			GrD->ControlWord = vcwStop;

			GrT->ComReg.bit.Stop = 0;
			GrT->ComReg.bit.Open = 0;
			GrT->ComReg.bit.Close = 0;
		}
		else if(GrT->ComReg.bit.Open)
		{
			GrD->ControlWord = vcwOpen;

			GrT->ComReg.bit.Stop = 0;
			GrT->ComReg.bit.Open = 0;
			GrT->ComReg.bit.Close = 0;
		}
		else if(GrT->ComReg.bit.Close)
		{
			GrD->ControlWord = vcwClose;

			GrT->ComReg.bit.Stop = 0;
			GrT->ComReg.bit.Open = 0;
			GrT->ComReg.bit.Close = 0;
		}
	}
	else
	{
		GrT->ComReg.bit.Stop = 0;
		GrT->ComReg.bit.Open = 0;
		GrT->ComReg.bit.Close = 0;
	}

	if (GrT->ComReg.bit.PrtReset)
	{
		GrD->PrtReset = 1;
		GrT->ComReg.bit.PrtReset = 0;
	}

	if (GrT->ComReg.bit.EnDiscrOutTest)
	{
		GrG->DiscrOutTest = 1;
	}
	else if (GrT->ComReg.bit.DisDiscrOutTest)
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
	else if (GrT->ComReg.bit.EnDiscrInTest)
	{
		GrG->DiscrInTest = 1;
	}
	else if (GrT->ComReg.bit.DisDiscrInTest)
	{
		if (GrG->DiscrInTest == 1)
		{
			GrG->Mode = 0;	
			GrG->DiscrInTest = 0;
		}
	}

	// На всякий случай сбрасываем регистр команд
	if (GrT->ComReg.all != 0)
		GrT->ComReg.all = 0;

// Убрали переключение МУ/ДУ
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
	GrT->PositionPr 	 = GrA->PositionPr;
	GrT->CycleCnt 		 = GrH->CycleCnt;
	GrT->Iu				 = GrA->Iu;
	GrT->Ur				 = GrA->Ur;
	GrT->Torque			 = GrA->Torque;
	GrT->Speed			 = GrA->Speed;
	GrT->RsStation		 = GrB->RsStation;

	//Состояние дискретных входов и выходов
	GrT->TsTu.bit.IsDiscrOutActive = (GrG->DiscrOutTest);
	GrT->TsTu.bit.IsDiscrInActive = (GrG->DiscrInTest);

	#if !BUR_M
	GrT->TsTu.bit.InOpen 	 = GrH->Inputs.bit.Open;
	GrT->TsTu.bit.InClose	 = GrH->Inputs.bit.Close;
	GrT->TsTu.bit.InStop 	 = GrH->Inputs.bit.Stop;
	GrT->TsTu.bit.InMu 		 = GrH->Inputs.bit.Mu;
	GrT->TsTu.bit.InDu 		 = GrH->Inputs.bit.Du;
	#else

		if((GrG->DiscrInTest)&&(IsTestMode()))
		{		
			if (GrT->TsTu.bit.InOpen && GrT->TsTu.bit.InClose)
			{
				GrT->TsTu.bit.InOpen = 0;
				GrT->TsTu.bit.InClose = 0;	
			}
			if (IsPowerOff() && GrT->TsTu.bit.InOpen && !ContTest)
			{
				GrT->TsTu.bit.InOpen = 0;
				Dmc.RequestDir = 1;
				ContTest = 1;
				if(!PhEl.Direction)
					{
						GrH->ContGroup = cgOpen;
					}
			}
		   if (IsPowerOff() && GrT->TsTu.bit.InClose && !ContTest)
			{
				GrT->TsTu.bit.InClose = 0;
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
						GrT->TsTu.bit.InOpen = 0;
							GrT->TsTu.bit.InClose = 0;
					ContTest = 0;
					Dmc.RequestDir = 0;	
					ContTestTimer = 0;
			 }
			 // если без теста есть напряжение.
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
		GrT->TsTu.bit.OutOpened = GrH->Outputs.bit.Dout7;
		GrT->TsTu.bit.OutClosed = GrH->Outputs.bit.Dout6;
	 	GrT->TsTu.bit.OutMufta  = GrH->Outputs.bit.Dout2;
		GrT->TsTu.bit.OutFault  = GrH->Outputs.bit.Dout3;
		GrT->TsTu.bit.OutNeispr = GrH->Outputs.bit.Dout8;
		#else
		GrT->TsTu.bit.OutOpened  = GrH->Outputs.bit.Dout2;
		GrT->TsTu.bit.OutClosed  = GrH->Outputs.bit.Dout1;
	 	GrT->TsTu.bit.OutMufta   = GrH->Outputs.bit.Dout3;
		GrT->TsTu.bit.OutFault   = GrH->Outputs.bit.Dout0;
		GrT->TsTu.bit.OutOpening = GrH->Outputs.bit.Dout5;
		GrT->TsTu.bit.OutClosing = GrH->Outputs.bit.Dout4;
		GrT->TsTu.bit.OutMuDu 	 = GrH->Outputs.bit.Dout6;
		GrT->TsTu.bit.OutNeispr  = GrH->Outputs.bit.Dout7;
		#endif
	}
	else
	{
		#if BUR_M
		GrH->Outputs.bit.Dout7 = GrT->TsTu.bit.OutOpened; 
		GrH->Outputs.bit.Dout6 = GrT->TsTu.bit.OutClosed;
	 	GrH->Outputs.bit.Dout2 = GrT->TsTu.bit.OutMufta;
		GrH->Outputs.bit.Dout3 = GrT->TsTu.bit.OutFault;
		GrH->Outputs.bit.Dout8 = GrT->TsTu.bit.OutNeispr;
		GrH->Outputs.bit.Dout5 = 1;								// питание ТС включено всегда
		#else
		GrH->Outputs.bit.Dout2 = GrT->TsTu.bit.OutOpened;
		GrH->Outputs.bit.Dout1 = GrT->TsTu.bit.OutClosed;
	 	GrH->Outputs.bit.Dout3 = GrT->TsTu.bit.OutMufta;
		GrH->Outputs.bit.Dout0 = GrT->TsTu.bit.OutFault;
		GrH->Outputs.bit.Dout5 = GrT->TsTu.bit.OutOpening; 
		GrH->Outputs.bit.Dout4 = GrT->TsTu.bit.OutClosing;
		GrH->Outputs.bit.Dout6 = GrT->TsTu.bit.OutMuDu;
		GrH->Outputs.bit.Dout7 = GrT->TsTu.bit.OutNeispr;
		#endif
	}
}

