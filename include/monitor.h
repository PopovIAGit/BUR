/*======================================================================
Имя файла:          monitor.h
Автор:              Саидов В.С.
Версия файла:       01.04
Дата изменения:		09/04/10
Описание:
Заголовный файл монитора
======================================================================*/

#ifndef MONITOR_
#define MONITOR_

TDataLog Dlog;

Int DlogCh1 = 0;
Int DlogCh2 = 0;
Int DlogTrig = 0;

__inline void MonitorInit(void)
{
	memset(&Dlog, 0, sizeof(TDataLog));
	
	Dlog.Mode       = DLOG_CONT;
	Dlog.CntrMax    = 0x200;
	Dlog.Level      = 1;
	Dlog.Delay      = 0;
	Dlog.TriggerPtr = &DlogTrig;
	Dlog.Data1Ptr   = &DlogCh1;
	Dlog.Data2Ptr   = &DlogCh2;
	Dlog.Graph1Ptr  = (Int *)0x3FA000;
	Dlog.Graph2Ptr  = (Int *)0x3FB000;
}

__inline void MonitorUpdate(void)
{
	if (++DlogTrig >= (HZ/50)) DlogTrig = 0;
	
	Dlog.Mode     = Hide->MonMode;
	Dlog.DotSpace = Hide->MonScope;

	switch (Hide->MonSelect)
	{
		case 0:	DlogCh1 = Rtos.Elem->Index * 100;	DlogCh2 = Rtos.TimeIsr; 				break;
		case 1:	DlogCh1 = UR.Input;				    	DlogCh2 = US.Input; 					break;
		case 2:	DlogCh1 = UR.Input;						DlogCh2 = UT.Input; 					break;
		case 3:	DlogCh1 = IU.Input;						DlogCh2 = IV.Input; 					break;
		case 4:	DlogCh1 = IU.Input;						DlogCh2 = IW.Input;						break;
		case 5:	DlogCh1 = VOLT_R;							DlogCh2 = VOLT_S; 						break;
		case 6:	DlogCh1 = VOLT_R;							DlogCh2 = VOLT_T;							break;
		case 7:	DlogCh1 = CRNT_U;							DlogCh2 = CRNT_V;							break;
		case 8:	DlogCh1 = CRNT_U;							DlogCh2 = CRNT_W;							break;
		case 9:	DlogCh1 = UR.Input;						DlogCh2 = UR.CurAngle << 5; 			break;
		case 10:	DlogCh1 = US.Input;						DlogCh2 = US.CurAngle << 5; 			break;
		case 11:	DlogCh1 = UT.Input;						DlogCh2 = UT.CurAngle << 5;			break;
		case 12:	DlogCh1 = UR.Input - US.Input;		DlogCh2 = IU.Input;						break;
		case 13:	DlogCh1 = US.Input - UT.Input;		DlogCh2 = IV.Input;						break;
		case 14:	DlogCh1 = UT.Input - UR.Input;		DlogCh2 = IW.Input;						break;
		case 15:	DlogCh1 = _IQtoIQ16(URfltr.Input);	DlogCh2 = _IQtoIQ16(URfltr.Output);	break;
		case 16:	DlogCh1 = _IQtoIQ16(IUfltr.Input);	DlogCh2 = _IQtoIQ16(IUfltr.Output); break;
		case 17:	DlogCh1 = UR.Input;						DlogCh2 = UR.Polarity << 5;			break;
	}
	
	DLOG_update(&Dlog);
}

#endif

