
#include "config.h"

#define MB_SCALE		HZ

#define TOUT_1_5		(Uint16)(0.300 * MB_SCALE)
#define TOUT_3_5		(Uint16)(0.400 * MB_SCALE)
#define TOUT_WAIT		(Uint16)(0.000 * MB_SCALE)
#define TOUT_CONN		(Uint16)(3.000 * MB_SCALE)
#define TOUT_ACK		(Uint16)(1.000 * MB_SCALE)
//may habara
#define TOUT_PREAMBLE		(Uint16)(0.200 * MB_SCALE)
#define TOUT_POSTAMBLE		(Uint16)(0.200 * MB_SCALE)

#define TOUT_TESTSCI		(Uint16)(1.200 * MB_SCALE)


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

Uint16  TestSCI=0;
Uint16 EnabledTransmit=0;

TMbPort Mb;

Bool MbConnect = false;
extern Bool PI_CalibEnable; 			// Флаг разрешения калибровки платы ПИ
Uns TempMbFlag = 0;

Uint16 CrcTable[256];
Uint16 MbTmpData[MB_DATA_MAX];
static Uns BaudRates[7] = SCI_DEFAULT_BAUD_RATES;


inline void   ModBusSetup(TMbPort *, Uint16 UartID, TTrEnable TrEnable);
static void   ModBusReset(TMbPort *);
inline void   ModBusRecieve(TMbPort *);
static Byte   ReadRegs(TMbPort *Port, Uint16 *Data, Uint16 Addr, Uint16 Count);
static Byte   WriteRegs(TMbPort *Port, Uint16 *Data, Uint16 Addr, Uint16 Count);
//static void   ReadAckRegs(TMbPort *Port, Uint16 Addr, Uint16 Count);
//static void   WriteAckRegs(TMbPort *Port, Uint16 Addr, Uint16 Count);
inline void   GenCrcTable(void);
inline Uint16 CalcCRC(Uint16 CRC, Uint16 c);
#if CRC_MODE
static Uint16 CalcBufCRC(Uint16 *Buf, Uint16 Count);
#endif
inline Uint16 MaCalcTout(Uint16 Tout, Uint16 Baud);
void ReStartReadLine(TMbPort *Port);
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
	if (PauseModbus > 0)
	{
	    return;
	}

	//if(TestSCI==1){
	//    return;
	//}

	MbConnect = Port->Frame.ConnFlg;	// Состояние связи Modbus
	if (EnabledTransmit==1) return;
	if (!Port->Frame.NewMsg) return;
	Port->Frame.NewMsg = 0;			// Перед switch'ем, а не после
	ModBusRecieve(Port);
	//Port->Frame.Counter = 0;

	/*switch(Port->Frame.Flg3_5)
	{
		case 0:
			ModBusRecieve(Port);
			Port->Frame.Counter = 0;	// в каждом case обнулять счетчик
			break;
		case 1:
			Port->Frame.Counter = 0;
			SCI_transmit(Port->Params.UartID, Port->Frame.Buf[0]);
			break;

		case 2:
			Port->Params.TrEnable(0);
			SCI_tx_disable(Port->Params.UartID);
			Port->Frame.Flg3_5 = 0;
			Port->Frame.Counter = 0;
			SCI_rx_enable(Port->Params.UartID);
			break;
	}*/
}

void SerialCommRxHandler(TMbPort *Port)
{
	Uint16 Data;
	if (EnabledTransmit==1) return;
	Port->Stat.RxBytesCount++;
	
	Data = SCI_getstatus(Port->Params.UartID);
	if (Data & SCI_BREAK)
	{
	    if (Port->Params.BaudRate!=11520)
	    {
		EnabledTransmit = 1;
	        SCI_rx_disable(Port->Params.UartID);
		SCI_tx_disable(Port->Params.UartID);
		Port->Params.TrEnable(0);
		Port->Frame.Size = 0;
		Port->Frame.Counter = 0;
		Port->Frame.Timer3_5 = 0;
		Port->Frame.TimerTestDelay = Port->Params.TimeoutTestDelay;
	    }

	    SCI_reset(Port->Params.UartID);
	    return;
	}

	Data = SCI_recieve(Port->Params.UartID);
	if (Port->Frame.Counter < MB_FRAME_MAX)
	{
		Port->Frame.Buf[Port->Frame.Counter++] = Data;
		Port->Frame.Timer3_5 = Port->Params.Timeout3_5;
	}
}

void SerialCommTxHandler(TMbPort *Port)
{
	Uint16 Data;

	if(TestSCI==1){
	    if (++Port->Frame.Counter <2)
	    {
		SCI_transmit(Port->Params.UartID, 0x55);
	    } else {
		Port->Frame.Counter=0;
		Port->Params.TrEnable(0);
		SCI_tx_disable(Port->Params.UartID);
		SCI_rx_enable(Port->Params.UartID);
		Port->Frame.TimerTestDelay = Port->Params.TimeoutTestDelay;
	    }
	}
	else {

	    Port->Stat.TxBytesCount++;

	    if (++Port->Frame.Counter < Port->Frame.Size)
	    {
		    Data = Port->Frame.Buf[Port->Frame.Counter];
		    SCI_transmit(Port->Params.UartID, Data);
		    //todo may habara
		    //взводим таймер для правильного завершения передачи
		    Port->Frame.TimerPost = Port->Params.TimeoutPost;
	    }
	}
}

void SerialCommTimings(TMbPort *Port)
{
	//may habara
        if (!TimerPending(&Port->Frame.TimerTestDelay))
        {
            SCI_reset(Port->Params.UartID);
            SCI_init(Port->Params.UartID, SCI_BRR(Port->Params.BaudRate), Port->Params.Parity, 8);
            SCI_rx_enable(Port->Params.UartID);
            EnabledTransmit = 0;
        }

	if (TestSCI==1){
	    if (!TimerPending(&Port->Frame.TimerTestDelay)){
		Port->Frame.Counter=0;
		SCI_rx_disable(Port->Params.UartID);
		SCI_tx_enable(Port->Params.UartID);
		Port->Params.TrEnable(1);
		SCI_transmit(Port->Params.UartID, 0x55);
	    }
	}

	if (!TimerPending(&Port->Frame.Timer3_5))
	{
	    SCI_rx_disable(Port->Params.UartID);
	    Port->Frame.NewMsg = 1;
	}
	//may habara
	if (!TimerPending(&Port->Frame.TimerConn)) {Port->Frame.ConnFlg = 0; }
						    //Port->Frame.Exception = EX_NO_CONNECTION;}
	//may habara
	// преамбула нужна для организации старта передачи
	if (!TimerPending(&Port->Frame.TimerPre))   {Port->Frame.Counter = 0;
				SCI_transmit(Port->Params.UartID, Port->Frame.Buf[0]);}
	// постамбула необходима для организации правильной логики завершения передачи
	if (!TimerPending(&Port->Frame.TimerPost))  {
						     //EnabledTransmit = 0;
						     Port->Params.TrEnable(0);
	    		                             SCI_tx_disable(Port->Params.UartID);
	    		                             SCI_rx_enable(Port->Params.UartID);
	    		                             Port->Frame.Counter = 0;
	}
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
	
	//may habara
	Port->Params.TimeoutPre = CalcTout(TOUT_PREAMBLE, Port->Params.BaudRate);
	Port->Params.TimeoutPost = CalcTout(TOUT_POSTAMBLE, Port->Params.BaudRate);
	Port->Params.TimeoutTestDelay = CalcTout(TOUT_TESTSCI, Port->Params.BaudRate);

	//may habara
	Port->Frame.NewMsg = 0;//may 1

	Port->Params.TrEnable(0);
	SCI_tx_disable(Port->Params.UartID);
	Port->Frame.Flg3_5 = 0;
	Port->Frame.Counter = 0;
	SCI_rx_enable(Port->Params.UartID);
	Port->Frame.TimerPre   = 0;
	Port->Frame.TimerPost  = 0;

	Port->Frame.Timer1_5   = 0;
	Port->Frame.Timer3_5   = 0;
	Port->Frame.TimerConn  = 0;
	Port->Frame.ConnFlg    = 0;
}

void ReStartReadLine(TMbPort *Port)
{
    Port->Params.TrEnable(0);
    Port->Frame.Size = 0;
    Port->Frame.Counter = 0;
    SCI_tx_disable(Port->Params.UartID);
    SCI_rx_enable(Port->Params.UartID);
    Port->Frame.Timer3_5 = 0;
}

inline void ModBusRecieve(TMbPort *Port)
{
	Uint16 Res, Func, Addr, Count;
	
	Port->Frame.Size = Port->Frame.Counter;
	
	#if CRC_MODE
	Port->Frame.Crc = CalcBufCRC(Port->Frame.Buf, Port->Frame.Size);
	#endif
	if ((Port->Frame.Buf[0] != Port->Params.Slave) ||
		 (Port->Frame.Crc != GOOD_CRC))
	{
		ReStartReadLine(Port);
		return;
	}
	
	Func  = Port->Frame.Buf[1];
	Addr  = (Port->Frame.Buf[2] << 8) | Port->Frame.Buf[3];
	Count = (Port->Frame.Buf[4] << 8) | Port->Frame.Buf[5];
	
	if (Port->Frame.Exception!=0)
	{	//test 485
	    Port->Frame.Exception = 0;
	}

	Port->Frame.Exception = 0;

	switch(Func)
	{
	    case MB_DIAGNOSTICS:
		if (Addr) Port->Frame.Exception = EX_ILLEGAL_FUNCTION;
		else Port->Frame.Size = Port->Frame.Size - 2;
	    break;
	    case MB_REP_SLAVE_ID:
	    break;
	    default:
		if (Port->Frame.Size < 5) { ReStartReadLine(Port); return;}
		if (!Count || (Count > MB_DATA_MAX)) {
		    Port->Frame.Exception = EX_ILLEGAL_DATA_VALUE; break;}
		Res = 1;

		if (GrC->MbOffsetMode && (Addr >= TEK_MB_START_ADDR))
		{
		    Addr = Addr - TEK_MB_START_ADDR;
		}
		if (GrC->ModbusConfiguration && (Addr >= 40000))		// 40000
		{
		    Addr = Addr - (TEK_MB_START_ADDR+1);			// 40001 - раньше с него начиналась группа Т
		}
		if (!Res) {Port->Frame.Exception = EX_ILLEGAL_DATA_ADDRESS; break;}

		switch(Func)
		{
		    case MB_READ_REGS:
				switch(Res)
				{
					case 1: Port->Frame.Exception = ReadRegs(Port, (Uint16 *)&Ram, Addr, Count);
					break;
					default: Port->Frame.Exception = EX_ILLEGAL_FUNCTION;
				}
				break;

		    case MB_WRITE_REGS:
		    case MB_WRITE_REG:
				switch(Res)
				{
					case 1:
						if ( ((Addr >= REG_TASK_CLOSE)&&(Addr <= REG_RS_RESET)) ||	// Если идет обращение по адресу от 210 по 221 (группа D - регистр Команд)
							  (Addr == REG_COM_REG) )								// или по адресу 3 (T4 Регистр команд)
						{
							Mcu.EvLog.Source = CMD_SRC_SERIAL;						// то выставляем источник команды RS-485
						}
						TempMbFlag = 1;
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

	if (!Port->Params.Slave)
	{
	    ReStartReadLine(Port);
	    return;
	}
	else
	{
	    if (Port->Frame.Exception)
	    {
		Port->Frame.Buf[1] |= 0x80;
		Port->Frame.Buf[2]  = Port->Frame.Exception;
		Port->Frame.Size    = 3;
	    }
	    else
	    {
		Port->Frame.ConnFlg = 1;
		Port->Frame.TimerConn = TOUT_CONN;
	    }

	    Port->Frame.Crc = CalcBufCRC(Port->Frame.Buf, Port->Frame.Size);
	    Port->Frame.Buf[Port->Frame.Size++] = Port->Frame.Crc & 0xFF;
	    Port->Frame.Buf[Port->Frame.Size++] = Port->Frame.Crc >> 8;

	    SCI_rx_disable(Port->Params.UartID);
	    SCI_tx_enable(Port->Params.UartID);
	    Port->Params.TrEnable(1);

	    Port->Frame.TimerPre = Port->Params.TimeoutPre;
	}

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

	// Для команды MB_WRITE_REGS - Если Count не совпадает - выдаем ошибку
	if ((Port->Frame.Buf[6] != (Count << 1))&&(Port->Frame.Buf[1] == MB_WRITE_REGS))
	{
		return EX_ILLEGAL_DATA_VALUE;
	}
	// Для команды MB_WRITE_REGS - Если длина посылки не бьется - выдаем ошибку
	else if ((Port->Frame.Size != (Port->Frame.Buf[6] + 9))&&(Port->Frame.Buf[1] == MB_WRITE_REGS))
	{
		return EX_ILLEGAL_DATA_VALUE;
	}
	// Для команды MB_WRITE_REG - Если длина посылки не бьется - выдаем ошибку
	else if ((Port->Frame.Size != 8)&&(Port->Frame.Buf[1] == MB_WRITE_REG))
	{
		return EX_ILLEGAL_DATA_VALUE;
	}
	else
	{
		if (Port->Frame.Buf[1] == MB_WRITE_REGS)
			Buf = &Port->Frame.Buf[7];
		else
		{
			Buf = &Port->Frame.Buf[4];
			Count = 1;
		}

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
				LogParam.MbBuffer[LogParam.MbIndex] = i + Addr;		// Запомнили адрес параметра, инкрементировали индекс
				LogParam.MbIndex++;
			}
			else if ( (Addr >= REG_TASK_CLOSE)&&(Addr <= REG_REV_CLOSE) )	// Ловушка для параметров калибровки. Они в память не записываются, но в журнал записываться должны
			{
				LogParam.MbBuffer[LogParam.MbIndex] = i + Addr;		// Запомнили адрес параметра, инкрементировали индекс
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

#if BUR_90
	// Если в параметр C1. Код доступа введен "особый пароль"
	if ((Addr == REG_FCODE)&&(Tmp == DEF_CALIB_PASS))
	{
		if (!PI_CalibEnable)				// Разрешаем калибровку платы ПИ, если она была запрещена
		{
			PI_CalibEnable = true;
			Mcu.Mpu.CalibEnableFlag = true;
		}
		else if (PI_CalibEnable)			// Запрещаем калибровку платы ПИ, если она была разрешена
		{
			PI_CalibEnable = false;
			Mcu.Mpu.CalibDisableFlag = true;
		}
		return 0;
	}
#endif

	if (Nvm && !IsMemParReady()) return EX_SLAVE_DEVICE_BUSY;

	memcpy(&Data[Addr], MbTmpData, Count);
	
	if (Nvm)
	{
		WritePar(Addr, &Data[Addr], Count);
		RefreshData(Addr);

//	Здесь можно зафиксировать измененный параметр для журнала
//	Mcu.EvLog.Value = CMD_PAR_CHANGE; ???
	}
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

inline Uint16 MaCalcTout(Uint16 Tout, Uint16 Baud)
{
	Tout = (Uint16)(Tout / Baud);
	if (!Tout) return 1;
	return Tout;
}

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

