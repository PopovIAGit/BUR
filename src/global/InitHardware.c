/*-----------------------------------
InitHardware

Модуль конфигурации оборудования
-------------------------------------*/
#include "config.h"

__inline void InitClocks(void);
__inline void InitGpio(void);
__inline void InitEPwm(void);
__inline void InitIrdaPwm(void);
__inline void InitAdc(void);
__inline void InitCap(void);

interrupt void CpuTimer0IsrHandler(void);
interrupt void SciaRxIsrHandler(void);
interrupt void SciaTxIsrHandler(void);
interrupt void ScibRxIsrHandler(void);
interrupt void ScibTxIsrHandler(void);


void InitHardware(void)
{
	// Initialize System Control

	// Функция инициализирует системные регистры по умолчанию
	// - Выключает Watchdog
	// - Выставляет PLLCR для надлежащей частоты
	// - Выставляет делитель частоты
	// - Включает тактовый сигнал для переферии
	InitSysCtrl(DSP28_PLLCR(SYSCLK, CLKIN), 0);

	// Инициализируем тактовый сигнал на переферию
	InitClocks();
	
	// Инициализируем GPIO
	InitGpio();
	InitSpiaGpio();
	InitSpicGpio();
	InitSciaGpio();
	InitScibGpio();
	
	// Disable global Interrupts and higher priority real-time debug events
	DINT;
	DRTM;
	
	// Initialize the PIE control registers to their default state
	InitPieCtrl();
		
	// Disable CPU interrupts and clear all CPU interrupt flags
	IER = 0x0000;
	IFR = 0x0000;
	
	// Initialize the PIE vector table with pointers to the shell Interrupt
	// Service Routines (ISR)
	InitPieVectTable();

	// User interrupts that are used in this project are re-mapped to
	// ISR functions found within this file
	EALLOW;
	PieVectTable.TINT0     = &CpuTimer0IsrHandler;
	PieVectTable.SCIRXINTA = &SciaRxIsrHandler;
	PieVectTable.SCITXINTA = &SciaTxIsrHandler;
	PieVectTable.SCIRXINTB = &ScibRxIsrHandler;
	PieVectTable.SCITXINTB = &ScibTxIsrHandler;
	EDIS;
	
	// Инициализируем переферию процессора
	InitEPwm();
	InitIrdaPwm();
	InitAdc();
	InitCap();
	ConfigCpuTimer(&CpuTimer0Regs, SYSCLK/HZ);
	ConfigCpuTimer(&CpuTimer1Regs, -1UL);
	
	#if defined(FLASH_PROJ)
	// Copy time critical code and Flash setup code to RAM
	// Если проект флешевый, то копируем начальный код проекта в ОЗУ
	// для более быстрого выполнения, т.к. частота флеш памяти ограничена
	MemCopy(&secureRamFuncs_loadstart, &secureRamFuncs_loadend, 
			&secureRamFuncs_runstart);
	
	// Call Flash Initialization to setup flash waitstates
	InitFlash();
	#endif
	
	// Enable user interrupts
	IER |= (M_INT1|M_INT9);

	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;	// TINT0
	PieCtrlRegs.PIEIER9.bit.INTx1 = 1;	// SCIRXINTA
	PieCtrlRegs.PIEIER9.bit.INTx2 = 1;	// SCITXINTA
	PieCtrlRegs.PIEIER9.bit.INTx3 = 1;	// SCIRXINTB
	PieCtrlRegs.PIEIER9.bit.INTx4 = 1;	// SCITXINTB
	
	CpuTimerIntEnable(&CpuTimer0Regs);	// Разрешаем прерывание по таймеру
	StartCpuTimer(&CpuTimer0Regs);		// Запускаем таймер
	StartCpuTimer(&CpuTimer1Regs);		// Запускаем таймер 1
}

__inline void InitClocks(void)
{
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.ADCENCLK   = 1;	// HSPCLK to ADC enabled
	SysCtrlRegs.PCLKCR0.bit.I2CAENCLK  = 0;	// SYSCLK to I2C-A disabled
	SysCtrlRegs.PCLKCR0.bit.SPICENCLK  = 1;	// LSPCLK to SPI-C disabled
	SysCtrlRegs.PCLKCR0.bit.SPIDENCLK  = 0;	// LSPCLK to SPI-D disabled
	SysCtrlRegs.PCLKCR0.bit.SPIAENCLK  = 1;	// LSPCLK to SPI-A enabled
	SysCtrlRegs.PCLKCR0.bit.SPIBENCLK  = 0;	// LSPCLK to SPI-B disabled
	SysCtrlRegs.PCLKCR0.bit.SCIAENCLK  = 1;	// LSPCLK to SCI-A enabled
	SysCtrlRegs.PCLKCR0.bit.SCIBENCLK  = 1;	// LSPCLK to SCI-B enabled
	SysCtrlRegs.PCLKCR0.bit.ECANAENCLK = 0;	// SYSCLK to eCAN-A disabled
	SysCtrlRegs.PCLKCR0.bit.ECANBENCLK = 0;	// SYSCLK to eCAN-B disabled

	SysCtrlRegs.PCLKCR1.bit.EPWM1ENCLK = 0;	// SYSCLK to EPWM1 disabled
	SysCtrlRegs.PCLKCR1.bit.EPWM2ENCLK = 0;	// SYSCLK to EPWM2 disabled
	SysCtrlRegs.PCLKCR1.bit.EPWM3ENCLK = 0;	// SYSCLK to EPWM3 disabled
	SysCtrlRegs.PCLKCR1.bit.EPWM4ENCLK = 1;	// SYSCLK to EPWM4 enabled
	SysCtrlRegs.PCLKCR1.bit.EPWM5ENCLK = 0;	// SYSCLK to EPWM5 disabled
	SysCtrlRegs.PCLKCR1.bit.EPWM6ENCLK = 1;	// SYSCLK to EPWM6 disabled
	SysCtrlRegs.PCLKCR1.bit.ECAP1ENCLK = 0;	// SYSCLK to ECAP1 disabled
	SysCtrlRegs.PCLKCR1.bit.ECAP2ENCLK = 0;	// SYSCLK to ECAP2 disabled
	SysCtrlRegs.PCLKCR1.bit.ECAP3ENCLK = 1;	// SYSCLK to ECAP3 disabled
	SysCtrlRegs.PCLKCR1.bit.ECAP4ENCLK = 0;	// SYSCLK to ECAP4 disabled
	SysCtrlRegs.PCLKCR1.bit.EQEP1ENCLK = 0;	// SYSCLK to EQEP1 disabled
	SysCtrlRegs.PCLKCR1.bit.EQEP2ENCLK = 0;	// SYSCLK to EQEP2 disabled
	EDIS;
}

__inline void InitGpio(void)
{

	EALLOW;

	// Group A pins
	GpioCtrlRegs.GPACTRL.all  = 0x0;  // QUALPRD = SYSCLKOUT for all group A GPIO

	// Group B pins
	GpioCtrlRegs.GPBCTRL.all  = 0x0;  // QUALPRD = SYSCLKOUT for all group B GPIO

	// Each GPIO pin can be: 
	// a) a GPIO input/output
	// b) peripheral function 1
	// c) peripheral function 2
	// d) peripheral function 3
	// By default, all are GPIO Inputs 
	GpioCtrlRegs.GPAMUX1.all = 0x0000;     // GPIO functionality GPIO0-GPIO15
	GpioCtrlRegs.GPAMUX2.all = 0x0000;     // GPIO functionality GPIO16-GPIO31
	GpioCtrlRegs.GPBMUX1.all = 0x0000;     // GPIO functionality GPIO32-GPIO34
	GpioCtrlRegs.GPADIR.all = 0x0000;      // GPIO0-GPIO31 are GP inputs
	GpioCtrlRegs.GPBDIR.all = 0x0000;      // GPIO32-GPIO34 are inputs   

	// Each input can have different qualification
	// a) input synchronized to SYSCLKOUT
	// b) input qualified by a sampling window
	// c) input sent asynchronously (valid for peripheral inputs only)
	GpioCtrlRegs.GPAQSEL1.all = 0x0000;    // GPIO0-GPIO15 Synch to SYSCLKOUT
	GpioCtrlRegs.GPAQSEL2.all = 0x0000;    // GPIO16-GPIO31 Synch to SYSCLKOUT
	GpioCtrlRegs.GPBQSEL1.all = 0x0000;    // GPIO32-GPIO34 Synch to SYSCLKOUT 

	// Pull-ups can be enabled or disabled. 
	GpioCtrlRegs.GPAPUD.all = 0x001F;      // Pullup's enabled GPIO0-GPIO31
	GpioCtrlRegs.GPBPUD.all = 0x0000;      // Pullup's enabled GPIO32-GPIO34
	// GpioCtrlRegs.GPAPUD.all = 0xFFFF;      // Pullup's disabled GPIO0-GPIO31
	// GpioCtrlRegs.GPBPUD.all = 0xFFFF;      // Pullup's disabled GPIO32-GPIO34

	// Direction selection and initial outputs condition
	// Special function selection
//	GpioCtrlRegs.GPAMUX1.bit.GPIO6  = 1;  // Tir ePwm

	// General IO pins configuration  DIR - направление 1 - выход 0 - вход. DAT - начальное состояние 
	GpioCtrlRegs.GPADIR.bit.GPIO0   = 1; // UR_PIN CU
	GpioDataRegs.GPADAT.bit.GPIO0   = 1;

	GpioCtrlRegs.GPADIR.bit.GPIO1   = 1; // US_PIN CV
	GpioDataRegs.GPADAT.bit.GPIO1   = 1;

	GpioCtrlRegs.GPADIR.bit.GPIO2   = 1; // UT_PIN CW
	GpioDataRegs.GPADAT.bit.GPIO2   = 1;

	GpioCtrlRegs.GPADIR.bit.GPIO3   = 1; // UR_REW_PIN CUW
	GpioDataRegs.GPADAT.bit.GPIO3   = 1;

	GpioCtrlRegs.GPADIR.bit.GPIO4   = 1; // UT_REW_PIN CWU
	GpioDataRegs.GPADAT.bit.GPIO4   = 1;

	GpioCtrlRegs.GPADIR.bit.GPIO5   = 1; // CS4
	GpioDataRegs.GPADAT.bit.GPIO5   = 0;

	GpioCtrlRegs.GPADIR.bit.GPIO6   = 1; // TRN_CLK
	GpioDataRegs.GPADAT.bit.GPIO6   = 1;

	GpioCtrlRegs.GPADIR.bit.GPIO7   = 1; //
	GpioDataRegs.GPADAT.bit.GPIO7   = 1;

	GpioCtrlRegs.GPADIR.bit.GPIO8   = 1; // ENC_MSBINV ??.
	GpioDataRegs.GPADAT.bit.GPIO8   = 1;

	GpioCtrlRegs.GPADIR.bit.GPIO9   = 1; // LCD_EN
	GpioDataRegs.GPADAT.bit.GPIO9   = 0;

	GpioCtrlRegs.GPADIR.bit.GPIO10 	= 1; // DSP_LED
	GpioDataRegs.GPADAT.bit.GPIO10 	= 1;

	GpioCtrlRegs.GPADIR.bit.GPIO11 	= 1; // ENC_STROB
	GpioDataRegs.GPADAT.bit.GPIO11 	= 1;

	GpioCtrlRegs.GPADIR.bit.GPIO12 	= 1; // CS2
	GpioDataRegs.GPADAT.bit.GPIO12 	= 0;

	GpioCtrlRegs.GPADIR.bit.GPIO13 	= 0; // VOLT_OFF
	GpioDataRegs.GPADAT.bit.GPIO13 	= 0;

//	GpioCtrlRegs.GPADIR.bit.GPIO14 	= 1; // RS485_TXD
//	GpioDataRegs.GPADAT.bit.GPIO14 	= 1;

//	GpioCtrlRegs.GPADIR.bit.GPIO15 	= 0; // RS485_RXD
//	GpioDataRegs.GPADAT.bit.GPIO15 	= 1;

//	GpioCtrlRegs.GPADIR.bit.GPIO16 	= 1; // SPI_MOSI
//	GpioDataRegs.GPADAT.bit.GPIO16 	= 1;

//	GpioCtrlRegs.GPADIR.bit.GPIO17 	= 0; // SPI_MISO
//	GpioDataRegs.GPADAT.bit.GPIO17 	= 1;

//	GpioCtrlRegs.GPADIR.bit.GPIO18 	= 1; // SPI_SCK
//	GpioDataRegs.GPADAT.bit.GPIO18 	= 1;

	GpioCtrlRegs.GPADIR.bit.GPIO19 	= 1; // LCD_RS
	GpioDataRegs.GPADAT.bit.GPIO19 	= 0;

//	GpioCtrlRegs.GPADIR.bit.GPIO21 	= 0; // SPI_EXT_MOSI
//	GpioDataRegs.GPADAT.bit.GPIO21 	= 1;

//	GpioCtrlRegs.GPADIR.bit.GPIO22 	= 1; // SPI_EXT_SCK
//	GpioDataRegs.GPADAT.bit.GPIO22 	= 1;

	GpioCtrlRegs.GPADIR.bit.GPIO23 	= 1; // CS_AT
	GpioDataRegs.GPADAT.bit.GPIO23 	= 1;

	GpioCtrlRegs.GPADIR.bit.GPIO24 	= 1; // RS485_DIR 
	GpioDataRegs.GPADAT.bit.GPIO24 	= 1;

	GpioCtrlRegs.GPADIR.bit.GPIO25 	= 1; // BT ON/OFF
	GpioDataRegs.GPADAT.bit.GPIO25 	= 0;

//	GpioCtrlRegs.GPADIR.bit.GPIO26 	= 0; 
//	GpioDataRegs.GPADAT.bit.GPIO26 	= 0;	// ECap3

	GpioCtrlRegs.GPADIR.bit.GPIO27 	= 1; // TEN_OFF
	GpioDataRegs.GPADAT.bit.GPIO27 	= 1;

//	GpioCtrlRegs.GPADIR.bit.GPIO28 	= 0; // IRDA/BT_RX
//	GpioDataRegs.GPADAT.bit.GPIO28 	= 0;

//	GpioCtrlRegs.GPADIR.bit.GPIO29 	= 1; // IRDA/BT_TX
//	GpioDataRegs.GPADAT.bit.GPIO29 	= 1;

	GpioCtrlRegs.GPADIR.bit.GPIO30 	= 1; // CS3
	GpioDataRegs.GPADAT.bit.GPIO30 	= 0;

	GpioCtrlRegs.GPADIR.bit.GPIO31 	= 1; // CS0
	GpioDataRegs.GPADAT.bit.GPIO31 	= 0;

	GpioCtrlRegs.GPBDIR.bit.GPIO32 	= 1; // TRN_ENB
	GpioDataRegs.GPBDAT.bit.GPIO32 	= 1;

	GpioCtrlRegs.GPBDIR.bit.GPIO33 	= 1; // IRDA/BT
	GpioDataRegs.GPBDAT.bit.GPIO33 	= 1;

	GpioCtrlRegs.GPBDIR.bit.GPIO34 	= 1; // REPROG
	GpioDataRegs.GPBDAT.bit.GPIO34 	= 1;

	EDIS;
}

__inline void InitCap(void)
{
   ECap3Regs.ECEINT.all = 0x0000;             // Disable all capture interrupts
   ECap3Regs.ECCLR.all = 0xFFFF;              // Clear all CAP interrupt flags
   ECap3Regs.ECCTL1.bit.CAPLDEN = 0;          // Disable CAP1-CAP4 register loads
   ECap3Regs.ECCTL2.bit.TSCTRSTOP = 0;        // Make sure the counter is stopped
   
   // Configure peripheral registers
   ECap3Regs.ECCTL2.bit.CONT_ONESHT = 1;      // One-shot
   ECap3Regs.ECCTL2.bit.STOP_WRAP = 0;        // Stop at 4 events
   ECap3Regs.ECCTL1.bit.CAP1POL = 1;          // Falling edge
   ECap3Regs.ECCTL1.bit.CAP2POL = 1;          // Falling edge
   ECap3Regs.ECCTL1.bit.CAP3POL = 0;          // Rising edge
   ECap3Regs.ECCTL1.bit.CAP4POL = 1;          // Falling edge
   ECap3Regs.ECCTL1.bit.CTRRST1 = 0;          // Difference operation         
   ECap3Regs.ECCTL1.bit.CTRRST2 = 0;          // Difference operation         
   ECap3Regs.ECCTL1.bit.CTRRST3 = 0;          // Difference operation         
   ECap3Regs.ECCTL1.bit.CTRRST4 = 0;          // Difference operation         
   ECap3Regs.ECCTL1.bit.CAPLDEN = 1;          // Enable capture units
   ECap3Regs.ECCTL1.bit.PRESCALE = 0;		  // Divide by 60;


   ECap3Regs.ECCTL2.bit.TSCTRSTOP = 1;        // Start Counter
   ECap3Regs.ECCTL2.bit.REARM = 1;            // arm one-shot
   ECap3Regs.ECCTL1.bit.CAPLDEN = 1;          // Enable CAP1-CAP4 register loads
   // ECap3Regs.ECEINT.bit.CEVT4 = 1;         // 4 events = interrupt

	// Special function select
	EALLOW;
	GpioCtrlRegs.GPAMUX2.bit.GPIO26 = 1;				// Переключаем вывод на ECap модуль
	EDIS;
}




__inline void InitEPwm(void)
{
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;		// Отсекаем тактовый сигнал со всех ШИМ модулей
	EDIS;
	
	// Disable all timers
	EPwm4Regs.TBCTL.bit.CTRMODE = TB_FREEZE;	// Останавливаем работу ШИМ модуля
	
	EPwm4Regs.TBPRD = CLK_VS_TRN_PERIOD;		// Период счета
	EPwm4Regs.TBPHS.half.TBPHS = 0;				// Фазовый сдвиг
	EPwm4Regs.TBCTR = 0x0000;					// Обнуляем счетный регистр
	EPwm4Regs.TBCTL.bit.FREE_SOFT = 3;			// Эмуляцию не используем, поэтому режим "Free run"
	EPwm4Regs.TBCTL.bit.PHSDIR = TB_UP;			// Направление счета - вверх
	EPwm4Regs.TBCTL.bit.CLKDIV = TB_DIV1;		// Предделитель частоты TBCLK = HSPCLK/1
	EPwm4Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;	// Высокоскоростной предделитель HSPCLK = SYSCLKOUT/1
	EPwm4Regs.TBCTL.bit.SWFSYNC  = 0;			// Программная синхронизация не используется
	EPwm4Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO; // Источник генерации сигнала синхронизации CTR = Zero
	EPwm4Regs.TBCTL.bit.PRDLD = TB_SHADOW;		// Новое значение периода записывается только после наступления события (счетчик обнулился и т.п.)
	EPwm4Regs.TBCTL.bit.PHSEN = TB_DISABLE;		// Т.к. фазовый сдвиг не используется 

	EPwm4Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD;	// Загружать новое значение CMPA при обнулении счетчика
	EPwm4Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO_PRD;	// Загружать новое значение CMPB при обнулении счетчика
	EPwm4Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;			// CMPA Shadow Mode
	EPwm4Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;			// CMPB Shadow Mode
	EPwm4Regs.CMPCTL.bit.SHDWAFULL = 0;					// don't care
	EPwm4Regs.CMPCTL.bit.SHDWBFULL = 0;					// don't care

	EPwm4Regs.CMPA.half.CMPA = 0.5 * CLK_VS_TRN_PERIOD;	// Значение регистра сравнения A
	
	EPwm4Regs.AQCTLA.bit.ZRO = AQ_SET;					// 1 когда CTR = 0
	EPwm4Regs.AQCTLA.bit.CAU = AQ_CLEAR;				// 0 когда CTR = CMPA
	EPwm4Regs.AQSFRC.all = 0x0000;						// Action-qualifier не используется
	EPwm4Regs.AQCSFRC.all = 0x0000;

	EPwm4Regs.DBCTL.bit.IN_MODE = DBA_ALL;				// Dead Band Generator не используется
	EPwm4Regs.DBCTL.bit.POLSEL = DB_ACTV_LOC;
	EPwm4Regs.DBCTL.bit.OUT_MODE = DB_DISABLE;
	EPwm4Regs.DBFED = 0;
	EPwm4Regs.DBRED = 0;

	EPwm4Regs.PCCTL.bit.CHPEN = CHP_DISABLE;			// PWM chopper не используется
	EPwm4Regs.TZSEL.all = 0;
	EPwm4Regs.TBCTL.all = 0;
	EPwm4Regs.TZEINT.all = 0;
	EPwm4Regs.TZFLG.all = 0;

	EPwm4Regs.TZCTL.bit.TZA = TZ_NO_CHANGE;				// Trip action disabled for output A
	EPwm4Regs.TZCTL.bit.TZB = TZ_NO_CHANGE;
														// Event Trigger модуль не используется
	EPwm4Regs.ETSEL.bit.SOCBEN = 0;						// disable
	EPwm4Regs.ETSEL.bit.SOCBSEL = 0;					// reserved
	EPwm4Regs.ETSEL.bit.SOCAEN = 0;						// disable
	EPwm4Regs.ETSEL.bit.SOCASEL = 0; 					// up cmp start conversation
	EPwm4Regs.ETSEL.bit.INTEN = 0;
	EPwm4Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;
	EPwm4Regs.ETPS.bit.SOCAPRD = ET_DISABLE;
	EPwm4Regs.ETPS.bit.INTPRD = ET_1ST;					// Generate INT on 1st event
	
	// Start timers
	EPwm4Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;			// Счет вверх

	// Special function select
	EALLOW;
	GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 1; 				// Переключаем вывод на ШИМ модуль
	EDIS;
}

__inline void InitIrdaPwm(void)
{
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;		// Отсекаем тактовый сигнал со всех ШИМ модулей
	EDIS;
	
	// Disable all timers
	EPwm6Regs.TBCTL.bit.CTRMODE = TB_FREEZE;	// Останавливаем работу ШИМ модуля
	
	EPwm6Regs.TBPRD = IRDA_FREQ;		// Период счета
	EPwm6Regs.TBPHS.half.TBPHS = 0;				// Фазовый сдвиг
	EPwm6Regs.TBCTR = 0x0000;					// Обнуляем счетный регистр
	EPwm6Regs.TBCTL.bit.FREE_SOFT = 3;			// Эмуляцию не используем, поэтому режим "Free run"
	EPwm6Regs.TBCTL.bit.PHSDIR = TB_UP;			// Направление счета - вверх
	EPwm6Regs.TBCTL.bit.CLKDIV = TB_DIV1;		// Предделитель частоты TBCLK = HSPCLK/1
	EPwm6Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;	// Высокоскоростной предделитель HSPCLK = SYSCLKOUT/1
	EPwm6Regs.TBCTL.bit.SWFSYNC  = 0;			// Программная синхронизация не используется
	EPwm6Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO; // Источник генерации сигнала синхронизации CTR = Zero
	EPwm6Regs.TBCTL.bit.PRDLD = TB_SHADOW;		// Новое значение периода записывается только после наступления события (счетчик обнулился и т.п.)
	EPwm6Regs.TBCTL.bit.PHSEN = TB_DISABLE;		// Т.к. фазовый сдвиг не используется 

	EPwm6Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD;	// Загружать новое значение CMPA при обнулении счетчика
	EPwm6Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO_PRD;	// Загружать новое значение CMPB при обнулении счетчика
	EPwm6Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;			// CMPA Shadow Mode
	EPwm6Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;			// CMPB Shadow Mode
	EPwm6Regs.CMPCTL.bit.SHDWAFULL = 0;					// don't care
	EPwm6Regs.CMPCTL.bit.SHDWBFULL = 0;					// don't care

	EPwm6Regs.CMPA.half.CMPA = 0.5 * IRDA_FREQ;	// Значение регистра сравнения A
	
	EPwm6Regs.AQCTLA.bit.ZRO = AQ_SET;					// 1 когда CTR = 0
	EPwm6Regs.AQCTLA.bit.CAU = AQ_CLEAR;				// 0 когда CTR = CMPA
	EPwm6Regs.AQSFRC.all = 0x0000;						// Action-qualifier не используется
	EPwm6Regs.AQCSFRC.all = 0x0000;

	EPwm6Regs.DBCTL.bit.IN_MODE = DBA_ALL;				// Dead Band Generator не используется
	EPwm6Regs.DBCTL.bit.POLSEL = DB_ACTV_LOC;
	EPwm6Regs.DBCTL.bit.OUT_MODE = DB_DISABLE;
	EPwm6Regs.DBFED = 0;
	EPwm6Regs.DBRED = 0;

	EPwm6Regs.PCCTL.bit.CHPEN = CHP_DISABLE;			// PWM chopper не используется
	EPwm6Regs.TZSEL.all = 0;
	EPwm6Regs.TBCTL.all = 0;
	EPwm6Regs.TZEINT.all = 0;
	EPwm6Regs.TZFLG.all = 0;

	EPwm6Regs.TZCTL.bit.TZA = TZ_NO_CHANGE;				// Trip action disabled for output A
	EPwm6Regs.TZCTL.bit.TZB = TZ_NO_CHANGE;
														// Event Trigger модуль не используется
	EPwm6Regs.ETSEL.bit.SOCBEN = 0;						// disable
	EPwm6Regs.ETSEL.bit.SOCBSEL = 0;					// reserved
	EPwm6Regs.ETSEL.bit.SOCAEN = 0;						// disable
	EPwm6Regs.ETSEL.bit.SOCASEL = 0; 					// up cmp start conversation
	EPwm6Regs.ETSEL.bit.INTEN = 0;
	EPwm6Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;
	EPwm6Regs.ETPS.bit.SOCAPRD = ET_DISABLE;
	EPwm6Regs.ETPS.bit.INTPRD = ET_1ST;					// Generate INT on 1st event
	
	// Start timers
	EPwm6Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;			// Счет вверх

	// Special function select
/*	EALLOW;
	GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 1; 				// Переключаем вывод на ШИМ модуль
	EDIS;*/
}

__inline void InitAdc(void)
{

 // Reset the ADC module
 AdcRegs.ADCTRL1.bit.RESET = 1;
 
 // Must wait 2 ADCCLK periods for the reset to take effect.
 asm(" RPT #14 || NOP");
 
 // Select the ADC reference, выбор источника питания
 AdcRegs.ADCREFSEL.bit.REF_SEL = 0;  // 0:internal, 1:external
 
 // Power-up reference and main ADC
 AdcRegs.ADCTRL3.all = 0x00E8;   // Power-up reference and main ADC, подключение внутреннего питания и включение внутреннего источника
 AdcRegs.ADCTRL3.bit.ADCCLKPS = 4;  // clock prescaler, FCLK=HSPCLK/(2*ADCCLKPS)
 
 // Wait 5ms before using the ADC
 DelayUs(5000);
 
 // Configure the other ADC register, сколько всего каналов будем опрашивать (все 16)
 AdcRegs.ADCMAXCONV.all = 0x000F;
 
 AdcRegs.ADCCHSELSEQ1.all = 0x3210;  //описываем какие каналы будут работать, порядок (все 16)
 AdcRegs.ADCCHSELSEQ2.all = 0x7654;
 AdcRegs.ADCCHSELSEQ3.all = 0xBA98;
 AdcRegs.ADCCHSELSEQ4.all = 0xFEDC;
 
 AdcRegs.ADCTRL1.bit.SUSMOD   = 0;   // ignore emulation suspend
 AdcRegs.ADCTRL1.bit.ACQ_PS   = 8;   // 16*ADCCLK
 AdcRegs.ADCTRL1.bit.CPS      = 1;  // ADCCLK = FCLK/2
 AdcRegs.ADCTRL1.bit.CONT_RUN = 1;  // start/stop mode
 AdcRegs.ADCTRL1.bit.SEQ_OVRD = 0;  // disabled
 AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;  // cascaded sequencer

   AdcRegs.ADCTRL2.bit.SOC_SEQ1 = 1;  //необходим при каскадном режиме

}



