/*==========================================================================*/
/*																			*/
/* FILE:    F2806.cmd														*/
/*																			*/
/* TITLE:   Linker Command File For F2806 Device							*/
/*																			*/
/*==========================================================================*/

MEMORY
{
PAGE 0: /* Program Memory */
	M1SARAM				: origin = 0x000480, length = 0x000380	/* on-chip RAM block M1 */
	OTP					: origin = 0x3D7800, length = 0x000400	/* on-chip OTP */
	FLASH				: origin = 0x3F0000, length = 0x007F80	/* on-chip FLASH */
	CSM_PWL				: origin = 0x3F7FF8, length = 0x000008 	/* Part of FLASHA.  CSM password locations in FLASHA */
	CSM_RSVD			: origin = 0x3F7F80, length = 0x000076	/* Part of FLASHA. Program with all 0x0000 when CSM is in use. */
	BEGIN				: origin = 0x3F7FF6, length = 0x000002	/* Part of FLASHA. Used for "boot to Flash" bootloader mode. */
	BOOTROM				: origin = 0x3FF000, length = 0x000FC0	/* Boot ROM */
	RESET				: origin = 0x3FFFC0, length = 0x000002	/* part of boot ROM  */
	VECTORS				: origin = 0x3FFFC2, length = 0x00003E	/* part of boot ROM  */

PAGE 1: /* Data Memory */
	M0SARAM				: origin = 0x000000, length = 0x000400	/* on-chip RAM block M0 */
	BOOT_RSVD			: origin = 0x000400, length = 0x000080	/* Part of M1, BOOT rom will use this for stack */
	LSARAM				: origin = 0x008000, length = 0x001F00	/* on-chip RAM block L0 and L1 */
	SDATA				: origin = 0x009F00, length = 0x000100	/* secure ram data */
}

SECTIONS
{
	/* Allocate program areas: */
	.cinit				: > FLASH			PAGE = 0
	.pinit				: > FLASH			PAGE = 0
	.text				: > FLASH			PAGE = 0
	codestart			: > BEGIN			PAGE = 0
	secureRamFuncs		:	LOAD = FLASH,	PAGE = 0
							RUN = M1SARAM, 	PAGE = 0
							LOAD_START(_secureRamFuncs_loadstart),
							LOAD_END(_secureRamFuncs_loadend),
							RUN_START(_secureRamFuncs_runstart)

	csmpasswds			: > CSM_PWL			PAGE = 0
	csm_rsvd			: > CSM_RSVD		PAGE = 0

	/* Allocate uninitalized data sections: */
	.stack				: > M0SARAM			PAGE = 1
	.ebss				: > LSARAM			PAGE = 1
	.esysmem			: > M0SARAM			PAGE = 1
	secureRamData		: > SDATA,			PAGE = 1

	/* Initalized sections go in Flash */
	.econst				: > FLASH			PAGE = 0
	.switch				: > FLASH			PAGE = 0
	
	/* Allocate IQ math areas: */
	IQmath				: > FLASH			PAGE = 0
	IQmathTables		: > BOOTROM			PAGE = 0,	TYPE = NOLOAD
	
	/* .reset is a standard section used by the compiler. It contains the */
	/* the address of the start of _c_int00 for C Code. */
	/* When using the boot ROM this section and the CPU vector */
	/* table is not needed. Thus the default type is set here to DSECT */
	.reset				: > RESET			PAGE = 0,	TYPE = DSECT
	vectors				: > VECTORS			PAGE = 0,	TYPE = DSECT
}


