/*------------------------
interface.c

Интерфейсный модуль
--------------------------*/
#define CREATE_DRIVE_DATA
#define CREATE_STRUCTS
//#define USE_DEFPAR_VALUES

#include "config.h"



TLogEv			LogEv = LOG_EV_DEFAULTS;				// Экземпляр структуры журнала событий
TLogEvBuffer	LogEvBuffer[LOG_EV_BUF_CELL_COUNT];		// Буфер параметров, которое записываются до наступления события
TLogCmd			LogCmd = LOG_CMD_DEFAULTS;				// Экземпляр структуры журнала команд
TLogParam 		LogParam = LOG_PARAM_DEFAULTS;			// Экземпляр структуры журнала изменения параметров

CALIBS      Calib = CALIBS_DEFAULT;
TVlvDrvCtrl Mcu   = DRIVE_CTRL_DEFAULT;
//TOutputReg  Dout  = {0x00};

MENU        Menu  = MENU_DEFAULT;					// Экземпляр структуры меню

Uns ImReadBuf[IM_READ_BUF_SIZE * 2];			// Буфер информационного модуля
TIM			Im = IM_DEFAULT;

Byte InterfRefrState = 0;
Bool DataError = false;

char LogEvBufIndex = 0;
bool LogEvMainDataFlag = false;
Byte LogEvBufCurIndex = 0;
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

Bool ClearLogFlag = False;	// очистка журнала ???
Uns  ClearLogSource = 0;
Uns  DefFlag = 0;			// Возвращение к дефайновым параметрам 
Uns  DefCode;
Uns  DefAddr;
Uns  CancelTimer = 0;		// время до отмены

Bool IsDriveTypeShow = true;
Uns ShowDriveTypeTimer = 0;
Uns PduKeyFlag = 0;

Uns DbgLog = 0;
Uns TempMuDu = 0;
Uns MuDuDefTimer = 0;

__inline void CheckParams(void);
__inline void DefParamsSet(Uns Code);
static   void PutAddData(Uns Addr, Uns *Value);
static   void UpdateCode(Uns Addr, Uns *Code, Uns Def);

Uns PowerOffCnt = 0;
Uns PowerOnCnt = 0;


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
	GrC->MkuPoSubVersion = (MODULE_VERSION * 100) + SUBVERSION;

	if (GrH->ScFaults) LowPowerReset |= BIT0;

	RefreshData();

	// Выводим момент
	Ram.GroupH.StartIndic =REG_TORQUE_ADDR; //8;

	// Включаем экспресс настройку
	if (IsNoCalib()) Menu.Express.Enable = TRUE;

	#if BUR_M
	GrB->DuSource = mdsSerial;
	#endif

	//memset(&RamTek, 0, sizeof(TTEKDriveData));

	// Подключаем функции Bluetooth к информационному модулю
	Im.EnableReceive = Bluetooth.EnableRx;
	Im.EnableTransmit = Bluetooth.EnableTx;
	Im.TransmitByte = Bluetooth.TransmitByte;
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
	GrA->CycleCnt 		 = GrH->CycleCnt;
}

void RefreshData(void)
{
	static Bool Flag = false;
	
	RimRefrState    = 1;
	DmcRefrState    = 1; 	
	InterfRefrState = 1;

	SerialCommRefresh();
	
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
		if ((Dcr.Config.all & DefCode) == DefCode)
	 		*(ToUnsPtr(&Ram) + DefAddr) = Dcr.Def;
		DefAddr++;
		return;
	}
	
	if (IsMemParReady())
	{
		WriteAllParams();
		RefreshData();
		DefFlag = 2;
	}
}

void PowerOn(void)
{
	static Bool DisplShcFlag = true;

	LcdEnable = 1;

// 1) Display.Restart = 5;		
// 2) Подождали
// 3) Display.Restart = 0;

	if (DisplayRestartFlag)
	{
		DisplayTimer = (PRD_10HZ * 1);
	//	DisplayRestartTimer = (PRD_10HZ * 2);
		DisplayRestartFlag = false;
	}
	
	// Смотрим при включении, если было КЗ
	if (DisplShcFlag && (GrH->FaultsLoad.bit.ShCU || GrH->FaultsLoad.bit.ShCV || GrH->FaultsLoad.bit.ShCW))
	{
		DisplayRestartTimer = (PRD_10HZ * 1);
		DisplayRestartFlag = true;
		DisplShcFlag = false;
	}

	if (!DisplayTimer) 
		Display.Enable = True;

	PowerEnable = True;	

	PowerOnCnt++;
}

void PowerOff(void)
{
	LcdEnable = 0;
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
	static Uns prevIndType = 123;			// Предыдущий тип индикатора

	if (prevIndType != GrH->IndicatorType)	// Если тип индикатора был изменен
	{
		prevIndType = GrH->IndicatorType;	// Пересбрасываем дисплей
		DisplayReset(&Display, (Uns)Ram.GroupH.IndicatorType);
		WritePar(GetAdr(GroupH.IndicatorType), &Ram.GroupH.IndicatorType, 1);
	}

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

	if (GrH->LogReset > 0)							// Обнуляем записи и адреса журналов
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

	if ((++PreTimer >= PRD_200HZ) && !LogEv.EventFlag)						// Запись по секунде	
	{			
		PreTimer = 0;

		LogEvBuffer[LogEvBufIndex].LogStatus 	 = GrA->Status;
		LogEvBuffer[LogEvBufIndex].LogPositionPr = GrA->PositionPr;
		LogEvBuffer[LogEvBufIndex].LogTorque	 = GrA->Torque;
		LogEvBuffer[LogEvBufIndex].LogUr		 = GrA->Ur;
		LogEvBuffer[LogEvBufIndex].LogUs		 = GrA->Us;
		LogEvBuffer[LogEvBufIndex].LogUt		 = GrA->Ut;
		LogEvBuffer[LogEvBufIndex].LogIu		 = GrA->Iu;
		LogEvBuffer[LogEvBufIndex].LogIv		 = GrA->Iv;
		LogEvBuffer[LogEvBufIndex].LogIw		 = GrA->Iw;
		LogEvBuffer[LogEvBufIndex].LogTemper	 = GrA->Temper;
		LogEvBuffer[LogEvBufIndex].LogInputs	 = GrA->Inputs.all;
		LogEvBuffer[LogEvBufIndex].LogOutputs 	 = GrA->Outputs.all;

		// Инкрементируем текущий индекс в буфере. Предыдущие накопленные данные не удаляем.
		if (++LogEvBufIndex >= LOG_EV_BUF_CELL_COUNT)	
			{ LogEvBufIndex = 0; }
	}
}

void LogEvControl(void)
{
	static Uns Timer = (Uns)LOG_START_TOUT;
	static Uns Addr;

/*
	if (IsMemParReady())
	{
		GrH->FaultsNet.all = 0;
		WritePar(GetAdr(GroupH.FaultsNet.all), GrH->FaultsNet.all, 1);
	}
*/

	if (Timer > 0) Timer--;													// Задержка работы журнала

	LogEv.Enable = !Timer;

	#if !BUR_M
	GrA->Faults.Dev.bit.LowPower = !PowerEnable;								// Выставляем бит здесь, т.к. он может сказать на работе, а по сути он нужен только для записи
	#endif

	LogEv.FaultsState[0] = GrA->Faults.Proc.all & PROCESS_EVLOG_MASK;		// Забираем значения регистров событий
	LogEv.FaultsState[1] = GrA->Faults.Net.all  & NET_EVLOG_MASK;
	LogEv.FaultsState[2] = GrA->Faults.Load.all & LOAD_EVLOG_MASK;
	LogEv.FaultsState[3] = GrA->Faults.Dev.all  & DEV_EVLOG_MASK;

/*
	LogEv.FaultsState[0] = DbgLog;
	LogEv.FaultsState[1] = DbgLog;
	LogEv.FaultsState[2] = 0;
	LogEv.FaultsState[3] = 0;
*/

//--------------------------------------------------------------------------------
	LogEvUpdate(&LogEv);													// Вызываем функцию формирования первой ячейки журнала событий

	#if !BUR_M
	GrA->Faults.Dev.bit.LowPower = 0;											// Сбрасываем бит, чтобы он отсутствовал при пуске
	#endif
//--------------------------------------------------------------------------------
	if (IsMemParReady())
	{
		if (GrH->LogEvAddr != PrevLogAddr[PREV_LEV_INDEX])					// Проверяем текущий адрес начала записи и предыдущий
		{
			WritePar(GetAdr(GroupH.LogEvAddr), &GrH->LogEvAddr, 1);			// Записываем новый адрес начала записи в память
			PrevLogAddr[PREV_LEV_INDEX] = GrH->LogEvAddr;					// Запоминаем адрес
		}
		else if (GrH->LogEvCount != PrevLogCount[PREV_LEV_INDEX])
		{
			WritePar(GetAdr(GroupH.LogEvCount), &GrH->LogEvCount, 1);
			PrevLogCount[PREV_LEV_INDEX] = GrH->LogEvCount;
		}
		else if (LogEv.WriteFlag)													// Проверяем готовность драйвера памяти и флаг разрешения записи
		{
			Addr = LOG_EV_START_ADDR + GrH->LogEvAddr * LOG_EV_DATA_CNT * LOG_EV_DATA_CELL;		// Формируем адрес 
			WritePar(Addr, LogEv.Data, LOG_EV_DATA_CNT);										// Записываем параметры в момент возникновения события

			LogEvMainDataFlag = true;
			LogEvBufCurIndex = LogEvBufIndex;
			LogEv.WriteFlag = false;															// Сбрасываем флаг разрешения записи. По этому флагу записываются только параметры в момент события

			// После записи основной ячейки, формируем начальный адрес буфера
			Addr = LOG_EV_BUF_START_ADDR + GrH->LogEvAddr * LOG_EV_BUF_DATA_CNT * LOG_EV_BUF_DATA_CELL;
		}
		else if (Menu.EvLogFlag)									// Чтение журнала из меню
		{
			if (ReadLogFlag)
			{
				Menu.EvLogFlag = False;
				ReadLogFlag = False;
			}
			else
			{
				// Формируем адрес на чтение из ПЗУ в зависимости от текущего положения в меню
				Addr = LOG_EV_START_ADDR + Menu.EvLog.Position * LOG_EV_DATA_CNT * LOG_EV_DATA_CELL;
				// Отправляем запрос драйверу на чтение. В качестве буфера - группа E Ram
				ReadPar(Addr, ToUnsPtr(&Ram) + LOG_EV_RAM_DATA_ADR, LOG_EV_DATA_CNT);

				ReadLogFlag = True;
			}
		}
	}

	if ((IsMemLogReady()) && (LogEvMainDataFlag))
	{
		if (--LogEvBufIndex < 0)										// Декрементируем индекс буфера, т.к. записываем сначала данные 1 секунды и так далее до 5-й секунды	 
			LogEvBufIndex = (LOG_EV_BUF_CELL_COUNT - 1);				// Индекс массива начинается не с 1, а с 0

		WriteLog(Addr, ToUnsPtr(&LogEvBuffer[LogEvBufIndex]), LOG_EV_BUF_DATA_CNT);		// Пишем в память
		
		Addr += LOG_EV_BUF_DATA_CNT;							// Увеличиваем адрес на количество записанный из буфера данных
		

		if (LogEvBufIndex == LogEvBufCurIndex)	{				// Если индексы сравнялись, значит мы дошли до самого последнего (5-я секунда)
			LogEvMainDataFlag = false;							// Записали все ячейки из буфера сбрасываем флаг записи основного события (0-я секунда)
			if (++GrH->LogEvAddr >= LOG_EV_CNT)					// Инкрементируем начальный адрес записи и проверяем его выход за пределы установленой области 
				GrH->LogEvAddr = 0;								// Если вышел за установленую область, то присваиваем самый первый адрес

			if (GrH->LogEvCount < LOG_EV_CNT)					// Если не превышено максимальное число записей
				GrH->LogEvCount++;								// Инкрементируем число записей журанала команд

			LogEv.EventFlag = false;							// Сбрасываем основной флаг события, после которого сможем заново заполнять 5-сек. буфер
		}
	}
}

void GetCurrentCmd(void)
{
	Uns LogControlWord = bcmNone;
	static Uns PrevEvLogValue = 0;
	static Bool FirstCmd = true;
	static Uns DelayTimer = 0;

	if (Mcu.EvLog.Value != 0)
		LogControlWord = bcmNone;
	else if (Mcu.EvLog.QueryValue)
	{
		if (DelayTimer++ > 25)
		{
			Mcu.EvLog.Value = Mcu.EvLog.QueryValue;
			Mcu.EvLog.QueryValue = 0;
			LogControlWord = bcmNone;
			DelayTimer = 0;
		}
	}

	// Отсекаем повторяющуся команду Стоп, открыть, закрыть
	if (Mcu.EvLog.Value <= CMD_OPEN)	// CMD_OPEN = 0x4, CMD_CLOSE = 0x2, CMD_STOP = 0x1, все это меньше или равно 0x4 
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
	}

	if (Mcu.EvLog.Value != 0)
	{
		PrevEvLogValue = Mcu.EvLog.Value;

		if (FirstCmd)
		{
			FirstCmd = false;

			// Если самая первая из команд - Стоп, то убираем ее
			if (Mcu.EvLog.Value == CMD_STOP)
				LogControlWord = bcmNone;
		}
	}
	if (Mcu.EvLog.Value)
	{
		LogControlWord = LogControlWord | Mcu.EvLog.Source;
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

//--------------------------------------------------------------------------------
	LogCmdUpdate(&LogCmd);																// Вызываем функцию формирования журнала
//--------------------------------------------------------------------------------
	// Записываем адрес ячейки журнала
	if (IsMemParReady())
	{
		if (GrH->LogCmdAddr != PrevLogAddr[PREV_LCMD_INDEX])							// Проверяем готовность ПЗУ
		{																				// Проверяем изменение адреса текущей ячейки журнала
			WritePar(GetAdr(GroupH.LogCmdAddr), &GrH->LogCmdAddr, 1);					// Записываем новый адрес в ПЗУ
			PrevLogAddr[PREV_LCMD_INDEX] = GrH->LogCmdAddr;								// Обновляем предыдущий адрес
		}
	//--------------------------------------------------------------------------------
		else if (LogCmd.WriteFlag)														// Проверяем готовность ПЗУ
		{																				// Проверяем наличие флага разрешения записи журнала
			Addr = LOG_CMD_START_ADDR + GrH->LogCmdAddr * LOG_CMD_DATA_CNT;				// Формируем начальный адрес записи
			WritePar(Addr, LogCmd.Data, LOG_CMD_DATA_CNT);								// Отправляем на запись драйверу Eeprom1

			LogCmd.WriteFlag = false;													// Сбрасываем флаг разрешения записи журнала

			if (GrH->LogCmdCount < LOG_CMD_CNT)											// Если не превышено максимальное число записей
				GrH->LogCmdCount++;														// Инкрементируем число записей журанала команд

			if (++GrH->LogCmdAddr >= LOG_CMD_CNT)										// Инкрементируем адрес текущей ячейки журнала
				GrH->LogCmdAddr = 0;													// Проверяем выход за пределы (превышено допустимое количество записей). Если превышено - обнуляем
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

// Формируем ячейку, когда нет флагов формирования и записи, чтобы не было конфликта с другими
// источниками изменения параметров. Так же, если индекс не нулевой, т.е. когда параметры по модбас есть
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
	LogParamUpdate(&LogParam);															// Вызываем функцию формирования журнала
//--------------------------------------------------------------------------------
	if (IsMemParReady())
	{
		if (GrH->LogParamAddr != PrevLogAddr[PREV_LPAR_INDEX])							// Проверяем условаие готовности ПЗУ
		{																				// Проверяем изменение адреса текущей ячейки
			WritePar(GetAdr(GroupH.LogParamAddr), &GrH->LogParamAddr, 1);				// Записываем новый адрес в ПЗУ
			PrevLogAddr[PREV_LPAR_INDEX] = GrH->LogParamAddr; 							// Обновляем предыдущий адрес ячейки журнала
		}
	//--------------------------------------------------------------------------------
		else if (LogParam.WriteFlag)													// Проверяем готовность памяти
		{																				// Проверяем наличие флага разрешения записи журнала
			Addr = LOG_PARAM_START_ADDR + GrH->LogParamAddr * LOG_PARAM_DATA_CNT;		// Формируем начальный адрес записи в ПЗУ
			WritePar(Addr, LogParam.Data, LOG_PARAM_DATA_CNT);							// Отправляем драйверу для записи
			LogParam.WriteFlag = false; 												// Сбрасываем флаг разрешения записи

			if (GrH->LogParamCount < LOG_PARAM_CNT)										// Если не превышено максимальное число записей
				GrH->LogParamCount++;													// Инкрементируем число записей журанала изменения параметров

			if (++GrH->LogParamAddr >= LOG_PARAM_CNT)									// Инкрементируем адрес ячейки журнала
				GrH->LogParamAddr = 0;													// Если он вышел за границы (превышено количество допустимых записей), обнуляем
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
			ReadValuesStr(Menu.HiString, 229 + GrC->DriveType);
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

// Определяем информацию, которая отображается на верхнем уровне меню
void StartDispl(String Str)
{
	Uns displPosition = 0;
	displPosition = !GrA->Faults.Dev.bit.PosSens ? GrA->PositionPr : 9999; // Если "сбой датчика положения", выводим 999.9
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

// Разрешение редактирование параметра меню
Bool EnableEdit(Uns Password, Uns Param)
{
	switch (Password)
	{
		case 1: if (IsPassword1()) return False; break; 
		case 2: if (IsPassword2()) return False; break; 
	}

	return True;
}
 
// Запись измененного параметра в ПЗУ
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
		RefreshData();
																	// Отправляем данные в журнал изменения параметров
		LogParam.Addr 	  = Param;									// Адрес параметра
		LogParam.NewValue = *Value;									// Новое значение параметра
		LogParam.ExecFlag = true;									// Выставляем флаг формирования журнала
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
		// Отображение текущего хода при проверке калибровки
		ReadAddStr(Menu.HiString, EXPR_STR_ADDR + (State<<1) + 0);
		DecToStr(GrA->PositionPr, &Menu.LoString[8], 1, 4, True, True);
		Menu.LoString[9] = '%';
	}
}

Byte GetExpressState(Byte CurrentState, Uns State)
{
	// Не забыть сбросить!
	static Byte ExCycleCount = 0;

	switch (State)
	{
		// Проверяем текущее состояние: открыто или закрыто
		case 0: if 		(IsClosed())	CurrentState = 14;		// Если закрыто, то следующее состояние	
				else if (IsOpened())	CurrentState = 20;		// Если открыто, то через два состояния
				break;
		// Двигаемся из состояния "Закрыто"
		case 1: if ((IsOpening()) || (IsClosing()))			CurrentState = 15;		// Если зафиксированна команда "Открыто", то переходим на следующий шаг
//		case 1: if (IsOpening())			CurrentState = 15;		// Если зафиксированна команда "Открыто", то переходим на следующий шаг
				break;
		// Останов во время движения
		case 2: if ((IsStopped()) && (!IsOpened()))	CurrentState = 16;	// Принудительный стоп
				else if (IsOpened())				CurrentState = 19;	// Доехали до открыто
				break;
		// Сброс калибровки
		case 3: GrD->CalibReset = 1;	ExCycleCount = 0;				CurrentState = 3;	// Возвращаемся к выбору типу калибровки
				break;
		// Доехали до "Открыто". Проверяем наличие полного цикла
		case 4: if (++ExCycleCount >= 2)			// Если был проделан полный цикл
					{	ExCycleCount = 0;	CurrentState = 26;	}
				else						CurrentState = 20;		// Иначе двигаемся в состояние "Закрыто"
				break;
				
		// Двигаемся из состояния "Открыто"
		case 5: if ((IsClosing()) || (IsOpening()))			CurrentState = 21;		// Если зафиксированна команда "Закрыто", то переходим на следующий шаг
//		case 5: if (IsClosing())			CurrentState = 21;		// Если зафиксированна команда "Закрыто", то переходим на следующий шаг
				break;
		// Останов во время движения
		case 6: if ((IsStopped()) && (!IsClosed()))	CurrentState = 22;	// Принудительный стоп
				else if (IsClosed())				CurrentState = 25;	// Доехали до закрыто
				break;
		// Доехали до "Открыто". Проверяем наличие полного цикла
		case 7: if (++ExCycleCount >= 2)			// Если был проделан полный цикл
					{	ExCycleCount = 0;	CurrentState = 26;	}
				else						CurrentState = 14;		// Иначе двигаемся в состояние "Закрыто"
				break;
		// Проверяем какой был выбран тип задвижки
		case 8: if (GrB->ValveType == vtKlin)	CurrentState = 29;
				else							CurrentState = 30;
				break;
		// Проверяем какой был выбран тип задвижки
		case 9: if (GrB->ValveType == vtKlin)	CurrentState = 31;
				else							CurrentState = 32;
				break;
		// Проверяем тип модификации блока
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
		if ((*Code == *Password) || (*Code == Def))
		{
			*Code = 0; *Password = 0;
			WriteFlag = True;
		}
	}
	else if (*Code != 0)
	{
		*Password = *Code; *Code = 0;
		WriteFlag = True;
	}
	
	if (WriteFlag) WritePar(Addr, Password, 1);
}

void AddControl(void)
{
	if (!IsTestMode())
	{
		if (!PowerEnable)				// если выключенно
		{
			GrH->BusyValue = 0;	// процент выполнения
			Menu.State = MS_START;		// меню в стартовый режим
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
		else if (DefFlag)	// если происходит сброс параметров к исходным
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
		else if (Mcu.Mpu.CancelFlag) //отмена команды пду
		{
			PutAddData(CMD_CANC_ADR, Null);
			if (++CancelTimer >= (Uns)CANCEL_TOUT)
			{
				Mcu.Mpu.CancelFlag = False;
				CancelTimer = 0;
			}
		}
		else
		{
			GrH->BusyValue = 0;
			Menu.Indication = True;
			return;
		}
	}
	else if (!GrG->DisplShow)	// наверно не нужно
	{
		GrH->BusyValue = 0;
		Menu.Indication = True;
		return;
	}
	
	BtnStatus &= BTN_STOP;	// если был стоп то остается стоп, если было чтото другое то добавляем стоп и команда не обрабатывается
	Pult.Key = 0;			// сбросили команду с пду
	Menu.Indication = False;// хз чтото с меню
}

void LocalControl(void) // изменен и не проверен
{
		THallBlock *Halls = &GrC->HallBlock;			// указатель для индикации
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
			//case BTN_STOP_MU:Mcu.Mpu.BtnKey = KEY_STOP; break;
			//case BTN_STOP_DU:Mcu.Mpu.BtnKey = KEY_STOP; break;
			case BTN_STOP:  
							#if BUR_M  	
							Mcu.Mpu.BtnKey = KEY_STOP;
							#else
								if (IsLocalControl() || GrB->MuDuSetup == mdOff)
									Mcu.Mpu.BtnKey = KEY_STOP;
							#endif
					break;
		}

		if (BtnStatus) BlinkTimer = (Uns)BLINK_TOUT;
	}   

	Halls->bit.Open    = BtnOpen.Flag;							// забираем данные с датчиков холла дл обработки
	Halls->bit.Close   = BtnClose.Flag;
	Halls->bit.StopMU  = BtnStop_MU.Flag;						// стоп забираем последним для приоритета
	Halls->bit.StopDU  = BtnStop_DU.Flag;

	BtnStatus &= BTN_STOP;										// если статус был  не стоп то статус допишется и не будет обрабатыватся									// сбрасываем команду статуса на стоп
	
	//Обработка команд с ПДУ
	if (Pult.Key != 0)											// если пришла команда по ирде от ПДУ			
	{
		if (IsLocalControl() || GrB->MuDuSetup == mdOff)
		Mcu.Mpu.PduKey = Pult.Key;								// Записываем команду в управление приводом, для возможной обработки

		Menu.Key = Pult.Key;
		PduKeyFlag = 1;
		PultActiveTimer = PULT_LED_TOUT;
		BlinkTimer = (Uns)BLINK_TOUT;							// мигаем светодиодиком что еслить действие :)
		Pult.Key = 0;											// сбрасываем ключ с пду
	}

	// Выход из начального экрана эксрпесс-настройки ручкой "Стоп"
	if ((Menu.State == MS_EXPRESS) && Menu.Express.Enable && 	\
		(Halls->bit.StopMU && Halls->bit.StopDU) && (Menu.Key == 0))
	{
		Menu.Express.Select = false;
		Menu.Key = KEY_ENTER;
	}

	if(Menu.State == MS_SELPAR && Menu.Key == KEY_ENTER && !IsStopped()) // если не в стопе и пришел запрос на редактирование то сбрасываем его
	{
		Menu.Key = 0;	
	}

// Выключаем дежурный режим ручкой
	if (IsSleeping())
	{
		if (Mcu.Mpu.BtnKey != 0)
		{
			Menu.Key = KEY_ESCAPE;
		}
	}

	// Когда едем, то на дежурный режим не реагируем
	if (!IsStopped())
	{
		Menu.SleepTimer = 0;
	}
}

void RemoteControl(void) //24 - 220 + маски, 
{
	#if !BUR_M					    // если Бур Т то обрабатываем дискретные входы
	if (!PiData.Connect) return;	// если нет связи с АВР то не обрабатывает сигналы ТУ
	Mcu.Tu.Enable = (!IsTestMode()) && (!IsParamEditing() && !GrG->TestCamera);  // если тест то не включаем работу с ту

	if(GrB->InputType == it24)		
	{
		ExtReg = ((Uns)(PiData.DiscrIn24 & 0x1F));
	}
	else if(GrB->InputType == it220)	
	{  	
		ExtReg = ((Uns)(PiData.DiscrIn220 & 0x1F));
	}
		
	GrH->Inputs.bit.Open  = (Uns)TuOpen.Flag  ^ (Uns)GrB->InputMask.bit.Open;	
	GrH->Inputs.bit.Close = (Uns)TuClose.Flag ^ (Uns)GrB->InputMask.bit.Close;	 
	GrH->Inputs.bit.Stop  = (Uns)TuStop.Flag  ^ (Uns)GrB->InputMask.bit.Stop;  
	GrH->Inputs.bit.Mu  = 	(Uns)TuMu.Flag    ^ (Uns)GrB->InputMask.bit.Mu; 
	GrH->Inputs.bit.Du  = 	(Uns)TuDu.Flag    ^ (Uns)GrB->InputMask.bit.Du;  

	switch(GrB->MuDuSetup)
	{
		case mdOff:		GrA->Faults.Proc.bit.MuDuDef = 0;			break;
		case mdMuOnly:	GrA->Faults.Proc.bit.MuDuDef = 0;			break;
		case mdDuOnly:	GrA->Faults.Proc.bit.MuDuDef = 0;			break;
		case mdSelect:		
					if(IsStopped() && !GrG->TestCamera) 
					{
						if(!GrH->Inputs.bit.Mu      && !GrH->Inputs.bit.Du)
						{	 	
								if (++MuDuDefTimer > (1 * PRD_50HZ))
								{
									Mcu.MuDuInput = 0;
									GrA->Faults.Proc.bit.MuDuDef = 1;
									MuDuDefTimer = 0;
								}		
						}
						else if(GrH->Inputs.bit.Mu  && !GrH->Inputs.bit.Du)
						{		
								MuDuDefTimer  = 0;
						 		Mcu.MuDuInput = 1;
								GrA->Faults.Proc.bit.MuDuDef = 0;
						}
						else if(!GrH->Inputs.bit.Mu && GrH->Inputs.bit.Du) 
						{ 		
								MuDuDefTimer  = 0;
						 		Mcu.MuDuInput = 0;
								GrA->Faults.Proc.bit.MuDuDef = 0;
						}
						else if(GrH->Inputs.bit.Mu  && GrH->Inputs.bit.Du)  
						{		
								if (++MuDuDefTimer > (1 * PRD_50HZ))
								{
									Mcu.MuDuInput = 0;
									GrA->Faults.Proc.bit.MuDuDef = 1;
									MuDuDefTimer = 0;
								}
						}
					} 
					break; 
	}
	#else
	switch(GrB->MuDuSetup)
	{
		case mdSelect:
					if (IsMemParReady())
					{
						GrB->MuDuSetup = mdOff;
						WritePar(GetAdr(GroupB.MuDuSetup), GrB->MuDuSetup, 1);
						
					}
					break;
	}
	#endif
}

void BlkSignalization(void)	// Сигнализация на блоке
{
	static Uns DspTimer = 0;

	TLedsReg *Reg = &GrH->LedsReg;	// копия 
	
	if (++DspTimer >= DSP_LED_TIMER)
	{
		DspTimer = 0;
		if (PowerEnable)
		{
			Reg->bit.DspOn = !Reg->bit.DspOn;
			Reg->bit.Pdu = !Reg->bit.Pdu;
		}
	}
	
	GrA->Status.bit.BlkIndic = IsFaultExist(pmBlkSign); // индикация на блоке если есть ошибка любая, поидее можо  всегда
	GrA->Status.bit.BlkDefect = IsDefectExist(pmBlkSign); // индикация на блоке если есть неисправность любая, поидее можо  всегда

	if (!PowerEnable) return;	// если выключен блок то уходим отсюда

	if (IsTestMode()) {Reg->all = GrG->LedsReg.all; return;} // если тест то заменяем реальные сигналы светодиодов тестовыми и выходим
	
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

	Reg->bit.Defect  = IsBlockDefect();   // наличие неисправности

	Reg->bit.MuDu  = IsLocalControl();	// как в ТЗ не проканает

	Reg->bit.Fault = IsBlockFault();	// если есть ошибка на блоке (как стопорящая дв. так и не стопорящая)

	Reg->bit.Muff  = IsMuffActive();	// муфта
}

void TsSignalization(void) //ТС
{
	TOutputReg *Reg = &GrH->Outputs;
	
	GrA->Status.bit.TsIndic  = IsFaultExist(pmBlkTsSign);// в статус о работе индикации ТУ
	GrA->Status.bit.TsDefect  = IsDefectExist(pmBlkTsSign);// в статус о работе индикации ТУ

	// Сбрасываем ТС перед выставлением.

	#if BUR_M
	Reg->all &= TS_SIGNAL_MASK_M;
	#else
	Reg->all &= TS_SIGNAL_MASK_T;
	#endif

	if(IsTestMode() || GrG->TestCamera)
	{
		if (!GrG->IsDiscrTestActive)
			Reg->all = GrG->OutputReg.all;
	}
	else	
	{ 
	#if BUR_M
//		Reg->bit.Dout0 = ()	 			 ^ 		(Uns)GrB->OutputMask.bit.Dout0;		//  Ком.Закрыть
//		Reg->bit.Dout1 = () 			 ^ 		(Uns)GrB->OutputMask.bit.Dout1;		//  Ком.Открыть
		Reg->bit.Dout2 = IsMuffActive()	 ^ 		(Uns)GrB->OutputMask.bit.mufta;		//  Муфта
		Reg->bit.Dout3 = IsTsFault()	 ^ 		(Uns)GrB->OutputMask.bit.fault;		//	тс аларм
//		Reg->bit.Dout4 = ()				 ^ 		(Uns)GrB->OutputMask.bit.Dout4;		//  Ком.Стоп
		Reg->bit.Dout5 = 1 		     	 ^ 		(Uns)GrB->OutputMask.bit.powerOn;	//  Питание
		Reg->bit.Dout6 = IsClosed()		 ^ 		(Uns)GrB->OutputMask.bit.closed;	//  Закрыто
		Reg->bit.Dout7 = IsOpened()		 ^ 		(Uns)GrB->OutputMask.bit.opened;	//  Открыто
		Reg->bit.Dout8 = IsTsDefect()	 ^ 		(Uns)GrB->OutputMask.bit.defect;	//  Неисправность 
		Reg->bit.Dout9 = IsOpened() 	||		(!(IsOpened()&& IsClosed()));		//  КВЗ 
		Reg->bit.Dout10 = IsClosed() 	||		(!(IsOpened()&& IsClosed()));		//  КВО 
	#else 
		Reg->bit.Dout0 = IsTsFault()	 ^ 		(Uns)GrB->OutputMask.bit.fault;		//	тс аларм
		Reg->bit.Dout1 = IsClosed()		 ^ 		(Uns)GrB->OutputMask.bit.closed;	//	закрыто
		Reg->bit.Dout2 = IsOpened()		 ^ 		(Uns)GrB->OutputMask.bit.opened;	//	открыто 
		Reg->bit.Dout3 = IsMuffActive()	 ^ 		(Uns)GrB->OutputMask.bit.mufta;		//	муфта
		Reg->bit.Dout4 = IsClosing()	 ^ 		(Uns)GrB->OutputMask.bit.closing;	//	закрывается
		Reg->bit.Dout5 = IsOpening()	 ^ 		(Uns)GrB->OutputMask.bit.opening;	//	открывается
		Reg->bit.Dout6 = !IsLocalControl()^ 	(Uns)GrB->OutputMask.bit.muDu; 		//	МУ/ДУ
		Reg->bit.Dout7 = IsTsDefect()	 ^ 		(Uns)GrB->OutputMask.bit.defect;	//  Неисправность 
		
	#endif
	}

	#if BUR_M
	// Уходит на serial communication
	if (PowerEnable || (!PowerEnable && (GrH->FaultsLoad.all & LOAD_SHC_MASK)))
	{
		PiData.DiscrOut = (Byte)(Reg->all & 0xFF);
		PiData.DiscrOut2 = (Byte)((Reg->all >> 8) & 0x1);
	}
	#else
	if (PowerEnable)
	{
		PiData.DiscrOut = (Byte)(Reg->all & 0xFF);
		PiData.DiscrOut2 = (Byte)((Reg->all >> 8) & 0x1);
	}
	#endif

}

void AlgControl(void)
{
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
	
	// Сброс защит
	if (GrD->PrtReset != 0)
	{	
		if (!IsStopped()) Mcu.Mpu.CancelFlag = True;
		else {ProtectionsClear(); Mcu.EvLog.Value = CMD_RES_FAULT;}
		GrD->PrtReset = 0;
	}
}

void ClbControl(void)	// управление калибровками 
{
	Calib.ResetFlag = !IsStopped();	// если остановленно то отправляем 1 в сброс 
	if (Calib.CancelFlag)			// если выставлен флаг отмены команды (если откалиброванно , или выставлен рессет флаг или идет автокалибровка)
	{
		Mcu.Mpu.CancelFlag = True;	// выставляем сброса команды МПУ
		Calib.CancelFlag = False;	// сбрасываем флаг в калибровках
	}
	
	switch (Calib.AutoCmd)			// проверяем автокалибровку и если команда есть то... если нету команды то пропускаем
	{
		case CLB_CLOSE: *Mcu.ControlWord = vcwClose; break;	// если команда закрытие то выставляем управляющие слово закрытие
		case CLB_OPEN:  *Mcu.ControlWord = vcwOpen;  break;	// аналогично но наоборот
	}
	Calib.AutoCmd = 0;				// сбрасываем команду автокалибровки

	GrA->Status.bit.Closed = IsStopped() && ((Calib.Zone & CLB_CLOSE) != 0);	// закрытие - если в стопе и откалиброванно "закрыто"
	GrA->Status.bit.Opened = IsStopped() && ((Calib.Zone & CLB_OPEN)  != 0);	// открытие - если в стопе и откалиброванно "открытие"
						
    GrH->Position      = Encoder.Revolution;						// забираем текущее положение (пока с теста)
//	GrC->ClosePosition = IndicPos(Calib.Indication->ClosePos);				// забираем	положение закрыто
//	GrC->OpenPosition  = IndicPos(Calib.Indication->OpenPos);				// забираем положение открыто
//	GrC->Position 	   = GrH->Position;										// копируем текущее положение в гр.С

	if (!IsNoCalib() && (Menu.State != MS_EXPRESS) && Menu.Express.Enable)
		Menu.Express.Enable = FALSE;

	if (GrD->CalibReset != 0)							// если подана команда на сброс калибровки
	{
		if (!IsStopped()) 									// если не остановленно то 
			Mcu.Mpu.CancelFlag = True;						// сбрасываем команду с МПУ
		else												// иначе если остановленно
		{
			GrD->TaskClose = trReset;
			GrD->TaskOpen  = trReset;
		
			Mcu.EvLog.Value = CMD_RES_CLB;					// подали в журнал управления приводом команду сброса датчика положения
		}
		GrD->CalibReset = 0;								// обнулили команду на сброс калибровка

		if (Menu.State != MS_EXPRESS)						// Только если не находимся в Экспресс-настройке
			Menu.Express.Enable = TRUE;
	}

	if (GrG->TestCamera)
		GrA->Position      = !Encoder.Error ? GrH->Position : 65535;
	else
		GrA->Position      = Encoder.Revolution;		
	
	if (GrD->CycleReset != 0)							// если подана команда на сброс счетчка колличества полных циклов
	{
		if (!IsStopped()) 									// если не остановленно то 
			Mcu.Mpu.CancelFlag = True;						// сбрасываем команду с МПУ
		else												// иначе если остановленно
		{
			GrH->CycleCnt = 0;							// Сбрасываем счетчик циклов
			Calib.CycleData = 0;							// сбросили счетчик (если полный цикл пройден небыл, его тоже обнуляем)
			Mcu.EvLog.Value = CMD_RES_CYCLE;				// подали в журнал управления приводом команду сброса коллчиества циклов
		}
		GrD->CycleReset = 0;							// сбрасываем команду сброса циклов
	}

	if (IsMemParReady())									// если есть готовность к записи параметров
	{
		if (GrH->CycleCnt != PrevCycle)										// если счетчик циклов обновился
		{	
			WritePar(REG_CYCLE_CNT, &GrH->CycleCnt, 1);						// записали параметр счетчик циклов
			PrevCycle = GrH->CycleCnt;										// запомнили записанный параметр, для последующей проверки
		}
		else if (GrH->CalibState != GrA->CalibState)						// если состояние калибровки изменилось
		{
			WritePar(REG_CALIB_STATE, &GrH->CalibState, sizeof(ClbIndication));// то записали состояние калибровки
			GrA->CalibState = GrH->CalibState;								// запоминаем записанный параметр, для последующей проверки
		}
	}


}

__inline void DefParamsSet(Uns Code)	// к славе ???
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
