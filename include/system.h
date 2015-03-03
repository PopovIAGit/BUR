/*======================================================================
Имя файла:          system.h
Автор:              Саидов В.С.
Версия файла:       01.07
Дата изменения:		23/06/10
Описание:
Модуль конфигурации задач для выполнения операционной системой
======================================================================*/

#ifndef SYSTEM_
#define SYSTEM_


#define _ELEMNAMES

#include "protectionI2T.h"

TRtosObj Rtos;

TPrdObj  Prd0;
TPrdObj  Prd1;
TPrdObj  Prd2;
TPrdObj  Prd3;
TPrdObj  Prd4;

// 18000 Hz (max = 0xFFFF)SerialCommTimings
TPrdElem ListM[] = {
	PrdElemInit(ProtectionsUpdate,	NULL, 					1),	//	1
	PrdElemInit(MotorControlUpdate,	NULL, 					1),	//	2
	PrdElemInit(SerialCommTimings,	&Mb, 					1),	//	3
	PrdElemInit(PultImpulseCapture,	&Pult,					1),	//	4
	PrdElemInit(PrdExecute,			&Prd0,					1)	//	5
};

// 2000 Hz (max = 18000/2000 = 9)
TPrdElem List0[] = {
	PrdElemInit(PrdExecute,				&Prd1,				1),	//	1
	PrdElemInit(PrdExecute,				&Prd2,				1),	//	2
	PrdElemInit(PrdExecute,				&Prd3,				1),	//	3
	PrdElemInit(PrdExecute,				&Prd4,				1),	//	4
	PrdElemInit(EEPROM_Update,			&Eeprom1,			1),	//	5
	PrdElemInit(EEPROM_Update,			&Eeprom2,			1),	//	6
	PrdElemInit(DISPL_Update,			&Display,			1),	//	7
	PrdElemInit(PiUpdate,				NULL,				1),	//	8
	PrdElemInit(PultKeyExecute,			&Pult,				1)	//	9
};

// 200 Hz (max = 2000/200 = 10)
TPrdElem List1[] = {
	PrdElemInit(DefineCalibParams,		&Calib,				1),	//	1
	PrdElemInit(ClbControl,				NULL, 				1),	//	2
	PrdElemInit(CalibStop,				NULL, 				1),	//	3
	PrdElemInit(ControlMode,			NULL, 				1),	//	4
	PrdElemInit(AtMegaAvagoEncoderCalc,	&Encoder,			1),	//	5
//	PrdElemInit(FrabaEncoderCalc,	    &Encoder,			1),	//	5
	PrdElemInit(DataBufferPre,			NULL, 				1),	//	6
	PrdElemInit(LogEvControl,			NULL, 				1),	//	7
	PrdElemInit(LogCmdControl,			NULL, 				1),	//	8
	PrdElemInit(LogParamMbExec,			NULL, 				1),	//	9
	PrdElemInit(LogParamControl,		NULL, 				1)	// 10
};

// 50 Hz (max = 2000/50 = 40)
TPrdElem List2[] = {
	PrdElemInit(PrtCalc,				&UvR,				1),	//	1
	PrdElemInit(PrtCalc, 				&UvS,				1),	//	2
	PrdElemInit(PrtCalc, 				&UvT,				1),	//	3
	PrdElemInit(PrtCalc,				&UvR_Def,			1),	//	4
	PrdElemInit(PrtCalc, 				&UvS_Def,			1),	//	5
	PrdElemInit(PrtCalc, 				&UvT_Def,			1),	//	6
	PrdElemInit(PrtCalc, 				&OvR,				1),	//	7
	PrdElemInit(PrtCalc, 				&OvS,				1),	//	8
	PrdElemInit(PrtCalc, 				&OvT,				1),	//	9
	PrdElemInit(PrtCalc, 				&OvR_Def,			1),	//	10
	PrdElemInit(PrtCalc, 				&OvS_Def,			1),	//	11
	PrdElemInit(PrtCalc, 				&OvT_Def,			1),	//	12
	PrdElemInit(PrtCalc,				&OvR_max,			1),	//	13
	PrdElemInit(PrtCalc,				&OvS_max,			1),	//	14
	PrdElemInit(PrtCalc,				&OvT_max,			1),	//	15
	PrdElemInit(PrtCalc,				&Vsk,				1),	//	16
	PrdElemInit(PrtCalc,				&BvR,				1),	//	17
	PrdElemInit(PrtCalc,				&BvS,				1),	//	18
	PrdElemInit(PrtCalc,				&BvT,				1),	//	19
	PrdElemInit(PrtCalc,				&PhlU,				1),	//	20
	PrdElemInit(PrtCalc,				&PhlV,				1),	//	21
	PrdElemInit(PrtCalc,				&PhlW,				1),	//	22
	PrdElemInit(ProtectionI2T_Update,	&i2tOverload,		1),	//	23
	PrdElemInit(PrtCalc,				&ISkew,				1),	//	24
	PrdElemInit(PrtCalc,				&Th,				1),	//	25
	PrdElemInit(PrtCalc,				&Th_Err,			1),	//	26
	PrdElemInit(PrtCalc,				&Tl,				1),	//	27
	PrdElemInit(PrtCalc,			    &DrvT,				1),	//	28
	PrdElemInit(DAC7513_Update,			&Dac, 				1) 	//	29
};

// 10 Hz (max = 2000/10 = 200)
TPrdElem List3[] = {
	PrdElemInit(MENU_Update,			&Menu,				1),	// 	1
	PrdElemInit(MENU_Display,			&Menu,				1),	// 	2
	PrdElemInit(RimIndication,			NULL,				1), // 	3
	PrdElemInit(AddControl,				NULL,				1), // 	4
	PrdElemInit(AlgControl,				NULL,				1), // 	5
	PrdElemInit(Calibration,			&Calib,				1), //	6
	PrdElemInit(CalcClbCycle,			&Calib,				1),	//	7
	PrdElemInit(InterfIndication,		NULL,				1),	//	8
	PrdElemInit(BlkSignalization,		NULL,				1),	//	9
	PrdElemInit(TsSignalization,		NULL,				1),	//	10
	PrdElemInit(SetPlisData,			NULL,               1),	//	11
	PrdElemInit(ADT7301_Update,			&TempSens,			1),	//	12
//	PrdElemInit(DS1305_Update,			&Ds1305,			3),	//	13
	PrdElemInit(DS3234_Update,			&Ds3234,			3),	//	13
	PrdElemInit(RtcControl,				NULL,				1),	//	14
	PrdElemInit(InterfaceRefresh,		NULL,				1),	//	15
	PrdElemInit(RimDevicesRefresh,		NULL,				1),	//	16
	PrdElemInit(DmcControlRefresh,		NULL,				1),	//	17
	PrdElemInit(SpeedCalc,				NULL,				1),	//	18
	PrdElemInit(ModBusControl,			NULL,				1),	//	19
	PrdElemInit(DisplayStart,			NULL,				1),	//	20
	PrdElemInit(BtTimer,				&Bluetooth,			1),	//	21
	PrdElemInit(ImTimer,				&Im,				1),	//	22
	PrdElemInit(DiscrInOutTestObserver,	NULL,				1),	//	23
	PrdElemInit(ShowDriveType,			NULL,				1),	//	24
	PrdElemInit(ProgonModeUpdate,		NULL,				1),	//	25
	PrdElemInit(TekModbusParamsUpdate,	NULL,				1)	//	26
};

// 50 Hz (max = 2000/50 = 40)
TPrdElem List4[] = {
	PrdElemInit(FaultIndication,		NULL,				1),	//	1
	PrdElemInit(DefineCtrlParams,		NULL,				1),	//	2
	PrdElemInit(LogInputCalc,			&BtnClose,			1),	//	3
	PrdElemInit(LogInputCalc,			&BtnStop_MU,		1),	//	4
	PrdElemInit(LogInputCalc,			&BtnStop_DU,		1),	//	5
	PrdElemInit(LogInputCalc,			&BtnOpen,			1),	//	6
	PrdElemInit(LogInputCalc,			&TuClose,			1),	//	7
	PrdElemInit(LogInputCalc,			&TuOpen,			1),	//	8
	PrdElemInit(LogInputCalc,			&TuStop,			1),	//	9	
	PrdElemInit(LocalControl,			NULL,				1),	//	10
	PrdElemInit(RemoteControl,			NULL,				1),	//	11
	PrdElemInit(DataSetting,			NULL,				1),	//	12
	PrdElemInit(CubCalc,				&Torq.Cub3,			1),	//	13
	PrdElemInit(LowPowerControl,		NULL,				1),	//	14
	PrdElemInit(ProtectionsEnable,		NULL,				1),	//	15
	PrdElemInit(ProtectionsControl,		NULL,				1),	//	16
	PrdElemInit(DiffInputCalc,			&Muf,				1),	//	17
	PrdElemInit(ValveDriveUpdate,		&Mcu,				1),	//	18
	PrdElemInit(GetCurrentCmd,			NULL,				1),	//	19
	PrdElemInit(EngPhOrdPrt,			NULL,				1),	//	20
	#if BUR_M
	PrdElemInit(NetMomitor,				NULL,				1),	//	21
	#endif
	PrdElemInit(DmcIndication1,			NULL,				1),	//	22
	PrdElemInit(DmcIndication2,			NULL,				1),	//	23
	PrdElemInit(LogInputCalc,			&TuMu,				1),	//	24
	PrdElemInit(LogInputCalc,			&TuDu,				1),	//	25
	PrdElemInit(PultTimer,				&Pult,				1) 	//	26
};


#ifdef _ELEMNAMES
extern TPrdElemName ElemNames[];
#define ELEM_NAMES	ElemNames
#else
#define ELEM_NAMES	NULL
#endif

__inline void SystemInit(void)
{
	Rtos.List       = ListM;
	Rtos.ListCount  = PRLCNT(ListM);
	Rtos.PrdCount   = 0;
	Rtos.TimeIsr    = 0;
	Rtos.TimeIsrMax = 0;
	Rtos.Timer      = ToPtr(&CpuTimer2Regs.TIM.all);
	Rtos.ElemNames  = ELEM_NAMES;
	
	PrdCreate(&Prd0,   HZ/PRD_2KHZ);
	PrdCreate(&Prd1, PRD_2KHZ/PRD_200HZ);
	PrdCreate(&Prd2, PRD_2KHZ/PRD_50HZ);
	PrdCreate(&Prd3, PRD_2KHZ/PRD_10HZ);
	PrdCreate(&Prd4, PRD_2KHZ/PRD_50HZ);

	
	PrdInsert(&Prd0, List0, PRLCNT(List0));
	PrdInsert(&Prd1, List1, PRLCNT(List1));
	PrdInsert(&Prd2, List2, PRLCNT(List2));
	PrdInsert(&Prd3, List3, PRLCNT(List3));
	PrdInsert(&Prd4, List4, PRLCNT(List4));

}


#ifdef _ELEMNAMES
TPrdElemName ElemNames[] = {
//----------------------------------------------
	"Undefined",
//----------------------------------------------
	"PrdExecute_Prd1",
	"PrdExecute_Prd2",
	"PrdExecute_Prd3",
	"PrdExecute_Prd4",
	"EEPROM_Update_Eeprom1",
	"EEPROM_Update_Eeprom2",
	"DISPL_Update_Display",
	"PiUpdate",
//----------------------------------------------
	"DefineCalibParams_Calib",
	"ClbControl",
	"CalibStop",
	"ControlMode",
	"GetSpiData_ExtReg",
	"DAC7513_Update_Dac",
	"EvLogControl",
//----------------------------------------------
	"LowPowerControl",
	"ProtectionsEnable",
	"ProtectionsControl",
	"DiffInputCalc_Muf",
	"EngPhOrdPrt",
	"PrtCalc_UvR",
	"PrtCalc_UvS",
	"PrtCalc_UvT",
	"PrtCalc_OvR",
	"PrtCalc_OvS",
	"PrtCalc_OvT",
	"PrtCalc_Vsk",
	"PrtCalc_BvR",
	"PrtCalc_BvS",
	"PrtCalc_BvT",
	"PrtCalc_PhlU",
	"PrtCalc_PhlV",
	"PrtCalc_PhlW",
	"PrtCalc_I2tMin",
	"PrtCalc_I2tMid",
	"PrtCalc_I2tMax",
	"PrtCalc_IUnLoad",
	"PrtCalc_ISkew",
	"PrtCalc_Th",
	"PrtCalc_Tl",
	"TorqueCalc",
	"CubCalc_Torq.Cub3",
//----------------------------------------------
	"ModBusControl",
	"RtcControl",
	"RimIndication",
	"AddControl",
	"AlgControl",
	"Calibration_Calib",
	"CalcClbCycle_Calib",
	"MENU_Update_Menu",
	"MENU_Display_Menu",
	"InterfIndication",
	"BlkSignalization",
	"TsSignalization",
	"SetSpiData_LedsReg",
	"SetSpiData_Dout",
	"AD5263_Update_Rdac",
	"ADT7301_Update_TempSens",
	"DS1305_Update_Ds1305",
	"DmcIndication",
	"InterfaceRefresh",
	"RimDevicesRefresh",
	"DmcControlRefresh",
	"ProtectionsRefresh",
	"SpeedCalc",
//----------------------------------------------
	"FaultIndication",
	"DefineCtrlParams",
	"LogInputCalc_BtnOpen",
	"LogInputCalc_BtnClose",
	"LogInputCalc_BtnStop",
	"LogInputCalc_TuOpen",
	"LogInputCalc_TuClose",
	"LogInputCalc_TuStop",
	"ProgOutCalc_Dout1",
	"ProgOutCalc_Dout2",
	"ProgOutCalc_Dout3",
	"ProgOutCalc_Dout4",
	"LocalControl",
	"RemoteControl",
	"ValveDriveUpdate_Mcu",
	"DataSetting"
//----------------------------------------------
};
#endif

#endif




