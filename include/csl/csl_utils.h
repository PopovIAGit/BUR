/*======================================================================
Имя файла:          csl_utils.h
Автор:              Саидов В.С.
Версия файла:       01.02
Дата изменения:		16/02/10
Описание:
Заголовочный файл сервисных функций
======================================================================*/

#ifndef CSL_UTILS_
#define CSL_UTILS_

#include "std.h"

#ifdef __cplusplus
extern "C" {
#endif

#define InRange(Val, Min, Max)	(((Val) >= (Min)) && ((Val) <= (Max)))

#define MIN_VAL(Val1, Val2)		((Val1) < (Val2) ? (Val1) : (Val2))
#define MAX_VAL(Val1, Val2)		((Val1) > (Val2) ? (Val1) : (Val2))

#if defined(__IAR_SYSTEMS_ICC__)

#ifndef F_SYSCLK
#define F_SYSCLK		8e6
#endif

#define DelayUs(A)  __delay_cycles((Extended)(F_SYSCLK / 1e6) * (Extended)(A))

#else
extern void DelayUs(Uns Delay);
#endif

extern Bool CheckRange(Uns Data, Uns Min, Uns Max);

extern Uns Min3UnsValue(Uns Val1, Uns Val2, Uns Val3);
extern Int Min3IntValue(Int Val1, Int Val2, Int Val3);
extern Uns Mid3UnsValue(Uns Val1, Uns Val2, Uns Val3);
extern Int Mid3IntValue(Int Val1, Int Val2, Int Val3);
extern Uns Max3UnsValue(Uns Val1, Uns Val2, Uns Val3);
extern Int Max3IntValue(Int Val1, Int Val2, Int Val3);

__inline Int Sign(Int Value) {return ((Value < 0) ? -1 : 1);}

extern Uns LIntersection(Uns P1_1, Uns P1_2, Uns P2_1, Uns P2_2);

#if defined(__TMS320C28X__)
#define ValueToPU0(Val, Base)	(Uns)_IQ22rmpy(_IQ22div(Val, Base), 100)
#define ValueToPU1(Val, Base)	(Uns)_IQ25rmpy(_IQ25div(Val, Base), 1000)
#define ValueToPU2(Val, Base)	(Uns)_IQ29rmpy(_IQ29div(Val, Base), 10000)

#define PU0ToValue(Val, Base)	(Uns)_IQ22rmpy(_IQ22div(Val, 100),   Base)
#define PU1ToValue(Val, Base)	(Uns)_IQ25rmpy(_IQ25div(Val, 1000),  Base)
#define PU2ToValue(Val, Base)	(Uns)_IQ29rmpy(_IQ29div(Val, 10000), Base)

#define GainPU(Base, Max)			_IQ22div(Max, Base)
#define ValueToPU(Value, Gain)	_IQ22rmpy(Value, Gain)
#endif

#ifdef __cplusplus
}
#endif // extern "C"

#endif /* CSL_UTILS_ */




