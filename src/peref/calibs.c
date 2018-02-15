#include "config.h"

// макросы

#define MpyDivK(V1,V2,K,Prec) 	_IQ7div((LgInt)(V1) * (LgInt)(V2), (LgInt)(K) << (7 - (Prec)))
#define DivKQ1(V1,V2,K,Prec) 	(Int)(_IQ1div((LgInt)(V1) * (LgInt)(K), (LgInt)(V2)) >> ((Prec) + 1))

static Byte CheckStatus(CALIBS *p, Uns Condition);

void Calibration(CALIBS *p)
{
	ClbCommand *Command = p->Command;
	ClbIndication *Indic = p->Indication; 
	LgUns Data;

	//задание/сброс закрыто
	if(Command->TaskClose !=0)						 // Проверяем поступила ли команда управления для "ЗАКРЫТО"
	{
		switch (Command->TaskClose)					 // выбераем калибровать (1) или сбрасывать (2)
		{
			case 1:
				if (!CheckStatus(p,CLB_CLOSE))break; // если уже откалибровано или вкл. авто то уходим, 
				Indic->ClosePos = *p->AbsPosition;	 // забераем данные с энкодера(абсолютное положение)
			    Indic->Status |= CLB_CLOSE;			 // добавляем в статус бит калибровки по закрыто

				break;
			case 2: 
				if (!CheckStatus(p,0))break;		 // если уже сброшено или авто то уходим
				Indic->ClosePos = 0;				 // выставляем положение на 0
				Indic->Status &= CLB_OPEN;			 // сбрасываем стоп бит в статусу
				break;
		}
		Command->TaskClose = 0;						 // сбросили команду управления
	}
	//задание/сброс открыто
	if(Command->TaskOpen !=0)
	{
		switch (Command->TaskOpen)					 // аналогично "ЗАКРЫТО", тока наоборот
		{
			case 1: 
				if (!CheckStatus(p,CLB_OPEN))break;
				Indic->OpenPos = *p->AbsPosition;
				Indic->Status |= CLB_OPEN;

				break;
			case 2: 
				if (!CheckStatus(p,0))break;
				Indic->OpenPos = 0;
				Indic->Status &= CLB_CLOSE;
				break;
		}
		Command->TaskOpen = 0;	
	}
	// задание количества оборотов на открытие
	if (Command->RevOpen > 0)								// Проверяем поступила ли команда на задание оборотов "Открыто"
	{
		if (CheckStatus(p, CLB_FLAG))						// Проверяем была ли выполнена калибровка, если нет то идем дальше
		{	
			Indic->ClosePos = *p->AbsPosition;				// Запомнинаем текущее положение как "ЗАКРЫТО"
			Data = CalcClbAbsRev(p, Command->RevOpen);		// расчитываем число меток энкодера в зависимости от задания
			if (*p->RodType) Data = Indic->ClosePos - Data;	// если выбран обратный тип штока то
			else Data = Indic->ClosePos + Data;				// если выбран прямой тип штока то
			Indic->OpenPos = Data & p->RevMax;				// Запоминаем положение открыто  и проверяем не привышает ли оно максимально количество оборотов
			Indic->Status  = CLB_FLAG;						// Выставляем флаг калибровки

		}
		Command->RevOpen = 0;								// Сбрасываем команду на задание оборотов "Открыто"
	}
	
	// задание количества оборотов на Закрытие
	if (Command->RevClose > 0)								// Аналогично оборотов "Открыто"
	{
		if (CheckStatus(p, CLB_FLAG))
		{
			Indic->OpenPos = *p->AbsPosition;
			Data = CalcClbAbsRev(p, Command->RevClose);
			if (*p->RodType) Data = Indic->OpenPos + Data;
			else Data = Indic->OpenPos - Data;
			Indic->ClosePos = Data & p->RevMax;
			Indic->Status = CLB_FLAG;

		}
		Command->RevClose = 0;
	}

	//Автокалибровка
	if(Command->RevAuto > 0)								// Проверяем поступила ли команда на автокалибровку
	{
		if(CheckStatus(p, CLB_FLAG))						// Проверяем, если небыло откалиброванно
		{
			p->AutoStep = 1;								// Выставляем первый шаг автокалибровки-> разрешаем автокалибровку
			p->RevAuto = Command->RevAuto;					// 
		}
		Command->RevAuto = 0;								// Сбрасываем команду на задание оборотов автокалибровки

		if(p->AutoStep>0)									// Проверяем, если разрешена автокалибровка, то выполняем ее. Если нет то выходим
		{	
			switch(p->AutoStep)								// Переключение шагов автокалибровки
			{
				case 1: p->AutoCmd = CLB_CLOSE; p->AutoStep = 2; break;	// команда автокалибровки - калибровка положения закрыто, +след. шаг
		   		case 2: if (*p->MuffFlag) {p->MuffTimer = p->MuffTime; p->AutoStep = 3;} break; // Если выставлен флаг крайнего положения, то задаем время успокоения калебаний, +след шаг
		   		case 3: 
					   if (p->MuffTimer > 0) p->MuffTimer--;	// если таймер успокоения колебаний не равен нулю, то ждем пока закончится
					   else										// если закончился таймер успокоения
					   {
						   Indic->ClosePos = *p->AbsPosition;	// Задаем текущее положение как "ЗАКРЫТО"
						   Indic->Status = CLB_CLOSE;			// Выставляем статус что откалибровано положение "ЗАКРЫТО"
						   p->AutoCmd = CLB_OPEN;				// выставляем команду автокалибровки калибровка положения открыто
						   p->AutoStep = 4;						// +след.шаг
					   }
					   break;
		  	   	case 4: if (!(*p->MuffFlag)) p->AutoStep = 5; break; // переход на следующий шаг только после выхода из крайнего положения
		   	   	case 5: if (*p->MuffFlag) {p->MuffTimer = p->MuffTime; p->AutoStep = 6;} break; // Если выставлен флаг крайнего положения, то задаем время успокоения колебаний, +след. шаг
		   	   	case 6: 
			           if (p->MuffTimer > 0) p->MuffTimer--;	// если таймер успокоения колебаний не равен нулу, то ждем пока закончится
						   else									// если закончился таймер успокоения
						   {
							   if (*p->CurWay < (Int)p->RevAuto) Indic->Status = 0; //проверяем, если текущий пройденый путь меньше задания автокалибровки, то сбрасываем калибровку
							   else								// иначе, если все хорошо
							   {
								   Indic->OpenPos = *p->AbsPosition;	// Задаем текущее положение как "ОТКРЫТО"
								   Indic->Status = CLB_FLAG;			// выставляем статус - Полностью откалиброванно
							   }
							   p->AutoStep = 0;							// сбрасываем счетчик шагов автокалибровки
						   }
					  break;
			}
		}	
	}
}

void DefineCalibParams(CALIBS *p)						// обработка параметров калибровки ДП и позиционировани
{
	ClbIndication *Indic = p->Indication;				// указатель на структуру Индикации калибровки
	LgUns Position, ClosePos, OpenPos;					// Внутренние переменные
	
	if (*p->RodType)									// если выбран обратный тип штока
	{
		Position = p->RevMax - *p->AbsPosition + 1;		// текущая позиция ровна максимальному количеству 
		ClosePos = p->RevMax - Indic->ClosePos + 1;		// положение закрыто  = макс пололжение - запомненное положение закрыто
		OpenPos  = p->RevMax - Indic->OpenPos  + 1;		// аналогтчно
	}
	else	// если выбран обычный шток
	{
		Position = *p->AbsPosition;			// запоминаем текущее положение
		ClosePos = Indic->ClosePos;			// запоминаем положение закрыто
		OpenPos  = Indic->OpenPos;			// запоминаем положение открыто
	}
	
	p->Zone = 0;							// обнуляем зону калибровки (не выбрана зона для калибровки)
	if (Indic->Status == CLB_FLAG)						// если калибровка выполнена
	{
		if (!p->FullStep)								// если полный ход не определен, расчитываем радиальное перемещение
		{
			p->FullStep = (OpenPos - ClosePos) & p->RevMax;	// полный ход (оборот дв) (побитовое сравнение нужно для того чтобы значение было не больше ревмакс)
			*p->FullWay = (Uns)CalcClbGearRev(p, (LgInt)p->FullStep);	// полный ход (вых звено)
			if ((*p->FullWay >> 1) <= *p->PositionAcc) p->FullStep = 0; // если полный ход меньше или равен смещения конечного положения то фул степ посчитан неверно или зона слишком большая то сбрасываем в 0
			if (!p->FullStep) p->Zone = CLB_FLAG;						// если сбросили фул степ то выставляем зону как откалиброванную
			else	// определяем базовое положение
			{	
				p->BasePos = (OpenPos + ClosePos) >> 1;					// базовое положение (А+Б)/2
				if (OpenPos >= ClosePos) p->BasePos = p->BasePos + (p->RevMax >> 1); // если открыто больше чем закрыто (тоесть есть переход через 0) то, базовое положение +1/2 от макс. колличества оборотов
			//	p->BasePos = p->RevMax - (p->FullStep >> 1);
				p->BasePos = p->BasePos & p->RevMax; 
			}
		}
		else	// если полный ход определен, расчитываем линейное перемещение
		{
			p->LinePos = Position - ClosePos;
			if (ClosePos > p->BasePos)
			{
				if (Position < p->BasePos) 
					p->LinePos = p->LinePos + p->RevMax + 1;
			}
			else
			{
				if (Position > p->BasePos) 
					p->LinePos = p->LinePos - p->RevMax - 1;
			}
			
			*p->CurWay = CalcClbGearRev(p, p->LinePos);		// пересчитываем на из линейного перемещения вала дв. в линейное перемещение выходного звена
			if (*p->CurWay <= (Int)*p->PositionAcc) p->Zone |= CLB_CLOSE; // если текущее положени выходного звена меньше 
			if (*p->CurWay >= ((Int)*p->FullWay - (Int)*p->PositionAcc)) p->Zone |= CLB_OPEN; // если текущее положение выходного звена
			
			*p->PositionPr = DivKQ1(p->LinePos, p->FullStep, 1000, 0);				// считаем текущее положение в процентах (0-открыто 100-закрыто)

			//if ((p->Zone & CLB_CLOSE) && (*p->PositionPr > 0))    *p->PositionPr = 0;	// <-- Зачем?
			//if ((p->Zone & CLB_OPEN)  && (*p->PositionPr < 1000)) *p->PositionPr = 1000;// <-- Зачем?
		}
	}
	else	// если калибровка не выполнена или выполнена не до конца
	{
		*p->PositionPr = 9999;  //  небывалое текущее положение в процентах
		*p->FullWay = 9999;		//	небывалый полный ход выходного звена
		p->FullStep = 0;		//	вобщем и полный ход
		
		if (!Indic->Status) {*p->CurWay = 9999; return;}	// если совсем не откалиброванно то текущий ход выходного звена неизвестен и выходим, нечего тут делать
		else if (Indic->Status & CLB_CLOSE) p->LinePos = (Position - ClosePos) & p->RevMax;	// если откалиброванно на закрыто то косвенно считаем линейное положение дв 
		else if (Indic->Status & CLB_OPEN)  p->LinePos = (OpenPos - Position)  & p->RevMax; // если откалиброванно на открыто то косвенно считаем линейное положение дв
		
		*p->CurWay = CalcClbGearRev(p, p->LinePos);	// по косвенным данным считаем текущее положение выходного звена
		if (*p->CurWay <= (Int)*p->PositionAcc) p->Zone |= Indic->Status;	// если текущее положение меньше смещения конечных положений то чтото не так и сбрасываем всю калибровку
	}
}

// -----------------------------------------------------------------

// -----------------------------------------------------------------
LgUns CalcClbGearInv(CALIBS *p) 
{
	#if defined(__TMS320C28X__)
	return 0UL;
	#else
	return ((16384000UL >> p->PosSensPow) / (LgUns)p->GearRatio + 1);
	#endif
}

LgUns CalcClbAbsRev(CALIBS *p, Uns GearRev)
{
	return MpyDivK(GearRev, p->GearRatio, 1000, *p->PosSensPow);
}

Int CalcClbGearRev(CALIBS *p, LgInt AbsRev)
{
	return DivKQ1(AbsRev, p->GearRatio, 1000, *p->PosSensPow);
}
/*
LgUns CalcClbPercentToAbs(CALIBS *p, Uns Percent)
{
	return MpyDivK(p->FullStep, Percent, 1000, 0);
}
*/
void CalcClbCycle(CALIBS *p)	// считалка полных циклов работы без выключения блока
{
	if (!p->Indication->Status) {p->CycleData = 0; return;}	//	если не откалиброванно то уходим
	
	if (p->Zone != 0)	// если находимся в зоне калибровке
	{
	    if (!p->CycleData && p->Zone != 0x3) p->CycleData = p->Zone;
		
		if (p->Zone == (p->CycleData & CLB_FLAG))	// если откалиброванно
		{
			if (p->CycleData & 0x40) return;
			p->CycleData |= 0x40;
			if (p->CycleData & 0x80)
			{
				*p->CycleCnt = *p->CycleCnt + 1;
				p->CycleData &= ~0x80;
			}
		}
		else
		{
			if (p->CycleData & 0x40)
			{
				p->CycleData &= ~0x40;
				p->CycleData |=  0x80;
			}
		}
	}
}

static Byte CheckStatus(CALIBS *p, Uns Condition) // проверка статуса - 1 если не откалиброванно, 0 если откалиброванно или выставлен флаг сброса или идет автокалибровка
{
	if ((p->Indication->Status & Condition) || p->ResetFlag || p->AutoStep) // если откалиброванно, или выставлен флаг сброса, или включена автокалибровка, то
	{
		p->CancelFlag = TRUE; // выставляем флаг отмены команды
		return 0;			  // выходим выставляя 0
	}
	return 1;				//иначе выходим выставляя 1
}
