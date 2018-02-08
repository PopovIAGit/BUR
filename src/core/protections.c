#include "config.h"
#include "protectionI2T.h"

DIFF_INPUT Muf = MUF_DEFAULT(&Revolution, 0); 	// Защита по перемещению

TPrtElem UvR = UV_DEFAULT(&Ram.GroupA.Ur, 0); 			// пониженное напряжение 
TPrtElem UvS = UV_DEFAULT(&Ram.GroupA.Us, 1);
TPrtElem UvT = UV_DEFAULT(&Ram.GroupA.Ut, 2);

TPrtElem UvR_Def = UVD_DEFAULT(&Ram.GroupA.Ur, 0); 		// пониженное напряжение неисправность
TPrtElem UvS_Def = UVD_DEFAULT(&Ram.GroupA.Us, 1);
TPrtElem UvT_Def = UVD_DEFAULT(&Ram.GroupA.Ut, 2);

TPrtElem OvR = OV_DEFAULT(&Ram.GroupA.Ur, 3); 			// повышенное напряжение
TPrtElem OvS = OV_DEFAULT(&Ram.GroupA.Us, 4);
TPrtElem OvT = OV_DEFAULT(&Ram.GroupA.Ut, 5);

TPrtElem OvR_Def = OVD_DEFAULT(&Ram.GroupA.Ur, 3); 		// повышенное напряжение немсправность
TPrtElem OvS_Def = OVD_DEFAULT(&Ram.GroupA.Us, 4);
TPrtElem OvT_Def = OVD_DEFAULT(&Ram.GroupA.Ut, 5); 

TPrtElem OvR_max = OV_MAX_DEFAULT(&Ram.GroupA.Ur, 11); 	// повышенное напряжение 47%
TPrtElem OvS_max = OV_MAX_DEFAULT(&Ram.GroupA.Us, 12);
TPrtElem OvT_max = OV_MAX_DEFAULT(&Ram.GroupA.Ut, 13);

TPrtElem Vsk = VSK_DEFAULT(&Ram.GroupH.VSkValue, 7); 	// небаланс напряжений

TPrtElem BvR = BV_DEFAULT(&Ram.GroupA.Ur, 8); 			// Обрыв фаз
TPrtElem BvS = BV_DEFAULT(&Ram.GroupA.Us, 9);
TPrtElem BvT = BV_DEFAULT(&Ram.GroupA.Ut, 10);

TPrtElem PhlU = LI_DEFAULT(&Ipr[0], 0);					// пониженный ток 
TPrtElem PhlV = LI_DEFAULT(&Ipr[1], 1);
TPrtElem PhlW = LI_DEFAULT(&Ipr[2], 2);
/*
TPrtElem I2tMin = IT_DEFAULT(&Imidpr, 0, 3);			// время токовая защита 1 ступень
TPrtElem I2tMid = IT_DEFAULT(&Imidpr, 1, 3);			// 2 ступень
TPrtElem I2tMax = IT_DEFAULT(&Imidpr, 2, 3);			// 3 ступень*/

//TPrtElem IUnLoad = IUL_DEFAULT(&Imidpr, 10);			// минимальная токовая (пропажа нагрузки)
TPrtElem ISkew   = ISK_DEFAULT(&Ram.GroupH.ISkewValue, 11); 	// небаланс токов

TPrtElem Th = OT_DEFAULT(&Ram.GroupA.Temper, 5);		// защита от перегрева
TPrtElem Th_Err = OTE_DEFAULT(&Ram.GroupA.Temper, 11);	// защита от перегрева
TPrtElem Tl = UT_DEFAULT(&Ram.GroupA.Temper, 6);		// Защита от "Замерзания"	


TPrtElem DrvT = DT_DEFAULT(&Ram.GroupC.DrvTInput , 6);
//LOG_INPUT DrvT = DT_DEFAULT(6, False);					// Защита от перегрева двигателя - аналогично кнопкам

TFltUnion Faults;
TFltUnion Defects;// Неисправности
 
Bool  onlyPosSens = false;		// Флаг, означающий, что сбой датчика положения - единственная авария
Uns   ShCLevel       = 32767; 	// максимальное инт число показывает уровень срабатывания защиты по кз
Uns   MuffFlag       = 0;	  	// флаг срабатывания муфты
Int   EngPhOrdValue  = 0; 		// показывает чередование фаз для направления вращения (0 небыло, 1 вперед, -1 назад)

Uns   OverWayFlag 	 = 0; 		// флаг недостигнутости уплотнения 
Uns   OtTime         = 0;
Uns   UtTime         = 0;

Uns   DrvTTout   	 	= (Uns)DRV_TEMPER_TOUT;
Uns   Fault_Delay 	 	= (Uns)FLT_DEF_DELAY;
Int   HighTemper		= 110;
Uns   VskTimer 			= 0;
Uns   FlagEngPhOrd		= 0;
Uns   PhOrdTimer 		= 0;

Bool IsShcReset = false;			// флаг сброса КЗ

__inline void DefDriveFaults(void);	
__inline void ShCProtect(void);

void ProtectionsInit(void)	// начальная инициализация для защит
{
	memset(&Faults, 		0,		sizeof(TFltUnion));	// Выставили все структуры с ошибками в 0 (нет ошибок)
	memset(&Defects, 		0,		sizeof(TFltUnion));	// Выставили все структуры с ошибками в 0 (нет ошибок)
	memset(&GrH->FaultsDev, 0,		sizeof(TDeviceReg));// Выставили все структуры с ошибками в 0 (нет ошибок)
	//!!!
	memset(&GrH->FaultsLoad, 0,		sizeof(TLoadReg));// Выставили все структуры с ошибками в 0 (нет ошибок)
	memset(&GrH->FaultsNet, 0,		sizeof(TNetReg));// Выставили все структуры с ошибками в 0 (нет ошибок)
 	PhOrdTimer = 0;
	EngPhOrdValue  = 0;



	//-------------Времятоковая перегрузка-------------------------------------------
	i2tOverload.inputCurr 		= &Ram.GroupH.Imid;
	i2tOverload.nominalCurr 	= &Ram.GroupC.Inom;
	ProtectionI2T_Init(&i2tOverload, PRD_50HZ);
}

void ProtectionsUpdate(void)// периодическое обновление в защитах
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

	ShCProtect();		//проверяем КЗ
	DefDriveFaults();	//проверяем наличие других ошибок

	GrC->DrvTInput = DRIVE_TEMPER - 10000;
	if(GrG->TestCamera && GrA->Faults.Proc.bit.Drv_T) GrA->Faults.Proc.bit.Drv_T = 0;
	if(GrG->TestCamera && GrA->Faults.Proc.bit.MuDuDef) GrA->Faults.Proc.bit.MuDuDef = 0;
	GrH->FaultsLoad.bit.I2t = i2tOverload.isFault;
}

void ProtectionsEnable(void)// проверка включения защит ЭД
{
	static Byte State = 0;
	Bool Enable;

	if(Fault_Delay > 0) return;

	switch(++State)
	{
		case 1: 				// включение по муфте
				Muf.Enable = ((Dmc.WorkMode & wmMuffEnable) != 0) && !GrG->SimulSpeedMode && !ZazorTimer && !KickModeEnable && !GrG->TestCamera;
		break;
		case 2: 				// включение по напряжению
			
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
		case 3: 				// включение по току
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
		case 4: 				// включение по температуре
				Enable = (GrC->TemperTrack != pmOff);
				Th.Cfg.bit.Enable = Enable;
				Th_Err.Cfg.bit.Enable = Enable;
				Tl.Cfg.bit.Enable = Enable;

				Enable = (GrC->DriveTemper != pmOff) && !GrG->TestCamera;
				DrvT.Cfg.bit.Enable = Enable;
				State = 0;		// вернулись к истокам
		break;
	}

	if (IsShcReset && IsMemParReady())
	{
		IsShcReset = false;
		GrH->ScFaults = 0;
		WritePar(REG_SC_FAULTS, &GrH->ScFaults, 1);
	}
}

void ProtectionsClear(void)	// полная отчистка ошибок (включая ошибки энкодера и микросхем)
{
	OverWayFlag = 0;
	MuffFlag = 0;

	GrA->Status.all &= ~STATUS_RESET_MASK;			//сбросили ошибку и муфту

	GrA->Faults.Proc.all 	= 0;					//сбросили ошибки процесса 
	GrH->FaultsNet.all      = 0;					//сбросили ошибки сети (U)
	GrH->FaultsLoad.all     = 0;					//сбросили ошибки нагрузки (I)
	GrH->FaultsDev.all     	= 0;					//сбросили ошибки устройства
	
	Encoder.Error  = False;
	enDPMA15.Error = false;
	Eeprom1.Error  = False;
	Eeprom2.Error  = False; 
	TempSens.Error = False;

	IsShcReset = true;
	i2tOverload.isFault = false;
	Encoder.skipDefectFl = 0;

}

void ProtectionsReset(void)	// сброс силовых защит(для возможного пуска двигателя)
{
	OverWayFlag = 0;
	MuffFlag = 0;

	GrA->Status.all &= ~STATUS_RESET_MASK;	
	
	GrA->Faults.Proc.all &= ~PROC_RESET_MASK;	// сбросили ошибки процесса (нет движен, неправильное чередование фаз, уплотнение не достигнуто) 
	GrH->FaultsLoad.all  &= ~LOAD_RESET_MASK;	// сбросили ошибки нагрузки (все)
	#if BUR_M
	GrH->FaultsNet.all   &= ~NET_RESET_MASK;					
	#endif
}

void ProtectionsControl(void)	// для расчета ассиметрии
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
			Dif1 = abs(GrA->Ur - GrH->Umid);			//Пример:   Ur = 222  Umid = 220 Dif1 = 2
			Dif2 = abs(GrA->Us - GrH->Umid);			//			Us = 218			 Dif2 = 2
			Dif3 = abs(GrA->Ut - GrH->Umid);			//			Ut = 220			 Dif3 = 0
			Tmp  = Max3UnsValue(Dif1, Dif2, Dif3);		//находим максимальное значение, или первое среди равных. тмп = 2			
			GrH->VSkValue = ValueToPU0(Tmp, GrH->Umid);	// (2/220)*100 = 0.9% небаланс
			
		}
	}
	#else
	// расчет значения входа для защиты от асиметрии входных напряжений
	Dif1 = abs(GrA->Ur - GrH->Umid);			//Пример:   Ur = 222  Umid = 220 Dif1 = 2
	Dif2 = abs(GrA->Us - GrH->Umid);			//			Us = 160			 Dif2 = 2
	Dif3 = abs(GrA->Ut - GrH->Umid);			//			Ut = 220			 Dif3 = 0
	Tmp  = Max3UnsValue(Dif1, Dif2, Dif3);		//находим максимальное значение, или первое среди равных. тмп = 2			
	GrH->VSkValue = ValueToPU0(Tmp, GrH->Umid);	// (2/220)*100 = 0.9% небаланс
	#endif

	if (IsStopped()) 							// если дв. отключен то не считаем
		GrH->ISkewValue = 0;
	else										// иначе
	{
		Dif1 = abs(Ipr[0] - Imidpr);			// как с напряжением
		Dif2 = abs(Ipr[1] - Imidpr);
		Dif3 = abs(Ipr[2] - Imidpr);
		Tmp  = Max3UnsValue(Dif1, Dif2, Dif3);
		GrH->ISkewValue = ValueToPU0(Tmp, Imidpr);
	}
}

void FaultIndication(void)				// индикация ошибок устройства и технологического процесса			
{
	static Uns 	tempMotorTimer = 0;

	if(Fault_Delay > 0) {Fault_Delay--; return;}

	GrA->Faults.Proc.bit.Overway = OverWayFlag;	// если уплотнение недастигнуто
	
	GrA->Faults.Proc.all &= ~PROC_CALIB_MASK;	// сбросили ошибки калибровки
	if (GrC->CalibIndic != pmOff)				// если защита по тех. процессу не отключенаы
	{
		GrA->Faults.Proc.bit.NoClose = IsNoClosePos();	// индикация отсутсвия калибровки на "закрыто"
		GrA->Faults.Proc.bit.NoOpen  = IsNoOpenPos();		// индикация отсутсвия калибровки на "открыто"
		GrA->Faults.Proc.bit.NoCalib = IsNoCalib();		// индикация отсутсвия калибровки
	}
	
	#if BUR_M
	GrH->FaultsNet.bit.PhOrd = 0;	// если бур м то нет такой аварии
	#else

//	GrH->FaultsNet.bit.PhOrd = !PhEl.Direction;				// ошибка висит если чередование фаз СЕТИ не определенно
	GrH->FaultsNet.bit.PhOrd = 0;	
	#endif

	GrH->FaultsDev.all &= ~DEV_ERR_MASK;					// сбросили ошибки устройства

	#if BUR_90
	// Обработка защиты от перегрева двигателя для БУР_90
	if (!PiData.Connect)					// Если нет связи с ПИ
	{
		PrtCalc(&DrvT);						// Защита работает по классическому методу
	}										// Иначе
	else if ( (GrC->DriveTemper != pmOff) && !GrG->TestCamera )
	{										// Смотрим на приходящий с ПИ сигнал
		if (ExtReg >> SBEXT_TEMPER_M)		// Если с ПИ пришел сигнал о перегреве двигателя
		{
			if (tempMotorTimer++ > PRD_50HZ)
			{								// .. то выставляем аварию при переполнении таймера
				tempMotorTimer = 0;
				GrA->Faults.Proc.bit.Drv_T = 1;
			}
		}
		else tempMotorTimer = 0;
	}
	else GrA->Faults.Proc.bit.Drv_T = 0;
	#endif

	if (GrC->ErrIndic != pmOff)							// если индикация ошибок не выключина
	{
		GrH->FaultsDev.bit.Memory1 = Eeprom1.Error;			// еепром 1 
		GrH->FaultsDev.bit.Memory2 = Eeprom2.Error;			// eeprom 2
		if(!GrC->RTCErrOff)
		{
			GrH->FaultsDev.bit.Rtc     = Ds3234.Error;			// часы
		}
		GrH->FaultsDev.bit.TSens   = TempSens.Error;		// температура
		GrH->FaultsDev.bit.AVRcon  = !PiData.Connect;		// наличие конекта до АВР
		/*
		#if !BUR_M
		GrH->FaultsDev.bit.LowPower = !PowerEnable;			// Включен режим сохранения энергии
		#endif
		*/
	}
	if (GrC->PosSensEnable != pmOff)
	{
		if(GrC->EncoderType == 0)
		{
			GrH->FaultsDev.bit.PosSens = Encoder.Error;			// энкодер
		}
		else
		{
			GrH->FaultsDev.bit.PosSens = enDPMA15.Error;			// энкодер
		}
	}

//	#if !BUR_M
//	GrH->FaultsDev.bit.LowPower = !PowerEnable;				// Включен режим сохранения энергии
//	#endif
}

__inline void DefDriveFaults(void)		// реакция на ошибки, системой
{
	Uns MufEnable;
	
	GrA->Status.bit.Fault = IsFaultExist(pmSignStop);	// если произошла срабатывание защиты и она включена
	GrA->Status.bit.Defect = IsDefectExist(pmSignStop); // выставляем в статус работы ошибку

	if (!IsStopped())					// если не остановленно
	{
		if (MuffFlag)					// если пришел сигнал с муфты
		{
			if (IsNoCalib()) MufEnable = !Calib.AutoStep;	// если не откалиброванно (если идет автокалибровка то отдастя 0 и ошибки не будет иначе отдаем 1 и выдается ошибка)
			else if (!Dmc.RequestDir) MufEnable = True;		// если нет вращения и не определенно направление
			else if (Dmc.RequestDir > 0) MufEnable = !IsOpened();// если направление движение не совпадает с заданным
			else MufEnable = !IsClosed();	//	аналогично
			
		
		if (Mcu.Valve.BreakFlag && BreakFlag)// выставляем муфту и аварии во всех случаях кроме зоны уплотненияы
			{
				MufEnable = 0;
			}

		GrA->Faults.Proc.bit.NoMove = MufEnable;// выставляем ошибку по отсутсвию движения или неправильном направлении вращения
		GrA->Status.bit.Mufta = MufEnable;// выставляем в статус что сработала муфта
			
	
			ValveDriveStop(&Mcu, True);//даем команду на остановку (без плавного)

			if (Mcu.EvLog.Value) Mcu.EvLog.QueryValue = CMD_DEFSTOP;
			else Mcu.EvLog.Value = CMD_DEFSTOP;	//???  Псевдокоманда для
		}

		if (IsFaulted())
		{
			if (!onlyPosSens || GrC->PosSensEnable == pmSignStop)
			{
				ValveDriveStop(&Mcu, True);	// если в статусе прочитали ошибку то даем команду на остановку 
				if (Mcu.EvLog.Value) Mcu.EvLog.QueryValue = CMD_DEFSTOP;
				else Mcu.EvLog.Value = CMD_DEFSTOP;
			}
		}

		if (IsDefected())
		{
		//	ValveDriveStop(&Mcu, True);	// если в статусе прочитали ошибку то даем команду на остановку 
		//	if (Mcu.EvLog.Value) Mcu.EvLog.QueryValue = CMD_DEFSTOP;
		//	else Mcu.EvLog.Value = CMD_DEFSTOP;
		}
		
	}
}

Bool IsDefectExist(TPrtMode Mode) // неисправность
{
	Defects.Proc.all = GrA->Faults.Proc.all; 	//забираем значение структур с ошибками   
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
	// удоляем то что является аварией
	if(Defects.Proc.bit.Drv_T)		Defects.Proc.bit.Drv_T 	 			 = 0;	// перегрев дв
	if(Defects.Proc.bit.NoMove)		Defects.Proc.bit.NoMove  			 = 0;	// нет движения
	if(Defects.Proc.bit.CycleMode)  Defects.Proc.bit.CycleMode			 = 0;
	if(Defects.Proc.bit.PhOrd)		Defects.Proc.bit.PhOrd 	 			 = 0;
	if(Defects.Dev.bit.PosSens)		Defects.Dev.bit.PosSens 	 		 = 0;
	if(Defects.Net.all  & NET_BV_MASK)			Defects.Net.all 		&=~NET_BV_MASK;		// обрыв вх фаз
	if(Defects.Load.all & LOAD_SHC_MASK)		Defects.Load.all 		&=~LOAD_SHC_MASK;	// КЗ
	if(Defects.Load.all & LOAD_I2T_MASK)		Defects.Load.all 		&=~LOAD_I2T_MASK;	// ВТЗ
 
	if(GrC->CalibIndic 		< Mode)	Defects.Proc.all 		&=~PROC_CALIB_MASK;	// Наличие калибровки
	if(GrC->MuDuDef 		< Mode) Defects.Proc.bit.MuDuDef = 0;   // Ошибка сигнала входов Му/Ду

	if(pmBlkTsSign   		< Mode) Defects.Net.all 			&=~NET_UV_MASK;		// Пониженное напряжение 
	if(pmBlkTsSign    		< Mode) Defects.Net.all 			&=~NET_OV_MASK;		// Повышенное 31% - только сигнализация
	if(GrC->VSk     		< Mode) Defects.Net.all 			&=~NET_VSK_MASK;	// Небаланс напряжений

	if(GrC->Phl				< Mode) Defects.Load.all 		&=~LOAD_PHL_MASK; 	// Обрыв дв 
  //	if(GrC->IUnLoad			< Mode) Defects.Load.all 		&=~LOAD_UNL_MASK;	// Пониженный ток
	if(GrC->ISkew			< Mode) Defects.Load.all 		&=~LOAD_ISK_MASK;	// Небаланс токов

	if(GrC->TemperTrack		< Mode) Defects.Dev.all 			&=~DEV_TMP_MASK;	// Перегрев, охлаждение блока - останов

	if(GrC->ErrIndic		< Mode) Defects.Dev.all 			&=~DEV_ERR_MASK;	// Неисправность Блока управления

	if(Defects.Proc.all) 			 		return true;	// если велючино и сработало возвращаем тру
	if(Defects.Net.all)  			 		return true;
	if(Defects.Load.all) 			 		return true;
	if(Defects.Dev.all) 					return true;	// проверять

	return false;						// если выключино или не сработало возвращаем фолс

}

Bool IsFaultExist(TPrtMode Mode) // сигнализация и выключение двигателя
{
// Mode - это параметр, который показывает что нужно делать при возникновении события
// например - ничего -0, включить индикацию и включить ТС и переход в СТОП - 3 1-только индикация 2-индикация и ТС
// Проверка на неправильное чередование фаз СЕТИ выполняется всегда

	//Faults.Proc.all = GrA->Faults.Proc.all; 	//забираем значение структур с ошибками
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

		if(GrC->DriveTemper		< Mode)	Faults.Proc.bit.Drv_T 	 = 0;	// сбросили защиту по перегреву ДВ
		if(GrC->PhOrd			< Mode)	Faults.Proc.bit.PhOrd 	 = 0;	// Направление вращения дв - останов

		if(GrC->Bv   			< Mode) Faults.Net.all 			&=~NET_BV_MASK;
		if(GrC->Ov    			< Mode) Faults.Net.all 			&=~NET_OV_MASK;		// Повышенное 31%
		if(GrC->Ov    			< Mode) Faults.Net.all 		    &=~NET_OV_MAX_MASK;	// Повышенное 47% - останов
		if(GrC->Uv   			< Mode) Faults.Net.all 			&=~NET_UV_MASK;		// Пониженное напряжение 
		#if BUR_M
		if(GrC->RST_Err			< Mode) Faults.Net.bit.RST_Err   = 0;
		#endif
		if(GrC->I2t				< Mode) Faults.Load.all 		&=~LOAD_I2T_MASK;
		if(GrC->ShC				< Mode) Faults.Load.all 		&=~LOAD_SHC_MASK;
		if(pmSignStop			< Mode) Faults.Dev.bit.Th_Err 	 = 0;	// Перегрев,  - останов
	
	 break;
	case spFire: 
	
		if(pmBlkTsSign			< Mode)	Faults.Proc.bit.Drv_T 	 = 0;	// сбросили защиту по перегреву ДВ
		if(pmBlkTsSign			< Mode)	Faults.Proc.bit.PhOrd 	 = 0;	// Направление вращения дв - останов

		if(pmBlkTsSign   		< Mode) Faults.Net.all 			&=~NET_BV_MASK;
		if(pmBlkTsSign    		< Mode) Faults.Net.all 			&=~NET_OV_MASK;		// Повышенное 31%
		if(pmBlkTsSign    		< Mode) Faults.Net.all 		    &=~NET_OV_MAX_MASK;	// Повышенное 47% - останов
		if(pmBlkTsSign   		< Mode) Faults.Net.all 			&=~NET_UV_MASK;		// Пониженное напряжение 
		#if BUR_M
		if(pmBlkTsSign			< Mode) Faults.Net.bit.RST_Err   = 0;
		#endif
		if(pmBlkTsSign			< Mode) Faults.Load.all 		&=~LOAD_I2T_MASK;
		if(GrC->ShC				< Mode) Faults.Load.all 		&=~LOAD_SHC_MASK;
		if(pmBlkTsSign			< Mode) Faults.Dev.bit.Th_Err 	 = 0;	// Перегрев,  - сигнализация	

	break;
}

	if(Faults.Proc.bit.Drv_T)				return true;	// перегрев дв
	if(Faults.Proc.bit.PhOrd)				return true;	// неверное чередование фаз двигателя
	if(Faults.Proc.bit.NoMove)				return true;	// нет движения
	if(Faults.Net.all  & NET_BV_MASK)		return true;	// обрыв вх фаз
	if(Faults.Net.all  & NET_UV_MASK)		return true;	// минимальное -50%
	if(Faults.Net.all  & NET_OV_MASK)		return true;	// максимальное напряжение +31%
	if(Faults.Net.all  & NET_OV_MAX_MASK)	return true;	// максимальное +47%
	#if BUR_M
	if(Faults.Net.all  & NET_RTSERR_MASK)	return true;	// неверное чередование фаз сети
	#endif
	if(Faults.Load.all & LOAD_SHC_MASK)		return true;	// КЗ
	if(Faults.Load.all & LOAD_I2T_MASK)		return true;	// ВТЗ
	if(Faults.Dev.all)
	{
		if (Faults.Dev.all == 1) onlyPosSens = true;
		return true;	// температура 110град и сбой датчика положения
	}

	return false;						// если выключино или не сработало возвращаем фолс

}
__inline void ShCProtect(void)			// проверка на КЗ 
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
			
void EngPhOrdPrt(void)					// проверка на правильность чередования фаз (ЧЕРЕДОВАНИЕ ФАЗ ДВИГАТЕЛЯ)
{
	          // флаг для единичной проверки правельности чередования фазы
	static LgUns StartPos;
	static Uns   Timer = 0;
	static Uns   PhOrdZone = 10;
    LgInt data = (RevMax + 1)/2;
	LgInt  Delta = 0;

//	if(Fault_Delay>0) return;

 	 #if BUR_M
	GrC->PhOrd = pmSignStop;
	if (IsStopped())	// если стоим то выходим, всегда включена
	{
		FlagEngPhOrd = False;					// сбросили флаг если выключали останавливали или откалибравали на данное чередование
		StartPos = Revolution;	// стартовое положение это текущее положение с энкодера
		Timer = 0;						// сбросили таймер
		PhOrdTimer = 0;
		return;							// вышли
	}

	 #else 
	if ((GrC->PhOrd == pmOff) || IsStopped())	// если защита от нерпав. черед. фаз выключина или все остановленно или откалеброванно 
	{
		FlagEngPhOrd = False;					// сбросили флаг если выключали останавливали или откалибравали на данное чередование
		StartPos = Encoder.Revolution;	// стартовое положение это текущее положение с энкодера
		Timer = 0;						// сбросили таймер
		return;							// вышли
	}
	 #endif

	if (Timer < (GrC->PhOrdTime * (Uns)PRT_SCALE)) Timer++;	// привели таймаут к 0.1 с и если таймер меньше инкрементировали	
	else if (!FlagEngPhOrd && !ZazorTimer)								// если не флаг и таймер зазора = 0 то
	{												// где REV_MAX максимальное численное значение энкодера
		Delta = Revolution - StartPos;				// посчитали дельту смещения как текущее положение минус запомненое начальное положение

		if (GrC->EncoderType == 0)
		{
			if (Delta > ((RevMax + 1) / 2))
				Delta -= (RevMax + 1);// если дельта больше 2000 то декрементируем дельту на 4000
			if (Delta < -((RevMax + 1) / 2))
				Delta += (RevMax + 1);// если дельта меньше -2000 то инкрементируем дельту на 4000
		}
		else if (GrC->EncoderType == 1)
		{
			if (Delta > data)
				Delta -= (RevMax + 1);// если дельта больше 2000 то декрементируем дельту на 4000
			if (Delta < -data)
				Delta += (RevMax + 1);// если дельта меньше -2000 то инкрементируем дельту на 4000
		}


		EngPhOrdValue = 0;	// сбрасываем чередование фаз, останется нулем если вращения небыло
	
		if (Delta >=  ((LgInt)PhOrdZone)) EngPhOrdValue =  1; // если дельта больше расстояния чередования фаз то чередование фаз на прямое направление вращения
		if (Delta <= -((LgInt)PhOrdZone)) EngPhOrdValue = -1; // если дельта меньше расстояния чередования фаз то чередование фаз на инверсное направление вращения
		
		if (GrB->RodType == rdtInverse) EngPhOrdValue = -EngPhOrdValue; // если тип штока инверсный то разворачиваем
		
		if ((EngPhOrdValue != 0) && (EngPhOrdValue != Dmc.RequestDir))		// если движение было но напрвление вращения не совпадает с заданным то 
			GrA->Faults.Proc.bit.PhOrd = 1;								// выставляем ошибку чередования фаз (тех. процесс)

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
	//	Flag = True;	// выставляем флаг проверки защиты в тру, показываем про проверка проводилась вне зависимости от результата
		#endif
	}
}

