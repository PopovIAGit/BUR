#include "config.h"



__inline void ResetCapture(TPult *);



void PultImpulseCapture(TPult *p)
{
	IrDA_NoiseFilter(p);

	if ((ECap3Regs.ECFLG.bit.CEVT1)&&(p->ReadyToRecieve))							// ��������� ���� "�������" ������ ��������
	{
		if (!p->CapComplete)
		{
			if (p->FirstPulse)														// ��� ������� ��������
			{
				ECap3Regs.TSCTR = 0;
				ECap3Regs.CAP1 = 0;
				p->FirstPulse = false;
			}

			if (ECap3Regs.CAP1 > SYNC_PULSE_VALUE)									// ���� �������������
			{				
				if (++p->SyncPulsesCounter >= SYNC_PULSES_NUM)
				{
					p->isFrameRecieved = true;										// ���������� ���� ��������� 11 ����� ������
				}

				// ���������� ���������� ���� � ���
				p->CapCode |= ECap3Regs.ECCTL1.bit.CAP1POL << (SYNC_PULSES_NUM - p->SyncPulsesCounter);
				
				ECap3Regs.TSCTR = 0;			
			}

			ECap3Regs.ECCTL1.bit.CAP1POL = !ECap3Regs.ECCTL1.bit.CAP1POL;			// ������ ����������� CAP-������
		}

		p->CapCounter = 0;
		ECap3Regs.ECCTL2.bit.REARM = 1;
		ECap3Regs.ECCLR.bit.CEVT1  = 1;
	}
}
//===========================================================================================================
// ������� ���������� ����� ��-���������
// ������� �������:
// 1) ������ ������� ���������� ������ �������� �� 11 �����
// 2) ������ ��� ������ ����� ����� �������, �.�. ����� ������ 11 ��� ������ ����� ������� �� ������ ��������
//    � ������� ������� ������ ��������� � "1" � ������� ��������� �������
void IrDA_NoiseFilter(TPult *p)
{
	//--- 1 ����� ������� --- �������� �� ��������� ������ ������� ����� ������� ������ ������
	if (p->ReadyToRecieve == false)						// ���� ���� ���������� ������ ��� �� ���������
	{
		if ((ECap3Regs.ECFLG.bit.CEVT1 == 0)&&			// ���� ���������� ������ ������� �� ����������
			(ECap3Regs.ECCTL1.bit.CAP1POL == 1))		// � ������� ������� �������� � "1"
		{
			if (p->preFrameTimer++ >= PRE_FRAME_TIMEOUT)// ���� ������ ������� �������� �� �������� 50 ��
			{
				p->preFrameTimer = 0;
				p->ReadyToRecieve = true;				// �������, ��� ���� ����� � ������ �������
			}
		}
		else											// ���� ����� ������� ���������
			p->preFrameTimer = 0;						// ������ ����������. � ������ � ������ ������� �� ������
	}
	//--- 2 ����� ������� --- �������� �� ��������� ������ ������� ����� ������ ������ ������
	if (p->isFrameRecieved)						// ���� ������� ����� ������
	{
		p->postFrameTimer++;					// �������� ����������� ������

		if ((ECap3Regs.ECFLG.bit.CEVT1 == 1)||		// ���� ��������� ��������� ������ �������
		    (ECap3Regs.ECCTL1.bit.CAP1POL == 0))	// ��� ������� ������� ����� "0" ...
		{											// �� ������� �������� ����� ������ "�����"
			p->postFrameTimer = 0;					// ���������� ��� ����� � ������� �� �������
			ResetCapture(p);
			p->ReadyToRecieve = false;				// ���������� ����, ��� �� �� ������ � ������ ����� ������, ���� ������ �� "�����������"
			return;
		}

		if (p->postFrameTimer >= POST_FRAME_TIMEOUT)// ���� ������ ������� �������� �� ��������
		{
			p->isFrameRecieved = false;
			p->CapComplete = true;					// ���������� ���� ���������� ������
		}
	}
}
//===========================================================================================================

void PultTimer(TPult *p)
{
	if (++p->CapCounter >= PULT_CAP_RESET) 						// �������������� ������ �������
	{
		ResetCapture(p);
	}
}

void PultKeyExecute(TPult *p)
{
	if (!p->CapComplete)
		return;

	if ((p->CapCode >> 9) == 3) 
	{
		p->CapCode = (p->CapCode << 1) & 0x7FF;
	}

	if ((p->CapCode >> 10) != 1)								// ��������� ������� ���������� ����
		return;
	
	if (((p->CapCode >> 8) & 0x03) != BUR_IR_ADDRESS)			// ��������� ������������ ������
		return;

//	if (p->Key != 0)											������ ���� ���������� �������

	p->KeyCode = (p->CapCode & 0xFF);							// ��������� ��� �������

	switch (p->KeyCode)
	{
		case CODE_STOP:		p->Key = KEY_STOP;		break;
		case CODE_OPEN:		p->Key = KEY_OPEN;		break;
		case CODE_CLOSE:	p->Key = KEY_CLOSE;		break;
		case CODE_RIGHT:	p->Key = KEY_RIGHT;		break;
		case CODE_DOWN:		p->Key = KEY_DOWN;		break;
		case CODE_UP:		p->Key = KEY_UP;		break;
		case CODE_LEFT:		p->Key = KEY_LEFT;		break;
		case CODE_ENTER:	p->Key = KEY_ENTER;		break;
		case CODE_ESCAPE:	p->Key = KEY_ESCAPE;	break;
	}

	p->KeyCode = 0;

	ResetCapture(p);
	p->ReadyToRecieve = false;									// ���������� ����, ��� �� �� ������ � ������ ����� ������, ���� ������ �� "�����������"
}

__inline void ResetCapture(TPult *p)
{
		p->FirstPulse 		 = true;
		p->CapComplete 		 = false;
		p->isFrameRecieved   = false;
		p->CapCode 			 = 0;
		p->CapCounter 		 = 0;
		p->SyncPulsesCounter = 0;

		ECap3Regs.ECCTL1.bit.CAP1POL = 1;
		ECap3Regs.ECCTL2.bit.REARM 	 = 1;
		ECap3Regs.ECCLR.bit.CEVT1 	 = 1;
}
