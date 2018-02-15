#include "config.h"

// �������

#define MpyDivK(V1,V2,K,Prec) 	_IQ7div((LgInt)(V1) * (LgInt)(V2), (LgInt)(K) << (7 - (Prec)))
#define DivKQ1(V1,V2,K,Prec) 	(Int)(_IQ1div((LgInt)(V1) * (LgInt)(K), (LgInt)(V2)) >> ((Prec) + 1))

static Byte CheckStatus(CALIBS *p, Uns Condition);

void Calibration(CALIBS *p)
{
	ClbCommand *Command = p->Command;
	ClbIndication *Indic = p->Indication; 
	LgUns Data;

	//�������/����� �������
	if(Command->TaskClose !=0)						 // ��������� ��������� �� ������� ���������� ��� "�������"
	{
		switch (Command->TaskClose)					 // �������� ����������� (1) ��� ���������� (2)
		{
			case 1:
				if (!CheckStatus(p,CLB_CLOSE))break; // ���� ��� ������������� ��� ���. ���� �� ������, 
				Indic->ClosePos = *p->AbsPosition;	 // �������� ������ � ��������(���������� ���������)
			    Indic->Status |= CLB_CLOSE;			 // ��������� � ������ ��� ���������� �� �������

				break;
			case 2: 
				if (!CheckStatus(p,0))break;		 // ���� ��� �������� ��� ���� �� ������
				Indic->ClosePos = 0;				 // ���������� ��������� �� 0
				Indic->Status &= CLB_OPEN;			 // ���������� ���� ��� � �������
				break;
		}
		Command->TaskClose = 0;						 // �������� ������� ����������
	}
	//�������/����� �������
	if(Command->TaskOpen !=0)
	{
		switch (Command->TaskOpen)					 // ���������� "�������", ���� ��������
		{
			case 1: 
				if (!CheckStatus(p,CLB_OPEN))break;
				Indic->OpenPos = *p->AbsPosition;
				Indic->Status |= CLB_OPEN;

				break;
			case 2: 
				if (!CheckStatus(p,0))break;
				Indic->OpenPos = 0;
				Indic->Status &= CLB_CLOSE;
				break;
		}
		Command->TaskOpen = 0;	
	}
	// ������� ���������� �������� �� ��������
	if (Command->RevOpen > 0)								// ��������� ��������� �� ������� �� ������� �������� "�������"
	{
		if (CheckStatus(p, CLB_FLAG))						// ��������� ���� �� ��������� ����������, ���� ��� �� ���� ������
		{	
			Indic->ClosePos = *p->AbsPosition;				// ����������� ������� ��������� ��� "�������"
			Data = CalcClbAbsRev(p, Command->RevOpen);		// ����������� ����� ����� �������� � ����������� �� �������
			if (*p->RodType) Data = Indic->ClosePos - Data;	// ���� ������ �������� ��� ����� ��
			else Data = Indic->ClosePos + Data;				// ���� ������ ������ ��� ����� ��
			Indic->OpenPos = Data & p->RevMax;				// ���������� ��������� �������  � ��������� �� ��������� �� ��� ����������� ���������� ��������
			Indic->Status  = CLB_FLAG;						// ���������� ���� ����������

		}
		Command->RevOpen = 0;								// ���������� ������� �� ������� �������� "�������"
	}
	
	// ������� ���������� �������� �� ��������
	if (Command->RevClose > 0)								// ���������� �������� "�������"
	{
		if (CheckStatus(p, CLB_FLAG))
		{
			Indic->OpenPos = *p->AbsPosition;
			Data = CalcClbAbsRev(p, Command->RevClose);
			if (*p->RodType) Data = Indic->OpenPos + Data;
			else Data = Indic->OpenPos - Data;
			Indic->ClosePos = Data & p->RevMax;
			Indic->Status = CLB_FLAG;

		}
		Command->RevClose = 0;
	}

	//��������������
	if(Command->RevAuto > 0)								// ��������� ��������� �� ������� �� ��������������
	{
		if(CheckStatus(p, CLB_FLAG))						// ���������, ���� ������ ��������������
		{
			p->AutoStep = 1;								// ���������� ������ ��� ��������������-> ��������� ��������������
			p->RevAuto = Command->RevAuto;					// 
		}
		Command->RevAuto = 0;								// ���������� ������� �� ������� �������� ��������������

		if(p->AutoStep>0)									// ���������, ���� ��������� ��������������, �� ��������� ��. ���� ��� �� �������
		{	
			switch(p->AutoStep)								// ������������ ����� ��������������
			{
				case 1: p->AutoCmd = CLB_CLOSE; p->AutoStep = 2; break;	// ������� �������������� - ���������� ��������� �������, +����. ���
		   		case 2: if (*p->MuffFlag) {p->MuffTimer = p->MuffTime; p->AutoStep = 3;} break; // ���� ��������� ���� �������� ���������, �� ������ ����� ���������� ���������, +���� ���
		   		case 3: 
					   if (p->MuffTimer > 0) p->MuffTimer--;	// ���� ������ ���������� ��������� �� ����� ����, �� ���� ���� ����������
					   else										// ���� ���������� ������ ����������
					   {
						   Indic->ClosePos = *p->AbsPosition;	// ������ ������� ��������� ��� "�������"
						   Indic->Status = CLB_CLOSE;			// ���������� ������ ��� ������������� ��������� "�������"
						   p->AutoCmd = CLB_OPEN;				// ���������� ������� �������������� ���������� ��������� �������
						   p->AutoStep = 4;						// +����.���
					   }
					   break;
		  	   	case 4: if (!(*p->MuffFlag)) p->AutoStep = 5; break; // ������� �� ��������� ��� ������ ����� ������ �� �������� ���������
		   	   	case 5: if (*p->MuffFlag) {p->MuffTimer = p->MuffTime; p->AutoStep = 6;} break; // ���� ��������� ���� �������� ���������, �� ������ ����� ���������� ���������, +����. ���
		   	   	case 6: 
			           if (p->MuffTimer > 0) p->MuffTimer--;	// ���� ������ ���������� ��������� �� ����� ����, �� ���� ���� ����������
						   else									// ���� ���������� ������ ����������
						   {
							   if (*p->CurWay < (Int)p->RevAuto) Indic->Status = 0; //���������, ���� ������� ��������� ���� ������ ������� ��������������, �� ���������� ����������
							   else								// �����, ���� ��� ������
							   {
								   Indic->OpenPos = *p->AbsPosition;	// ������ ������� ��������� ��� "�������"
								   Indic->Status = CLB_FLAG;			// ���������� ������ - ��������� ��������������
							   }
							   p->AutoStep = 0;							// ���������� ������� ����� ��������������
						   }
					  break;
			}
		}	
	}
}

void DefineCalibParams(CALIBS *p)						// ��������� ���������� ���������� �� � ���������������
{
	ClbIndication *Indic = p->Indication;				// ��������� �� ��������� ��������� ����������
	LgUns Position, ClosePos, OpenPos;					// ���������� ����������
	
	if (*p->RodType)									// ���� ������ �������� ��� �����
	{
		Position = p->RevMax - *p->AbsPosition + 1;		// ������� ������� ����� ������������� ���������� 
		ClosePos = p->RevMax - Indic->ClosePos + 1;		// ��������� �������  = ���� ���������� - ����������� ��������� �������
		OpenPos  = p->RevMax - Indic->OpenPos  + 1;		// ����������
	}
	else	// ���� ������ ������� ����
	{
		Position = *p->AbsPosition;			// ���������� ������� ���������
		ClosePos = Indic->ClosePos;			// ���������� ��������� �������
		OpenPos  = Indic->OpenPos;			// ���������� ��������� �������
	}
	
	p->Zone = 0;							// �������� ���� ���������� (�� ������� ���� ��� ����������)
	if (Indic->Status == CLB_FLAG)						// ���� ���������� ���������
	{
		if (!p->FullStep)								// ���� ������ ��� �� ���������, ����������� ���������� �����������
		{
			p->FullStep = (OpenPos - ClosePos) & p->RevMax;	// ������ ��� (������ ��) (��������� ��������� ����� ��� ���� ����� �������� ���� �� ������ �������)
			*p->FullWay = (Uns)CalcClbGearRev(p, (LgInt)p->FullStep);	// ������ ��� (��� �����)
			if ((*p->FullWay >> 1) <= *p->PositionAcc) p->FullStep = 0; // ���� ������ ��� ������ ��� ����� �������� ��������� ��������� �� ��� ���� �������� ������� ��� ���� ������� ������� �� ���������� � 0
			if (!p->FullStep) p->Zone = CLB_FLAG;						// ���� �������� ��� ���� �� ���������� ���� ��� ���������������
			else	// ���������� ������� ���������
			{	
				p->BasePos = (OpenPos + ClosePos) >> 1;					// ������� ��������� (�+�)/2
				if (OpenPos >= ClosePos) p->BasePos = p->BasePos + (p->RevMax >> 1); // ���� ������� ������ ��� ������� (������ ���� ������� ����� 0) ��, ������� ��������� +1/2 �� ����. ����������� ��������
			//	p->BasePos = p->RevMax - (p->FullStep >> 1);
				p->BasePos = p->BasePos & p->RevMax; 
			}
		}
		else	// ���� ������ ��� ���������, ����������� �������� �����������
		{
			p->LinePos = Position - ClosePos;
			if (ClosePos > p->BasePos)
			{
				if (Position < p->BasePos) 
					p->LinePos = p->LinePos + p->RevMax + 1;
			}
			else
			{
				if (Position > p->BasePos) 
					p->LinePos = p->LinePos - p->RevMax - 1;
			}
			
			*p->CurWay = CalcClbGearRev(p, p->LinePos);		// ������������� �� �� ��������� ����������� ���� ��. � �������� ����������� ��������� �����
			if (*p->CurWay <= (Int)*p->PositionAcc) p->Zone |= CLB_CLOSE; // ���� ������� �������� ��������� ����� ������ 
			if (*p->CurWay >= ((Int)*p->FullWay - (Int)*p->PositionAcc)) p->Zone |= CLB_OPEN; // ���� ������� ��������� ��������� �����
			
			*p->PositionPr = DivKQ1(p->LinePos, p->FullStep, 1000, 0);				// ������� ������� ��������� � ��������� (0-������� 100-�������)

			//if ((p->Zone & CLB_CLOSE) && (*p->PositionPr > 0))    *p->PositionPr = 0;	// <-- �����?
			//if ((p->Zone & CLB_OPEN)  && (*p->PositionPr < 1000)) *p->PositionPr = 1000;// <-- �����?
		}
	}
	else	// ���� ���������� �� ��������� ��� ��������� �� �� �����
	{
		*p->PositionPr = 9999;  //  ��������� ������� ��������� � ���������
		*p->FullWay = 9999;		//	��������� ������ ��� ��������� �����
		p->FullStep = 0;		//	������ � ������ ���
		
		if (!Indic->Status) {*p->CurWay = 9999; return;}	// ���� ������ �� �������������� �� ������� ��� ��������� ����� ���������� � �������, ������ ��� ������
		else if (Indic->Status & CLB_CLOSE) p->LinePos = (Position - ClosePos) & p->RevMax;	// ���� �������������� �� ������� �� �������� ������� �������� ��������� �� 
		else if (Indic->Status & CLB_OPEN)  p->LinePos = (OpenPos - Position)  & p->RevMax; // ���� �������������� �� ������� �� �������� ������� �������� ��������� ��
		
		*p->CurWay = CalcClbGearRev(p, p->LinePos);	// �� ��������� ������ ������� ������� ��������� ��������� �����
		if (*p->CurWay <= (Int)*p->PositionAcc) p->Zone |= Indic->Status;	// ���� ������� ��������� ������ �������� �������� ��������� �� ����� �� ��� � ���������� ��� ����������
	}
}

// -----------------------------------------------------------------

// -----------------------------------------------------------------
LgUns CalcClbGearInv(CALIBS *p) 
{
	#if defined(__TMS320C28X__)
	return 0UL;
	#else
	return ((16384000UL >> p->PosSensPow) / (LgUns)p->GearRatio + 1);
	#endif
}

LgUns CalcClbAbsRev(CALIBS *p, Uns GearRev)
{
	return MpyDivK(GearRev, p->GearRatio, 1000, *p->PosSensPow);
}

Int CalcClbGearRev(CALIBS *p, LgInt AbsRev)
{
	return DivKQ1(AbsRev, p->GearRatio, 1000, *p->PosSensPow);
}
/*
LgUns CalcClbPercentToAbs(CALIBS *p, Uns Percent)
{
	return MpyDivK(p->FullStep, Percent, 1000, 0);
}
*/
void CalcClbCycle(CALIBS *p)	// �������� ������ ������ ������ ��� ���������� �����
{
	if (!p->Indication->Status) {p->CycleData = 0; return;}	//	���� �� �������������� �� ������
	
	if (p->Zone != 0)	// ���� ��������� � ���� ����������
	{
	    if (!p->CycleData && p->Zone != 0x3) p->CycleData = p->Zone;
		
		if (p->Zone == (p->CycleData & CLB_FLAG))	// ���� ��������������
		{
			if (p->CycleData & 0x40) return;
			p->CycleData |= 0x40;
			if (p->CycleData & 0x80)
			{
				*p->CycleCnt = *p->CycleCnt + 1;
				p->CycleData &= ~0x80;
			}
		}
		else
		{
			if (p->CycleData & 0x40)
			{
				p->CycleData &= ~0x40;
				p->CycleData |=  0x80;
			}
		}
	}
}

static Byte CheckStatus(CALIBS *p, Uns Condition) // �������� ������� - 1 ���� �� ��������������, 0 ���� �������������� ��� ��������� ���� ������ ��� ���� ��������������
{
	if ((p->Indication->Status & Condition) || p->ResetFlag || p->AutoStep) // ���� ��������������, ��� ��������� ���� ������, ��� �������� ��������������, ��
	{
		p->CancelFlag = TRUE; // ���������� ���� ������ �������
		return 0;			  // ������� ��������� 0
	}
	return 1;				//����� ������� ��������� 1
}
