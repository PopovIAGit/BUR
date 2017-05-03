

#include "csl\csl_spi.h"
#include "csl\csl_memory.h"
#include "peref_drv_DPMA15.h"
//#include "timings.h"
#include "config.h"
//#include "chip\DSP280x_Spi.h"

#define SetCs()	p->CsFunc(0)
#define ClrCs()	p->CsFunc(1)

void encoderDPMA15_Init(EN_DPMA15 *p)
{
	p->SpiId = PLIS_SPI;              // ������������� SPI-����� (0-SPIA, 1-SPIB, ...)
	p->SpiBaud 	= SPI_BRR(1000);  // ������� ��������������� (����������� ��� ������������ ����)
	p->bitCount=0;        		  // ���������� ����/��� ������
	p->RevMax=0;			      // ����������� �������� ���������
	p->revolution=0;              // �������� ��������� � ������� � ������� 16 ��������
	p->EncoderData=0;		      // ������, ���������� �� ��������
	p->prevRevolution=0;	      // ���������� �������� ��������
	p->RevMisc=0;			      // ����������� �����/�������� ������������ �������
	p->RevErrCount=0;		      // ������� ������
	p->RevTimer=0;		          // ������� ��� ������� ������
	p->ResetCounter=0;	          // ������� ������ ��������
	p->ResetDelay=0;		      // ���� �������� ������ ��������
	p->Error=0;                   // ������� ����
	p->TmpError=0;		          // ������� ���������� ����, ������� ������������ �������������
	p->CsFunc(0);                 // ������� ������ ����������

}

// ������� ���������� ������ � ������� ��������� ����-15
void encoder_DPMA15_GetData(EN_DPMA15 *p) // 50 Hz
{
	Byte	Data1, Data2;				// ��� ������� ������ �� 8 ���

	//if (Eeprom.  != 0) return; //��������� �� ������ �� ����� SPI

	SPI_init(p->SpiId, SPI_MASTER, 0, p->SpiBaud, 8);

	SetCs();
	//DelayUs(); 			// �������� t1 = 10.4 �����
	// ��������� ������ ��������

	Data1 = SPI_send(p->SpiId, 0x00);
	DelayUs(ENC_ATMEGA_SPI_DELAY_US);
	Data2 = SPI_send(p->SpiId, 0x00);
	ClrCs();
												// ��� ����� �� ����� ��������� � ���� ���������� long
	p->EncoderData  = (Data1 << 8);
	p->EncoderData  |= Data2;
	p->revolution = p->EncoderData & 0x7fff;
}








