/*======================================================================
Имя файла:          csl.h
Автор:              Саидов В.С.
Версия файла:       01.03
Дата изменения:		31/03/10
Описание:
Библиотека поддержки чипа
======================================================================*/

#ifndef CSL_
#define CSL_

#ifndef HSPCLK_PRESC
#define HSPCLK_PRESC	0
#endif

#ifndef LSPCLK_PRESC
#define LSPCLK_PRESC	0
#endif

#if (HSPCLK_PRESC == 0)
#define HSPCLK_DIV		1
#else
#define HSPCLK_DIV		(HSPCLK_PRESC * 2)
#endif	

#if (LSPCLK_PRESC == 0)
#define LSPCLK_DIV		1
#else
#define LSPCLK_DIV		(LSPCLK_PRESC * 2)
#endif	

#define F_HSPCLK	(SYSCLK / HSPCLK_DIV)
#define F_LSPCLK	(SYSCLK / LSPCLK_DIV)

#include "csl\csl_rtos.h"
#include "csl\csl_queue.h"
#include "csl\csl_utils.h"
#include "csl\csl_io.h"
#include "csl\csl_spi.h"
#include "csl\csl_sci.h"
#include "csl\csl_analog.h"
#include "csl\csl_i2c.h"
#include "csl\csl_memory.h"
#include "csl\csl_timer.h"
#include "csl\csl_dlog.h"

#endif







