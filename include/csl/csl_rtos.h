/*======================================================================
Имя файла:          csl_rtos.h
Автор:              Саидов В.С.
Версия файла:       01.02
Дата изменения:		16/02/10
Описание:
Заголовочный файл операционной системы
======================================================================*/

#ifndef CSL_RTOS_
#define CSL_RTOS_

#include "csl_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PrdElemInit(Func, Data, NumSteps) \
	{ {NULL, NULL}, NULL, (TPrdFunc)(Func), (Ptr)(Data), (NumSteps), 0, 0, 0 }

typedef void (*TPrdFunc)(Ptr Data);

typedef const Char TPrdElemName[31];

typedef struct _TPrdElem
{
	TQueElem Queue;
	String   Name;
	TPrdFunc Func;
	Ptr      Data;
	Byte     NumSteps;
	Uns		Index;
	Uns		TimeIsr;
	Uns		TimeIsrMax;
} TPrdElem;

#define PRLCNT(List)	(sizeof(List) / sizeof(TPrdElem))

typedef struct _TPrdObj
{
	TPrdElem  Queue;
	TPrdElem *Elem;
	Uns       Length;
	Uns       Period;
	Uns       Tick;
	Byte		 ExecStep;
} TPrdObj;

typedef struct _TRtosObj
{
	TPrdElem *List;
	TPrdElem *Elem;
	Uns      *Timer;
	Uns		 ListCount;
	Uns       PrdCount;
	Uns		 TimeIsr;
	Uns		 TimeIsrMax;
	Ptr       ElemNames;
} TRtosObj;

extern TRtosObj Rtos;

void PrdCreate(TPrdObj *, Uns Period);
void PrdInsert(TPrdObj *, Ptr List, Uns ListCount);
void PrdExecute(TPrdObj *);
void PrdElemExecute(TPrdElem *);
void RtosExecute(void);

#ifdef __cplusplus
}
#endif // extern "C"

#endif /* CSL_RTOS_ */

