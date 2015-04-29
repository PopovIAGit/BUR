/*======================================================================
Имя файла:          VlvDrvCtrl.c
Автор:              Саидов В.С. (Обновил - Попов И.А.)
Версия файла:       01.05
Дата изменения:		23/04/12
======================================================================*/

#include "VlvDrvCtrl.h"
#include "Calibs.h"
#include "key_codes.h"

#define IsCalib()		(Calib && (Calib->Indication->Status == CLB_FLAG))

#define IsBreakClose()	((*p->Valve.BreakMode == vtClose) || (*p->Valve.BreakMode == vtBoth))
#define IsBreakOpen()	((*p->Valve.BreakMode == vtOpen)  || (*p->Valve.BreakMode == vtBoth))

#define False			((Bool)0)
#define True			((Bool)1) 

static CALIBS *Calib = NULL;

__inline void GetActiveControls(TVlvDrvCtrl *);
__inline void MpuControl(TVlvDrvCtrl *);
__inline void TeleControl(TVlvDrvCtrl *);
__inline void DemoControl(TVlvDrvCtrl *);
__inline void UnitControl(TVlvDrvCtrl *);
__inline void DriveStop(TVlvDrvCtrl *);
__inline void DriveClose(TVlvDrvCtrl *);
__inline void DriveOpen(TVlvDrvCtrl *);
__inline void DriveDemo(TVlvDrvCtrl *);
__inline void UpdateComm(TVlvDrvCtrl *);
static   void ReverseDrive(TVlvDrvCtrl *);
static   void UpdateControls(TVlvDrvCtrl *, Bool FreeRun);


void ValveDriveUpdate(TVlvDrvCtrl *p)
{
	Calib = (CALIBS *)(p->Valve.CalibData);
	
	GetActiveControls(p);
	TeleControl(p);
	MpuControl(p);
	DemoControl(p);
	UnitControl(p);
}

void ValveDriveStop(TVlvDrvCtrl *p, Bool Flag)
{
	if (Flag)
	{
		p->Smooth.Active = False;
		p->Demo.Active   = False;
	}
	p->Valve.BreakFlag = False;
	if (Calib)
	{
		if (!(*Calib->MuffFlag && Calib->AutoStep))
			Calib->AutoStep = 0;
	}
	
	if (p->Tu.ReleStop) p->Tu.ReleStop = False;
	else if (p->Status->bit.Fault || !(*p->Tu.LockSeal)) p->Tu.Ready = False;
	p->Tu.ReleStopEnable = True;
	
	p->Valve.Position = POS_UNDEF;

	p->IgnorComFlag = 0;
	
	p->StopControl();
}

void ValveDriveMove(TVlvDrvCtrl *p, Uns Percent)
{
	TValveCmd MoveControl = vcwNone;
	LgInt Position;

	if (!p->Valve.PosRegEnable || !IsCalib() || !p->Status->bit.Stop) return;
	
	Position = (LgInt)((Calib->FullStep * (LgUns)Percent * 131UL) >> 17);
	if (Position < (Calib->LinePos - POS_ERR)) MoveControl = vcwClose;
	if (Position > (Calib->LinePos + POS_ERR)) MoveControl = vcwOpen;
	
	if (MoveControl != vcwNone)
	{
		p->Valve.BreakFlag = False;
		p->Valve.Position  = Position;
		p->EvLog.Value     = CMD_MOVE;
		p->StartControl(MoveControl);
	}
}

__inline void GetActiveControls(TVlvDrvCtrl *p)
{
	Bool Flag = False;
	Uns  DigState = 0;

	p->ActiveControls = 0;

	switch(*p->MuDuSetup)
	{
		case mdOff:    p->Status->bit.MuDu = 0; Flag = True; break;
		case mdSelect: p->Status->bit.MuDu = p->MuDuInput; break;
		case mdMuOnly: p->Status->bit.MuDu = 1; break;
		case mdDuOnly: p->Status->bit.MuDu = 0; break;
		default:       return;
	}

	if (Flag || p->Status->bit.MuDu)
	{
		p->ActiveControls |= (CMD_SRC_PDU|CMD_SRC_MPU);
		if (p->Tu.LocalFlag) p->ActiveControls |= CMD_SRC_DIGITAL;
	}
	
	if (Flag || !p->Status->bit.MuDu)
	{
		DigState = p->Tu.LocalFlag ? 0 : CMD_SRC_DIGITAL;
		switch(*p->DuSource)
		{
			case mdsAll:     p->ActiveControls |= (DigState|CMD_SRC_SERIAL); break;
			case mdsDigital: p->ActiveControls |= DigState; break;
			case mdsSerial:  p->ActiveControls |= CMD_SRC_SERIAL; break;
		}
	}
}

__inline void MpuControl(TVlvDrvCtrl *p)
{
	Byte Key = 0;
	Uns  Active = 0;
	
	if (!p->Mpu.Enable) return;

	if (p->Mpu.BtnKey)
	{
		Key = p->Mpu.BtnKey;
		Active = (p->ActiveControls & CMD_SRC_MPU);
		p->EvLog.Source = CMD_SRC_MPU;
		p->Mpu.BtnKey = 0;
	}
	
	if (p->Mpu.PduKey)
	{
		Key = p->Mpu.PduKey;
		Active = (p->ActiveControls & CMD_SRC_PDU);
		p->EvLog.Source = CMD_SRC_PDU;
		p->Mpu.PduKey = 0;
	}

	switch (Key)
	{
		case KEY_STOP:
			*p->ControlWord = vcwStop;
			break;
		case KEY_CLOSE:
			if (Active) *p->ControlWord = vcwClose;
			else p->Mpu.CancelFlag = True;
			break;
		case KEY_OPEN:
			if (Active) *p->ControlWord = vcwOpen;
			else p->Mpu.CancelFlag = True;
			break;
	}
}

__inline void TeleControl(TVlvDrvCtrl *p)
{
	TValveCmd TuControl = vcwNone;
	Bool Ready;
	
	if (!p->Tu.Enable) return;

	if (!(p->ActiveControls & CMD_SRC_DIGITAL))
	{
		p->Tu.State &= ~(TU_STOP|TU_CLOSE|TU_OPEN);
		p->Tu.Ready = True;
		p->Tu.ReleStop = False;
		p->Tu.ReleStopEnable = True;
		return;
	}

	Ready = p->Status->bit.Stop && p->Tu.Ready;
	switch(p->Tu.State & (TU_CLOSE|TU_OPEN))
	{
		case TU_CLOSE: if (Ready) TuControl = vcwClose; break;
		case TU_OPEN:	if (Ready) TuControl = vcwOpen;  break;
		case (TU_CLOSE|TU_OPEN):  TuControl = vcwStop;  break;
		default:
			p->Tu.Ready = True;
			if (p->Status->bit.Stop || !(*p->Tu.ReleMode) || !p->Tu.ReleStopEnable) break;
			p->Tu.ReleStop = True;
			TuControl = vcwStop;
	}

	if ((p->Tu.State & TU_STOP) && !(*p->Tu.ReleMode)) TuControl = vcwStop;
	
	if (TuControl != vcwNone)
	{
		*p->ControlWord = TuControl;
		p->EvLog.Source = CMD_SRC_DIGITAL;
	}
	p->Tu.State &= ~(TU_STOP|TU_CLOSE|TU_OPEN);
}

__inline void DemoControl(TVlvDrvCtrl *p)
{
	if (!p->Demo.Enable) return;
	if (!p->Demo.Active) {p->Demo.Timer = 0; return;}
	if (!p->Status->bit.Stop) return;
	
	if (++p->Demo.Timer >= p->Demo.Timeout)
	{
		if (p->Demo.TaskPos == *p->Demo.UpPos) 
			  p->Demo.TaskPos = *p->Demo.DownPos;
		else p->Demo.TaskPos = *p->Demo.UpPos;
		p->EvLog.Source = CMD_SRC_MPU;
		ValveDriveMove(p, p->Demo.TaskPos);
		p->Demo.Timer = 0;
	}
}

__inline void UnitControl(TVlvDrvCtrl *p)
{
	UpdateComm(p);
	
	if (p->StartDelay > 0) 
	{
		p->StartDelay--;
		if (*p->ControlWord) *p->ControlWord = vcwNone;
	}
	else if (*p->ControlWord)
	{
		switch(*p->ControlWord)
		{
			case vcwStop:      DriveStop(p);  break;
			case vcwClose:
			case vcwTestClose: DriveClose(p); break;
			case vcwOpen:
			case vcwTestOpen:  DriveOpen(p);  break;
			case vcwDemo:      DriveDemo(p);  break;
			case vcwTestEng:
			default: p->StartControl(*p->ControlWord);
		}
		*p->ControlWord = vcwNone;
	}
}

__inline void DriveStop(TVlvDrvCtrl *p)
{
	ValveDriveStop(p, True);
	p->EvLog.Value = CMD_STOP;
}

__inline void DriveClose(TVlvDrvCtrl *p)
{
	Bool FreeRun;

	if (p->Status->bit.Closed && (*p->ControlWord != vcwTestClose)) // SVS
	{
		p->IgnorComFlag = 1;
	 	return;
	}
	if(p->Status->bit.Closing) return;
	if (p->Status->bit.Opening) {ReverseDrive(p); return;}

	p->Valve.BreakFlag = False;
	FreeRun = (*p->ControlWord == vcwTestClose) || !IsCalib();
	if (FreeRun) p->Valve.Position = POS_UNDEF;
	else if (!IsBreakClose()) p->Valve.Position = 0;
	else
	{
		p->Valve.BreakFlag = True;
		p->Valve.Position  = -(LgInt)p->Valve.BreakDelta;
	}

	UpdateControls(p, FreeRun);

	p->StartControl(*p->ControlWord);
}

__inline void DriveOpen(TVlvDrvCtrl *p)
{
	Bool FreeRun;

	if (p->Status->bit.Opened && (*p->ControlWord != vcwTestOpen)) // SVS 
	{	
		p->IgnorComFlag = 1;
	 	return;
	}
	if (p->Status->bit.Opening) return;
	if (p->Status->bit.Closing) {ReverseDrive(p); return;}
	
	p->Valve.BreakFlag = False;
	FreeRun = (*p->ControlWord == vcwTestOpen) || !IsCalib();
	if (FreeRun) p->Valve.Position = POS_UNDEF;
	else if (!IsBreakOpen()) p->Valve.Position = (LgInt)Calib->FullStep;
	else
	{
		p->Valve.BreakFlag = True;
		p->Valve.Position  = (LgInt)Calib->FullStep + (LgInt)p->Valve.BreakDelta;
	}

	UpdateControls(p, FreeRun);

	p->StartControl(*p->ControlWord);
}

__inline void DriveDemo(TVlvDrvCtrl *p)
{
	if (!p->Valve.PosRegEnable)  {p->StartControl(*p->ControlWord); return;}
	if (!p->Demo.Enable || !IsCalib()) return;
	
	p->Demo.Active = True;
}

__inline void UpdateComm(TVlvDrvCtrl *p)
{
	if (p->Command != vcwNone)
	{
		if (p->Command == vcwStop) *p->ControlWord = vcwStop;
		else if (!p->Status->bit.Stop) return;
		else *p->ControlWord = p->Command;
		p->Command = vcwNone;
	}
}

void ReverseDrive(TVlvDrvCtrl *p)
{
/*	if (*p->ReverseType != rvtAuto) p->Command = vcwStop;
	else {p->Command = *p->ControlWord; ValveDriveStop(p, True);}*/

	switch (*p->ReverseType)
	{
		case rvtStop:  p->Command = vcwStop; break;
		case rvtAuto: {
						p->Command = *p->ControlWord; ValveDriveStop(p, True);
						break;
					  }
		case rvtNone: *p->ControlWord = vcwNone; break;
	}
}

void UpdateControls(TVlvDrvCtrl *p, Bool FreeRun)
{
	p->Smooth.Active = p->Smooth.Enable && !FreeRun;
	if (p->EvLog.Source != CMD_SRC_DIGITAL) p->Tu.ReleStopEnable = False;
}


