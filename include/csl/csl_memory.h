/*======================================================================
Имя файла:          csl_memory.h
Автор:              Саидов В.С.
Версия файла:       01.01
Дата изменения:		16/02/10
Описание:
Заголовочный файл для работы с памятью
======================================================================*/

#ifndef CSL_MEMORY_
#define CSL_MEMORY_

#include "std.h"

#ifdef __cplusplus
extern "C" {
#endif

#define F_READ			0x01
#define F_WRITE			0x02
#define F_GETSTATUS		0x04
#define F_ERASE			0x08
#define F_GET_DEVID		0x10
#define F_POWER_CTRL	0x20
#define F_PROTECT		0x40

#define MEM_PAR			0
#define MEM_LOG			1
#define MEM_INTER		2
#define MEM_MENU		3

#define ReadPar(Adr, Buf, Cnt)  	ParFunc(MEM_PAR,     F_READ,  Adr, (Ptr)(Buf), Cnt)
#define WritePar(Adr, Buf, Cnt) 	ParFunc(MEM_PAR,     F_WRITE, Adr, (Ptr)(Buf), Cnt)

#define ReadLog(Adr, Buf, Cnt)  	LogFunc(MEM_LOG,     F_READ,  Adr, (Ptr)(Buf), Cnt)
#define WriteLog(Adr, Buf, Cnt) 	LogFunc(MEM_LOG,     F_WRITE, Adr, (Ptr)(Buf), Cnt)

#define ReadInter(Adr, Buf, Cnt) 	InterFunc(MEM_INTER, F_READ,  Adr, (Ptr)(Buf), Cnt)
#define WriteInter(Adr, Buf, Cnt)	InterFunc(MEM_INTER, F_WRITE, Adr, (Ptr)(Buf), Cnt)
#define ReadMenu(Adr, Buf, Cnt) 	MenuFunc(MEM_MENU,   F_READ,  Adr, (Ptr)(Buf), Cnt)
#define WriteMenu(Adr, Buf, Cnt)	MenuFunc(MEM_MENU,   F_WRITE, Adr, (Ptr)(Buf), Cnt)

#define IsMemParReady()				(!MemPar.Func)
#define IsMemLogReady()				(!MemLog.Func)
#define IsMemInterReady()				(!MemInter.Func)

extern void PFUNC_blkRead(Int *, Int *, Uns);
extern void PFUNC_blkWrite(Int *, Int *, Uns);
extern void PFUNC_strRead(Char *, Char *);
extern void PFUNC_strWrite(Char *, Char *);
extern  Int PFUNC_wordRead(Int *);
extern void PFUNC_wordWrite(Int *, Int);

#ifdef __cplusplus
}
#endif // extern "C"

#endif /* CSL_MEMORY_ */

