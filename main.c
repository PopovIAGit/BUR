#include "config.h"
#include "system.h"

Uint16 CpuTime = 0;
Uint16 CpuTime1 = 0;
Uint16 CpuTime2 = 0;
Uint16 CpuTimeMax = 0;
extern Uns PauseModbus;

void main(void)
{
 	InitHardware();
	SystemInit();
	RimDevicesInit();
	MotorControlInit();
	SerialCommInit();
	InterfaceInit();
	ProtectionsInit();
	SerialCommunicationInit();
	SerialCommRefresh();

	GrA->Status.bit.Power = 1;			// Выставляем при наличии сервисного питания
	DisplayRestartFlag = true;
	MonitorInit();			// Инициализация графического монитора реального времени
	#if BUR_M
	ContactorControl(cgStop);
	#endif

	EnableInterrupts();
	
	PauseModbus = GrC->ModbusPauseStart;


	while(1)
	{
		if (PauseModbus == 0)	// Вызываем SerialCommUpdate только если прошел таймаут паузы
		{
			SerialCommUpdate(&Mb);
		}
		BtWTUpdate(&Bluetooth);
		ImUpdate(&Im);
	}
}

interrupt void CpuTimer0IsrHandler(void)
{
	ResetCpuTimerValue(&CpuTimer1Regs);

	CpuTime1 = (Uint16)(-ReadCpuTimerCounter(&CpuTimer1Regs));
	RtosExecute();
	CpuTime2 = (Uint16)(-ReadCpuTimerCounter(&CpuTimer1Regs)) - CpuTime1;

	#if IR_IMP_TEST
	IrImpCalc();
	#endif

	CpuTime = (Uint16)(-ReadCpuTimerCounter(&CpuTimer1Regs));
	if (CpuTime > CpuTimeMax) CpuTimeMax = CpuTime;

	PieCtrlRegs.PIEACK.bit.ACK1 = 1;
}

interrupt void ScibRxIsrHandler(void)
{
	SerialCommRxHandler(&Mb);
	PieCtrlRegs.PIEACK.bit.ACK9 = 1;
}

interrupt void ScibTxIsrHandler(void)
{
	SerialCommTxHandler(&Mb);
	PieCtrlRegs.PIEACK.bit.ACK9 = 1;
}

interrupt void SciaRxIsrHandler(void)
{
	BluetoothRxHandler(&Bluetooth);
	PieCtrlRegs.PIEACK.bit.ACK9 = 1;
}

interrupt void SciaTxIsrHandler(void)
{
	BluetoothTxHandler(&Bluetooth);
	PieCtrlRegs.PIEACK.bit.ACK9 = 1;
}
