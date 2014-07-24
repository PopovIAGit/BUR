

#include "config.h"

TPiData PiData;
Bool CsAtBusy = false;

TBt Bluetooth = BTWT41_DEFAULT;

// Шаблон имени устройства Bluetooth
//[Наименование устройста][Год изготовления 2 цифры][Номер блока 2 цифры]
#if BUR_M
char StrDev[] = {"BUR-M_000000"};			
#else
char StrDev[] = {"BUR-T_000000"};
#endif

Uns DbgDiscrIn = 0;

Uns PiCalcCRC(Byte *Buf, Byte Length);


void SerialCommunicationInit(void)
{
	Uns DevFactNum = GrC->FactoryNumber;
	Uns DevProdYear = GrC->ProductYear;

	memset(&PiData, 0, sizeof(TPiData));
	PiData.DiscrOut = 0;

// Настройка для Bluetooth

	if (DevFactNum > 9999)
		DevFactNum = 0;
	if (DevProdYear > 99)
		DevProdYear = 0;

	DecToStr(DevFactNum, &StrDev[11], 0, 3, False, False);
	DecToStr(DevProdYear, &StrDev[7], 0, 1, False, False);
	Bluetooth.DeviceNameString = &StrDev[0];
	Bluetooth.DeviceAuthCodeString = BT_AUTH_CODE_STRING;

}



void BtHardwareSetup(Bool Enable)
{
	SCI_init(BT_SCI, SCI_BRR(BT_SCI_BAUD), 0, 8);
	SciaRegs.SCICCR.bit.STOPBITS = 0;

	EnableBtRx();

// Питание Bluetooth включается сразу в InitHardware
/*
	GpioDataRegs.GPBDAT.bit.GPIO33 = 1;
	DelayUs(5);
	GpioDataRegs.GPADAT.bit.GPIO25 = 0;
*/
}

Bool CheckSciCommErr(void)
{
	Byte Data = SCI_getstatus(BT_SCI);
	Bool Error = false;
	
	if (Data & SCI_BREAK) 
		{	SCI_reset(BT_SCI); Error = true;	}

	return Error;
}

void EnableBtRx(void)
{
	SCI_tx_disable(BT_SCI);		
	SCI_rx_enable(BT_SCI);
}

void EnableBtTx(void)
{
	SCI_rx_disable(BT_SCI);		
	SCI_tx_enable(BT_SCI);
}

Byte ReceiveBtByte(void)
{
	return SCI_recieve(BT_SCI);
}

void TransmitBtByte(Byte Data)
{
	SCI_transmit(BT_SCI, Data);
}



void PiUpdate(void)
{
	register Uns CRC;

	switch (PiData.State)
	{
		case 0:	{							// Заполняем буфер данными
			SPI_init(PI_ID, SPI_MASTER, 0, SPI_BRR(PI_BAUD), 8);

			PiData.TxFrame[0] = 0x55;
			PiData.TxFrame[1] = PiData.DiscrOut;
			PiData.TxFrame[2] = PiData.DiscrOut2;

			CRC = PiCalcCRC(PiData.TxFrame, PI_FRAME_SIZE-2);
			PiData.TxFrame[PI_FRAME_SIZE-2] = CRC & 0xFF;
			PiData.TxFrame[PI_FRAME_SIZE-1] = CRC >> 8;

			PiData.State = 1;										
			PiData.Counter = 0;	
			AtCsSet(0); CsAtBusy = true;			
			break;
		}
		case 1:	{
			if (!PiData.Counter) PiData.Counter++;
			else if (!SPI_ready(PI_ID))	{	break;	}
			else
			{
				PiData.RxFrame[PiData.Counter-1] = SPI_recieve(PI_ID);
				if (++PiData.Counter > PI_FRAME_SIZE) 
					{PiData.State = 2; break;}
			}
			SPI_transmit(PI_ID, PiData.TxFrame[PiData.Counter-1]);
			break;
		}
		case 2: PiData.State = 3; break;
		default:	{
			AtCsSet(1); CsAtBusy = false;;
			CRC  = (Uns)PiData.RxFrame[PI_FRAME_SIZE - 2];
			CRC |= (Uns)PiData.RxFrame[PI_FRAME_SIZE - 1] << 8;

			if ((PiData.RxFrame[1] != 0xAA) && 
			    (CRC != PiCalcCRC(PiData.RxFrame, PI_FRAME_SIZE-2)))	{
				PiData.ConnErr++;	
			}
			else	{
				PiData.DiscrIn220 = PiData.RxFrame[2];
				PiData.DiscrIn24  = PiData.RxFrame[3];
					
				PiData.ConnTimer = 0;
				PiData.Connect = TRUE;
			}
			PiData.State = 0;
			break;
		}
	}
	
	if (PiData.ConnTimer < (Uns)PI_CONN_TOUT) PiData.ConnTimer++;
	else PiData.Connect = FALSE;
}


Uns PiCalcCRC(Byte *Buf, Byte Length)
{
  Uns CRC = 0;
  while(Length--) CRC = CRC + (Uns)*Buf++;
  return ~CRC;
}


