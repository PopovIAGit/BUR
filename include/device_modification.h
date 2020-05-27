#ifndef _DEVICE_MODIFICATION_H_
#define _DEVICE_MODIFICATION_H_


// Модификация блока БУР
#define BUR_M		0
#define PLAT_VERSION_7  1
#define BUR_90		0


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
