
#include "config.h"

#define MB_SCALE		HZ

#define TOUT_1_5		(Uint16)(0.300 * MB_SCALE)
#define TOUT_3_5		(Uint16)(0.400 * MB_SCALE)
#define TOUT_WAIT		(Uint16)(0.000 * MB_SCALE)
#define TOUT_CONN		(Uint16)(3.000 * MB_SCALE)
#define TOUT_ACK		(Uint16)(1.000 * MB_SCALE)

#define INIT_CRC		0xFFFF
#define GOOD_CRC		0x0000
#define GENER_CRC		0xA001
#define CRC_MODE		1

#define TEK_MB_START_ADDR		40000
#define CHECK_TEK_MB_ADDR(Addr)	(((Addr>>8) & 0x9C) == 0x9C)
#if BUR_M
#define TEK_DISCR_TEST_MASK 	8167
#else
#define TEK_DISCR_TEST_MASK 	8160
#endif

TMbPort Mb;

Bool MbConnect = false;

Uns TempMbFlag = 0;

Uint16 CrcTable[256];
Uint16 MbTmpData[MB_DATA_MAX];
static Uns BaudRates[7] = SCI_DEFAULT_BAUD_RATES;

inline void   ModBusSetup(TMbPort *, Uint16 UartID, TTrEnable TrEnable);
static void   ModBusReset(TMbPort *);
inline void   ModBusRecieve(TMbPort *);
static Byte	  ReadRegs(TMbPort *Port, Uint16 *Data, Uint16 Addr, Uint16 Count);
static Byte   WriteRegs(TMbPort *Port, Uint16 *Data, Uint16 Addr, Uint16 Count);
//static void   ReadAckRegs(TMbPort *Port, Uint16 Addr, Uint16 Count);
//static void   WriteAckRegs(TMbPort *Port, Uint16 Addr, Uint16 Count);
inline void   GenCrcTable(void);
inline Uint16 CalcCRC(Uint16 CRC, Uint16 c);
#if CRC_MODE
static Uint16 CalcBufCRC(Uint16 *Buf, Uint16 Count);
#endif
inline Uint16 CalcTout(Uint16 Tout, Uint16 Baud);
inline Uint16 TimerPending(Uint16 *Timer);
static void   MbTxCtrl(Uint16 State);

void SerialCommInit(void)
{
	GenCrcTable();
	
	ModBusSetup(&Mb, MB_SCIID, MbTxCtrl);
}

void ModBusControl(void)
{
	if (GrD->RsReset) {SerialCommRefresh(); GrD->RsReset = 0;}
}

void SerialCommUpdate(TMbPort *Port)
{
	MbConnect = Port->Frame.ConnFlg;								// Состояние связи Modbus

	if (!Port->Frame.NewMsg) return;
	Port->Frame.NewMsg = 0;											// Перед switch'ем, а не после

	switch(Port->Frame.Flg3_5)
	{
		case 0:
			ModBusRecieve(Port);
			#if !CRC_MODE
			Port->Frame.Crc = INIT_CRC;
			#endif
			Port->Frame.Counter = 0;								// в каждом case обнулять счетчик
			break;
		case 1:
			Port->Frame.Counter = 0;
			SCI_transmit(Port->Params.UartID, Port->Frame.Buf[0]);
			break;
		case 2:
			Port->Params.TrEnable(0);
			SCI_tx_disable(Port->Params.UartID);
			#if !CRC_MODE
			Port->Frame.Crc = INIT_CRC;
			#endif
			Port->Frame.Flg3_5 = 0;
			Port->Frame.Counter = 0;
			SCI_rx_enable(Port->Params.UartID);
			break;
	}	
}

void SerialCommRxHandler(TMbPort *Port)
{
	Uint16 Data;

	Port->Stat.RxBytesCount++;
	
	Data = SCI_getstatus(Port->Params.UartID);
	if (Data & SCI_BREAK) {SCI_reset(Port->Params.UartID); return;}

	Data = SCI_recieve(Port->Params.UartID);
	if (Port->Frame.Counter < MB_FRAME_MAX)
	{
		Port->Frame.Buf[Port->Frame.Counter++] = Data;
		#if !CRC_MODE
		Port->Frame.Crc = CalcCRC(Port->Frame.Crc, Data);
		#endif
		Port->Frame.Timer1_5 = Port->Params.Timeout1_5;
		Port->Frame.Timer3_5 = Port->Params.Timeout3_5;
	}
}

void SerialCommTxHandler(TMbPort *Port)
{
	Uint16 Data;

	Port->Stat.TxBytesCount++;

	if (++Port->Frame.Counter < Port->Frame.Size)
	{
		Data = Port->Frame.Buf[Port->Frame.Counter];
		#if !CRC_MODE
		Port->Frame.Crc = CalcCRC(Port->Frame.Crc, Data);
		#endif
		SCI_transmit(Port->Params.UartID, Data);
	}
	#if !CRC_MODE
	else switch(Port->Frame.CrcIndex)
	{
		case 0:
			SCI_transmit(Port->Params.UartID, Port->Frame.Crc & 0xFF);
			Port->Frame.CrcIndex = 1;
			break;
		case 1:
			SCI_transmit(Port->Params.UartID, Port->Frame.Crc >> 8);
			Port->Frame.CrcIndex = 2;
			break;
		case 2: 
			Port->Frame.Flg3_5 = 2;
			Port->Frame.Timer3_5 = Port->Params.Timeout3_5;
			break;
	}
	#else
	else
	{
		Port->Frame.Flg3_5 = 2;
		Port->Frame.Timer3_5 = Port->Params.Timeout3_5;
	}
	#endif
}

void SerialCommTimings(TMbPort *Port)
{
	if (!TimerPending(&Port->Frame.Timer1_5))  {SCI_rx_disable(Port->Params.UartID);}
	if (!TimerPending(&Port->Frame.Timer3_5))  {Port->Frame.NewMsg = 1;}
	if (!TimerPending(&Port->Frame.TimerConn)) {Port->Frame.ConnFlg = 0; Port->Frame.Exception = EX_NO_CONNECTION;}
	if (!TimerPending(&Port->Frame.TimerAck))  {Port->Frame.Acknoledge = 0;}
}

void SerialCommRefresh(void)
{
	ModBusReset(&Mb);
}

inline void ModBusSetup(TMbPort *Port, 
	Uint16 UartID, TTrEnable TrEnable)
{
	memset(&Port->Frame, 0, sizeof(TMbFrame));
	memset(&Port->Stat,  0, sizeof(TMbStat));

	Port->Params.UartID   = UartID;
	Port->Params.Slave    = GrB->RsStation;
	Port->Params.BaudRate = BaudRates[GrB->RsBaudRate];
	Port->Params.Parity   = GrB->RsMode;
	Port->Params.TrEnable = TrEnable;
}

static void ModBusReset(TMbPort *Port)
{
	Port->Params.Slave    = GrB->RsStation;
	Port->Params.BaudRate = BaudRates[GrB->RsBaudRate];
	Port->Params.Parity   = GrB->RsMode;

	SCI_init(Port->Params.UartID, SCI_BRR(Port->Params.BaudRate), Port->Params.Parity, 8);
	
	Port->Params.Timeout1_5 = CalcTout(TOUT_1_5, Port->Params.BaudRate);
	Port->Params.Timeout3_5 = CalcTout(TOUT_3_5, Port->Params.BaudRate);
	Port->Params.TimeoutAck = TOUT_ACK;
	
	Port->Frame.NewMsg     = 1;
	Port->Frame.Flg3_5     = 2;
	Port->Frame.Timer1_5   = 0;
	Port->Frame.Timer3_5   = 0;
	Port->Frame.TimerConn  = 0;
	Port->Frame.TimerAck   = 0;
	Port->Frame.Acknoledge = 0;
}

inline void ModBusRecieve(TMbPort *Port)
{
	Uint16 Res, Func, Addr, Count;
	
	Port->Frame.Size = Port->Frame.Counter;
	
	#if CRC_MODE
	Port->Frame.Crc = CalcBufCRC(Port->Frame.Buf, Port->Frame.Size);
	#endif
// ! if ((Port->Frame.Size < 5) || 
	if ((Port->Frame.Buf[0] != Port->Params.Slave) ||
		 (Port->Frame.Crc != GOOD_CRC))
	{
		SCI_rx_enable(Port->Params.UartID);
		return;
	}
	
	Func  = Port->Frame.Buf[1];
	Addr  = (Port->Frame.Buf[2] << 8) | Port->Frame.Buf[3];
	Count = (Port->Frame.Buf[4] << 8) | Port->Frame.Buf[5];
	
	Port->Frame.Exception = 0;

	{
		switch(Func)
		{
			case MB_DIAGNOSTICS:
				if (Addr) Port->Frame.Exception = EX_ILLEGAL_FUNCTION;
				else Port->Frame.Size = Port->Frame.Size - 2;
				break;
			case MB_REP_SLAVE_ID:
				break;
			default:
				if (Port->Frame.Size < 5)
					{SCI_rx_enable(Port->Params.UartID); return;}

				if (!Count || (Count > MB_DATA_MAX))
					{Port->Frame.Exception = EX_ILLEGAL_DATA_VALUE; break;}
	// !
				Res = 1;

				if (GrC->MbOffsetMode && (Addr >= TEK_MB_START_ADDR))
				{
					Addr = Addr - TEK_MB_START_ADDR;
				}
				if (GrC->ModbusConfiguration && (Addr >= 40000))		// 40000
				{
					Addr = Addr - (TEK_MB_START_ADDR+1);						// 40001 - раньше с него начиналась группа Т
				}
				//if (Tmp <= (RAM_DATA_SIZE + 5)) {Res = 1;}
				//else if (CHECK_TEK_MB_ADDR(Addr))	{Res = 5;}
	//			else if ((Addr >= LOG_EV_RAM_DATA_ADR) && (Tmp <= LOG_EV_RAM_DATA_LADR))      
	//				  {Res = 4; Addr = Addr - LOG_DATA_ADDR;}
				//else {Res = 0;}

				if (!Res) {Port->Frame.Exception = EX_ILLEGAL_DATA_ADDRESS; break;}

				switch(Func)
				{
					case MB_READ_REGS:
						switch(Res)
						{
							case 1: Port->Frame.Exception = ReadRegs(Port, (Uint16 *)&Ram, Addr, Count);
									break;
							//case 2:  ReadRegs(Port, (Uint16 *)MON_CH_ADR, Addr, Count); 	break;
							//case 4:  ReadAckRegs(Port, Addr + LOG_MEM_ADDR,  Count); 		break;
							//case 5: Port->Frame.Exception = ReadRegs(Port, (Uint16 *)&RamTek, (Addr - TEK_MB_START_ADDR), Count);
							//		break;
							default: Port->Frame.Exception = EX_ILLEGAL_FUNCTION;
						}
						break;
					case MB_WRITE_REGS:
						switch(Res)
						{
							case 1:
								if ((Addr >= REG_TASK_CLOSE)&&(Addr <= REG_RS_RESET))
								Mcu.EvLog.Source = CMD_SRC_SERIAL;
															TempMbFlag = 1;
								Port->Frame.Exception = WriteRegs(Port, (Uint16 *)&Ram, Addr, Count);
								if (!Port->Frame.Exception) SerialCommRefresh();

								break;
							//case 5:
							//	Port->Frame.Exception = WriteRegsTek(Port, (Uint16 *)&RamTek, (Uint16 *)&Ram, (Addr - TEK_MB_START_ADDR), Count);
							//	if (!Port->Frame.Exception) SerialCommRefresh();
							//	break;
							default: Port->Frame.Exception = EX_ILLEGAL_FUNCTION;
										
						}
						break;

						case MB_WRITE_REG:
						switch(Res)
						{
							case 1:
								Port->Frame.Exception = WriteRegs(Port, (Uint16 *)&Ram, Addr, Count);
								if (!Port->Frame.Exception) SerialCommRefresh();
								break;
							default: Port->Frame.Exception = EX_ILLEGAL_FUNCTION;
										
						}
						break;

					default:
						Port->Frame.Exception = EX_ILLEGAL_FUNCTION;
				}
		}
	}
		
	if (!Port->Params.Slave)
	{
		Port->Frame.Flg3_5 = 2;
		Port->Frame.Timer3_5 = Port->Params.Timeout3_5;		
	}
	else
	{
		if (Port->Frame.Exception)
		{
			if (Port->Frame.Exception == EX_ACKNOWLEDGE)
			{
				Port->Frame.Acknoledge = 1;
				Port->Frame.TimerAck = Port->Params.TimeoutAck;
			}
			Port->Frame.Buf[1] |= 0x80;
			Port->Frame.Buf[2]  = Port->Frame.Exception;
			Port->Frame.Size    = 3;
		}
		else
		{
			Port->Frame.ConnFlg = 1;
			Port->Frame.TimerConn = TOUT_CONN;
		}

		#if CRC_MODE
		Port->Frame.Crc = CalcBufCRC(Port->Frame.Buf, Port->Frame.Size);
		Port->Frame.Buf[Port->Frame.Size++] = Port->Frame.Crc & 0xFF;
		Port->Frame.Buf[Port->Frame.Size++] = Port->Frame.Crc >> 8;
		#else
		Port->Frame.CrcIndex = 0;
		Port->Frame.Crc = CalcCRC(INIT_CRC, Port->Frame.Buf[0]);
		#endif

		SCI_tx_enable(Port->Params.UartID);
		Port->Params.TrEnable(1);

		Port->Frame.Flg3_5 = 1;
		Port->Frame.Timer3_5 = Port->Params.Timeout3_5 + TOUT_WAIT;
	}

	//!!! или в ModBusRecieve
//	MbConnect = !Port->Frame.Exception;

}

static Byte ReadRegs(TMbPort *Port, Uint16 *Data, Uint16 Addr, Uint16 Count)
{
	Uint16 i, Tmp, *Buf;

	Buf = &Port->Frame.Buf[3];
	for (i=0; i < Count; i++)
	{
		Tmp = Data[Addr++];
		*Buf++ = Tmp >> 8;
		*Buf++ = Tmp & 0xFF;
	}
	
	Port->Frame.Buf[2] = Count << 1;
	Port->Frame.Size   = Port->Frame.Buf[2] + 3;

	return 0;
}

static Byte WriteRegs(TMbPort *Port, Uint16 *Data, Uint16 Addr, Uint16 Count)
{
	struct MENU_DCR Dcr;
	struct MENU_VAL_CFG *Val = &Dcr.Config.Val;
	Uint16 i, Tmp, *Buf;
	Bool Nvm = False;


	if (Port->Frame.Buf[6] != (Count << 1)) return EX_ILLEGAL_DATA_VALUE;
  	else if (Port->Frame.Size != (Port->Frame.Buf[6] + 9)) return EX_ILLEGAL_DATA_VALUE;
	else
	{
		Buf = &Port->Frame.Buf[7];
		for (i=0; i < Count; i++)
		{
			Tmp  = (*Buf++) << 8;
			Tmp |= (*Buf++) & 0xFF;
			GetDcr(i + Addr, &Dcr);
			if (Val->ReadOnly) {MbTmpData[i] = Data[i+Addr]; continue;}
			if (!CheckRange(Tmp, Dcr.Min, Dcr.Max))
				return EX_ILLEGAL_DATA_VALUE; 
			if (Val->Memory) 
			{
				Nvm = True;
				LogParam.MbBuffer[LogParam.MbIndex] = i + Addr;			// Запомнили адрес параметра, инкрементировали индекс
				LogParam.MbIndex++;
			}
			MbTmpData[i] = Tmp;
		}
		Port->Frame.Size = 6;	// ???
	}

	
//	Проверяем была ли подана команда по последовательному интерфейсу.
//	Если была подана, а режим ДУ отключен, то выдваем исключение

	if (InRange(REG_CONTROL, Addr, Addr + (Count - 1)))
	{
		Tmp = MbTmpData[REG_CONTROL - Addr];
		// Если стоит блокировка, то не пропускаем команды, кроме команды стоп
		if (!(Mcu.ActiveControls & CMD_SRC_SERIAL) && (Tmp != 0) && Tmp != vcwStop)
			return EX_ILLEGAL_DATA_VALUE;
		if (Tmp == vcwStop) Mcu.Tu.Ready = False;
	}
	

	
	if (Nvm && !IsMemParReady()) return EX_SLAVE_DEVICE_BUSY;

	memcpy(&Data[Addr], MbTmpData, Count);
	
	if (Nvm)
	{
		WritePar(Addr, &Data[Addr], Count);
		RefreshData();



//	Здесь можно зафиксировать измененный параметр для журнала


//	Mcu.EvLog.Value = CMD_PAR_CHANGE; ???
	}
//	Mcu.EvLog.Source = CMD_SRC_SERIAL; ???

	return 0;
}

inline void GenCrcTable(void)
{
	Uint16 i, j, v;

	for (i=0; i < 256; i++)
	{
		v = i;
		for (j=0; j < 8; j++)
		{
			if (v & 1) v = (v >> 1) ^ GENER_CRC;
			else v = (v >> 1);
		}
		CrcTable[i] = v;
	}
}

inline Uint16 CalcCRC(Uint16 CRC, Uint16 c)
{
	return ((CRC >> 8) ^ CrcTable[(CRC ^ c) & 0xFF]);
}

#if CRC_MODE
static Uint16 CalcBufCRC(Uint16 *Buf, Uint16 Count)
{
	Uint16 CRC = INIT_CRC;
	while(Count--) CRC = CalcCRC(CRC, *Buf++); 
	return CRC;
}
#endif

inline Uint16 CalcTout(Uint16 Tout, Uint16 Baud)
{
	Tout = (Uint16)_IQ1div(Tout, Baud);
	if (!Tout) return 1;
	return Tout;
}

inline Uint16 TimerPending(Uint16 *Timer)
{
	if (*Timer > 0)
	{
		*Timer = *Timer - 1;
		if (*Timer == 0) return 0;
	}
	return 1;
}

static void MbTxCtrl(Uint16 State)
{
	RS_DIR = !State;
}

