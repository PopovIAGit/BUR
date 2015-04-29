/*======================================================================
Имя файла:          hardware.h
Автор:              Саидов В.С.
Версия файла:       01.01
Дата изменения:		05/04/10
Описание:
Интерфейсный модуль
======================================================================*/

#ifndef HARDWARE_
#define HARDWARE_

#include "chip\f280xbmsk.h"

#define CLK_VS_PERIOD			(SYSCLK / CLK_VS_FREQ)

void InitHardware(void);

#endif

