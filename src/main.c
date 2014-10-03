#include "config.h"
#include "system.h"

Uint16 CpuTime = 0;
Uint16 CpuTime1 = 0;
Uint16 CpuTime2 = 0;
Uint16 CpuTimeMax = 0;



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



//!!!
	GrA->Status.bit.Power = 1;			// ���������� ��� ������� ���������� �������
	DisplayRestartFlag = true;

	//!!!
	// ����������������� ��� ������ ����� (IRDA)
//	GpioDataRegs.GPBDAT.bit.GPIO33 = 0;
//	DelayUs(5);
//	GpioDataRegs.GPADAT.bit.GPIO25 = 1;
	

	#if BUR_M
	ContactorControl(cgStop);
	#endif

//	GrA->Status.bit.Stop = 1;

	EnableInterrupts();
	
	while(1)
	{
		SerialCommUpdate(&Mb);
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
