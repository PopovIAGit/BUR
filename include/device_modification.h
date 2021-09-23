#ifndef _DEVICE_MODIFICATION_H_
#define _DEVICE_MODIFICATION_H_


// Модификация блока БУР
#define BUR_M			0
#define BUR_90			1

#if BUR_90
#define PLAT_VERSION_7  1 // может быть только 1
#else
#define PLAT_VERSION_7  1 // менять в зависимости от версии
#endif



// Идентификатор блока
#if BUR_M
#define DEVICE_ID		4503	// Идентификатор БУР-М 25.02.15
#else
#define DEVICE_ID		4003	// Идентификатор БУР-Т 25.02.15
#endif

/*
#if BUR_M			// Только для модификации M
	...
#else				// Только для модификации T
	...
#endif
*/

#endif
