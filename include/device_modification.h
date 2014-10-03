#ifndef _DEVICE_MODIFICATION_H_
#define _DEVICE_MODIFICATION_H_


// Модификация блока БУР
#define BUR_M		0


// Идентификатор блока
#if BUR_M
#define DEVICE_ID		4001	// Идентификатор БУР-М
#else
#define DEVICE_ID		4000	// Идентификатор БУР-Т
#endif

/*
#if BUR_M			// Только для модификации M
	...
#else				// Только для модификации T
	...
#endif
*/

#endif
