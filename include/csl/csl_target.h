/*======================================================================
Имя файла:          csl_target.h
Автор:              Саидов В.С.
Версия файла:       01.01
Дата изменения:		28/01/2010
======================================================================*/

#ifndef CSL_TARGET_
#define CSL_TARGET_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CHIP_280X)
#include "chip\DSP280x_Device.h"

#elif defined(CHIP_281X)
#include "chip\DSP281x_Device.h"

#elif defined(CHIP_24XX)
#include "chip\DSP24xx_Device.h"

#elif defined(__IAR_SYSTEMS_ICC__)
#include <ioavr.h>
#include <ina90.h>
#include "chip\AVR_GlobalPrototypes.h"

#else
#error "Undefined CHIP_XXXX!"
#endif

#ifdef __cplusplus
}
#endif // extern "C"

#endif /* CSL_I2C_ */




