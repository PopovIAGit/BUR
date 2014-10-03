/*======================================================================
Имя файла:          csl_queue.h
Автор:              Саидов В.С.
Версия файла:       01.02
Дата изменения:		16/02/10
Описание:
Заголовочный файл для работы с очередью
======================================================================*/

#ifndef CSL_QUEUE_
#define CSL_QUEUE_

#include "std.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _TQueElem
{
	struct _TQueElem *Next;
	struct _TQueElem *Prev;
} TQueElem;

#define QueNew(Elem)			(Elem)->Next = (Elem)->Prev = (Elem)
#define QueClear(Elem)			(Elem)->Next = (Elem)->Prev = NULL
#define QueCreate(Elem)		{ Elem, Elem  }
#define QueNext(Elem)			((Ptr)((TQueElem *)(Elem))->Next)
#define QuePrev(Elem)			((Ptr)((TQueElem *)(Elem))->Prev)
#define QueEmpty(Queue)		((Queue)->Next == (Queue))

#ifdef _INLINE
__inline void QueRemove(Ptr Elem)
{
	((TQueElem *)Elem)->Prev->Next = ((TQueElem *)Elem)->Next; \
	((TQueElem *)Elem)->Next->Prev = ((TQueElem *)Elem)->Prev; \
}
#else
extern void QueRemove(Ptr Elem);
#endif

#ifdef _INLINE
__inline void QueEnqueue(TQueElem *Queue, Ptr Elem)
{
    TQueElem *Prev = Queue->Prev;

    ((TQueElem *)Elem)->Next = Queue;
    ((TQueElem *)Elem)->Prev = Prev;
    
    Prev->Next  = (TQueElem *)Elem;
    Queue->Prev = (TQueElem *)Elem;
}
#else
extern void QueEnqueue(TQueElem *Queue, Ptr Elem);
#endif

#ifdef __cplusplus
}
#endif // extern "C"

#endif /* CSL_QUEUE_ */

