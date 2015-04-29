/*======================================================================
Имя файла:          rtc.с
Автор:              Саидов В.С.
Версия библиотеки:  01.00
Версия файла:       01.00
======================================================================*/

#include "rtc.h"

// Маска для режима AM/PM
#define AM_PM_MASK  (RTC_AM_PM|RTC_AM_PM_MODE)


void RTC_Update(void *p)
{
	RTC_Obj *Obj = (RTC_Obj *)p;
	Byte Mode = Obj->Hour & AM_PM_MASK;
	Byte Hour = Obj->Hour & (~AM_PM_MASK);

	Obj->Sec++;
	if (Obj->Sec >= 60) {Obj->Min++; Obj->Sec = 0;}
	
	if (Obj->Min >= 60) {Hour++; Obj->Min = 0;}
	
	if (Mode & RTC_AM_PM_MODE)
	{
		if (Hour > 12)
		{
			if (!(Mode & RTC_AM_PM)) Mode |= RTC_AM_PM;
			else {Obj->WeekDay++; Obj->Day++; Mode &= ~RTC_AM_PM;}
			Hour = 1;
		}
	}
	else if (Hour >= 24)
	{
	   Obj->WeekDay++;
	   Obj->Day++;
	   Hour = 0;
  }
	Obj->Hour = Mode | Hour;
	
	if (Obj->WeekDay > 7) Obj->WeekDay = 1;
	
	if (Obj->Day > RTC_DefMaxDay(Obj->Year, Obj->Month)) {Obj->Month++; Obj->Day = 1;}
	
	if (Obj->Month > 12) {Obj->Year++; Obj->Month = 1;}
	
	if (Obj->Year > 99) Obj->Year = 0;
}

void RTC_Tick(void *p)
{
	RTC_Lite *Obj = (RTC_Lite *)p;

	if (++Obj->Sec >= 60)
	{
		if (++Obj->Min >= 60)
		{
			if (++Obj->Hour >= 24)
			{
				Obj->Day++;
				Obj->Hour = 0;
			}
			Obj->Min = 0;
		}
		Obj->Sec = 0;
	}
}

Byte RTC_DefMaxDay(Byte Year, Byte Month)
{
	switch (Month)
	{
	   case 2:
	      return ((Year % 4) ? 28 : 29);
  	case 4:
  	case 6:
  	case 9:
  	case 11:
  	   return 30;
	   default:
	      return 31;
	}
}

void RTC_GetTime(void *p, TTimeVar *Time)
{
	RTC_Obj *Obj = (RTC_Obj *)p;

	Time->bit.Hour = Obj->Hour & (~AM_PM_MASK);
	Time->bit.Min  = Obj->Min;
}

void RTC_GetDate(void *p, TDateVar *Date)
{
	RTC_Obj *Obj = (RTC_Obj *)p;

	Date->bit.Year  = Obj->Year;
	Date->bit.Month = Obj->Month;
	Date->bit.Day   = Obj->Day;
}

void RTC_SetTime(void *p, TTimeVar *Time, Byte Sec)
{
	RTC_Obj *Obj = (RTC_Obj *)p;

	Byte Mode = Obj->Hour & AM_PM_MASK;
	if (Mode & RTC_AM_PM_MODE) {
		if ((Time->bit.Hour < 1) || (Time->bit.Hour > 12)) Time->bit.Hour = 1;
	} else {
		if (Time->bit.Hour >= 24) Time->bit.Hour = 0;
	}
	
	if (Time->bit.Min >= 60) Time->bit.Min = 0;
	if (Sec >= 60) Sec = 0;
	
	Obj->Hour = Mode | Time->bit.Hour;
	Obj->Min = Time->bit.Min;
	Obj->Sec = Sec;
}

void RTC_SetDate(void *p, TDateVar *Date, Byte WeekDay)
{
	RTC_Obj *Obj = (RTC_Obj *)p;

	if (Date->bit.Year > 99) Date->bit.Year = 0;
	if ((Date->bit.Month < 1) || (Date->bit.Month > 12)) Date->bit.Month = 1;
	if ((Date->bit.Day < 1) || (Date->bit.Day > RTC_DefMaxDay(Date->bit.Year, Date->bit.Month))) Date->bit.Day = 1;
	if ((WeekDay < 1) || (WeekDay > 7)) WeekDay = 1;

	Obj->Year    = Date->bit.Year;
	Obj->Month   = Date->bit.Month;
	Obj->Day     = Date->bit.Day;
	Obj->WeekDay = WeekDay;
}

Byte RTC_TimeCorr(void *p, Int Corr)
{
	RTC_Obj *Obj = (RTC_Obj *)p;
	Byte Mode = Obj->Hour & AM_PM_MASK;
	Byte Hour = Obj->Hour & (~AM_PM_MASK);
	Int  Tmp;
	
	if (Hour == 1)
	{
		if (!(Mode & RTC_AM_PM_MODE)) Tmp = 0;
		else if (!(Mode & RTC_AM_PM)) Tmp = 12;
		else return 0;

		if (Obj->PrevHour == (Byte)Tmp)
		{
			Tmp = ((Int)Obj->Min * 60 + (Int)Obj->Sec) + Corr;
			if (Tmp >= 0) {
				Obj->Min = Tmp / 60;
				Obj->Sec = Tmp - (Int)Obj->Min * 60;
				
				Obj->PrevHour = 1;
				return 1;
			}
		}
	}
	else Obj->PrevHour = Hour;

	return 0;
}

