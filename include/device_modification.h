#ifndef _DEVICE_MODIFICATION_H_
#define _DEVICE_MODIFICATION_H_


// ����������� ����� ���
#define BUR_M		0
#define PLAT_VERSION_7  1
#define BUR_90		0


// ������������� �����
#if BUR_M
#define DEVICE_ID		4503	// ������������� ���-� 25.02.15
#else
#define DEVICE_ID		4003	// ������������� ���-� 25.02.15
#endif

/*
#if BUR_M			// ������ ��� ����������� M
	...
#else				// ������ ��� ����������� T
	...
#endif
*/

#endif
