// TI File $Revision: /main/1 $
// Checkin $Date: December 1, 2004   12:31:30 $
//###########################################################################
//
// FILE:    DSP280x_CpuTimers.h
//
// TITLE:   DSP280x CPU 32-bit Timers Register Definitions.
//
// NOTES:   CpuTimer1 and CpuTimer2 are reserved for use with DSP BIOS and
//          other realtime operating systems.  
//
//          Do not use these two timers in your application if you ever plan
//          on integrating DSP-BIOS or another realtime OS. 
//
//          For this reason, the code to manipulate these two timers is
//          commented out and not used in these examples.
//
//###########################################################################
// $TI Release: DSP280x, DSP2801x Header Files V1.41 $
// $Release Date: August 7th, 2006 $
//###########################################################################

#ifndef DSP280x_CPU_TIMERS_H
#define DSP280x_CPU_TIMERS_H

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// CPU Timer Register Bit Definitions:
//
//
// TCR: Control register bit definitions:
struct  TCR_BITS {          // bits  description
   Uint16    rsvd1:4;       // 3:0   reserved
   Uint16    TSS:1;         // 4     Timer Start/Stop
   Uint16    TRB:1;         // 5     Timer reload
   Uint16    rsvd2:4;       // 9:6   reserved
   Uint16    SOFT:1;        // 10    Emulation modes
   Uint16    FREE:1;        // 11
   Uint16    rsvd3:2;       // 12:13 reserved
   Uint16    TIE:1;         // 14    Output enable
   Uint16    TIF:1;         // 15    Interrupt flag
}; 

union TCR_REG {
   Uint16           all;
   struct TCR_BITS  bit;
};

// TPR: Pre-scale low bit definitions:
struct  TPR_BITS {        // bits  description
   Uint16     TDDR:8;     // 7:0   Divide-down low
   Uint16     PSC:8;      // 15:8  Prescale counter low
};

union TPR_REG {
   Uint16           all;
   struct TPR_BITS  bit;
};

// TPRH: Pre-scale high bit definitions:
struct  TPRH_BITS {       // bits  description
   Uint16     TDDRH:8;      // 7:0   Divide-down high
   Uint16     PSCH:8;       // 15:8  Prescale counter high
};

union TPRH_REG {
   Uint16           all;
   struct TPRH_BITS bit;
};

// TIM, TIMH: Timer register definitions:
struct TIM_REG {
   Uint16  LSW;
   Uint16  MSW;
};

union TIM_GROUP {
   Uint32          all;
   struct TIM_REG  half;
};

// PRD, PRDH: Period register definitions:
struct PRD_REG {
   Uint16  LSW;
   Uint16  MSW;
};

union PRD_GROUP {
   Uint32          all;
   struct PRD_REG  half;
};

//---------------------------------------------------------------------------
// CPU Timer Register File:
//
struct CPUTIMER_REGS {
   union TIM_GROUP TIM;   // Timer counter register
   union PRD_GROUP PRD;   // Period register
   union TCR_REG   TCR;   // Timer control register
   Uint16          rsvd1; // reserved
   union TPR_REG   TPR;   // Timer pre-scale low
   union TPRH_REG  TPRH;  // Timer pre-scale high
};

//---------------------------------------------------------------------------
// CPU Timer Support Variables:
//
struct CPUTIMER_VARS {
   volatile struct  CPUTIMER_REGS  *RegsAddr;
   Uint32    InterruptCount;
   float   CPUFreqInMHz;
   float   PeriodInUSec;
};

//---------------------------------------------------------------------------
// Function prototypes and external definitions:
//
void ConfigCpuTimer(volatile struct CPUTIMER_REGS *CpuTimer, Uint32 Period);

extern volatile struct CPUTIMER_REGS CpuTimer0Regs;
extern volatile struct CPUTIMER_REGS CpuTimer1Regs;
extern volatile struct CPUTIMER_REGS CpuTimer2Regs;

//---------------------------------------------------------------------------
// Usefull Timer Operations:
//
#define StartCpuTimer(CpuTimer)			(CpuTimer)->TCR.bit.TSS = 0
#define StopCpuTimer(CpuTimer)			(CpuTimer)->TCR.bit.TSS = 1
#define ReloadCpuTimer(CpuTimer)		(CpuTimer)->TCR.bit.TRB = 1
#define ReadCpuTimerCounter(CpuTimer)	(CpuTimer)->TIM.all
#define ReadCpuTimerPeriod(CpuTimer)	(CpuTimer)->PRD.all
#define ResetCpuTimerValue(CpuTimer)	(CpuTimer)->TIM.all = 0
#define GetCpuTimerValue(CpuTimer)		((CpuTimer)->PRD.all - (CpuTimer)->TIM.all)
#define CpuTimerIntEnable(CpuTimer)	(CpuTimer)->TCR.bit.TIE = 1
#define CpuTimerIntDisable(CpuTimer)	(CpuTimer)->TCR.bit.TIE = 0

#ifdef __cplusplus
}
#endif // extern "C"

#endif  // end of DSP280x_CPU_TIMERS_H definition


//===========================================================================
// End of file.
//===========================================================================
