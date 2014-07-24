/*======================================================================
��� �����:          value_conv.h
�����:              ������ �.�.
������ ����������:  01.02
������ �����:       01.01
�������������:      ��� ���� �����
������� ������:		
���������� �������:
��������:
	���������� ��� �������������� �����/�����

========================================================================
�������:
------------------------------------------------------------------------
	24/04/09	������ 01.00

	10/09/09	������ 01.01
1.��������� �������������� ������� � ���� � ������ ��� 4� ����������� ����������

	10/09/09	������ 01.02
2.����������� ������� ����������
----------------------------------------------------------------------*/

#ifndef VALUE_CONV_
#define VALUE_CONV_

#include "std.h"

#ifdef __cplusplus
extern "C" {
#endif

// ����������-�����������
extern Char FloatSeparator;
extern Char FloatPositive;
extern Char TimeSeparator;
extern Char DateSeparator;

union _TTimeVar;
union _TDateVar;

__inline void ByteToStr(Byte Value, String Str)
{
	register Byte Tmp = Value / 10;
	Str[0] = Tmp | 0x30;
	Str[1] = (Value - Tmp * 10) | 0x30;
}

// ��������� �������
Byte DecToStr(Uns Value, String Str, Byte Prec, Byte RateMax, Bool Clear, Bool Sign);
void DecToStr4s(Uns Value, String Str);
Byte BinToStr(Uns Value, String Str, Byte RateMax, Bool BinChar);
void BinToStr4s(Uns Value, String Str, Byte Bit);
Byte HexToStr(Uns Value, String Str, Byte RateMax, Bool HexChar);
void HexToStr4s(Uns Value, String Str);
Byte TimeToStr(union _TTimeVar *Time, String Str);
void TimeToStr4s(union _TTimeVar *Time, String Str);
Byte DateToStr(union _TDateVar *Date, String Str);
void DateToStr4s(union _TDateVar *Date, String Str, Bool Flag);
Uns  StrToDec(String Str, Byte RateMax);
Uns  StrToBin(String Str, Byte RateMax);
Uns  StrToHex(String Str, Byte RateMax);
Uns  StrToTime(String Str);
Uns  StrToDate(String Str);

#ifdef __cplusplus
}
#endif // extern "C"

#endif

