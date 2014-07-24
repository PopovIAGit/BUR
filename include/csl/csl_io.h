/*======================================================================
Имя файла:          csl_io.h
Автор:              Саидов В.С.
Версия файла:       01.02
Дата изменения:		16/02/10
Описание:
Заголовочный файл для работы с вводом/выводом
======================================================================*/

#ifndef CSL_IO_
#define CSL_IO_

#include "std.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__TMS320C28X__) || defined(_TMS320C2XX)
#define IO_PORT		volatile unsigned int
#define IO_DATA		unsigned int
#define IO_BITS		16

#elif defined(__IAR_SYSTEMS_ICC__)
#define IO_PORT		unsigned char volatile __io
#define IO_DATA		unsigned char
#define IO_BITS		8

#else
#define IO_PORT		unsigned int
#define IO_DATA		unsigned int
#define IO_BITS		32
#endif

#define IO_BIT_NUM_INIT(Bit)		((Bit) % IO_BITS)
#define IO_BIT_INIT(Bit)			((IO_DATA)1 << IO_BIT_NUM_INIT(Bit))
#define IO_PORT_INIT(Port, Bit)	((IO_PORT *)&(Port) + (Bit) / IO_BITS)
#define IO_INIT(Port, Bit)		IO_PORT_INIT(Port, Bit), IO_BIT_INIT(Bit)

typedef struct _IOpin {
	IO_PORT *PortAddr;
	IO_DATA  Bit;
} IOpin;

#define IOset(hPin)	*(hPin)->PortAddr |=  (hPin)->Bit
#define IOclr(hPin)	*(hPin)->PortAddr &= ~(hPin)->Bit

typedef void (*IOfxn)(IOpin *Pin);

typedef struct _IOport {
	IO_PORT *Addr;
	Byte     Bit;
	IO_DATA  Mask;
} IOport;

#define IOput(hPort, Data) \
	*(hPort)->Addr = (*(hPort)->Addr & (~(hPort)->Mask)) | ((IO_DATA)(Data) << (hPort)->Bit)

#define IOget(hPort) \
	((*(hPort)->Addr >> (hPort)->Bit) & (hPort)->Mask)

#define IOmask(hPort, Mask) \
	*(hPort)->Addr = *(hPort)->Addr & ((IO_DATA)(Mask) << (hPort)->Bit)

#ifdef __cplusplus
}
#endif // extern "C"

#endif /* CSL_IO_ */

