/*======================================================================
��� �����:          protectionI2T.c
�����:              Denis
��������:
������ �����-������� ������
======================================================================*/
#include "protectionI2T.h"

TAlarmI2T	i2tOverload;

// ������������ ������
void ProtectionI2T_Init(TAlarmI2T *p, Uns Freq)
{
	p->enable = true;
	p->isFault = false;
	p->minHighCurrent = (Uns)_IQ15int( _IQmpy (_IQ15(*p->nominalCurr), _IQ(K_HIGH_I)) );
	p->maxLowCurrent  = (Uns)_IQ15int( _IQmpy (_IQ15(*p->nominalCurr), _IQ(K_LOW_I)) );
	p->scale =_IQ15div(1, Freq);	// ������� ������� �������, ��������� �� �������, �� ������� ������� ����������
}
//--------------------------------------------------------
void ProtectionI2T_Update(TAlarmI2T *p)
{
	if (!p->enable)						// ���� "���������� ������"=0, �� ���� "����", ��
	{
		p->timer = 0;
		return;							// ������� �� �������
	}

	if (!p->isFault)					// ���� ������ ��� 
	{
		if (*p->inputCurr < p->maxLowCurrent)	// ���� ��� ���� ��������, ��� ������� �����-������� ������ �� �������
		{	
			p->timer = 0;			// ��������� ����������
		}
		else
		{
			p->timeout =  I2T_CONV( *p->inputCurr, *p->nominalCurr, _IQ15(50), _IQ15(1.4), _IQ15(60));
			if (p->timer < p->timeout)	// ���� ������ �� ������ ������� ������������ ������
				p->timer += p->scale;// ���������� ������
			else					// ��� ������ ������ ������ ������� ������������ ������
			{
				p->isFault = true;	// ���������� ������
				p->timer = 0;
			}
		}
	}
} 
//---------����� �����------------------------------------ 

