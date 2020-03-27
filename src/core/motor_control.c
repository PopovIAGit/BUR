/*======================================================================
Имя файла:          motor_control.c
Автор:              $Author:  $     
Описание:
Модуль управления двигателем
======================================================================*/
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
APFILTER3  Trqfltr	= TORQ_FLTR_DEFAULT;

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

Int  lastDirection	= 0;		// Последнее запомненное направление вращения. Необходим для выставления "муфты в открытие/закрытие"
Uns  ZazorTimer 	= 0;
Uns  Iload[3] 		= {0,0,0};	// действующие токи в А (токи нагрузки)
Uns  Ipr[3]   		= {0,0,0};	// действующие токи в % от номинального
Uns  Imid	  		= 0;		// средний действующий ток	
Uns  Imidpr	  		= 0;		// средний действующий ток в процентах от Iн
Bool DynBrakeEnable = False;	// запрет динамического торможения по старту
Uns  KickCounter    = 0;		// счетчик количества ударов
Uns  KickModeTimer  = 0;		// счетчик для режима ударного момента
Uns  UporModeTimer  = 0;		// счетчик для упора
Uns  timerMaxTorque = 0;		// время для защиты от раскачки в режимах
Uns  PauseModeTimer = 0;		// счетчик для режима паузы
Uns  DynModeTimer   = 0;		// счетчик для динамического торможения
Byte DmcRefrState   = 0;
Uns  AbsSpeed 		= 0;
Uns  LowPowerReset  = 0;		// сбросы режима провала напряжения
Uns  SaveDirection  = SIFU_NONE;
Uns  SaveContactorDirection = 0;
Bool KickModeEnable = False;	// разрешение ударного момента
#if BUR_M
Uns Net_Mon_Timer   = 0;		// таймер перед тем как считаем наличие напряжение как команду на пуск
Uns PowerLostTimer2 = 0;
#endif
Uns BreakFlag 			= 0;
Uns PowerSupplyEnable 	= 0; 	// Наличие питания источника 
Uns PowerSupplyCnt 		= 0;	// Задержка на выключение питания
Uns ReqDirection 		= 0;
Uns DebugStartDelayCnt2 = 10;
Uns BreakVoltFlag 		= 0;
Bool 	waitStartVoltageFlag = false;	// Флаг того, что блок ждет напряжение при старте
Bool	calibStopFlag = 0; 				// Флаг который выставляется, если электропривод был остановлен путем достижения концевика
TValveCmd SaveContGroup = vcwNone;
Uns secflag = 0;
Uns secpausetimer = 0;
// ----------------------------------------	
Int InomDefU[11]  	 = {13,11,18,52,52,47,56,110,85,148,27};					// default значения для Inom для разных приводов уфа
Int InomDefS[10]	 = {11,9,13,32,32,33,73,85,95,150};						// Сарапуль
Int MomMaxDef[10]  	 = {10,10,40,40,80,100,400,400,1000,1000};				//					для Mmax 
Int TransCurrDef[10] = {1000,1000,1000,1000,1000,1000,1100,1100,1100,1100};	//					для TransCur править
Int TransCurr100A25 = 2000;
Int GearRatioDef[5]	 = {5250,7360,15675,16016,16016};						//для передаточного числа редуктора 


Uns TimerInterp = 0;
Uns AngleInterp(Uns StartValue, Uns EndValue, Uns Time); 
//-------------Монитор------------------
Uns program   = 1;
Uns mon_index = 0;
Uns dot       = 0;
Uns dot_max   = 10;
Int buffer1[256];
Int buffer2[256];

Uns TrqTimer = 0;

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
// Логика управления тиристорами для динамического торможения:
void SifuControlForDynBrake(SIFU *);

//---- управление двигателем-----
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

	memset(&UR, 0, sizeof(ILEG_TRN));	// обнуляем структуры состояния токов и напряжений
	memset(&US, 0, sizeof(ILEG_TRN));	// чтоб после кампиляции и прошивки там шлака небыло
	memset(&UT, 0, sizeof(ILEG_TRN));
	memset(&IU, 0, sizeof(ILEG_TRN));
	memset(&IV, 0, sizeof(ILEG_TRN));
	memset(&IW, 0, sizeof(ILEG_TRN));

	memset(&Torq,  0, sizeof(TTorqObs)); // обнуляем структуру расчета моментов для того же
	
	CubInit(&Torq.Cub1, &TqCurr);		 // инициализация структуры куба для расчета момента
	CubInit(&Torq.Cub2, &TqAngUI);
	CubInit(&Torq.Cub3, &TqAngSf);
}
// -----------------------------------------------------------------
void MotorControlUpdate(void)
{
#if BUR_M
	if (!SaveContactorDirection)
	{
		GrD->ControlWord = vcwStop;
		SaveContactorDirection = 1;
	}
#endif
	ADC_Aquisition();					// фильтрация ацп
	DmcPrepare();						// калибровка датчиков U I и расчеты полярностей и углов фаз
	ph_order_trn(&PhEl);				// чередование фаз
	if (UR.Output < 70) PhEl.Direction = 0;
	TestThyrControl();					// тест тиристоров - угол открытия 0
	PulsePhaseControl();				// Сифу
	//MonitorUpdate1();					// Монитор - убрать потом
}
// -----------------------------------------------------------------
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
// -----------------------------------------------------------------
__inline void DmcPrepare(void){	//18kHz

		UR.Input = ADC_CONV(_IQtoIQ16(URfltr.Output), GrH->UR_Mpy, GrH->UR_Offset);
		US.Input = ADC_CONV(_IQtoIQ16(USfltr.Output), GrH->US_Mpy, GrH->US_Offset);
		UT.Input = ADC_CONV(_IQtoIQ16(UTfltr.Output), GrH->UT_Mpy, GrH->UT_Offset);
		IU.Input = ADC_CONV(_IQtoIQ16(IUfltr.Output), GrH->IU_Mpy, GrH->IU_Offset);
		IV.Input = ADC_CONV(_IQtoIQ16(IVfltr.Output), GrH->IV_Mpy, GrH->IV_Offset);
		IW.Input = ADC_CONV(_IQtoIQ16(IWfltr.Output), GrH->IW_Mpy, GrH->IW_Offset);

		// автоподстройка датчиков тока

		if (IsStopped())
		{
			if (!InRange(IU.Input, -15, 15))
			{
				if(IU.Input > 0) GrH->IU_Offset++;
				if(IU.Input < 0) GrH->IU_Offset--;
			}
				if (!InRange(IV.Input, -15, 15))
			{
				if(IV.Input > 0) GrH->IV_Offset++;
				if(IV.Input < 0) GrH->IV_Offset--;
			}
				if (!InRange(IW.Input, -15, 15))
			{
				if(IW.Input > 0) GrH->IW_Offset++;
				if(IW.Input < 0) GrH->IW_Offset--;
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
// -----------------------------------------------------------------
__inline void PulsePhaseControl(void){	//18kHz	

	sifu_calc2(&Sifu);	
	
	THYR_PORT |= ((LgUns)Sifu.Status.all & ((LgUns)THYR_MASK));   //
	THYR_PORT &= ((LgUns)Sifu.Status.all | (~((LgUns)THYR_MASK)));//

	OUT_SET(ENB_TRN, (Sifu.Status.all >> SIFU_EN_TRN) & 0x1);
}
// -----------------------------------------------------------------
void DmcIndication1(void)
{
	Uns koef1 = 1, koef2 = 1;
#if BUR_90
	if (GrC->Drive_Type == dt100_A50_S)
	{
		koef1 = 5;
		koef2 = 1;
	}
	else if (GrC->Drive_Type == dt100_A25_S)
	{
		koef1 = 5;
		koef2 = 1;
	}
	else if (GrC->Drive_Type == dt4000_G18_U)
	{
		koef1 = 5;
		koef2 = 3;
	}
#endif
	// токи нагрузки в А
	Iload[0] = (I_RMS_CALC(IU.Output)*koef2)/koef1;
	Iload[1] = (I_RMS_CALC(IV.Output)*koef2)/koef1;
	Iload[2] = (I_RMS_CALC(IW.Output)*koef2)/koef1;
	if (IsStopped() && !IsTestMode()) memset(Iload, 0, 3);		// если стоп и не тест то 0

	// токи нагрузки в %
	Ipr[0] = ValueToPU1(Iload[0], GrC->Inom);
	Ipr[1] = ValueToPU1(Iload[1], GrC->Inom);
	Ipr[2] = ValueToPU1(Iload[2], GrC->Inom);

	// индикация токов нагрузки и угла открытия тиристоров
	if (GrB->IIndicMode == imRms)     memcpy(&GrA->Iu, Iload, 3);
	if (GrB->IIndicMode == imPercent) memcpy(&GrA->Iu, Ipr,   3);
//	GrC->OpenAngle = Sifu.OpenAngle;

	// средний действующий ток
/*	if (GrB->IIndicMode == imRms) */    GrH->Imid = Imid;//	выбор индикации среднего тока, в А или %
//	if (GrB->IIndicMode == imPercent) GrH->Imid = Imidpr;
	
	// угол между током и напряжением
	ApFilter1Calc(&Phifltr);
	GrA->AngleUI = _IQtoIQ16(Phifltr.Output);
	if (IsStopped() && !IsTestMode()) GrA->AngleUI = 0; // если не едим и не тест, то 0

	GrH->ADC_iu = ADC_IU; 
	GrH->ADC_iv = ADC_IV;
	GrH->ADC_iw = ADC_IW;
}
// -----------------------------------------------------------------
void DmcIndication2(void)
{
	// входные напряжения
	GrA->Ur = UR.Output;
	GrA->Us = US.Output;
	GrA->Ut = UT.Output;

	// среднее действующее напряжение и чередование фаз сети
	Umfltr.Input = _IQ16toIQ(Mid3UnsValue(GrA->Ur, GrA->Us, GrA->Ut));
	ApFilter1Calc(&Umfltr);
	GrH->Umid = _IQtoIQ16(Umfltr.Output);

		#if BUR_M
		GrA->Status.bit.Power = 1;
		#else
		if (IsPowerOn())
		{	if(DebugStartDelayCnt2)DebugStartDelayCnt2--;
			else {
				GrA->Status.bit.Power = 0;
				DebugStartDelayCnt2 = 0;
			}
		}	
		else
		{
			//memset(&GrA->Ur, 1, 3);
		}
		#endif

			
	if (!PhEl.Direction) GrH->PhOrdValue = 0;					// чередование фаз сети
	else if (PhEl.Direction > 0) GrH->PhOrdValue = 2;
	else GrH->PhOrdValue = 1; 
	
	// копии в диагностику
	if(TrqTimer++ >(5 * GrC->TrqViewTime))
	{
		GrA->Torque  = GrH->Torque;
		TrqTimer = 0;
	}

	GrA->Speed   = GrH->Speed;

	GrC->Drive_Type = GrB->DriveType;					// Копируем тип привода в группу C чисто для индикации
}
// -----------------------------------------------------------------
void DefineCtrlParams(void) // задачи контролируемых параметров в 
{
	Int MaxZone, CloseZone, OpenZone;	// максимальная зона, зона открытия, зона закрытия
	
	if (GrB->ValveType == vtShiber){GrB->BreakMode = vtNone;} // Если выбранна шиберная задвижка то сбрасываем принудительно тип уплотнения в "отсутвует"

	MaxZone = (GrA->FullWay >> 1) - 1; // максимальная зона это (полный путь/2)-1 
	
	CloseZone = GrB->CloseZone;			// забираем заданную зону закрытия
	if (CloseZone > MaxZone) CloseZone = MaxZone; // если заданя больше максимума задаем максимум
	
	OpenZone = GrB->OpenZone;			// тоже самое что и закрытия
	if (OpenZone > MaxZone) OpenZone = MaxZone;
	OpenZone = GrA->FullWay - OpenZone;	// зона открытия это полный путь - зона открытия заданная
	
	if (!Dmc.RequestDir)  // если не определенно направление вращения 
		Dmc.TorqueSet = 0;// момент задания = 0
	else if (Dmc.RequestDir > 0) // если подана команда на открытие
	{
		Dmc.TorqueSet = GrB->MoveOpenTorque; // момент задания забираемs
		BreakFlag = 0;
		if (!IsNoCalib())		 // если откалиброванн датик положения
		{
			if 		(GrA->CurWay <= CloseZone)    	{ Dmc.TorqueSet = GrB->StartOpenTorque;  BreakFlag = 0;}// если текущее положение находится в зоне закрыто то задаем момент как момент трогания на закрытие
			else if (GrA->CurWay >= OpenZone)		{ Dmc.TorqueSet = GrB->BreakOpenTorque; BreakFlag = 1;} // если находимся в зоне открытия то  задаем момене как момент уплотнения на открытие
		}
	}
	else	// если команда закрываться то так же но наоборот и моменты называются по другому
	{
		Dmc.TorqueSet = GrB->MoveCloseTorque;
		BreakFlag = 0;
		if (!IsNoCalib())
		{
			if      (GrA->CurWay <= CloseZone)    	{ Dmc.TorqueSet = GrB->BreakCloseTorque; BreakFlag = 1;}
			else if (GrA->CurWay >= OpenZone) 		{ Dmc.TorqueSet = GrB->StartCloseTorque; BreakFlag = 0;}
		}
	}

#if !TORQ_TEST
	Dmc.TorqueSetPr = ValueToPU0(Dmc.TorqueSet, GrC->MaxTorque * 10); // считаем момент в % от максимального
#endif
}
//-----------------------Управление при стопе--------------------------------
void StopPowerControl(void) // упровление при стопе
{
	GrA->Status.bit.Stop	  	= 1;	// выставили в статус устройства стоп и все остальное сбросили
	GrA->Status.bit.Closing 	= 0;
	GrA->Status.bit.Opening 	= 0;
	GrA->Status.bit.Test	  	= 0;

	#if BUR_M
		GrH->ContGroup   = cgStop;
		PhEl.Direction 	 = 0;
		PowerLostTimer2  = 0;
		Net_Mon_Timer = 0;

		if (GrC->ReversKVOKVZ == 1 && secflag == 0)
		    SaveContGroup = vcwNone;

	#endif 

	// если останов не по аварии и не по муфте, то
	// вызываем режим динамического торможения
	if (MuffFlag || IsFaulted() || IsDefected())
	 Dmc.WorkMode = wmStop;
	#if BUR_M
	else  Dmc.WorkMode = wmStop;// В модиф.М нет динамического торможения
	#else 
	else if (DynBrakeEnable) Dmc.WorkMode = wmDynBrake;// В модиф.Т используем динамическое торможение
	#endif
	// задержка перед следующим пуском
	Mcu.StartDelay = (Uns)START_DELAY_TIME;	
	BreakVoltFlag = 0;
	Muf.Timer = 0;
}

//----------------Отслеживание состояние питающей сети для подачи команд БУР М
#if BUR_M
void NetMonitor(void)
{
	if ((GrH->FaultsLoad.all & LOAD_SHC_MASK) && IsPowerOn())	
		GrD->ControlWord = vcwStop;	

	if (!IsPowerOff())
	{
	    waitStartVoltageFlag = false;	// Снимаем флаг ожидания напржения
	}

	if ((!IsPowerOff()) && (!IsTestMode())&& (Dmc.WorkMode == wmStop) && (GrH->CalibState == csCalib) && (!(GrH->FaultsLoad.all & LOAD_SHC_MASK))) // если есть напряжение, нету аварий по напряжению и находимся в стопе

	{ 
		if (IsPowerOff())
		{
			 Net_Mon_Timer	= 0;
			 PhEl.Direction = 0;
		}
		

		if (Net_Mon_Timer < NET_MON_START_TIME)	// ждем 
		{ 
			Net_Mon_Timer++;		
		}
		else 
		{
			Net_Mon_Timer = 0;
#if BUR_90									// Заплатка только для БУР-90 - решает проблему, когда привод ехал, и у него оборвали питание
			if (Fault_Delay)				// чтобы после восстановления электропитания привод не ехал снова
			{								// Если прилетело 380 В, а у нас еще Fault_Delay,
				GrH->ContGroup = cgStopKvoKvz;	// то принудительно зажимаем "СТОП", чтобы отхлопнуть контакторы
				PhEl.Direction = 0;
			}
#endif
			switch(PhEl.Direction)
			{
				case -1: GrD->ControlWord = GrG->TestCamera ? vcwTestClose : vcwClose;
						Mcu.EvLog.Source = CMD_SRC_MPO_MPZ;
						break;
				case  1: GrD->ControlWord = GrG->TestCamera ? vcwTestOpen  : vcwOpen;
						Mcu.EvLog.Source = CMD_SRC_MPO_MPZ;
						break;
			}	
		}
	}
	if (IsPowerOff())
	{
		 PhEl.Direction = 0;
	}
	if (IsPowerOff() && (Dmc.WorkMode != wmStop) && !waitStartVoltageFlag) //  если нет  напряжения и мы едем то выключаем
	{
		if(PowerLostTimer2 < NET_MON_STOP_TIME2) //ждем 1 с   (Uns)(GrC->BURM_Timer2 * PRD_50HZ) NET_MON_STOP_TIME2
		{
			PowerLostTimer2++;
		}
		else 
		{
			GrD->ControlWord = vcwStop;	
			Mcu.EvLog.Source = CMD_SRC_MPO_MPZ;
			PowerLostTimer2   = 0;
		}		
	}
	if ((PhEl.Direction == 0)&&(IsPowerOn()))  //  если есть хоть одно напряжение, а чсередование фаз не определено
	{
		 BreakVoltFlag = 1;

		 if (GrH->FaultsNet.bit.BvR ||GrH->FaultsNet.bit.BvS ||GrH->FaultsNet.bit.BvT)
		 {
		 	GrD->ControlWord = vcwStop;
		 	Mcu.EvLog.Source = CMD_SRC_BLOCK;
			BreakVoltFlag = 0;
		 }
	}
	else 
	{
		BreakVoltFlag = 0;
	}
	
	if (Mcu.IgnorComFlag != 0) GrD->ControlWord = vcwStop;

	if ((Dmc.RequestDir != PhEl.Direction)&&(Dmc.RequestDir !=0)&&(PhEl.Direction != 0))
		GrH->FaultsNet.bit.RST_Err = 1;	

	if(GrH->FaultsNet.bit.RST_Err)
	{
		PhEl.Direction = 0;
	}
}
//----------Управление контакторами для БУР М
void ContactorControl(TContactorGroup i) // если 0 то 
{
	static Uns StopSetTimer 	= 0;
	if(IsTestMode() || GrG->TestCamera) return; // SVS
   switch(i)
   {
		case cgStop: 	
					GrH->Outputs.bit.Dout1 = 0;	//  Ком.Открыть
					GrH->Outputs.bit.Dout0 = 0;	//  Ком.Закрыть
					GrH->Outputs.bit.Dout4 = 1;	//  Ком.Стоп Стоп инвертный если 0 то стоп если 1 то ничего
					
				  if ((++StopSetTimer > CONECTOR_STOP_TIME)&&(!GrT->StopButtonState))	// Выключаем рэле по истечению таймера, или пока ручку СТОП не отпустят
	   				{
	   					StopSetTimer   = 0;
						GrH->ContGroup = cgOff;
	   				}		 
					
				break;
		case cgOpen: 	
					GrH->Outputs.bit.Dout1 = 1;	//  Ком.Открыть
					GrH->Outputs.bit.Dout0 = 0;	//  Ком.Закрыть
					GrH->Outputs.bit.Dout4 = 0;	//  Ком.Стоп

					if (++StopSetTimer > CONECTOR_TIME)
	   				{
	   					StopSetTimer   = 0;
						GrH->ContGroup = cgOff;
	   				}
					
				break;  
		case cgClose: 	
					GrH->Outputs.bit.Dout1 = 0;	//  Ком.Открыть
					GrH->Outputs.bit.Dout0 = 1;	//  Ком.Закрыть
					GrH->Outputs.bit.Dout4 = 0;	//  Ком.Стоп
					
				if (++StopSetTimer > CONECTOR_TIME)
	   				{
	   					StopSetTimer   = 0;
						GrH->ContGroup = cgOff;
	   				}
					
				break;
		case cgOff: //выключаем все
				GrH->Outputs.bit.Dout1 = 0;	//  Ком.Открыть
				GrH->Outputs.bit.Dout0 = 0;	//  Ком.Закрыть
				GrH->Outputs.bit.Dout4 = 0;	//  Ком.Стоп		
				break;

		case cgStopKvoKvz:
				GrH->Outputs.bit.Dout1 = 0;	//  Ком.Открыть
				GrH->Outputs.bit.Dout0 = 0;	//  Ком.Закрыть
				GrH->Outputs.bit.Dout4 = 1;	//  Ком.Стоп Стоп инвертный если 0 то стоп если 1 то ничего
				if (GrC->ReversKVOKVZ == 0)
				{
					GrH->Outputs.bit.Dout9 = GrH->Outputs.bit.Dout10 = 1;		// КВЗ и КВО
				}
				else
				{
					GrH->Outputs.bit.Dout9 = GrH->Outputs.bit.Dout10 = 0;		// КВЗ и КВО
				}

				if (++StopSetTimer > CONECTOR_STOP_TIME)	// Выключаем рэле по истечению таймера, или пока ручку СТОП не отпустят
				{
					StopSetTimer   = 0;
					GrH->ContGroup = cgOff;
				}

   }

}
#endif

//----------Управление при пуске-----------------------
void StartPowerControl(TValveCmd ControlWord)	// управление при пуске
{
	static Uns  RunStatus = 0;
register Uns Tmp;

	if (GrH->FaultsLoad.all & LOAD_SHC_MASK) return;		// если есть кз то выходим

#if !BUR_M
	if (!PhEl.Direction) return;							// если не определили еще чередование фаз, то выходим для бурТ
#endif							// если не определили еще чередование фаз, то выходим
	
	switch(ControlWord)	// смотрим управляющие слова
	{
		case vcwClose:					// если закрытие
			Dmc.WorkMode   = wmTestPh;	// стм. тест фаз
			Dmc.RequestDir = -1;		// направления закрытие
			#if BUR_M
			if ( IsPowerOff() )			// Управляем реле "Закрыть" только в том случае, если контакторы МПО и МПЗ разомкнуты (нету ~380 В). А иначе смысла подавать управляющие сигналы на пускатели - нету
			{
				if(!PhEl.Direction)
				{
					GrH->ContGroup = cgClose;
				}
				if (GrC->ReversKVOKVZ == 1)
				{
					SaveContGroup = vcwClose;
				}
			}

			#endif 
			Mcu.EvLog.Value = CMD_CLOSE;
			break;
		case vcwOpen:					 
			Dmc.WorkMode   = wmTestPh;	 
			Dmc.RequestDir = 1;
			#if BUR_M
			if ( IsPowerOff() ) 		// Управляем реле "Открыть" только в том случае, если контакторы МПО и МПЗ разомкнуты (нету ~380 В). А иначе смысла подавать управляющие сигналы на пускатели - нету
			{
				if(!PhEl.Direction)
				{
					GrH->ContGroup = cgOpen;

				}
				if (GrC->ReversKVOKVZ == 1)
				{
					SaveContGroup = vcwOpen;
				}
			}
			#endif 
			Mcu.EvLog.Value = CMD_OPEN;
			break;
		case vcwTestClose:				 // если тест закрытие 
			Dmc.WorkMode   = wmSpeedTest;// вызываем тестовое закрытие
			Dmc.RequestDir = -1;		 // направление закрытия
			GrA->Status.bit.Test = 1;  // выставляем бит теста
			#if BUR_M
			if(!PhEl.Direction)
			{
			GrH->ContGroup = cgClose;
			}
			#endif 
			Mcu.EvLog.Value = CMD_CLOSE;
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
			Mcu.EvLog.Value = CMD_OPEN;
			break;
		default:
			return;						 // по дефолту пока не пришла команда выходим 
	}
	
	// включение режима "пускового момента"
	KickModeEnable = GrH->KickCount && (Dmc.RequestDir > 0); // если заданно количество ударов и определенно направление
	Torq.ObsEnable = True;//  Разрешить расчет момента
	
	// сброс аварий необходимый для пуска (кроме напряжений)
	ProtectionsReset(); 

	// разрешение тактирования тиристоров
	ClkThyrControl(1);

	// сброс счетчиков режимов
	KickCounter      = 0;
	KickModeTimer    = 0;
	UporModeTimer    = 0;
	timerMaxTorque = 0;
	PauseModeTimer   = 0;
	DynModeTimer     = 0;
	#if BUR_M
	Net_Mon_Timer    = 0;
	PowerLostTimer2  = 0;
	#endif

	// определения открываемой группы тиристоров
	#if BUR_M
	ReqDirection = SIFU_UP;
	#else
	// определения открываемой группы тиристоров
	if (( (Dmc.RequestDir != PhEl.Direction) && !GrB->ReversMove)|| 	// если необходимое чередование фаз не соответсвует данному то меняем задание на сифу
		( (Dmc.RequestDir == PhEl.Direction) && GrB->ReversMove) )
	{
		ReqDirection  = SIFU_DOWN;
	}
	else if (( (Dmc.RequestDir != PhEl.Direction) && GrB->ReversMove)|| 	// если необходимое чередование фаз не соответсвует данному то меняем задание на сифу
			 ( (Dmc.RequestDir == PhEl.Direction) && !GrB->ReversMove) )
	{
		ReqDirection  = SIFU_UP;
	}
	#endif

	// сигнализация о движении и установка параметров
	GrA->Status.bit.Stop = 0; //
	Dmc.TorqueSet = 0xFFFF;     // выставляем максимальный момент
	if (Dmc.RequestDir < 0) GrA->Status.bit.Closing = 1; // выставлям состояни "закрытие"
	if (Dmc.RequestDir > 0) GrA->Status.bit.Opening = 1; // выставлям состояни "открытие"

	lastDirection = Dmc.RequestDir;					     // Запоминаем направление вращения в переменную lastDirection

	Tmp = (GrA->Status.all & STATUS_MOVE_MASK);  // Записываем в темп 2 бита (открывается и закрывается, который есть тот и стоит)
	if (RunStatus == Tmp) ZazorTimer = 0;		   // если направление не поменялось ничего
	else {ZazorTimer = GrH->ZazorTime * (Uns)ZAZOR_SCALE; RunStatus = Tmp;} // иначе если величена зазора определенна то считаем время зазора или 0
	
	// разрешение динамического торможения
	#if BUR_M
	DynBrakeEnable = False;
	#else
	DynBrakeEnable = True;	
	#endif

#if BUR_90
	DRV_ON = 0;		// Разрешаем управление тиристорами
#endif
	waitStartVoltageFlag = true; 		// Выставляем флаг ожидания напряжения при старте
}

//---state machine---------

void ControlMode(void) // 200Hz
{
	PowerCheck(); // постоянно проверяем наличие питания источника

#if BUR_M
	ContactorControl(GrH->ContGroup);
#endif

	if (IsTestMode()) {DmcTest(); return;}	// если тест запускаем тест и выходим

	TorqueCalc();

	if (GrC->ReversKVOKVZ == 1 && Dmc.WorkMode == wmStop && SaveContGroup != vcwNone && secflag == 1)
	{

	    if (secpausetimer-- == 1)
	    {
		    GrD->ControlWord = SaveContGroup;
		    SaveContGroup = vcwNone;
		    secflag = 0;
		    secpausetimer = 0;
	    }
	}

	if (secflag == 1 && secpausetimer > 0 && SaveContGroup == vcwNone)
	{
	    secflag = 0;
	    secpausetimer = 0;
	}


	switch (Dmc.WorkMode) // стейт машина на основе управляющего слова
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
	
	if (ZazorTimer > 0) ZazorTimer--;	// если есть обработка зазора
}

// Обработка режима прогона-------------------------------------------------
void ProgonModeUpdate (void)	// 10 Гц
{
	static Uns progonDelay = 0,			// Задержка перед пуском, когда привод достигает крайней точки
	           halfCycle = 0,			// Полцикла (открыто -> закрыто или закрыто -> открыто). Два полцикла равны 1 циклу
	   	   	   stopTimer = 0;			// Таймер задержки снятия режима прогона, если стоп
	static Byte isComandDone = false;	// Флаг, подана ли команда. Команда должна подаваться только 1 раз из положения закрыто или открыто

	if (GrC->progonCycles == 0) return;
	
	if (IsFaulted() || IsNoCalib() ) 	// Без калибровки режим тестового прогона не работает
	{
		GrC->progonCycles = 0;
		GrA->Faults.Proc.bit.CycleMode = 0;
	}

	if (!GrA->Faults.Proc.bit.CycleMode)
	{
		if (IsClosing() || IsOpening()) GrA->Faults.Proc.bit.CycleMode = 1;
	}
	else
	{
		if (IsClosed() || IsOpened())
		{
			stopTimer = 0;
			if (!isComandDone)			// Если команда еще не была подана
			{
				if (progonDelay++ > 50)	// 50 = 5 сек на 10 Гц
				{
					if(GrC->progonCycles != 0)
					{
						GrD->ControlWord = IsClosed() ? vcwOpen : vcwClose;
					}
					isComandDone = true;
					progonDelay = 0;
					if (++halfCycle == 2) // Два полцикла = один полный цикл
					{
						halfCycle = 0;
						GrC->progonCycles--;
						if (GrC->progonCycles == 0)
						{
							progonDelay = 0;
							GrA->Faults.Proc.bit.CycleMode = 0;
						}
					}
				}
			}
		}
		else 
		{
			isComandDone = false;
			if (IsStopped() ) // Если выполнилось это условие, то во время прогона поступила команда "стоп"
			{
				if (stopTimer++ > 20)	// Снимаем статус режима прогона с задержкой в 2 секунды
				{
					GrC->progonCycles = 0;
					GrA->Faults.Proc.bit.CycleMode = 0;
					stopTimer = 0;
				}
			}
		}
	}
}
// -----------------------------------------------------------------
__inline void StopMode(void)		// стм. стоп
{
	Sifu.SetAngle   = SIFU_MAX_ANG; // закрываем тиристоры
	Sifu.AccelTime  = 0;			// скорость закрытия 0(самая быстрая)
	Sifu.Direction  = SIFU_NONE;    // выключили сифу
	GrH->Torque = 0;				// момент ноль
	Dmc.RequestDir = 0;

#if BUR_90
	DRV_ON = 1;						// Отключаем управление тиристорами
#endif
	
	waitStartVoltageFlag = false;	// Снимаем флаг ожидания напржения

	ClkThyrControl(0);				// выключили тактирование тиристоров
}
// -----------------------------------------------------------------
__inline void TestPhMode(void)		// стм. тест фаз
{									// проверка на КЗ в protection.c
	static Uns TestModeTimer  = 0;	// время в тестовом режиме

	#if BUR_M	
	if (!PhEl.Direction)	return;
	#endif	 
	Sifu.Direction  = ReqDirection;
	Sifu.SetAngle   = 115;			// 
	Sifu.AccelTime  = 100;
	GrH->Torque = 0;			// момент не показываем

	if (++TestModeTimer >= (Uns)TEST_STATE_TIME)// считаем время теста 0.04 сек
	{
		Dmc.WorkMode = wmUporStart;				// переходим на стм. упор (режим ограничения момента на старте)
		TestModeTimer = 0;						// сбрасываем таймер тестового режима
	}
}
// -----------------------------------------------------------------
__inline void UporStartMode(void)	// стм. Упор старт
{
	Sifu.SetAngle   = Torq.SetAngle;// угол открытия на осове момента
	Sifu.AccelTime  = GrC->VoltAcc; // интенсивность 100 
	GrH->Torque 	= Dmc.TorqueSet - 1; // показываем момент = моменту задания
	
//	#if 0 // ???
	if(GrH->UporOnly == 1) return;
	if (++UporModeTimer >= (Uns)UPOR_STATE_TIME) // считаем время упора 2 сек.
	{
		if (AbsSpeed > MIN_FREQ_RPM) // если скорость поднилаась выше 600 об/мин, то 
		{
			Dmc.WorkMode = wmMove;	 // стм. движение
			KickModeEnable = False;  // режим удара не требуется
		}
		else if (KickModeEnable)	 // иначе если включен режим удара
		{
			if (KickCounter < GrH->KickCount) Dmc.WorkMode = wmKick; // если счетчик ударов не привышен то запускаем режим удара
			else KickModeEnable = False;								 // иначе выключаем режим удара
		}
		UporModeTimer = 0;	// сбрасываем таймер упора, если слово не поменялось начинаем заново
	}
//	#endif
}
// -----------------------------------------------------------------
__inline void MoveMode(void)	// стм. движение
{
	Sifu.SetAngle   = 0;	// полное открытие тиристоров (задание на угол открытия тиристоров)
	Sifu.AccelTime  = GrC->VoltAcc;	// 100
	GrH->Torque 	= Torq.Indication; // показываем момент как он есть

	if (!Sifu.OpenAngle) timerMaxTorque++;		// если угол открытия тиристоров 0 то пускаем таймер
	if (timerMaxTorque >= (Uns)MOVE_STATE_TIME)	// если находимся в режиме движения дольше 1 с
	{
		if ((Torq.Indication >= Dmc.TorqueSet) || (AbsSpeed < MIN_FREQ_RPM)) // если слищком большой момент или слишком маленькая скорость
			Dmc.WorkMode = wmPause;	// стм. пауза
		timerMaxTorque = 0;		// сбросили таймер 
	}
}
// -----------------------------------------------------------------
__inline void PauseMode(void) // стм. пауза
{
	Sifu.OpenAngle  = SIFU_MAX_ANG; // закрываем тиристоры
	Sifu.AccelTime  = GrC->VoltAcc; //100
	GrH->Torque = Dmc.TorqueSet - 1;	   // момен берем из задания 

	if (++PauseModeTimer >= (Uns)PAUSE_STATE_TIME) // пауза 0.04 секунды. 
	{
		Dmc.WorkMode = wmUporFinish;	// переходим в режим стм. упор финиш
		PauseModeTimer = 0;				// сбросили таймер
	}
}
// -----------------------------------------------------------------
__inline void UporFinishMode(void)	// стм. упор финиш
{
	Sifu.SetAngle   = Torq.SetAngle;// угол по моменту
	Sifu.AccelTime  = GrC->VoltAcc; //100
	GrH->Torque 	= Dmc.TorqueSet - 1; // момент по заданию
	
//	#if 0 // ???
	if(GrH->UporOnly == 1) return;
	if (++UporModeTimer >= (Uns)UPOR_STATE_TIME)	// 2 сек
	{
		if (AbsSpeed > MIN_FREQ_RPM) Dmc.WorkMode = wmMove; // если скорость больше 600 об/мин то уходим обратно в стм. движени
		UporModeTimer = 0;									// сбросили таймер
	}
//	#endif
}
// -----------------------------------------------------------------
__inline void KickMode(void)		// стм. удар
{
	static Uns  KickSetAngle = 0;			// задание угла открытия по ударному режиму
	Sifu.SetAngle   = KickSetAngle;	// полное открытие
	Sifu.AccelTime  = 0;			// макс. быстро
	GrH->Torque = Dmc.TorqueSet - 1;// момент как момент задания
	
	KickModeTimer++;		// пустили таймер удара
	KickSetAngle = 150;		// закрыли тиристоры на 150 град
	if (KickModeTimer >= (Uns)KICK_ST0_TIME) KickSetAngle = 0; // если таймер больше 0.04 то открываем
	if (KickModeTimer >= (Uns)KICK_ST1_TIME)				   // если таймер досчитал до 0.08 то 
	{
		Dmc.WorkMode = wmUporStart;	// вернулись на упор старта
		KickCounter++;				// повысили счетчик режима ударов
		KickModeTimer = 0;			// сбросили таймер
	}
}
// -----------------------------------------------------------------
__inline void SpeedTestMode(void)	// стм. тест
{
	#if BUR_M
	if (!PhEl.Direction)	return;
	#endif	
	
	Sifu.Direction  = ReqDirection;
	Sifu.SetAngle   = GrG->ThyrOpenAngle; // 0
	Sifu.AccelTime  = GrC->VoltAcc;     // 100
	GrH->Torque = Torq.Indication; // как есть
}
// -----------------------------------------------------------------
// режим динамического торможения 
__inline void DynBrakeMode(void) // 200 Hz в ControlMode(void)
{
	Uns pauseTime = 0;			// Время паузы
	Uns brakeTime = 0;			// Время торможения

	Sifu.AccelTime  = 0;										// максимальная скорость открытия
	GrA->Torque = 2;											// не показываем момент
	Torq.Indication = 2;
	//pauseTime = (Uns)DYN_PAUSE_TIME;
	pauseTime = GrC->NoCurrPause*2;								// GrC->NoCurrPause * 200 / 100 =  GrC->NoCurrPause*2
	brakeTime = GrC->BrakeTime * (Uns)CTRLMODE_SCALE;
	Mcu.StartDelay = 0xFFFF; 									// запрещаем обработку команд управления

	if (++DynModeTimer < (brakeTime + pauseTime))	// если время торможения не привысело заданное время
	{
		if (DynModeTimer < pauseTime) 							// если таймер меньше 0.04с то
		{
			Sifu.SetAngle  = SIFU_MAX_ANG;						// угол максимальный
			Sifu.Direction = SIFU_NONE;							// сифу выключенно
			TimerInterp = 0;									// Обнуляем таймер, чтобы в функции AngleInterp сбросилась переменная OutputIQ15
		}
		else // выбор угла в зависимости от чередования фаз
		{
			Sifu.MaxAngle = SIFU_MAX_ANG;					// 180
			Sifu.SetAngle = GrC->BrakeAngle;
			Sifu.OpenAngle = Sifu.SetAngle;
			Sifu.Direction = SIFU_DYN_BREAK; 				// выставляем режим сифу динамическое торможение
		}
	}
	else // если время торможения вышло то уходим в стм. стоп
	{
		Sifu.MaxAngle  = SIFU_MAX_ANG;							// максимальный угол  
		Dmc.WorkMode   = wmStop;
		DynBrakeEnable = False;									// запрещаем режим динамического торможения
		DynModeTimer   = 0;										// сюрасываем таймер дин.торм.
		Mcu.StartDelay = (Uns)START_DELAY_TIME; 				// задаем паузу между пусками
	}
}
// -----------------------------------------------------------------
__inline void DmcTest(void)				// тест работы тиристоров
{
	if (!GrG->SifuEnable)		// если запрещена работа сифу в тестовом режиме
	{									// полностью закрываем сифу и все все все
		Dmc.WorkMode   = wmStop;		// пишем что в стопе 
		Sifu.SetAngle  = Sifu.MaxAngle; // закрываем тиристоры
		Sifu.AccelTime = 0;				// скорость открытия максимальная
		Sifu.Direction = SIFU_NONE;     // выключаем сифу
		GrA->Status.bit.Test = 0;     // сбрасываем бит теста чтобы в следующем такте работать уже по нормальным установкам
		ClkThyrControl(0);				// запрещаем тактирование тиристоров
	}
	else
	{
		Dmc.WorkMode    = wmTestThyr;	// пишем что тестируем тиристоры
		Sifu.SetAngle   = SIFU_OPEN_ANG;// угол открытия 0 градусов
		Sifu.AccelTime  = 0;			// максимальная скорость открытия
		#if BUR_M
		Sifu.Direction = SIFU_UP;
		#else
		Sifu.Direction  = (!GrG->ThyrGroup) ? SIFU_UP : SIFU_DOWN; // выбор группу тиристоров для проверки - прямую или реверсивную
		#endif
		GrA->Status.bit.Test = 1;     // выставляем что находимся в тесте
		ClkThyrControl(1);				// разришаем тактирование тиристоров 
	}
}
// -----------------------------------------------------------------
__inline void TestThyrControl(void)     //сим. тест тиристоров
{
	if (Dmc.WorkMode != wmTestThyr) return;	// если не тест то выходим от сюда от греха
	
	UR.CurAngle = SIFU_OPEN_ANG;			// открываем все тиристоры
	US.CurAngle = SIFU_OPEN_ANG;
	UT.CurAngle = SIFU_OPEN_ANG;
}
// -----------------------------------------------------------------
void CalibStop(void)	// остановка по данным калибровки
{
	Bool StopFlag = False; // внутенний флаг остановки

	if (Mcu.Valve.Position == POS_UNDEF) {Dmc.TargetPos = POS_UNDEF; return;}  // если не знаем текущее положение то ставим целевое в неизвестное и выходим
	Dmc.TargetPos = Calib.LinePos - Mcu.Valve.Position; // если значем положение, то  сложный расчет :)
	
	if (IsStopped() || GrG->TestCamera) return; // если остановленно то выходим
	
	if ((Dmc.RequestDir < 0) && (Dmc.TargetPos <= GrC->BrakeZone))
	{
		StopFlag = True; // если направление вращения закрытие целевое положение достигнуто или привышено то ставим флаг стопа
		calibStopFlag = CLB_CLOSE;	// Выставляем флаг, что привод был остановлен в "Закрыто"
	}
	if ((Dmc.RequestDir > 0) && (Dmc.TargetPos >= -(Int)GrC->BrakeZone))
	{
		StopFlag = True; // если направление вращение открытие
		calibStopFlag = CLB_OPEN;	// Выставляем флаг, что привод был остановлен в "Открыто"
	}
	
	if (StopFlag)	// если нужно останавливаться
	{
		if (Mcu.Valve.BreakFlag) OverWayFlag = 1; // если работаем с уплотнением то выставляем что уплотнение не достигнуто
		else 
			{
				ValveDriveStop(&Mcu, False);		// если без уплотнения то даем команду на стоп с плавным
				Mcu.EvLog.Value = CMD_STOP;			// стоп по конечнику
				Mcu.EvLog.Source = CMD_SRC_BLOCK;	// Источник команды - блок управления
			}
	}
}
// -----------------------------------------------------------------
void TorqueCalc(void)	// расчет момента по кубу
{
	register TCubStr *Cub;	//
	register Int Tmp;
	register Int Add;		// ручной контроль индикации момента
	register Int UporAdd;	// ручной контроль момента упора

	Imfltr.Input = _IQ16toIQ(Mid3UnsValue(Iload[0], Iload[1], Iload[2]));
	ApFilter3Calc(&Imfltr);
	Imid = _IQtoIQ16(Imfltr.Output);
	Imidpr = ValueToPU1(Imid, GrC->Inom);

#if !TORQ_TEST	// если не тест
	if (!Torq.ObsEnable) {Torq.Indication = 0; Torq.SetAngle = Sifu.MaxAngle; return;} //если выключен расчет момента то индикация 0 и закрываем тиристоры и выходим

	Cub = (Imidpr >= GrH->TransCurr) ? &Torq.Cub1 : &Torq.Cub2;  // выбераем по какому кубу работаем для маленьких или больших токов
#else
	Cub = (TqImidPr >= GrH->TransCurr) ? &Torq.Cub1 : &Torq.Cub2; // тестовый расчет момента
#endif
	
	CubCalc(Cub);	// считаем выбранный куб
	
#if !TORQ_TEST

	if (Dmc.TorqueSetPr < 30)		  Add = GrC->Corr25Trq;
	else if (Dmc.TorqueSetPr < 40) 	  Add = GrC->Corr40Trq;		// Добавил PIA 09.10.2012
	else if (Dmc.TorqueSetPr < 60)	  Add = GrC->Corr60Trq;		// корректировка индикации момента для +- изменения времени перехода на поверхность упора			
	else if (Dmc.TorqueSetPr < 80)	  Add = GrC->Corr80Trq;
	else if (Dmc.TorqueSetPr < 110)	  Add = GrC->Corr110Trq;

	Trqfltr.Input = _IQ16toIQ(Cub->Output); // фильтруем значение момента
	ApFilter3Calc(&Trqfltr);				// 
	Tmp =(_IQtoIQ16(Trqfltr.Output)) + Add; 

	if(Dmc.RequestDir > 0)
	{
		Tmp = Tmp + GrC->UporAddOpen;
	}

	if (Tmp < TORQ_MIN_PR) Tmp = TORQ_MIN_PR;	// проверяем на вхождение в зону от 
	if (Tmp > TORQ_MAX_PR) Tmp = TORQ_MAX_PR;   // 0 до 110 %
	
	Torq.Indication = PU0ToValue(Tmp, GrC->MaxTorque * 10);// переводим проценты в Нм относительно максимального М
#else
	Torq.Indication = Cub->Output;	// если тест забираем без фильтра просто число
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

	/*if (Dmc.RequestDir > 0)						// Если направление "Открытие"
		UporAdd = UporAdd + GrC->UporAddOpen;	// Добавляем дополнительную корректировку упора на открытие*/

	Torq.SetAngle   = Torq.Cub3.Output - UporAdd; // забираем задание для сифу (Упор)
}
// -----------------------------------------------------------------
__inline void TorqueObsInit(void)
{	
	switch (GrB->DriveType)
	{
		case dt100_A25_U: PFUNC_blkRead(&drive1,   			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
						  PFUNC_blkRead(&TransCurrDef[0], 	(Int *)(&Ram.GroupH.TransCurr),		  1);
						  GrH->UporOnly = 1;
					      #if BUR_90
						  GrH->PP90Reg.bit.DevOn = 0;
						  #endif
						  if ((GrC->Inom != InomDefU[0])||(GrC->MaxTorque != MomMaxDef[0]))
						  {
						  	if (IsMemParReady())
						  	{
								GrC->GearRatio = GearRatioDef[0];
								GrC->Inom = InomDefU[0];
								GrC->MaxTorque = MomMaxDef[0];
						  		WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3); 
						  	}	
						  }						  
		 		break;//1
		case dt100_A50_U: PFUNC_blkRead(&drive2,   			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
						  GrH->UporOnly = GrC->UporOnly;
						  PFUNC_blkRead(&TransCurrDef[1], 	(Int *)(&Ram.GroupH.TransCurr),		  1);

						  #if BUR_90
						  GrH->PP90Reg.bit.DevOn = 0;
						  #endif

						  if ((GrC->Inom != InomDefU[1])||(GrC->MaxTorque != MomMaxDef[1]))
						  {
						  	if (IsMemParReady())
						  	{
								GrC->GearRatio = GearRatioDef[0];
								GrC->Inom = InomDefU[1];
								GrC->MaxTorque = MomMaxDef[1];
						  		WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3); 
						  	}	
						  }							  
				break;//2
		case dt400_B20_U:   PFUNC_blkRead(&drive3,   			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
					 		GrH->UporOnly = GrC->UporOnly;
						  PFUNC_blkRead(&TransCurrDef[2], 	(Int *)(&Ram.GroupH.TransCurr),		  1);

						  #if BUR_90
						  GrH->PP90Reg.bit.DevOn = 0;
						  #endif
						  if ((GrC->Inom != InomDefU[2])||(GrC->MaxTorque != MomMaxDef[2]))
						  {
						  	if (IsMemParReady())
						  	{
								GrC->GearRatio = GearRatioDef[1];
								GrC->Inom = InomDefU[2];
								GrC->MaxTorque = MomMaxDef[2];
						  		WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3); 
						  	}	
						  }							 
		 		break;//3
		case dt400_B50_U:   PFUNC_blkRead(&drive4,   			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
					 		GrH->UporOnly = GrC->UporOnly;
						  PFUNC_blkRead(&TransCurrDef[3], 	(Int *)(&Ram.GroupH.TransCurr),		  1);

						  #if BUR_90
						  GrH->PP90Reg.bit.DevOn = 0;
						  #endif
						  if ((GrC->Inom != InomDefU[3])||(GrC->MaxTorque != MomMaxDef[3]))
						  {
						  	if (IsMemParReady())
						  	{
								GrC->GearRatio = GearRatioDef[1];
								GrC->Inom = InomDefU[3];
								GrC->MaxTorque = MomMaxDef[3];
						  		WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3); 
						  	}	
						  }	
				break;//4
		case dt800_V40_U:   PFUNC_blkRead(&drive5,   			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
						  GrH->UporOnly = GrC->UporOnly;
						  PFUNC_blkRead(&TransCurrDef[4], 	(Int *)(&Ram.GroupH.TransCurr),		  1);

						  #if BUR_90
						  GrH->PP90Reg.bit.DevOn = 0;
						  #endif
						  if ((GrC->Inom != InomDefU[4])||(GrC->MaxTorque != MomMaxDef[4]))
						  {
						  	if (IsMemParReady())
						  	{
								GrC->GearRatio = GearRatioDef[1];
								GrC->Inom = InomDefU[4];
								GrC->MaxTorque = MomMaxDef[4];
						  		WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3); 
						  	//	ProtectionI2T_Init(&Ram.GroupC.Inom, PRD_50HZ);
						  	}	
						  }			  
				break;//5
		case dt1000_V20_U : PFUNC_blkRead(&drive6,   			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
					GrH->UporOnly = GrC->UporOnly;
						  PFUNC_blkRead(&TransCurrDef[5], 	(Int *)(&Ram.GroupH.TransCurr),		  1);

						  #if BUR_90
						  GrH->PP90Reg.bit.DevOn = 0;
						  #endif
						  if ((GrC->Inom != InomDefU[5])||(GrC->MaxTorque != MomMaxDef[5]))
						  {
						  	if (IsMemParReady()) 
						  	{
								GrC->GearRatio = GearRatioDef[1];
								GrC->Inom = InomDefU[5];
								GrC->MaxTorque = MomMaxDef[5];
						  		WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3); 
						  	}	
						  }	  
		 		break;//6
		case dt4000_G9_U  : PFUNC_blkRead(&drive7,   			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
					GrH->UporOnly = GrC->UporOnly;
						  PFUNC_blkRead(&TransCurrDef[6], 	(Int *)(&Ram.GroupH.TransCurr),		  1);

						  #if BUR_90
						  GrH->PP90Reg.bit.DevOn = 1;
						  #endif
						  if ((GrC->Inom != InomDefU[6])||(GrC->MaxTorque != MomMaxDef[6]))
						  {
						  	if (IsMemParReady())
						  	{
								GrC->GearRatio = GearRatioDef[2];
								GrC->Inom = InomDefU[6];
								GrC->MaxTorque = MomMaxDef[6];
						  		WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3); 
						  	}	
						  }						  
				break;//7
		case dt4000_G18_U : PFUNC_blkRead(&drive8,   			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
					 	GrH->UporOnly = GrC->UporOnly;
						  PFUNC_blkRead(&TransCurrDef[7], 	(Int *)(&Ram.GroupH.TransCurr),		  1);

						  #if BUR_90
						  GrH->PP90Reg.bit.DevOn = 1;
						  #endif
						  if ((GrC->Inom != InomDefU[7])||(GrC->MaxTorque != MomMaxDef[7]))
						  {
						  	if (IsMemParReady())
						  	{
								GrC->GearRatio = GearRatioDef[2];
								GrC->Inom = InomDefU[7];
								GrC->MaxTorque = MomMaxDef[7];
						  		WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3); 
						  	}	
						  }			 				 
				break;//8
		case dt10000_D6_U : PFUNC_blkRead(&drive9,   			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
					 	  GrH->UporOnly = GrC->UporOnly;
						  PFUNC_blkRead(&TransCurrDef[8], 	(Int *)(&Ram.GroupH.TransCurr),		  1);

						  #if BUR_90
						  GrH->PP90Reg.bit.DevOn = 1;
						  #endif
						  if ((GrC->Inom != InomDefU[8])||(GrC->MaxTorque != MomMaxDef[8]))
						  {
						  	if (IsMemParReady())
						  	{
								GrC->GearRatio = GearRatioDef[3];
								GrC->Inom = InomDefU[8];
								GrC->MaxTorque = MomMaxDef[8];
						  		WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3); 
						  	}	
						  }	
				break;//9
		case dt10000_D12_U: PFUNC_blkRead(&drive10,  			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
						  GrH->UporOnly = GrC->UporOnly;
						  PFUNC_blkRead(&TransCurrDef[9], 	(Int *)(&Ram.GroupH.TransCurr),		  1);

						  #if BUR_90
						  GrH->PP90Reg.bit.DevOn = 1;
						  #endif
						  if ((GrC->Inom != InomDefU[9])||(GrC->MaxTorque != MomMaxDef[9]))
						  {
						  	if (IsMemParReady())
						  	{
								GrC->GearRatio = GearRatioDef[4];
								GrC->Inom = InomDefU[9];
								GrC->MaxTorque = MomMaxDef[9];
						  		WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3); 
						  	}	
						  }							  
				break;//10
//-----------------Сарапульские------------------------------------------------------
		case dt100_A25_S:   PFUNC_blkRead(&drive11,   			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
								  PFUNC_blkRead(&TransCurr100A25, 	(Int *)(&Ram.GroupH.TransCurr),		  1);
								  GrH->UporOnly = GrC->UporOnly;

					      #if BUR_90
						  GrH->PP90Reg.bit.DevOn = 0;
						  #endif
								  if ((GrC->Inom != InomDefS[0])||(GrC->MaxTorque != MomMaxDef[0]))
								  {
								  	if (IsMemParReady())
								  	{
										GrC->GearRatio = GearRatioDef[0];
										GrC->Inom = InomDefS[0];
										GrC->MaxTorque = MomMaxDef[0];
								  		WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3);
								  	}
								  }
				 		break;//11
				case dt100_A50_S:
							#if BUR_90
							    PFUNC_blkRead(&drive22,   			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
							    GrH->PP90Reg.bit.DevOn = 0;
							#else
							    PFUNC_blkRead(&drive12,   			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
							#endif
							    GrH->UporOnly = GrC->UporOnly;
								  PFUNC_blkRead(&TransCurrDef[1], 	(Int *)(&Ram.GroupH.TransCurr),		  1);

								  if ((GrC->Inom != InomDefS[1])||(GrC->MaxTorque != MomMaxDef[1]))
								  {
								  	if (IsMemParReady())
								  	{
										GrC->GearRatio = GearRatioDef[0];
										GrC->Inom = InomDefS[1];
										GrC->MaxTorque = MomMaxDef[1];
								  		WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3);
								  	}
								  }
						break;//12
				case dt400_B20_S:   PFUNC_blkRead(&drive13,   			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
							 		GrH->UporOnly = GrC->UporOnly;
								  PFUNC_blkRead(&TransCurrDef[2], 	(Int *)(&Ram.GroupH.TransCurr),		  1);

								  #if BUR_90
						  GrH->PP90Reg.bit.DevOn = 0;
						  #endif
								  if ((GrC->Inom != InomDefS[2])||(GrC->MaxTorque != MomMaxDef[2]))
								  {
								  	if (IsMemParReady())
								  	{
										GrC->GearRatio = GearRatioDef[1];
										GrC->Inom = InomDefS[2];
										GrC->MaxTorque = MomMaxDef[2];
								  		WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3);
								  	}
								  }
				 		break;//13
				case dt400_B50_S:   PFUNC_blkRead(&drive14,   			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
							 		GrH->UporOnly = GrC->UporOnly;
								  PFUNC_blkRead(&TransCurrDef[3], 	(Int *)(&Ram.GroupH.TransCurr),		  1);

								  #if BUR_90
						  GrH->PP90Reg.bit.DevOn = 0;
						  #endif
								  if ((GrC->Inom != InomDefS[3])||(GrC->MaxTorque != MomMaxDef[3]))
								  {
								  	if (IsMemParReady())
								  	{
										GrC->GearRatio = GearRatioDef[1];
										GrC->Inom = InomDefS[3];
										GrC->MaxTorque = MomMaxDef[3];
								  		WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3);
								  	}
								  }
						break;//14
				case dt800_V40_S:   PFUNC_blkRead(&drive15,   			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
								  GrH->UporOnly = GrC->UporOnly;
								  PFUNC_blkRead(&TransCurrDef[4], 	(Int *)(&Ram.GroupH.TransCurr),		  1);

								  #if BUR_90
						  GrH->PP90Reg.bit.DevOn = 0;
						  #endif
								  if ((GrC->Inom != InomDefS[4])||(GrC->MaxTorque != MomMaxDef[4]))
								  {
								  	if (IsMemParReady())
								  	{
										GrC->GearRatio = GearRatioDef[1];
										GrC->Inom = InomDefS[4];
										GrC->MaxTorque = MomMaxDef[4];
								  		WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3);
								  	}
								  }
						break;//15
				case dt1000_V20_S : PFUNC_blkRead(&drive16,   			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
								GrH->UporOnly = GrC->UporOnly;
								  PFUNC_blkRead(&TransCurrDef[5], 	(Int *)(&Ram.GroupH.TransCurr),		  1);

								  if ((GrC->Inom != InomDefS[5])||(GrC->MaxTorque != MomMaxDef[5]))
								  {
								  	if (IsMemParReady())
								  	{
										GrC->GearRatio = GearRatioDef[1];
										GrC->Inom = InomDefS[5];
										GrC->MaxTorque = MomMaxDef[5];
								  		WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3);
								  	}
								  }
				 		break;//16
				case dt4000_G9_S  : PFUNC_blkRead(&drive17,   			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
								GrH->UporOnly = GrC->UporOnly;
								  PFUNC_blkRead(&TransCurrDef[6], 	(Int *)(&Ram.GroupH.TransCurr),		  1);

								  #if BUR_90
						  GrH->PP90Reg.bit.DevOn = 1;
						  #endif
								  if ((GrC->Inom != InomDefS[6])||(GrC->MaxTorque != MomMaxDef[6]))
								  {
								  	if (IsMemParReady())
								  	{
										GrC->GearRatio = GearRatioDef[2];
										GrC->Inom = InomDefS[6];
										GrC->MaxTorque = MomMaxDef[6];
								  		WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3);
								  	}
								  }
						break;//17
				case dt4000_G18_S : PFUNC_blkRead(&drive18,   			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
							 	GrH->UporOnly = GrC->UporOnly;
								  PFUNC_blkRead(&TransCurrDef[7], 	(Int *)(&Ram.GroupH.TransCurr),		  1);
									#if BUR_90
									GrH->PP90Reg.bit.DevOn = 1;
									#endif
								  if ((GrC->Inom != InomDefS[7])||(GrC->MaxTorque != MomMaxDef[7]))
								  {
								  	if (IsMemParReady())
								  	{
										GrC->GearRatio = GearRatioDef[2];
										GrC->Inom = InomDefS[7];
										GrC->MaxTorque = MomMaxDef[7];
								  		WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3);
								  	}
								  }
						break;//18
				case dt10000_D6_S : PFUNC_blkRead(&drive19,   			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
							 	  GrH->UporOnly = GrC->UporOnly;
								  PFUNC_blkRead(&TransCurrDef[8], 	(Int *)(&Ram.GroupH.TransCurr),		  1);
									#if BUR_90
									GrH->PP90Reg.bit.DevOn = 1;
									#endif
								  if ((GrC->Inom != InomDefS[8])||(GrC->MaxTorque != MomMaxDef[8]))
								  {
								  	if (IsMemParReady())
								  	{
										GrC->GearRatio = GearRatioDef[3];
										GrC->Inom = InomDefS[8];
										GrC->MaxTorque = MomMaxDef[8];
								  		WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3);
								   	}
								  }
						break;//19
				case dt10000_D12_S: PFUNC_blkRead(&drive20,  			(Int *)(&Ram.GroupH.TqCurr), LENGTH_TRQ);
								  GrH->UporOnly = GrC->UporOnly;
								  PFUNC_blkRead(&TransCurrDef[9], 	(Int *)(&Ram.GroupH.TransCurr),		  1);

								  #if BUR_90
								  GrH->PP90Reg.bit.DevOn = 1;
								  #endif

								  if ((GrC->Inom != InomDefS[9])||(GrC->MaxTorque != MomMaxDef[9]))
								  {
								  	if (IsMemParReady())
								  	{
										GrC->GearRatio = GearRatioDef[4];
										GrC->Inom = InomDefS[9];
										GrC->MaxTorque = MomMaxDef[9];
								  		WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3);

								  	}
								  }
						break;//20
				    case dt400_B40_U:
					PFUNC_blkRead(&drive21, (Int *) (&Ram.GroupH.TqCurr), LENGTH_TRQ);
					GrH->UporOnly = GrC->UporOnly;
					PFUNC_blkRead(&TransCurrDef[9], (Int *) (&Ram.GroupH.TransCurr), 1);

					#if BUR_90
					GrH->PP90Reg.bit.DevOn = 0;
					#endif

					if ((GrC->Inom != InomDefU[10]) || (GrC->MaxTorque != MomMaxDef[2]))
					{
					    if (IsMemParReady())
					    {
						GrC->GearRatio = GearRatioDef[1];
						GrC->Inom = InomDefU[10];
						GrC->MaxTorque = MomMaxDef[2];
						WritePar(GetAdr(GroupC.MaxTorque), &GrC->MaxTorque, 3);

					    }
					}
					break; //21
	}
} 
// -----------------------------------------------------------------
void SpeedCalc(void) // расчет скорости
{
	static   Uns   Timer = 0;
	static   LgUns RevPrev = -1UL;
	register LgInt Delta;
	
	if (GrG->SimulSpeedMode)	// Если включен режим симуляции скорости
	{							// выставляем подставную скорость
		if (IsOpening()) GrH->Speed = 3000;
		else if (IsClosing()) GrH->Speed = -3000;
		else GrH->Speed = 0;

		AbsSpeed = abs(GrH->Speed);
		return;					// Выходим, не произволя расчетов
	}

	if (++Timer >= (Uns)SP_CALC_TOUT) // интервал расчета скорости 1 сек
	{
		if (RevPrev == -1UL) 		  // 1 итерация
			Delta = 0;				 // дельту в 0
		else
		{
			Delta = Revolution - RevPrev; // дельта = новое значение - предидущие
			if (Delta >  SP_POS_LIMIT) Delta -= (RevMax+1); // если перешли через ноль сдвигаем на половину
			if (Delta < -SP_POS_LIMIT) Delta += (RevMax+1);
		}
		RevPrev = Revolution; // запоминаем предидущие значение

		GrH->Speed = (Delta * (LgInt)GrC->BlockToGearRatio) >> 2; // считаем скорость. изменение положение по времени * на передаточное число = скорость выходного звена
		AbsSpeed = abs(GrH->Speed);	//абсолютная скорость

		Timer = 0;						// сбросили таймер
	}
}
// -----------------------------------------------------------------
void LowPowerControl(void)		// управление при провале напряжения ??? 
{
#if !LOWPOW_TEST
	static Bool ShCFlag = False;
	static Uns  LowPowerTimer  = 0;		// таймер режима провала напряжения
	Uns ShCState = 0;
	
	ShCState = GrH->FaultsLoad.all & LOAD_SHC_MASK;	// смотрим состояние КЗ
	if (!ShCState) ShCFlag = False;					// если нет КЗ то сбросили флаг
	
	if (IsTestMode()) PowerOn();					// если тестовый режим то все включено
	else if (LowPowerTimer > 0) LowPowerTimer--;	// если 
	else if (LowPowerReset & BIT0)					// если выставлен сброс режима провала напряжения
	{
		PowerOn();									// включаем питание
		if (IsMemParReady())						// если готов записывать
		{
			GrH->FaultsLoad.all |= GrH->ScFaults;	//  
			GrH->ScFaults = 0;						// 
//!!!		WritePar(REG_SC_FAULTS, &GrH->ScFaults, 1);
			LowPowerReset &= ~BIT0;					// убераем сброс режима
		}
	}
	else if (!ShCFlag && ShCState)					// если КЗ
	{
		DisplayRestartFlag = true;
		PowerOff();									// выключаем 
		if (IsMemParReady())
		{
			GrH->ScFaults = ShCState;				// Индикация КЗ
			WritePar(REG_SC_FAULTS, &GrH->ScFaults, 1);
			LowPowerReset |= BIT0;					// готовы к ресссету
			LowPowerTimer = (Uns)POWER_TOUT;		// пауза в 3 сек
			ShCFlag = True;						    // флаг КЗ поднят
		}
		
		#if BUR_M
			GrH->ContGroup = cgStop;				// Отключаем контакторы на М
		//GrD->ControlWord = vcwStop;			
		#endif
	}
	#if BUR_M
	else if (GrA->Status.bit.Power)				// 
	#else
	else if (!GrA->Status.bit.Power)			// 
	#endif		
	{	
		if (!PowerSupplyEnable)	// Если источник питания потерял потребление
		{
		    if (GrC->ReversKVOKVZ == 1)
		    {

			secflag = 1;
		    }

			ClkThyrControl(0);				
			Sifu.Direction = SIFU_NONE; // выключели сифу
			DisplayRestartFlag = true;
			PowerOff();
			LowPowerReset |= BIT1;		// готовы к рессету
			LowPowerTimer = 10;
			InitSysCtrl(DSP28_PLLCR(SYSCLK, CLKIN*4), 0);					// Уменьшаем частоту в два раза
		}
		else if (LowPowerReset & BIT1)	// если готовы к продолжению работы
		{
			InitSysCtrl(DSP28_PLLCR(SYSCLK, CLKIN), 0);						// Вернули частота на прежний уровень
			PowerEnable = True;			
#if BUR_M
			if (GrC->ReversKVOKVZ == 1)
			{
			   /* GrD->ControlWord = SaveContGroup;
			    GrH->ContGroup = cgOff;*/
			    secflag = 1;
			    secpausetimer = 3*PRD_200HZ;
			}
			else if  (GrC->ReversKVOKVZ == 0)
			{
			    Sifu.Direction = SaveDirection;
			}
#else
			Sifu.Direction = SaveDirection; //SIFU_NONE; // // едим в том же направлении что и было
#endif
			ClkThyrControl(1);
			DisplayRestartFlag = true;
			PowerOn();					// включаем	
			LowPowerReset &= ~BIT1;     // сбрасываем бит готовности к рессету
		}
		else SaveDirection = Sifu.Direction; // запомнили в каком направлении ехали
	}
	else	// если резервное питание 
	{
	//	DisplayRestartFlag = true;
		PowerOn();	// включили все
		LowPowerReset &= ~BIT1;//убрали рессет 
	}
#endif
}
// -----------------------------------------------------------------
Bool DmcControlRefresh(void) // это в RefreshData
{
	if (!DmcRefrState) return true;	

	switch(++DmcRefrState)
	{
		case 2:
			TorqueObsInit();		
			ProtectionI2T_Init(&i2tOverload, PRD_50HZ);
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
		case 5:	break;
		case 6: CubRefresh(&Torq.Cub1, &GrH->TqCurr);  break;
		case 7: CubRefresh(&Torq.Cub2, &GrH->TqAngUI); break;
		case 8: CubRefresh(&Torq.Cub3, &GrH->TqAngSf);
				DmcRefrState = 0; 
				break;
	}
	
	return !DmcRefrState;
}
// -----------------------------------------------------------------
void CubInit(TCubStr *v, TCubConfig *Cfg)	//инициализация куба
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
// -----------------------------------------------------------------
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
// -----------------------------------------------------------------
void CubCalc(TCubStr *v)
{
	Int A0, A1, B0, B1;
	
	// контроль входных данных на случай если они выпали за пределы куба
	v->Input.X = *v->InputX;
	v->Input.Y = *v->InputY;
	
	if (v->Input.X < v->Min.X) v->Input.X = v->Min.X;
	if (v->Input.X > v->Max.X) v->Input.X = v->Max.X;
	if (v->Input.Y < v->Min.Y) v->Input.Y = v->Min.Y;
	if (v->Input.Y > v->Max.Y) v->Input.Y = v->Max.Y;
	
	// поиск квадрата
	for (v->Num1=0; v->Num1 < CUB_COUNT1-1; v->Num1++)
		if (v->Input.X < v->Points[v->Num1+1][0].X) break;
	for (v->Num2=0; v->Num2 < CUB_COUNT2-1; v->Num2++)
		if (v->Input.Y < v->Points[v->Num1][v->Num2+1].Y) break;

	// расчет точки R
	A0 = v->Points[v->Num1][v->Num2].X;
	A1 = v->Points[v->Num1+1][v->Num2].X;
	B0 = v->Points[v->Num1][v->Num2].Z;
	B1 = v->Points[v->Num1+1][v->Num2].Z;
	
	v->PtR  = B0 + _IQ1div((LgInt)(v->Input.X - A0) * (B1 - B0), (A1 - A0) << 1);
	
	// расчет точки T
	A0 = v->Points[v->Num1][v->Num2+1].X;
	A1 = v->Points[v->Num1+1][v->Num2+1].X;
	B0 = v->Points[v->Num1][v->Num2+1].Z;
	B1 = v->Points[v->Num1+1][v->Num2+1].Z;

	v->PtT  = B0 + _IQ1div((LgInt)(v->Input.X - A0) * (B1 - B0), (A1 - A0) << 1);

	// последний расчет
	A0 = v->Points[v->Num1][v->Num2].Y;
	A1 = v->Points[v->Num1][v->Num2+1].Y;
	B0 = v->PtR;
	B1 = v->PtT;
	
	v->Output = B0 + _IQ1div((LgInt)(v->Input.Y - A0) * (B1 - B0), (A1 - A0) << 1);
}
// -----------------------------------------------------------------
// тактирование тиристоров
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
// -----------------------------------------------------------------
void sifu_calc2(SIFU *v)					//функция сифу - изменена - фаза S и R поменялись местами, т.к. 
{
	v->Status.all = 0x3F;								// выставляем в статусе единички на все используемые ножки и разрешение работы	тоесть 0011 1111 - чтобы сбросить разрешение тиристорам на работу при новом вызове функции
	if (v->Direction == SIFU_NONE)						// если не задано направление работы - тоесть сифу выключенно то
	{
		v->AccelTimer = 0;								// обнуляем таймер для последующего использования
		v->OpenAngle = v->SetAngle; 					// и держим задание на угол открытия на текущем угле открытия
		v->Status.all = 0x3F;
	}
	else if (v->Direction == SIFU_DYN_BREAK)			// Если сейчас режим динамического торможения нового образца
	{
		SifuControlForDynBrake(v);						// управляем тиристорами как при динамическом торможении
	}
	else												// если сифу включенно и есть задание
	{
		v->Status.all &= ~(1<<SIFU_EN_TRN);				// сбрасываем бит включения чтобы не поехать раньше времени Status = 0001 1111

		if (!v->AccelTime) v->OpenAngle = v->SetAngle;	// если открытие происходит на частоте вызова функции тоесть не задана задержка открытия то задаем угол открытия = углу задания
		else if (++v->AccelTimer >= v->AccelTime) 		// если задержка открытия заданна то запускаем таймер  отсчитывания задержки и когда он достчитает
		{
			if (v->OpenAngle < v->SetAngle) v->OpenAngle++;	// если текущий угол открытия меньше угла задания то увеличиваем угол открытия на 1
			if (v->OpenAngle > v->SetAngle) v->OpenAngle--; // если угол открытия больше угла задания то уменьшаем текущий угол на 1
			v->AccelTimer = 0;								// сбрасываем таймер для следующей итерации
		}
																			  // тут происходит синхронизация сифу с входной сетью
		if ((*v->UrAngle >= v->OpenAngle) && (*v->UrAngle < v->MaxAngle - 15))// если угол фазы сети больше угла отктрытия и угол фазы корректен - тоесть не больше 170 градусов
			 v->Status.all &= ~((1<<SIFU_UR)|(1<<SIFU_UR_REV));				  // сбрасываем бит статуса отвечающий за ножку управления тиристора фазы UR. сбрасываем т.к. тиристор открывается нулем Status = 0001 1110

		if ((*v->UsAngle >= v->OpenAngle) && (*v->UsAngle < v->MaxAngle - 15))// аналогично
		 	 v->Status.all &= ~(1<<SIFU_US);								  // тут обнуляем 2 бита для прямой и реверсивной группы фазы S Status = 0000 1101

	  	if ((*v->UtAngle >= v->OpenAngle) && (*v->UtAngle < v->MaxAngle - 15))// аналогично
			 v->Status.all &= ~((1<<SIFU_UT)|(1<<SIFU_UT_REV));				  // тут обнуляем 2 бита для прямой и реверсивной группы фазы S Status = 0001 0011

		switch(v->Direction)		// в зависимости от выбранного направления выдаем управление на тиристоры через статусный регистр
		{
			case SIFU_DOWN: v->Status.all |= ((1<<SIFU_UR)|(1<<SIFU_UT)); break;// если реверсивное вращение то выставляем 1 для битов фазы R S если они были обнулены
			case SIFU_UP:   v->Status.all |= ((1<<SIFU_UR_REV)|(1<<SIFU_UT_REV)); break; // аналогично только единички выставляются для битов реверсивной группы
		}
	}
}
//---------------------Проверка питания источника-------------------------------
void PowerCheck(void)			// 200 Hz 
{
#if PLAT_VERSION_7
	if(!POWER_CONTROL)			// если питание отключено
#else
	if(POWER_CONTROL)			// если питание отключено
#endif
	{
		PowerSupplyCnt++;		// задержка на выключение
		if (PowerSupplyCnt == 5)
		{
		    PowerSupplyEnable = 0;
		}
	}
	else 						// если питание включено
	{
		PowerSupplyEnable = 1;
		PowerSupplyCnt = 0;
	}				
}
//----------Логика управления тиристорами для динамического торможения---------------------------
void SifuControlForDynBrake (SIFU *p)
{
	p->Status.bit.sifu_ENB = 0;

	if (PhEl.Direction < 0)	// чер. фаз "Обратное SRT"
	{
		p->Status.all = 0x001F;					// закрыли на всё торможение

		if ((*p->UsAngle > p->OpenAngle)&&(*p->UsAngle < 145)&&(US.Input > 20))	// смотрим только на фазу S - общая, работает в обоих направлениях
		{														// Даем сигнал на закрытие тиристоров немного заранее
			p->Status.bit.sifu_S = 0;
			if (Ram.GroupC.selectBraking == 0)
			{
				if (ReqDirection == SIFU_DOWN)
					p->Status.bit.sifu_R = 0;
				else if (ReqDirection == SIFU_UP)
					p->Status.bit.sifu_RT = 0;
			}
			else if (Ram.GroupC.selectBraking == 1)
			{
				p->Status.bit.sifu_R = 0;
				p->Status.bit.sifu_RT = 0;
			}
		}
		else
		{
			p->Status.all = 0x001F;
		}
	}
	else if (PhEl.Direction > 0)	// чер. фаз "Прямое RST"
	{
		p->Status.all = 0x001F;					// закрыли на всё торможение

		if ((*p->UsAngle > p->OpenAngle)&&(*p->UsAngle < 145)&&(US.Input > 20))	// смотрим только на фазу S - общая, работает в обоих направлениях
		{														// Даем сигнал на закрытие тиристоров немного заранее
			p->Status.bit.sifu_S = 0;
			if (Ram.GroupC.selectBraking == 0)
			{
				if (ReqDirection == SIFU_DOWN)
					p->Status.bit.sifu_T = 0;
				else if (ReqDirection == SIFU_UP)
					p->Status.bit.sifu_TR = 0;
			}
			else if (Ram.GroupC.selectBraking == 1)
			{
				p->Status.bit.sifu_T = 0;
				p->Status.bit.sifu_TR = 0;
			}
		}
		else
		{
			p->Status.all = 0x001F;
		}
	}
	else
		p->Status.all = 0x003F;
}
//---------------------Плавное изменение угла во времени------------------------------------
Uns AngleInterp(Uns StartValue, Uns EndValue, Uns Time)
{
	static LgInt OutputQ15 = 0;

	if (TimerInterp == 0) OutputQ15 = (LgInt)StartValue << 15;
	else OutputQ15 = OutputQ15 - _IQ15div(((LgInt)(StartValue - EndValue) << 15), _IQ15mpy((LgInt)Time * 3277, _IQ15(200.0)));	// При переносе функции из другого проекта, будьте внимательными!!

	TimerInterp++;
	
	if (OutputQ15 <= ((LgInt)EndValue << 15)) return EndValue;
	else if (OutputQ15 >= ((LgInt)StartValue << 15)) return StartValue;
		 else return (Uns)(OutputQ15 >> 15);
} 
//-----------Конец файла------------------------------ 
