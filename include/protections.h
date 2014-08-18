#ifndef PROTECTION_
#define PROTECTION_

#include "Inputs.h"		// Обработка входных сигналов
#include "PrtCalc.h"	// Реализация алгоритма работы защит

// Константы
#define SHC_START_ANG	115	// Угол работы защиты КЗ в старте
#define SHC_MOVE_ANG	30		// Угол работы защиты КЗ в движении

#define PRT_SCALE (0.100 * PRD2)

#define MUF_DEFAULT(Input, Bit) { \
	False, ToPtr(Input), &MuffFlag, \
	&Ram.GroupC.MuffZone, \
	&Ram.GroupB.NoMoveTime, \
	(1<<Bit), PRT_SCALE, 0, 0 \
}

#if BUR_M
#define UV_DEFAULT(Input, Bit) { \
	PRT_CFG_SET(0,0,Bit,0), \
	ToPtr(Input), \
	&Ram.GroupH.FaultsNet.all, \
	ToPtr(&Ram.GroupC.UvLevel), \
	&Ram.GroupC.UvTime, \
	PRT_SCALE, 0 \
}

#define UVD_DEFAULT(Input, Bit) { \
	PRT_CFG_SET(0,0,Bit,0), \
	ToPtr(Input), \
	&Ram.GroupH.DefectsNet.all, \
	ToPtr(&Ram.GroupC.UvLevel), \
	&Ram.GroupC.UvDTime, \
	PRT_SCALE, 0 \
}

#define OV_DEFAULT(Input, Bit) { \
	PRT_CFG_SET(0,1,Bit,0), \
	ToPtr(Input), \
	&Ram.GroupH.FaultsNet.all, \
	ToPtr(&Ram.GroupC.OvLevel), \
	&Ram.GroupC.OvTime, \
	PRT_SCALE, 0 \
}

#define OVD_DEFAULT(Input, Bit) { \
	PRT_CFG_SET(0,1,Bit,0), \
	ToPtr(Input), \
	&Ram.GroupH.DefectsNet.all, \
	ToPtr(&Ram.GroupC.OvLevel), \
	&Ram.GroupC.OvDTime, \
	PRT_SCALE, 0 \
}

#define OV_MAX_DEFAULT(Input, Bit) { \
	PRT_CFG_SET(0,1,Bit,0), \
	ToPtr(Input), \
	&Ram.GroupH.FaultsNet.all, \
	ToPtr(&Ram.GroupC.OvLevel_max), \
	&Ram.GroupC.OvTime_max, \
	PRT_SCALE, 0 \
}

#define VSK_DEFAULT(Input, Bit) { \
	PRT_CFG_SET(0,1,Bit,0), \
	ToPtr(Input), \
	&Ram.GroupH.FaultsNet.all, \
	ToPtr(&Ram.GroupC.VSkLevel), \
	&Ram.GroupC.VSkTime, \
	PRT_SCALE, 0 \
}

#define BV_DEFAULT(Input, Bit) { \
	PRT_CFG_SET(1,0,Bit,0), \
	ToPtr(Input), \
	&Ram.GroupH.FaultsNet.all, \
	ToPtr(&Ram.GroupC.BvLevel), \
	&Ram.GroupC.BvTime, \
	PRT_SCALE, 0 \
}

#else
#define UV_DEFAULT(Input, Bit) { \
	PRT_CFG_SET(0,0,Bit,20), \
	ToPtr(Input), \
	&Ram.GroupH.FaultsNet.all, \
	ToPtr(&Ram.GroupC.UvLevel), \
	&Ram.GroupC.UvTime, \
	PRT_SCALE, 0 \
}

#define UVD_DEFAULT(Input, Bit) { \
	PRT_CFG_SET(0,0,Bit,20), \
	ToPtr(Input), \
	&Ram.GroupH.DefectsNet.all, \
	ToPtr(&Ram.GroupC.UvLevel), \
	&Ram.GroupC.UvDTime, \
	PRT_SCALE, 0 \
}

#define OV_DEFAULT(Input, Bit) { \
	PRT_CFG_SET(0,1,Bit,20), \
	ToPtr(Input), \
	&Ram.GroupH.FaultsNet.all, \
	ToPtr(&Ram.GroupC.OvLevel), \
	&Ram.GroupC.OvTime, \
	PRT_SCALE, 0 \
}

#define OVD_DEFAULT(Input, Bit) { \
	PRT_CFG_SET(0,1,Bit,20), \
	ToPtr(Input), \
	&Ram.GroupH.DefectsNet.all, \
	ToPtr(&Ram.GroupC.OvLevel), \
	&Ram.GroupC.OvDTime, \
	PRT_SCALE, 0 \
}

#define OV_MAX_DEFAULT(Input, Bit) { \
	PRT_CFG_SET(0,1,Bit,20), \
	ToPtr(Input), \
	&Ram.GroupH.FaultsNet.all, \
	ToPtr(&Ram.GroupC.OvLevel_max), \
	&Ram.GroupC.OvTime_max, \
	PRT_SCALE, 0 \
}

#define VSK_DEFAULT(Input, Bit) { \
	PRT_CFG_SET(0,1,Bit,2), \
	ToPtr(Input), \
	&Ram.GroupH.FaultsNet.all, \
	ToPtr(&Ram.GroupC.VSkLevel), \
	&Ram.GroupC.VSkTime, \
	PRT_SCALE, 0 \
}

#define BV_DEFAULT(Input, Bit) { \
	PRT_CFG_SET(1,0,Bit,0), \
	ToPtr(Input), \
	&Ram.GroupH.FaultsNet.all, \
	ToPtr(&Ram.GroupC.BvLevel), \
	&Ram.GroupC.BvTime, \
	PRT_SCALE, 0 \
}
#endif

#define LI_DEFAULT(Input, Bit) { \
	PRT_CFG_SET(1,0,Bit,0), \
	ToPtr(Input), \
	&Ram.GroupH.FaultsLoad.all, \
	ToPtr(&Ram.GroupC.PhlLevel), \
	&Ram.GroupC.PhlTime, \
	PRT_SCALE, 0 \
}

#define IT_DEFAULT(Input, Index, Bit) { \
	PRT_CFG_SET(1,1,Bit,0), \
	ToPtr(Input), \
	&Ram.GroupH.FaultsLoad.all, \
	ToPtr(&Ram.GroupC.I2tMinCurrent + Index), \
	&Ram.GroupC.I2tMinTime + Index, \
	PRT_SCALE, 0 \
}

#define IUL_DEFAULT(Input, Bit) { \
	PRT_CFG_SET(0,0,Bit,0), \
	ToPtr(Input), \
	&Ram.GroupH.FaultsLoad.all, \
	ToPtr(&Ram.GroupC.IUnLevel), \
	&Ram.GroupC.IUnTime, \
	PRT_SCALE, 0 \
}

#define ISK_DEFAULT(Input, Bit) { \
	PRT_CFG_SET(1,1,Bit,0), \
	ToPtr(Input), \
	&Ram.GroupH.FaultsLoad.all, \
	ToPtr(&Ram.GroupC.ISkewLevel), \
	&Ram.GroupC.ISkewTime, \
	PRT_SCALE, 0 \
}

#define OT_DEFAULT(Input, Bit)	{ \
	PRT_CFG_SET(0,1,Bit,1), \
	ToPtr(Input), \
	&Ram.GroupH.FaultsDev.all, \
	ToPtr(&Ram.GroupC.TemperHigh), \
	&OtTime, \
	TB_SCALE, 0 \
}

#define OTE_DEFAULT(Input, Bit)	{ \
	PRT_CFG_SET(1,1,Bit,1), \
	ToPtr(Input), \
	&Ram.GroupH.FaultsDev.all, \
	ToPtr(&HighTemper), \
	&OtTime, \
	TB_SCALE, 0 \
}

#define UT_DEFAULT(Input, Bit)	{ \
	PRT_CFG_SET(0,0,Bit,1), \
	ToPtr(Input), \
	&Ram.GroupH.FaultsDev.all, \
	ToPtr(&Ram.GroupC.TemperLow), \
	&UtTime, \
	TB_SCALE, 0 \
}

#define DT_DEFAULT(Input, Bit)	{ \
	PRT_CFG_SET(1,1,Bit,1), \
	ToPtr(Input), \
	&Ram.GroupA.Faults.Proc.all, \
	ToPtr(&Ram.GroupC.DrvTLevel), \
	&OtTime, \
	TB_SCALE, 0 \
}

/*
 // 03.07 поменял ltAnMin на ltAnMax
#define DT_DEFAULT(Bit, Match) { \
	ltAnMax, False, \
	Null, &Ram.GroupA.Faults.Proc.all, \
	&Ram.GroupC.DrvTLevel, &DrvTTout, \
	(1<<Bit), 1, 0, Match, False \
}*/

// Глобальные переменные модуля
extern DIFF_INPUT Muf;
extern TPrtElem   UvR;
extern TPrtElem   UvS;
extern TPrtElem   UvT;
extern TPrtElem   UvR_Def;
extern TPrtElem   UvS_Def;
extern TPrtElem   UvT_Def; 
extern TPrtElem   OvR;
extern TPrtElem   OvS;
extern TPrtElem   OvT;
extern TPrtElem   OvR_Def;
extern TPrtElem   OvS_Def;
extern TPrtElem   OvT_Def;
extern TPrtElem   Vsk;
extern TPrtElem   BvR;
extern TPrtElem   BvS;
extern TPrtElem   BvT;
extern TPrtElem   PhlU;
extern TPrtElem   PhlV;
extern TPrtElem   PhlW;
extern TPrtElem   I2tMin;
extern TPrtElem   I2tMid;
extern TPrtElem   I2tMax;
extern TPrtElem   IUnLoad;
extern TPrtElem   ISkew;
extern TPrtElem   Th;
extern TPrtElem   Th_Err;
extern TPrtElem   Tl;

extern TPrtElem   OvR_max;
extern TPrtElem   OvS_max;
extern TPrtElem   OvT_max;

extern TPrtElem  DrvT;

extern Uns   ShCLevel;      
extern Uns   MuffFlag;     
extern Int   EngPhOrdValue; 
extern Uns 	 OverWayFlag;
extern Uns   OtTime;
extern Uns   UtTime;
extern Int   HighTemper;
extern Uns   DrvTTout;

// Прототипы функций
void ProtectionsInit(void);
void ProtectionsUpdate(void);
void ProtectionsEnable(void);
void ProtectionsClear(void);
void ProtectionsReset(void);
void ProtectionsControl(void);
Bool IsFaultExist(TPrtMode Mode);
Bool IsDefectExist(TPrtMode Mode);
void EngPhOrdPrt(void);	
void FaultIndication(void);
void LimProtectParams(void);

#endif
