/*======================================================================
Имя файла:          csl_dlog.c
Автор:              Denis        
Версия файла:      	0.664
Дата изменения:		Date: 11.07.2014
Описание:
Графический монитор реального времени
======================================================================*/

#include "config.h"
//#include "tempObserver.h"

TDataLog Dlog;

Int DlogCh1 = 0;
Int DlogCh2 = 0;
Uns MonSelect = 1;
Uns 	MonMode = 1,
	MonScope = 0,
	DlogTestTimer1 = 0,
	DlogTestTimer2 = 0;

extern Uns PowerSupplyEnable; 	// Наличие питания источника

void MonitorInit(void)
{
	memset(&Dlog, 0, sizeof(TDataLog));
	
	Dlog.Mode       = DLOG_STOP;
	Dlog.DotSpace	= 10;
	Dlog.CntrMax    = 0x200;
	Dlog.Level      = 1;
	Dlog.Delay      = 0;
	Dlog.OutOfTrig  = 1;
	Dlog.Data1Ptr   = &DlogCh1;
	Dlog.Data2Ptr   = &DlogCh2;
	Dlog.Graph1Ptr  = (Int *)0x3FBC00;
	Dlog.Graph2Ptr  = (Int *)0x3FBE00;
}

void MonitorUpdate(void)
{
	Dlog.Mode     = MonMode;
	Dlog.DotSpace = MonScope;

	switch (MonSelect)
	{
	 	case 0: DlogCh1 = ++DlogTestTimer1;
		        DlogCh2 = --DlogTestTimer2; break;
		        	
		 case 1: DlogCh1 = (Int)US.Input;
			 DlogCh2 = (Int)IU.Input;		break;

		 case 2: DlogCh1 = (Int)US.Input;
			 DlogCh2 = (Int)IV.Input;		break;

		 case 3: DlogCh1 = (Int)US.Input;
			 DlogCh2 = (Int)IW.Input;		break;

		 case 4: DlogCh1 = (Int)IU.Input;
			 DlogCh2 = (Int)IV.Input;		break;

		 case 5: DlogCh1 = (Int)IU.Input;
			 DlogCh2 = (Int)IW.Input;		break;

		 case 6: DlogCh1 = (Int)GpioDataRegs.GPADAT.bit.GPIO13;
			 DlogCh2 = (Int)PowerSupplyEnable;	break;

		 case 7: DlogCh1 = (Int)AdcRegs.ADCRESULT0;
			 DlogCh2 = (Int)AdcRegs.ADCRESULT2;	break;

		 case 10:DlogCh1 = (Int)US.CurAngle;
			 DlogCh2 = (Int)IU.CurAngle;		break;

		 case 11:DlogCh1 = (Int)UR.Input;
			 DlogCh2 = (Int)UR.Output;		break;

		 case 12:DlogCh1 = (Int)AdcRegs.ADCRESULT3;
			 DlogCh2 = (Int)AdcRegs.ADCRESULT4;break;

		 case 13:DlogCh1 = (Int)AdcRegs.ADCRESULT3;
			 DlogCh2 = (Int)AdcRegs.ADCRESULT5;		break;

		 case 14:DlogCh1 = (Int)IU.CurAngle;
			 DlogCh2 = (Int)UR.CurAngle;		break;

		 case 15:DlogCh1 = (Int)IV.CurAngle;
			 DlogCh2 = (Int)US.CurAngle;		break;

		 case 16:DlogCh1 = (Int)IW.CurAngle;
			 DlogCh2 = (Int)UT.CurAngle;		break;

		default: return;
	}

	DLOG_update(&Dlog);
}

void DLOG_update(TDataLog *p)
{
	if (p->Mode == DLOG_STOP)
	{
		p->Counter = 0;
		p->Prescaller = 0;
		p->Timer = 0;
		return;
	}
	
	if (p->OutOfTrig || p->Trigger)
	{
		p->Timer = 0;
		if (++p->Prescaller >= p->DotSpace)
		{
			p->Prescaller = 0;
			
			p->Graph1Ptr[p->Counter] = *p->Data1Ptr;
			p->Graph2Ptr[p->Counter] = *p->Data2Ptr;
			
			if (++p->Counter >= p->CntrMax)
			{
				p->Counter = 0;
				p->Trigger = 0;
				if (p->Mode != DLOG_CONT) p->Mode = DLOG_STOP;
			}
		}
	}
	else if (p->Timer < p->Delay) p->Timer++;
	else
	{
		if ((*p->TriggerPtr >= p->Level) && (p->DataPrev < p->Level)) p->Trigger = 1;
		p->DataPrev = *p->TriggerPtr;
	}
}

