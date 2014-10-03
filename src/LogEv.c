/*
������ ��������
*/

#include "LogEv.h"



void LogEvUpdate(TLogEv *p)
{
	Uns i, Temp, *Fltr, *FltrPrev, *pBuf = p->Data;

	if (!p->Enable)								
	{
		p->ExecFlag   = false;								
		p->WriteFlag  = false;
		memset(p->FaultsFiltr,     0, sizeof(p->FaultsFiltr));	
		memset(p->FaultsFiltrPrev, 0, sizeof(p->FaultsFiltrPrev));	
		return;						
	}
	
	for (i=0; i < p->FaultsCount; i++)
	{
		Fltr     = &p->FaultsFiltr[i];
		FltrPrev = &p->FaultsFiltrPrev[i];

		Temp   = p->FaultsState[i];
		*Fltr &= Temp;
		Temp  &= ~(*Fltr);
		if (Temp) *Fltr |= Temp;

		if ((*Fltr) ^ (*FltrPrev))
		{
			p->ExecFlag = True;
			//	p->DelayTimer = 0;
			*FltrPrev = *Fltr;
		}
	}
	
	if (p->ExecFlag)													// ��������� ���� ������������ ������
	{
		*pBuf++ = *p->Time;
		*pBuf++ = *p->Date;
		*pBuf++ = *p->Status;
		memcpy(pBuf, p->FaultsState, p->FaultsCount * sizeof(Uns));
		pBuf += p->FaultsCount;
		*pBuf++ = (Uns)(*p->PositionPr);
		*pBuf++ = *p->Torque;
		*pBuf++ = *p->Ur;	*pBuf++ = *p->Us;	*pBuf++ = *p->Ut;
		*pBuf++ = *p->Iu;	*pBuf++ = *p->Iv;	*pBuf++ = *p->Iw;
		*pBuf++ = (Uns)(*p->Temper);
		*pBuf++ = *p->Inputs;
		*pBuf++ = *p->Outputs;
		*pBuf++ = *p->Seconds;

		if (p->FirstEvent)
		{
			p->FirstEvent = false;
			*pBuf = 250;
		}
		else
			*pBuf = 0;
		
		p->WriteFlag = True;
		p->ExecFlag  = False;
		p->EventFlag = True;
	}
}


void LogCmdUpdate(TLogCmd *p)
{
	register Uns *pBuf = p->Data;

	if (!p->Enable)								// ������ ��������
	{
		p->ExecFlag   = false;					// ���������� �����
		p->WriteFlag  = false;
		p->CmdRegPrev = 0;						// �������� ���������� �������� �������� ������
		
		return;
	}

	if (p->CmdReg)					// ���� ������� ������� ������ �� ����� ����
	{
		p->ExecFlag = true;			// ��������� ������������ �������
	}

	if (p->ExecFlag)					// ���������� ������
	{
		*pBuf++ = *p->Time;				
		*pBuf++ = *p->Date;				
		*pBuf++ = *p->Seconds;
		*pBuf++ = p->CmdReg;			// ������� ������
			p->CmdReg = 0;				// �������� ������� ������
		*pBuf++ = *p->StatusReg;		// ��������� ������� �����
		
		p->WriteFlag = true;
		p->ExecFlag  = false;
	}
}

void LogParamUpdate(TLogParam *p)
{
	register Uns *pBuf = p->Data;

	if (!p->Enable)									// ��������� ���������� �������
	{
		p->ExecFlag = false;						// ���������� ����� � ������� �� �������
		p->WriteFlag = false;
		
		return;
	}

	if (p->ExecFlag)						// �� ����� ��������� ������
	{
		*pBuf++ = *p->Time;				
		*pBuf++ = *p->Date;				
		*pBuf++ = (Uns)(*p->Seconds);
		*pBuf++ = p->Addr;					// ����� ���������
		*pBuf++ = p->NewValue;				// ����� �������� ���������
											// ������ �����������
		p->WriteFlag = true;				// ���������� ���� ���������� ������ ������� � ���
		p->ExecFlag = false;				// ���������� ���� ������������ �������
	}
}
