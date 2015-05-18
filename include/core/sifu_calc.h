/*======================================================================
Имя файла:         	sifu_calc.h
Автор:             	Саидов В.С.
Версия файла:      	01.02
Дата изменения:		12/03/10
Описание:
Библиотека системы импульсно-фазового управления для ТРН
======================================================================*/

#ifndef SIFU_CALC_
#define SIFU_CALC_

#ifdef __cplusplus
extern "C" {
#endif

#include "std.h"

#define SIFU_NONE		0
#define SIFU_DOWN		1
#define SIFU_UP			2
#define SIFU_DYN_BREAK	3

#define SIFU_UR			0
#define SIFU_US			1
#define SIFU_UT			2
#define SIFU_UR_REV		3
#define SIFU_UT_REV		4


#define SIFU_EN_TRN		5


#define SIFU_OPEN_ANG	0
#define SIFU_CLOSE_ANG	180
#define SIFU_MAX_ANG	180

//регистр управляющих ножек тиристоров
typedef union {
	Uns all;
	struct {
   		Uns sifu_R:1;		// 0 фаза R
   		Uns sifu_S:1;		// 1 фаза S
   		Uns sifu_T:1;		// 2 фаза T
   		Uns sifu_RT:1;		// 3 фаза RT
   		Uns sifu_TR:1;		// 4 фаза TR
   		Uns sifu_ENB:1;		// 5 enable
		Uns NoUse:10;		// 6-15	Не используются
	} bit;
} TRegSifu;

typedef struct {
	Uns      *UrAngle;			// Вход:       Указатель на угол напряжения фазы R
	Uns      *UsAngle;			// Вход:       Указатель на угол напряжения фазы S
	Uns      *UtAngle;			// Вход:       Указатель на угол напряжения фазы T
	Int      *Polarity;			// Вход:       Указатель на полярность напряжения
	Uns       Direction;		// Вход:			Направление включения
	Uns       SetAngle;			// Вход:       Заданный угол открытия
	TRegSifu  Status;			// Выход:		Состояние управляющих ножек
	Uns       MaxAngle;			// Параметр:   Максимальный угол открытия
	Uns       OpenAngle;		// Переменная: Текущий угол открытия
	Uns       AccelTime;		// Параметр:   Задатчик интенсивности угла открытия
	Uns       AccelTimer;		// Переменная: Cчетчик задатчика
} SIFU;

void sifu_calc(SIFU *);
void sifu_calc2(SIFU *); 
 
#ifdef __cplusplus
}
#endif // extern "C"

#endif


