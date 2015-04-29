#include "btwt41.h"
#include "structs.h"
#include "interface.h"

#define BT_TIMER_SCALE			PRD_10HZ
#define BT_TIMER				1.00 * BT_TIMER_SCALE

Byte RxState = 0;

void BtWTInit(TBt *p);
void SendOneString(TBt *p, char *String);
void SendTwoString(TBt *p, char *FirstString, char *SecondString);
void SendCommandOne(TBt *p, char *ComStr);
void SendCommandTwo(TBt *p, char *ComStr, char *AddStr);

void ClearInitValues(TBt *p);
void ClearValues(TBt *p);
__inline void RxCommandMode(TBt *p);
__inline void RxDataMode(TBt *p);
Bool CheckString(TBt *p, char *Str);



void BtWTInit(TBt *p)
{

/*
//	p->EnableRx();
	p->StrIndex = 0;
	p->CmdState = 0;

	p->State = 0;
	p->Status = 0;
	p->StrIndex = 0;
	p->TxBusy = false;
	p->Error = false;
*/

	#if BT_DBG
	p->RxBytesCount = 0;
	p->TxBytesCount = 0;
	#endif

	/*
 	Im.IsTxBusy = &p->TxBusy;
	Im.TransmitByte = p->TransmitByte;
	Im.EnableReceive = p->EnableRx;
	Im.EnableTransmit = p->EnableTx;
	*/

	p->BtHardwareSetup(true);

	// ����� ��������� Bluetooth ��������� ������
	p->Timer = p->Period;

}

void BtWTUpdate(TBt *p)
{
	switch (p->State)
	{
		// ������������ ��������
		case 0: BtWTInit(p);
				p->State++;
				break;
				
		// ����������� ������ ���� ����� ����� �������������
		case 1: if (!p->Timer)	
				{ 
					ClearValues(p);	p->State++;
				}
				break;

		//
		case 2: SendCommandOne(p, CMD_CONTROL_CONFIG);
				if (p->Status == BT_RECEIVE_COMPLETE)
				{ 
					ClearValues(p);	p->State++;
				}
				break;

		case 3: SendCommandOne(p, CMD_CONTROL_ECHO);
				if (p->Status == BT_RECEIVE_COMPLETE)	
				{ 
					ClearValues(p);	p->State++;
				}
				break;
				
		case 4:	SendCommandTwo(p, CMD_CONTROL_NAME, p->DeviceNameString);
				if (p->Status == BT_RECEIVE_COMPLETE)	
				{ 
					ClearValues(p);	p->State++;
				}
				break;

		case 5: SendCommandTwo(p, CMD_CONTROL_AUTH, p->DeviceAuthCodeString);
				if (p->Status == BT_RECEIVE_COMPLETE)	
				{ 
					ClearValues(p);	p->State++;
				}
				break;
		case 6: SendCommandOne(p, CMD_CONTROL_BAUD);
				if (p->Status == BT_RECEIVE_COMPLETE)	
				{ 
					ClearValues(p);	p->State++;
				}
				break;

		// ����� ����� ���� �������� ������� ������� ������ ������
		// ���� �������� ��� ���

		// ����� �������� ����������
		case 7:	if (p->Status == BT_RECEIVE_COMPLETE)
				{
		 			if (CheckString(p, "RING"))				// ������� ����������
						p->State++;							// ������ ������ RING
					else 
						ClearValues(p);						// ������ ����� ��������, ���� ������ ���-�� ������
				}											// ������ �������� OK ��� PAIR
				break;

		// ������� ����������� �� �����������
		/*	if (CheckString(p, "PAIR"))				*/

		case 8: p->Mode = BT_DATA_MODE;					// ������� � ����� ������
				ClearValues(p);
				p->State++;
				break;

		case 9: if (p->Mode == BT_COMMAND_MODE)			// �������� � ������ ������,
				{										// ������� ������� � ����� ������
					ClearValues(p);
					p->State = 7;						// ������� � ����� �������� ����������
				}										
				break;				
	}

	p->IsConnected = (p->Mode == BT_DATA_MODE);
}

void ClearValues(TBt *p)
{
	p->StrIndex = 0;
	p->CmdState = 0;
	p->Status = BT_IDLE;
}

// �������� ������� �� ���, �.�. strlen ������� ���-�� �����
void SendCommandOne(TBt *p, char *ComStr)
{
	switch(p->CmdState)
	{
		case 0: p->EnableTx();
				p->StrIndex = 0;
				p->CmdState = 1;
				break;

		case 1: SendOneString(p, ComStr);

				if (p->Status == BT_TRANSMIT_COMPLETE)
					p->CmdState = 2;
				break;

		case 2:	p->EnableRx();
				p->StrIndex = 0;
				p->CmdState = 3;
				break;
				
		case 3:	if (p->Status == BT_RECEIVE_COMPLETE)		// ���� ���������� ������, �.�. ��� �������� ������� ������ ��������
					p->CmdState = 4;
				break;
	}
}


void SendCommandTwo(TBt *p, char *ComStr, char *AddStr)
{
	switch(p->CmdState)
	{
		case 0: p->EnableTx();
				p->StrIndex = 0;
				p->CmdState = 1;
				break;

		case 1: SendTwoString(p, ComStr, AddStr);
				if (p->Status == BT_TRANSMIT_COMPLETE)
					p->CmdState = 2;
				break;

		case 2:	p->EnableRx();
				p->StrIndex = 0;
				p->CmdState = 3;
				break;
				
		case 3:	if (p->Status == BT_RECEIVE_COMPLETE)		// ���� ���������� ������, �.�. ��� �������� ������� ������ ��������
					p->CmdState = 4;
				break;
	}
}



void BluetoothRxHandler(TBt *p)
{
	// ���������� ���������� ������� �� �������� ������ Bluetooth
	if 		(p->Mode == BT_COMMAND_MODE)	RxCommandMode(p);
	else if (p->Mode == BT_DATA_MODE)		RxDataMode(p);
}

__inline void RxCommandMode(TBt *p)
{
	char Data;

	p->Error = p->CheckCommError();

	if (p->Error)
		return;

	Data = p->ReceiveByte();

#if BT_DBG
	p->RxBytesCount++;
#endif

	if (p->StrIndex <= BT_RX_BUFFER_SIZE)	
		p->RxBuffer[p->StrIndex] = Data;

	// ����� ��������� ������� - 2 ������� "��", ������ ��������� ����� "\r\n", ������� ���� ������ �������
	// if ((p->StrIndex >= 2) && (Data == '\n'))
		p->Status = (Data == '\n') ? BT_RECEIVE_COMPLETE : BT_RECEIVE_BUSY;

	p->StrIndex++;
}

__inline void RxDataMode(TBt *p)
{
	char Data;

	p->Error = p->CheckCommError();

	if (p->Error)
		return;

	Data = p->ReceiveByte();

#if BT_DBG
	p->RxBytesCount++;
#endif

	switch (++RxState)
	{
		case 0: if (Data != 'N')	RxState = 0;	break;
		case 1: if (Data != 'O')	RxState = 0;	break;
		case 2: if (Data != ' ')	RxState = 0;	break;
		case 3: if (Data != 'C')	RxState = 0;	break;
		case 4: if (Data != 'A')	RxState = 0;	break;
		case 5: if (Data != 'R')	RxState = 0;	break;
		case 6: if (Data != 'R')	RxState = 0;	break;
		case 7: if (Data != 'I')	RxState = 0;	break;
	}

	// ����� ������ ��� ���.������
	ImReceiveData(&Im, Data);

	if (RxState >= 7)	
	{	
		p->Mode = BT_COMMAND_MODE;
		RxState = 0;
	}
}

void BluetoothTxHandler(TBt *p)
{
#if BT_DBG
	p->TxBytesCount++;
#endif

	p->TxBusy = false;
}

void BtTimer(TBt *p)
{
	if (p->Timer > 0) p->Timer--;
}

Bool CheckString(TBt *p, char *Str)
{
	Uns i, j;
	Uns Lenght = strlen(Str);						// ����� ������� ������
	Uns BufIndex = 0;	Uns CheckIndex = 0;

	p->StrIndex = 0;

	if (Lenght > BT_RX_BUFFER_SIZE)		return false;							// ���� ������ ��������� ����� ������, �� �������
	else								BufIndex = BT_RX_BUFFER_SIZE - Lenght;	// ������� ������ �������� ������ � ������
															
	for (i = 0; i <= BufIndex; i++)						// �������� ��������� ��� ������ � �������� ������� �������, ����� � �������
	{													// � �.�. �� ��� ��� ���� �� �������� ������ �������� 
		for (j = 0; j < Lenght; j++)					// ���������� ������� � ������ � ��������� � ������
		{												// �� ��� ��� ���� �� �������� ����� ������
			if (Str[j] == p->RxBuffer[j+i])				// ���� ������� �������, �� �������������� CheckIndex
				CheckIndex++;			
		}
		if (Lenght == CheckIndex)	return true;		// ���� ����� ������ ������� � CheckIndex, �� �������� �������
		else						CheckIndex = 0;		// ����� �������� ������ � ���������� ������
	}	

	return false;										// ������ �� �������
}


void SendOneString(TBt *p, char *String)
{
	char symbol = 0;

	if (!p->TxBusy)
	{
		symbol = String[p->StrIndex];					// ��������� ������� ������
		if (symbol == '\0')								// �������� ����� ������
		{
			p->Status = BT_TRANSMIT_COMPLETE;			// ������ ���������� ��������
			p->StrIndex = 0;
		}
		else
		{
			p->StrIndex++;
			p->Status = BT_TRANSMIT_BUSY;				// ������ ��������
			p->TxBusy = true;							// ���������� ���� �������� 
			p->TransmitByte(symbol);					// �������� �� SCI
		}
	}
}


void SendTwoString(TBt *p, char *FirstString, char *SecondString)
{
	char symbol = 0;

	static Bool isFirstStr = true;
	static Bool isEndSymbols = false;
	static Byte len1 = 0;
	static Byte len2 = 0;

	if (!p->TxBusy)
	{
		if (p->Status != BT_TRANSMIT_BUSY)
		{
			len1 = strlen(FirstString);
			len2 = strlen(SecondString);
		}

		if (isFirstStr && (p->StrIndex >= len1))
		{
			p->StrIndex = 0;
			isFirstStr = false;
		}

		if (!isFirstStr && (p->StrIndex >= len2))
		{ 
			isEndSymbols = true;
			p->StrIndex = 0;
		}

		symbol = (isFirstStr) ? FirstString[p->StrIndex] : SecondString[p->StrIndex];

		if (isEndSymbols && p->StrIndex < 2)
			symbol = (p->StrIndex == 0) ? '\r' : '\n';
		else if (isEndSymbols && p->StrIndex >= 2)
			symbol = '\0';

		if (symbol == '\0')								// �������� ����� ������
		{
			p->Status = BT_TRANSMIT_COMPLETE;			// ������ ���������� ��������
			p->StrIndex = 0;
			isFirstStr = true;
			isEndSymbols = false;
			len1 = 0;
			len2 = 0;
		}
		else
		{
			p->StrIndex++;
			p->Status = BT_TRANSMIT_BUSY;				// ������ ��������
			p->TxBusy = true;							// ���������� ���� �������� 
			p->TransmitByte(symbol);					// �������� �� SCI
		}
	}
}
