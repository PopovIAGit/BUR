#ifndef _DISPLHAL_H_
#define _DISPLHAL_H_

#define __CLR       0x01  // ������� �������
#define __HOME      0x02  // ����������� ������� � ��������� �������

#define __CGADDR    0x40  // ����� ��� ��������� ��������
#define __DDADDR    0x80  // ����� ��� ������

#define __FSTR      0x00|__DDADDR  // ����� ������ �������
#define __SSTR      0x40|__DDADDR  // ����� ������ �������

#define __S         0x01  // ����� ������ �������
#define __ID        0x02  // �����������������/����������������� DDRAM ������

#define __B         0x01  // ���������/���������� ������� �������
#define __C         0x02  // ���������/���������� �������
#define __D         0x04  // ���������/���������� �������

#define __RL        0x04  // ����� �����/�����
#define __SC        0x08  // ����� ����� �������/����������� �������

#define __LUM100    0x00  // ������� 100%
#define __LUM75     0x01  // ������� 75%
#define __LUM50     0x02  // ������� 50%
#define __LUM25     0x03  // ������� 25%

#define __FT0		0x00
#define __FT1		0x02
#define __F         0x04  // ����� ������
#define __N         0x08  // ���������� �����
#define __DL        0x10  // ����� ������

#define __BF		0x80  // Busy Flag

// ������� ������� ����������

// FT1 = 1; FT0 = 0; N = 1; DL = 1;
// ������ ������� ��� OLED WINSTAR
#define WINSTAR_FUNCTION_SET	(0x20|__DL|__N|__FT1)	// ����� 5*10 �����, 2 ������, ����� �� 8 ���, ������� ������� ��������
// ������ ������� ��� VAC FUTABA
#define FUTABA_FUNCTION_SET		(0x20|__DL|__N|__LUM100)// 2 ������, ����� �� 8 ���, 100 % �������

// D = 1
#define DISPLAY_ON        (0x08 | __D)
// D = 0
#define DISPLAY_OFF       (0x08)
// ID = 1
#define ENTRY_MODE_SET    (__ID | 0x04)

// SC = 0; RL = 1
#define CUR_DISPL_SHIFT   (__RL | 0x10)

#define DISPL_PAUSE_TIME	50

static char Symbols[16][7] = {
	0x11,0x11,0x13,0x15,0x19,0x11,0x11, // 0x00 - �
	0x11,0x11,0x11,0x19,0x15,0x15,0x19, // 0x01 - �
	0x1f,0x11,0x11,0x11,0x11,0x11,0x11, // 0x02 - �
	0x0f,0x05,0x05,0x05,0x05,0x15,0x09, // 0x03 - �
	0x15,0x15,0x15,0x15,0x15,0x15,0x1f, // 0x04 - �	
	0x0f,0x05,0x05,0x09,0x11,0x1f,0x11, // 0x05 - �
	0x0e,0x15,0x15,0x15,0x0e,0x04,0x04, // 0x06 - �
	0x08,0x08,0x08,0x0e,0x09,0x09,0x0e, // 0x07 - �
	0x1f,0x11,0x10,0x10,0x10,0x10,0x10, // 0x08 - �
	0x15,0x15,0x15,0x0e,0x15,0x15,0x15, // 0x09 - �
	0x11,0x11,0x11,0x11,0x11,0x1f,0x01, // 0x0a - �
	0x11,0x11,0x11,0x0f,0x01,0x01,0x01, // 0x0b - �
	0x1f,0x11,0x10,0x1e,0x11,0x11,0x1e, // 0x0c - �
	0x15,0x15,0x15,0x15,0x15,0x1f,0x01, // 0x0d - �
	0x12,0x15,0x15,0x1d,0x15,0x15,0x12, // 0x0e - �
	0x0f,0x11,0x11,0x0f,0x05,0x09,0x11  // 0x0f - �
};

static char WINSTAR_RusTable[] = {
	0x41, // '�', // 0xc0 // 0
	0xA0, // '�', // 0xc1 // 1
	0x42, // '�', // 0xc2 // 2
	0xA1, // '�', // 0xc3 // 3
	0xE0, // '�', // 0xc4 // 4
	0x45, // '�', // 0xc5 // 5
	0xA3, // '�', // 0xc6 // 6
	0xA4, // '�', // 0xc7 // 7
	0xA5, // '�', // 0xc8 // 8
	0xA6, // '�', // 0xc9 // 9
	0x4B, // '�', // 0xca // 10
	0xA7, // '�', // 0xcb // 11
	0x4D, // '�', // 0xcc // 12
	0x48, // '�', // 0xcd // 13
	0x4F, // '�', // 0xce // 14
	0xA8, // '�', // 0xcf // 15
	0x50, // '�', // 0xd0 // 16
	0x43, // '�', // 0xd1 // 17
	0x54, // '�', // 0xd2 // 18
	0xA9, // '�', // 0xd3 // 19
	0xAA, // '�', // 0xd4 // 20
	0x58, // '�', // 0xd5 // 21
	0xE1, // '�', // 0xd6 // 22
	0xAB, // '�', // 0xd7 // 23
	0xAC, // '�', // 0xd8 // 24
	0xE2, // '�', // 0xd9 // 25
	0xAD, // '�', // 0xda // 26
	0xAE, // '�', // 0xdb // 27
	0xC4, // '�', // 0xdc // 28
	0xAF, // '�', // 0xdd // 29
	0xB0, // '�', // 0xde // 30
	0xB1, // '�', // 0xdf // 31
	0x61, // '�', // 0xe0 // 32
	0xB2, // '�', // 0xe1 // 33
	0xB3, // '�', // 0xe2 // 34
	0xB4, // '�', // 0xe3 // 35
	0xE3, // '�', // 0xe4 // 36
	0x65, // '�', // 0xe5 // 37
	0xB6, // '�', // 0xe6 // 38
	0xB7, // '�', // 0xe7 // 39
	0xB8, // '�', // 0xe8 // 40
	0xB9, // '�', // 0xe9 // 41
	0xBA, // '�', // 0xea // 42
	0xBB, // '�', // 0xeb // 43
	0xBC, // '�', // 0xec // 44
	0xBD, // '�', // 0xed // 45
	0x6F, // '�', // 0xee // 46
	0xBE, // '�', // 0xef // 47
	0x70, // '�', // 0xf0 // 48
	0x63, // '�', // 0xf1 // 49
	0xBF, // '�', // 0xf2 // 50
	0x79, // '�', // 0xf3 // 51
	0xE4, // '�', // 0xf4 // 52
	0x78, // '�', // 0xf5 // 53
	0xE5, // '�', // 0xf6 // 54
	0xC0, // '�', // 0xf7 // 55
	0xC1, // '�', // 0xf8 // 56
	0xE6, // '�', // 0xf9 // 57
	0xC2, // '�', // 0xfa // 58
	0xC3, // '�', // 0xfb // 59
	0xC4, // '�', // 0xfc // 60
	0xC5, // '�', // 0xfd // 61
	0xC6, // '�', // 0xfe // 62
	0xC7, // '�', // 0xff // 63
	0xA2, // '�', // 0xa8 // 64
	0xB5  // '�', // 0xb8 // 65
};


static char FUTABA_RusTable[] = {
	0x41, // '�', // 0xc0 // 0
	0x80, // '�', // 0xc1 // 1
	0x42, // '�', // 0xc2 // 2
	0x92, // '�', // 0xc3 // 3
	0x05, // '�', // 0xc4 // 4
	0x45, // '�', // 0xc5 // 5
	0x82, // '�', // 0xc6 // 6
	0x83, // '�', // 0xc7 // 7
	0x84, // '�', // 0xc8 // 8
	0x85, // '�', // 0xc9 // 9
	0x4b, // '�', // 0xca // 10
	0x86, // '�', // 0xcb // 11
	0x4d, // '�', // 0xcc // 12
	0x48, // '�', // 0xcd // 13
	0x4f, // '�', // 0xce // 14
	0x87, // '�', // 0xcf // 15
	0x50, // '�', // 0xd0 // 16
	0x43, // '�', // 0xd1 // 17
	0x54, // '�', // 0xd2 // 18
	0x88, // '�', // 0xd3 // 19
	0x06, // '�', // 0xd4 // 20
	0x58, // '�', // 0xd5 // 21
	0x89, // '�', // 0xd6 // 22
	0x8a, // '�', // 0xd7 // 23
	0x04, // '�', // 0xd8 // 24
	0x8c, // '�', // 0xd9 // 25
	0x8d, // '�', // 0xda // 26
	0x8e, // '�', // 0xdb // 27
	0x07, // '�', // 0xdc // 28
	0x8f, // '�', // 0xdd // 29
	0xac, // '�', // 0xde // 30
	0xad, // '�', // 0xdf // 31
	0x41, // '�', // 0xe0 // 32
	0x80, // '�', // 0xe1 // 33
	0x42, // '�', // 0xe2 // 34
	0x92, // '�', // 0xe3 // 35
	0x05, // '�', // 0xe4 // 36
	0x45, // '�', // 0xe5 // 37
	0x82, // '�', // 0xe6 // 38
	0x83, // '�', // 0xe7 // 39
	0x84, // '�', // 0xe8 // 40
	0x85, // '�', // 0xe9 // 41
	0x4b, // '�', // 0xea // 42
	0x86, // '�', // 0xeb // 43
	0x4d, // '�', // 0xec // 44
	0x48, // '�', // 0xed // 45
	0x4f, // '�', // 0xee // 46
	0x87, // '�', // 0xef // 47
	0x50, // '�', // 0xf0 // 48
	0x43, // '�', // 0xf1 // 49
	0x54, // '�', // 0xf2 // 50
	0x88, // '�', // 0xf3 // 51
	0x06, // '�', // 0xf4 // 52
	0x58, // '�', // 0xf5 // 53
	0x89, // '�', // 0xf6 // 54
	0x8a, // '�', // 0xf7 // 55
	0x04, // '�', // 0xf8 // 56
	0x8c, // '�', // 0xf9 // 57
	0x8d, // '�', // 0xfa // 58
	0x8e, // '�', // 0xfb // 59
	0x07, // '�', // 0xfc // 60
	0x8f, // '�', // 0xfd // 61
	0xac, // '�', // 0xfe // 62
	0xad, // '�', // 0xff // 63
	0xcb, // '�', // 0xa8 // 64
	0xcb  // '�', // 0xb8 // 65
};

#endif




