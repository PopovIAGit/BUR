// TI File $Revision: /main/5 $
// Checkin $Date: August 2, 2006   16:54:21 $
//###########################################################################
//
// FILE:   DSP280x_GlobalPrototypes.h
//
// TITLE:  Global prototypes for DSP280x
// 
//###########################################################################
// $TI Release: DSP280x, DSP2801x Header Files V1.41 $
// $Release Date: August 7th, 2006 $
//###########################################################################

#ifndef DSP280X_GLOBALPROTOTYPES_H
#define DSP280X_GLOBALPROTOTYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#define DSP28_PLLCR(FCK, FIN)	(((FCK) * 2L) / (FIN))

/*---- shared global function prototypes -----------------------------------*/
extern void InitPieCtrl(void);
extern void InitPieVectTable(void);
extern void InitSysCtrl(Uint16 val, Uint16 CLKINDIV);
extern void InitPll(Uint16 PLLCR, Uint16 CLKINDIV);
extern void InitPeripheralClocks(void);
extern void EnableInterrupts(void);
extern void ServiceDog(void);
extern void DisableDog(void);
extern void SetDBGIER(Uint16 dbgier);
extern void InitSciaGpio(void);
extern void InitScibGpio(void);
extern void InitSpiaGpio(void);
extern void InitSpibGpio(void);
extern void InitSpicGpio(void);
extern void InitSpidGpio(void);
extern void InitI2CGpio(void);
extern Uint16 DSP28x_DisableInt(void);
extern void DSP28x_RestoreInt(Uint16 State);

#define KickDog ServiceDog     // For compatiblity with previous versions

#define OUT_TOGGLE(Reg)			Reg = !Reg
#define OUT_SET(Reg, Value)	  {Reg = Value; asm(" RPT #9 || NOP");}

#define SCI_BRR(Baud)			(Uint16)((Uint32)(F_LSPCLK / 800)  / (Baud) - 1)
#define SPI_BRR(Baud)			(Uint16)((Uint32)(F_LSPCLK / 1000) / (Baud) - 1)

extern volatile struct SCI_REGS *SciRegs[];

extern volatile struct SPI_REGS *SpiRegs[];
extern void *SPI_Idents[];

#define SPI_IsLocked(Id, Ident)	((SPI_Idents[Id] != 0) && (SPI_Idents[Id] != Ident))
#define SPI_Lock(Id, Ident)		SPI_Idents[Id] = Ident
#define SPI_UnLock(Id)				SPI_Idents[Id] = 0

//                 CAUTION
// This function MUST be executed out of RAM. Executing it
// out of OTP/Flash will yield unpredictable results
extern void InitFlash(void);

extern void MemCopy(Uint16 *SourceAddr, Uint16 *SourceEndAddr, Uint16 *DestAddr);

//---------------------------------------------------------------------------
// External symbols created by the linker cmd file
// DSP28 examples will use these to relocate code from one LOAD location 
// in either Flash or XINTF to a different RUN location in internal
// RAM
extern Uint16 secureRamFuncs_loadstart;
extern Uint16 secureRamFuncs_loadend;
extern Uint16 secureRamFuncs_runstart;

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif   // - end of DSP280X_GLOBALPROTOTYPES_H

//===========================================================================
// End of file.
//===========================================================================
