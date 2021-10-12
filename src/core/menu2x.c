#include "config.h"

#define EXPRESS_MAX_STATE		34

__inline Bool SleepMode(MENU *);
__inline void StartState(MENU *);
__inline void SelectGroup(MENU *);
__inline void SelectParam(MENU *);
__inline void EditParam(MENU *);
__inline void ExpressState(MENU *);	
__inline void EditDec(MENU *, Char Key);
__inline void EditStr(MENU *, Char Key);
__inline void EditTime(MENU *, Char Key);
__inline void EditDate(MENU *, Char Key);
__inline void EditBin(MENU *);
static   void ReadDcrFull(MENU *, Uns Addr);
static   void ChangeCoordinate(MENU *, Char Key, Bool GroupFlag);
__inline void ShowStart(MENU *);
__inline void ShowGroup(MENU *);
__inline Byte DefineBit(MENU *, Uns Value);
static   void ShowValue(MENU *, Uns Position, Bool Edit);
static   void ShowParam(MENU *, Bool Edit);
__inline void ShowExpress(MENU *);
__inline void ShowExpressChoice(MENU *p, Byte YesOne, Byte YesTwo, 
									Byte NoOne, Byte NoTwo, Bool Yes);


Bool ExpNextStateRdy = false;
extern Bool PI_CalibEnable; 			// Флаг разрешения калибровки платы ПИ


void MENU_Update(MENU *p)
{
	if (SleepMode(p))
		return;
	
	if (!p->EvLogFlag)
	{
		switch (p->State)
		{
		   case MS_START:	StartState(p);  break;
		   case MS_SELGR:   SelectGroup(p); break;
		   case MS_SELPAR:  SelectParam(p); break;
		   case MS_EDITPAR: EditParam(p);   break;
		   case MS_EXPRESS: ExpressState(p); break;
		}
	}
	
	p->Key = 0;
}

void MENU_Display(MENU *p)
{
	if (!p->Indication) {p->Update = TRUE; return;}
	
	if (p->EvLogFlag) return;
	
	if (p->Update)
	{
		memset(p->HiString, ' ', 16);
		memset(p->LoString, ' ', 16);
	}

	if (!p->SleepActive)
	{
		switch (p->State)
		{
		   case MS_START:   ShowStart(p);        break;
		   case MS_SELGR:   ShowGroup(p);        break;
		   case MS_SELPAR:  ShowParam(p, FALSE); break;
		   case MS_EDITPAR: ShowParam(p, TRUE);  break;
		   case MS_EXPRESS: ShowExpress(p);   	 break;
		}
	}

	p->Update = FALSE;
}

void MENU_ReadDcr(MENU *p, struct MENU_DCR *Dcr, Uns Addr)
{
	Uns Kmm = *p->MinMaxGain;

	PFUNC_blkRead((Ptr)&p->Params[Addr].Dcr, (Ptr)Dcr, sizeof(struct MENU_DCR));
	
	if ((Dcr->Config.Val.Type == MT_DEC) && (Dcr->Config.Val.MinMax))
	{
		Dcr->Min = Dcr->Min * Kmm;
		Dcr->Max = Dcr->Max * Kmm;
		Dcr->Def = Dcr->Def * Kmm;
	}
}

__inline Bool SleepMode(MENU *p)
{
	if (!p->Key)
	{
		if (!p->SleepActive)
		{
			if (!p->SleepTime) p->SleepTimer = 0;
			else if (p->SleepTimer < p->SleepTime) p->SleepTimer++;
			else {p->SleepActive = TRUE; p->Update = TRUE;}
		}
	}
	else
	{
		p->SleepTimer = 0;
		if (p->SleepActive)
		{
			p->State = MS_START;
			p->SleepActive = FALSE;
			p->Update = TRUE;
			p->Key = 0;
		}
	}
	
	return p->SleepActive;
}

__inline void StartState(MENU *p)
{
	if (p->Express.Enable)
	{
		p->State = MS_EXPRESS;
		return;
	}
	
	if (p->Key == KEY_ENTER)
	{
		p->State = MS_SELGR;
		p->Level = 1;
		p->EvLogSelected = FALSE;
		p->Update = TRUE;
		p->Group.Position  = p->Group.Count - 1;
		ChangeCoordinate(p, KEY_UP, TRUE);
	}

	if (GrH->extraPassword) GrH->extraPassword = 0;
}

__inline void SelectGroup(MENU *p)
{
	switch (p->Key)
	{
	   case KEY_UP:
	   case KEY_DOWN:
		   ChangeCoordinate(p, p->Key, TRUE);
		   p->EvLogFlag = p->EvLogSelected;
		   break;
	   case KEY_ESCAPE:
		   if (p->EvLogSelected) p->EvLogSelected = FALSE;
		   else if (p->Level == 2) p->Level = 1;
		   else p->State = MS_START;
		   break;
	   case KEY_ENTER:
		   if ((p->Group.Position == p->EvLogGroup) && !p->EvLogSelected)
		   {
			   p->EvLogSelected = TRUE;
			   if (p->Data[p->EvLogAdress] > 0) p->EvLog.Position = p->Data[p->EvLogAdress];
			   else p->EvLog.Position = *p->EvLogCount;	//!!!
//			   else p->EvLog.Position = p->EvLog.Count;	//!!!
			   
			   p->EvLogFlag = TRUE;
				ChangeCoordinate(p, KEY_DOWN, TRUE);
			   break;
		   }
		   
		   if ((p->Level == 1) && (p->MaxLevel == 2))
		   {
			   p->Level = 2;
			   p->SubGroup.Position = p->SubGroup.Count - 1;
			   ChangeCoordinate(p, KEY_UP, TRUE);
		   }
		   else
		   {
			   p->Param.Position = p->Param.Count - 1;
			   ChangeCoordinate(p, KEY_UP, FALSE);
			   p->State = MS_SELPAR;
		   }
		   break;
	   default:
		   return;
	}
	
	p->Update = TRUE;
}

__inline void SelectParam(MENU *p)
{
	struct MENU_VAL_CFG *Val = &p->Dcr.Config.Val;

	if (GrH->extraPassword)
	{
		p->State = MS_EDITPAR;
		p->Blink = FALSE;
		p->Rate  = 0;
	}
	
	switch (p->Key)
	{
	   case KEY_UP:
	   case KEY_DOWN:
		   ChangeCoordinate(p, p->Key, FALSE);
		   break;
	   case KEY_ESCAPE:
		   p->State = MS_SELGR;
		   break;
	   case KEY_ENTER:
		   if (Val->ReadOnly) return;
		   if (!p->EnableEdit(Val->PaswwPrt, p->Param.Position + p->Param.Addr)) return;
		   p->Rate  = 0;
		   p->Blink = FALSE;
		   p->State = MS_EDITPAR;
		   break;
	   default:
		   return;
	}

	p->Update = TRUE;
}

__inline void EditParam(MENU *p)
{
	struct MENU_VAL_CFG *Val = &p->Dcr.Config.Val;
	
	switch (p->Key)
	{
	   case KEY_UP:
	   case KEY_DOWN:
		   switch (Val->Type)
		   {
		      case MT_DEC:  EditDec (p, p->Key); break;
		      case MT_STR:  EditStr (p, p->Key); break;
		      case MT_TIME: EditTime(p, p->Key); break;
		      case MT_DATE: EditDate(p, p->Key); break;
		      case MT_BIN:  EditBin (p);         break;
		   }
		   break;
	   case KEY_LEFT:   if (p->Rate < p->EditRate) p->Rate++; else p->Rate = 0; break;
	   case KEY_RIGHT:  if (p->Rate > 0) p->Rate--; else p->Rate = p->EditRate; break;
	   case KEY_ESCAPE:
		   p->State = MS_SELPAR;
		   if (GrH->extraPassword) {GrH->extraPassword = 0; p->Update = true;}
		   break;
	   case KEY_ENTER:
		   if (!CheckRange(p->Value, p->Dcr.Min, p->Dcr.Max)) break;
		   if (p->State == MS_EXPRESS)
			{	
				if (!p->WriteValue(Val->Memory, p->Express.List->Buf[p->Express.Index], &p->Value)) break;		
			}
		   else
		   {
		   		if (!p->WriteValue(Val->Memory, p->Param.Position + p->Param.Addr, &p->Value)) break;
		   }
		   p->State = MS_SELPAR;
		   break;
	}
}

__inline void ExpressState(MENU *p)
{
	Byte NextState;
	
	p->Express.List = 0;

	if (p->Express.Enable)						// Вывод сообщения о выборе режима экспресс настройки
	{
		switch(p->Key)
		{
			case KEY_LEFT:
			case KEY_RIGHT:
				p->Express.Select = !p->Express.Select;			// Выбираем "да" или "нет"
				break; 
			case KEY_ENTER:
				if (!p->Express.Select) p->State = MS_START;	// Если выбрано нет, то возвращаемся в стартовое состояние меню
				p->Express.State = 0;
				p->Express.Index = 0;
				p->Update = TRUE;								// Выставляем флаг для считывания описания параметра
				p->Express.Enable = FALSE;						// Выключаем сообщение о выборе режима экспресс настройки
				p->Express.First = true;
		}
		return;
	}

	if (p->Update) return;										// Если считывается описание параметра, то выходим

	switch(p->Express.State)									// Шаги экспресс настройки
	{
		case 0: p->Express.List = &p->Express.List1; NextState = 1;		break;
		// Задать момент калибровки
		case 1: NextState = 2;											break;
		case 2: p->Express.List = &p->Express.List2; NextState = 3;		break;
		// Тип калибровки
		case 3: if (p->Express.Select) NextState = 4;	// Ручная
				else				   NextState = 8;	// Количеством оборотов
																		break;
//-------------------------------------------------------------------------------
		// Переведите в положение закрыто
		case 4: NextState = 5;											break;
		// Задание закрыто
		case 5: p->Express.List = &p->Express.List3;	NextState = 6;	break;
		// Переведите в положение открыто
		case 6: NextState = 7;											break;
		// Задание открыто
		case 7: p->Express.List = &p->Express.List4;	NextState = 12;	break;
//-------------------------------------------------------------------------------
		// Переведите в крайнее положение
		case 8: NextState = 9;	break;
		// Положение открыто или закрыто
		case 9: if (p->Express.Select)	NextState = 10;		
				else					NextState = 11;
																					break;
		// Обороты на открытие
		case 10: p->Express.List = &p->Express.List5;	NextState = 12;				break;
		// Обороты на закрытие
		case 11: p->Express.List = &p->Express.List6;	NextState = 12;				break;
//-------------------------------------------------------------------------------
		// Проверить калибровку?
		case 12: if (p->Express.Select)	NextState = 13;			// Проверочный цикл
				 else					NextState = 27;			// Пропускаем проверочный цикл							
				 break;
		// Проверяем текущее положение: открыто или закрыто. На основе этого выбираем состояние
		case 13: NextState = GetExpressState(NextState, 0); p->Key = KEY_ENTER;			break;
	//---------------------------------------------------------------------------
		// Пустите в открыто
		case 14: NextState = GetExpressState(NextState, 1);								break;
		// Выводим положение в оборотах
		case 15: NextState = GetExpressState(NextState, 2);								break;
		// Остановились по "Стоп". Программно нажимаем "Ввод" для перехода к следующему состоянию
		case 16: NextState = 17;	p->Key = KEY_ENTER; 								break;
		// Сбросить калибровку?
		case 17: if (p->Express.Select)		NextState = 18;		// Да. Сброс калибровки
				 else						NextState = 14;		// Нет. Продолжаем открывать
				 break;
		// Сброс калибровки и возврат к выбору типа калибровки
		case 18: NextState = GetExpressState(NextState, 3);
				 p->Key = KEY_ENTER;	//	Программно нажимаем "Ввод" для перехода к следующему состоянию							
				 break;
		// Доехали до "Открыто"
		case 19: NextState = GetExpressState(NextState, 4);
				 p->Key = KEY_ENTER;	//	Программно нажимаем "Ввод" для перехода к следующему состоянию		
				 break;
	//---------------------------------------------------------------------------
		// Пустите в закрыто
		case 20: NextState = GetExpressState(NextState, 5);								break;
		// Выводим положение в оборотах
		case 21: NextState = GetExpressState(NextState, 6);								break;
		// Остановились по "Стоп". Программно нажимаем "Ввод" для перехода к следующему состоянию
		case 22: NextState = 23;	p->Key = KEY_ENTER; 								break;
		// Сбросить калибровку?
		case 23: if (p->Express.Select)		NextState = 24;		// Да. Сброс калибровки
				 else						NextState = 20;		// Нет. Продолжаем закрывать
				 break;
		// Сброс калибровки и возврат к выбору типа калибровки
		case 24: NextState = GetExpressState(NextState, 3);
				 p->Key = KEY_ENTER;	//	Программно нажимаем "Ввод" для перехода к следующему состоянию							
				 break;
		// Доехали до "Закрыто"
		case 25: NextState = GetExpressState(NextState, 7);
				 p->Key = KEY_ENTER;	//	Программно нажимаем "Ввод" для перехода к следующему состоянию		
				 break;
		// Вас устраивает калибровка?
		case 26: if (p->Express.Select)		NextState = 27;		// Да. Переходим на следующий шаг
				 else						NextState = 18;		// Нет. Сбрасываем калибровку
				 break;
//-------------------------------------------------------------------------------
		// Задать рабочие моменты
		case 27: NextState = 28;												break;
		// Задаем моменты
		case 28: p->Express.List = &p->Express.List2;
				 NextState = GetExpressState(NextState, 8);			// Состояние в зависимости от типа задвижки
				 break;
		// Момент уплотнения только для клиновой 
		case 29: p->Express.List = &p->Express.List7;	NextState = 30;			break;
		// Задаем момент
		case 30: p->Express.List = &p->Express.List8;		 					
				 NextState = GetExpressState(NextState, 9);			// Состояние в зависимости от типа задвижки
				 break;
		// Момент уплотнения только для клиновой 
		case 31: p->Express.List = &p->Express.List9;	NextState = 32;			break;
		// Задаем момент
		case 32: p->Express.List = &p->Express.List10;		 					
				 NextState = GetExpressState(NextState, 10);			// Состояние в зависимости от типа модификации БУР
				 break;
		// Выбираем тип входного сигнала только для БУР-Т
		case 33: p->Express.List = &p->Express.List11;	NextState = 34;			break;
		// Выбор режима МУ/ДУ
		case 34: p->Express.List = &p->Express.List12;	/*NextState = 35;*/		break;
	}

	// Выводим текущее положение
	if ((NextState == 15) || (NextState == 16) ||  (NextState == 19) ||
			(NextState == 21) || (NextState == 22) || (NextState == 25))
		{	
			p->Express.State = NextState;
			return;
		}

	if (p->Express.List) {EditParam(p); p->State = MS_EXPRESS;}			// Если выбран List с адресами параметров, то вызываем функцию редактирования параметра,
																		// после которой выбираем состояние меню MS_EXPRESS, т.к. после редактирования оно выставляется другим
	

	switch (p->Key)
	{
		case KEY_LEFT:
		case KEY_RIGHT:
						if (!p->Express.List)
							p->Express.Select = !p->Express.Select;			// Выбираем "да" или "нет"
						break;
		case KEY_ENTER:
						if (p->Express.List)
						{
							ExpNextStateRdy = true;					// Выставляем флаг, что готовы переходить к следующему параметру или состоянию							
						}
						else
						{
							if (p->Express.State < EXPRESS_MAX_STATE) p->Express.State = NextState;			// 3 - максимальный стейт
							else {p->Express.State = 0; p->State = MS_START;}
						}
						p->Express.First = true;
						p->Update = TRUE;
						break;
		// Выход из экспресс-настройки
		case KEY_ESCAPE:
						p->Express.State = 0; 
						p->State = MS_START;
						p->Update = true;
						break;
		}


	if (ExpNextStateRdy)
	{
		if (IsMemParReady())		// Необходимо подождать, когда запишет параметр, прежде, чем переходить к другому
		{
			ExpNextStateRdy = false;
			p->Update = true;

			if (++p->Express.Index >= p->Express.List->Cnt) 
			{
				if (p->Express.State < EXPRESS_MAX_STATE) p->Express.State = NextState;
				else {p->Express.State = 0; p->State = MS_START;}
				p->Express.Index = 0;
			}
		}
	}
}

__inline void EditDec(MENU *p, Char Key)
{
	struct MENU_VAL_CFG *Val = &p->Dcr.Config.Val;
	Char  i, *pBuffer, SignChange;
	Uns   Step;
	LgInt Data;
	
	Step = 1;	
	for (i=0; i < p->Rate; i++) Step = Step * 10;

	pBuffer = &p->BufValue[5 - p->Rate];
	SignChange = (Val->Signed && (p->Rate == p->EditRate));
	
	switch (Key)
	{
	   case KEY_UP:
		   if (!p->EditType)
		   {
			   if (Val->Signed)
			   {
				   Data = (LgInt)((Int)p->Value) + (LgInt)Step;
				   if (Data > (LgInt)((Int)p->Dcr.Max)) p->Value = p->Dcr.Max;
				   else p->Value = p->Value + Step;
			   }
			   else
			   {
				   if (Step > (p->Dcr.Max - p->Value)) p->Value = p->Dcr.Max;
				   else p->Value = p->Value + Step;
			   }
			   return;
		   }
		   else if (!SignChange)
		   {
			   if (*pBuffer < '9') *pBuffer = *pBuffer + 1;
			   else *pBuffer = '0';
		   }
		   break;
	   case KEY_DOWN:
		   if (!p->EditType)
		   {
			   if (Val->Signed)
			   {
				   Data = (LgInt)((Int)p->Value) - (LgInt)Step;
				   if (Data < (LgInt)((Int)p->Dcr.Min)) p->Value = p->Dcr.Min;
				   else p->Value = p->Value - Step;
			   }
			   else
			   {
				   if (Step > (p->Value - p->Dcr.Min)) p->Value = p->Dcr.Min;
				   else p->Value = p->Value - Step;
			   }
			   return;
		   }
		   else if (!SignChange)
		   {
			   if (*pBuffer > '0') *pBuffer = *pBuffer - 1;
			   else *pBuffer = '9';
		   }
		   break;
	   default:
		   return;
	}
	
	pBuffer = &p->BufValue[4 - Val->RateMax];
	if (SignChange)
	{
		if (*pBuffer == '-') *pBuffer = '+';
		else *pBuffer = '-';
	}
	p->Value = StrToDec(pBuffer + 1, Val->RateMax);
	if (*pBuffer == '-') p->Value = -p->Value;
}

__inline void EditStr(MENU *p, Char Key)
{
	Uns  Value, Count, RsvdLen, MaxLen;
	Char Tmp, *BufAddr, *BufPtr;
	Uns Position = p->Param.Position + p->Param.Addr;
	
	Value   = p->Value;
	Count   = p->Dcr.Max - p->Dcr.Min + 1;
	RsvdLen = strlen(p->Rsvd.Str);
	MaxLen  = 16 - RsvdLen;
	
	while(1)
	{
		if (Count > 0) Count--; else break;
		
		switch (Key)
		{
	   	case KEY_UP:   if (Value < p->Dcr.Max) Value++; else Value = p->Dcr.Min; break;
		   case KEY_DOWN: if (Value > p->Dcr.Min) Value--; else Value = p->Dcr.Max; break;
		}

		#if BUR_M	// Заплатка для БУР М: при редактировании параметра "В17. Режим МУ/ДУ" пролистываем значения "выбор режима" и "только МУ"
			if (Position == REG_MUDUSETUP)
			{
				if (Value == mdSelect || Value == mdMuOnly) Value = mdDuOnly;
			}
		#endif

		PFUNC_blkRead((Int *)p->Values[p->Dcr.Config.Str.Addr + Value].Str, (Int *)p->BufTmp, 16);
		if (p->ShowReserved) break;
		
		BufAddr = p->BufTmp;
		BufAddr[16] = '\0';
		while (*BufAddr) {if (*BufAddr != ' ') break; BufAddr++;}
		if ((Uns)(BufAddr - p->BufTmp) > MaxLen) break;
     
		BufPtr = &BufAddr[RsvdLen];
		Tmp = *BufPtr; *BufPtr = '\0';
		if (strcmp(p->Rsvd.Str, BufAddr)) {*BufPtr = Tmp; break;}
	}
  
	p->Value = Value;
}

__inline void EditTime(MENU *p, Char Key)
{
	TTimeVar *Time = (TTimeVar *)&p->Value;
	
	switch (Key)
	{
	   case KEY_UP:
		   switch (p->Rate)
		   {
		      case 0: if (Time->bit.Min  < 59) Time->bit.Min++;  else Time->bit.Min  = 0;  break;
		      case 1: if (Time->bit.Hour < 23) Time->bit.Hour++; else Time->bit.Hour = 0;  break;
		   }
		   break;
	   case KEY_DOWN:
		   switch (p->Rate)
		   {
		      case 0: if (Time->bit.Min  > 0)  Time->bit.Min--;  else Time->bit.Min  = 59; break;
		      case 1: if (Time->bit.Hour > 0)  Time->bit.Hour--; else Time->bit.Hour = 23; break;
		   }
		   break;
	}
}

__inline void EditDate(MENU *p, Char Key)
{
	TDateVar *Date = (TDateVar *)&p->Value;
	
	switch (Key)
	{
	   case KEY_UP:
		   switch (p->Rate)
		   {
		      case 0: if (Date->bit.Year  < 99) Date->bit.Year++;  else Date->bit.Year  = 0; break;
		      case 1: if (Date->bit.Month < 12) Date->bit.Month++; else Date->bit.Month = 1; break;
		      case 2:
			      if (Date->bit.Day < RTC_DefMaxDay(Date->bit.Year, Date->bit.Month)) Date->bit.Day++;
			      else Date->bit.Day = 1;
			      break;
		   }
		   break;
	   case KEY_DOWN:
		   switch (p->Rate)
		   {
		      case 0: if (Date->bit.Year  > 0) Date->bit.Year--;  else Date->bit.Year  = 99; break;
		      case 1: if (Date->bit.Month > 1) Date->bit.Month--; else Date->bit.Month = 12; break;
		      case 2:
			      if (Date->bit.Day > 1) Date->bit.Day--;
			      else Date->bit.Day = RTC_DefMaxDay(Date->bit.Year, Date->bit.Month);
			      break;
		   }
		   break;
	}
}

__inline void EditBin(MENU *p)
{
	Uns Step = 1U << p->Rate;

	if (p->Value & Step) p->Value &= ~Step;
	else p->Value |= Step;
}

static void ReadDcrFull(MENU *p, Uns Addr)
{
	struct MENU_VAL_CFG *Val = &p->Dcr.Config.Val;
	
	MENU_ReadDcr(p, &p->Dcr, Addr);

	p->Bit = 0xFF;

	p->EditRate = Val->RateMax;
	if (p->EditType && (Val->Type == MT_DEC) && Val->Signed) p->EditRate++;
}

static void ChangeCoordinate(MENU *p, Char Key, Bool GroupFlag)
{
	struct MENU_CRD *Crd;
	Uns Count, RsvdLen, MaxLen;
	Char *Addr, *GrAddr, Name[17];

	if (GroupFlag)
	{
		if (p->EvLogSelected) {Crd = &p->EvLog; Addr = (Char *)p->Groups;}
		else if (p->Level == 1) {Crd = &p->Group; Addr = (Char *)p->Groups;}
		else {Crd = &p->SubGroup; Addr = (Char *)p->SubGroups;}
	}
	else
	{
		Crd = &p->Param;
		Count = p->Param.Count;
		RsvdLen = strlen(p->Rsvd.Str);
		MaxLen = 16 - RsvdLen;
	}
	
	while(1)
	{
		switch (Key)
		{
		   case KEY_UP:
			   if (Crd->Position < Crd->Count-1) Crd->Position++;
			   else Crd->Position = 0;
			   break;
		   case KEY_DOWN:
			   if (Crd->Position > 0) Crd->Position--;
			   else Crd->Position = Crd->Count-1;
			   break;
		}
		
		if (GroupFlag)
		{
			if (p->EvLogSelected) GrAddr = Addr + p->Group.Position * sizeof(struct MENU_GROUP);
			else GrAddr = Addr + Crd->Position * sizeof(struct MENU_GROUP);
			Count = PFUNC_wordRead((Int *)&((struct MENU_GROUP *)GrAddr)->Count);
			if (Count || p->ShowReserved) break;
		}
		else
		{
			if (p->ShowReserved) break;
			if (Count > 0) Count--; else break;
			PFUNC_blkRead((Int *)p->Params[Crd->Position + Crd->Addr].Name, (Int *)Name, 16);
			Addr = strchr(Name, (Int)'.');
			if (!Addr) break;
			Addr++;
			if ((Int)(Addr - Name) > MaxLen) break;
			Addr[RsvdLen] = '\0';
			if (strcmp(p->Rsvd.Str, Addr) && (PI_CalibEnable || (Crd->Position + Crd->Addr) != REG_TASK_PI_CALIB )) break;
			continue;
		}
	}
	
	if (GroupFlag)
	{
		Crd = (p->Level == 2) ? &p->SubGroup : &p->Param;
		Crd->Addr  = PFUNC_wordRead((Int *)&((struct MENU_GROUP *)GrAddr)->Addr);
		Crd->Count = Count;
	}
	else ReadDcrFull(p, p->Param.Position + p->Param.Addr);
}

__inline void ShowStart(MENU *p)
{
#if !IR_IMP_TEST
	Uns Position;
#endif
	
	memset(p->HiString, ' ', 16);
	
#if !IR_IMP_TEST
	Position = p->Data[p->StartAddr] + p->StartOffset;
	if (p->Update) ReadDcrFull(p, Position);
	ShowValue(p, Position, FALSE);
#endif

	p->StartDispl(p->HiString);
}

static void ShowGroup(MENU *p)
{
	const struct MENU_GROUP *GroupTbl;
	Char *Str;
	
	if (p->Update)
	{
		if (p->EvLogSelected)
		{
			PFUNC_blkRead((Int *)p->Values[p->EvLog.Addr].Str, (Int *)p->HiString, 16);
			Str = &p->HiString[15]; while (*(Str-6) == ' ') Str--;
			DecToStr(p->EvLog.Position + 1, Str, 0, 4, FALSE, FALSE);
			TimeToStr((TTimeVar *)&p->Data[p->EvLogTime], (String)&p->LoString[1]);
			DateToStr((TDateVar *)&p->Data[p->EvLogDate], (String)&p->LoString[7]);
		}
		else
		{
			if (p->Level == 1) GroupTbl = &p->Groups[p->Group.Position];
			if (p->Level == 2) GroupTbl = &p->SubGroups[p->SubGroup.Position + p->SubGroup.Addr];
			PFUNC_blkRead((Int *)GroupTbl->HiString, (Int *)p->HiString, 16);
			PFUNC_blkRead((Int *)GroupTbl->LoString, (Int *)p->LoString, 16);

			if (p->Group.Position == 2)
			  {
			    if (IsPassword2()) Menu.HiString[15] = CODEON_ICO;
			    else Menu.HiString[15] = CODEOFF_ICO;
			  }
                        if (p->Group.Position == 1)
                          {
                            if (IsPassword1()) Menu.HiString[15] = CODEON_ICO;
                            else Menu.HiString[15] = CODEOFF_ICO;
                          }

                        if (p->Group.Position == 3)
                          {
                            if (IsPassword1()) Menu.HiString[15] = CODEON_ICO;
                            else Menu.HiString[15] = CODEOFF_ICO;
                          }
		}
	}
}

__inline Byte DefineBit(MENU *p, Uns Value)
{
	if (!Value) {p->Bit = 0xFF; return 0;}
	
	if (p->Bit == 0xFF) p->BitTimer = 0;
	
	if (p->BitTimer > 0) p->BitTimer--;
	else
	{
		do {if (++p->Bit >= 15) p->Bit = 0;}
		while (!(Value & (1 << p->Bit)));
		p->BitTimer = p->BitTime;
	}
	
	return (p->Bit + 1);
}

static void ShowValue(MENU *p, Uns Position, Bool Edit)
{
	struct MENU_VAL_CFG *Val = &p->Dcr.Config.Val;
	Char i, *Str;
	Uns  Addr, Timeout;
	
	if (Edit)
	{
		Timeout = !p->Blink ? p->BlinkHiTime : p->BlinkLoTime;
		if (++p->BlinkTimer >= Timeout)
		{
			p->Blink = !p->Blink;
			p->BlinkTimer = 0;
		}
	}
	else
	{
//----- Экспресс настройка -------
		if ((p->State == MS_EXPRESS) && ((p->Express.State == 5) || (p->Express.State == 7)))
				{	p->Value = 1;	}
		else	p->Value = p->Data[Position];
//-----------------
		p->Blink = FALSE;
		p->BlinkTimer = 0;
	}
	
	memset(p->LoString, ' ', 16);
	
	switch (Val->Type)
	{
	   case MT_VERS:
		   Str = &p->LoString[10];
		   
		   if (!Edit)
		   {
			   DecToStr(p->Value, Str, Val->Precision, Val->RateMax, TRUE, Val->Signed);
				p->LoString[4] = p->LoString[5];
				p->LoString[5] = p->LoString[6];
				p->LoString[6] = '.';
		   }

	   break;
	   case MT_DEC:
		   Str = &p->LoString[8];
		   
		   if (!p->EditType)
		   {
			   if ((Position == REG_CODE || Position == REG_FCODE) && Edit)
			   {
					DecToStr(p->Value, Str, Val->Precision, Val->RateMax, !Edit, Val->Signed);
					for (i = 0; i < 5; i++)
					{
						if (i != p->Rate) p->LoString[8-i] = '*';
					}
			   }
			   else
				   DecToStr(p->Value, Str, Val->Precision, Val->RateMax, !Edit, Val->Signed);
		   }
		   else if (!Edit)
		   {
			   DecToStr(p->Value, Str, Val->Precision, Val->RateMax, TRUE, Val->Signed);
			   if (Val->Signed) FloatPositive = '+';
			   DecToStr(p->Value, &p->BufValue[5], 0, Val->RateMax, FALSE, Val->Signed);
			   FloatPositive = ' ';
		   }
		   else for (i=0; i <= p->EditRate; i++)
        {
				if (Val->Precision && (i == Val->Precision)) *Str-- = FloatSeparator;
				*Str-- = p->BufValue[5 - i];
		   }
		   
		   if (p->Blink)
		   {
			   Str = &p->LoString[8 - p->Rate];
			   if (Val->Precision && (p->Rate >= Val->Precision)) Str--;
			   *Str = ' ';
		   }
		   
		   if (p->Update) PFUNC_blkRead((Int *)p->Params[Position].Unit, (Int *)p->BufTmp, 4);
		   else memcpy(&p->LoString[10], p->BufTmp, 4);
		   break;
	   
	   case MT_STR:
		   if (p->Blink) break;
			if (!Edit)
			{
				Addr = p->Dcr.Config.Str.Addr + p->Value;
				PFUNC_blkRead((Int *)p->Values[Addr].Str, (Int *)p->BufTmp, 16);
			}
			memcpy(p->LoString, p->BufTmp, 16);
		   break;
	   
	   case MT_RUN:
		   if (p->Blink) break;
		   Addr = p->Dcr.Config.Str.Addr + DefineBit(p, p->Value);
		   PFUNC_blkRead((Int *)p->Values[Addr].Str, (Int *)p->LoString, 16);
		   break;
	   
	   case MT_TIME:
		   TimeToStr((TTimeVar *)&p->Value, (String)&p->LoString[5]);
		   if (p->Blink)
		   {
			   switch (p->Rate)
			   {
			      case 0: p->LoString[8] = ' '; p->LoString[9] = ' '; break;
			      case 1: p->LoString[5] = ' '; p->LoString[6] = ' '; break;
			   }
		   }
		   break;
	   	   
	   case MT_DATE:
		   DateToStr((TDateVar *)&p->Value, (String)&p->LoString[4]);
		   if (p->Blink)
		   {
			   switch (p->Rate)
			   {
			      case 0: p->LoString[10] = ' '; p->LoString[11] = ' '; break;
			      case 1: p->LoString[7]  = ' '; p->LoString[8]  = ' '; break;
			      case 2: p->LoString[4]  = ' '; p->LoString[5]  = ' '; break;
			   }
		   }
		   break;
	   
	   case MT_BIN:
		   Str = &p->LoString[9 + Val->RateMax / 2];
		   for (i=0; i <= (Char)Val->RateMax; i++)
		   {
			   Str--;
			   if (p->Blink && (i == p->Rate)) continue;
			   *Str = (p->Value & (1 << i)) ? '1' : '0';
		   }
		   break;
	}
}

static void ShowParam(MENU *p, Bool Edit)
{
	Uns Position = p->Param.Position + p->Param.Addr;
	
	if (p->Update) PFUNC_blkRead((Int *)p->Params[Position].Name, (Int *)p->HiString, 16);
	
	if (GrH->extraPassword) strcpy(p->HiString," ПОВТОРИТЕ КОД  ");

	ShowValue(p, Position, Edit);
}

__inline void ShowExpress(MENU *p)
{
	Uns Addr;

	p->Express.List = 0;
	
	if (p->Express.Enable)							// Включено сообщение о выборе экспресс настройки
	{
		p->GetExpressText(0);						// Выводим первое сообщение
		ShowExpressChoice(p, 3,6,7,11, false);		// Выводим скобочки текущего выбора
		return;
	}

	if (ExpNextStateRdy)	return;

	switch(p->Express.State)							// Шаги экспресс настройки
	{
		case 0: p->Express.List = &p->Express.List1; 								break;
		case 1:	p->GetExpressText(1);	ShowExpressChoice(p, 11,14, 0, 0, true);	break;
		case 2: p->Express.List = &p->Express.List2;								break;
		case 3:	p->GetExpressText(2);	ShowExpressChoice(p,   0, 5, 6,15, true);	break;
		// Переведите в положение закрыто
		case 4: p->GetExpressText(3);	ShowExpressChoice(p,  10,13, 0, 0, true);	break;
		// Задание закрыто
		case 5: p->Express.List = &p->Express.List3;								break;
		// Переведите в положение открыто
		case 6: p->GetExpressText(4);	ShowExpressChoice(p,  10, 13, 0, 0, true);	break;
		// Задание открыто
		case 7: p->Express.List = &p->Express.List4;								break;
		// Переведите в крайнее положение
		case 8: p->GetExpressText(5);	ShowExpressChoice(p,  11, 14, 0, 0, true);	break;
		// Положение открыто или закрыто
		case 9: p->GetExpressText(6);	ShowExpressChoice(p,   2,  7, 8,13, true);	break;
		// Обороты на открытие
		case 10: p->Express.List = &p->Express.List5;								break;
		// Обороты на закрытие
		case 11: p->Express.List = &p->Express.List6;								break;
		// Проверить калибровку
		case 12: p->GetExpressText(7);	ShowExpressChoice(p,   3,  6, 7,11, true);	break;
		// Проверяем текущее положение: открыто или закрыто
		case 13:																	break;
		// Пустите в открыто
		case 14: p->GetExpressText(8);												break;
		// Выводим положение в оборотах
		case 15: p->GetExpressText(9);												break;
		// Остановились по "Стоп". Программно нажимаем "Ввод" для перехода к следующему состоянию
		case 16: 																	break;
		// Сбросить калибровку?
		case 17: p->GetExpressText(10);	ShowExpressChoice(p,   3,  6, 7,11, false);	break;
		// Сброс калибровки и возврат к выбору типа калибровки
		case 18:																	break;
		// Доехали до "Открыто"
		case 19:																	break;
	//---------------------------------------------------------------------------
		// Пустите в закрыто
		case 20: p->GetExpressText(11);												break;
		// Выводим положение в оборотах
		case 21: p->GetExpressText(9);												break;
		// Остановились по "Стоп". Программно нажимаем "Ввод" для перехода к следующему состоянию
		case 22: 																	break;
		// Сбросить калибровку?
		case 23: p->GetExpressText(10);	ShowExpressChoice(p,   3,  6, 7,11, true);	break;
		// Сброс калибровки и возврат к выбору типа калибровки
		case 24:																	break;
		// Доехали до "Открыто"
		case 25:																	break;
		// Вас устраивает калибровка?
		case 26: p->GetExpressText(12);	ShowExpressChoice(p,   3,  6, 7,11, true);	break;
		// Задать рабочие моменты
		case 27: p->GetExpressText(13);	ShowExpressChoice(p,   11,14, 0, 0, true);	break;
		// Задаем моменты
		case 28: p->Express.List = &p->Express.List2;				 				break;
		// Момент уплотнения только для клиновой 
		case 29: p->Express.List = &p->Express.List7;								break;
		// Задаем момент
		case 30: p->Express.List = &p->Express.List8;								break;
		// Момент уплотнения только для клиновой 
		case 31: p->Express.List = &p->Express.List9;								break;
		// Задаем момент
		case 32: p->Express.List = &p->Express.List10;							 	break;
		// Выбираем тип входного сигнала только для БУР-Т
		case 33: p->Express.List = &p->Express.List11;								break;
		// Выбор режима МУ/ДУ
		case 34: p->Express.List = &p->Express.List12;								break;
	}

	
	if (p->Express.List)																	// Если выбран List с адресами параметров
	{
		Addr = p->Express.List->Buf[p->Express.Index];										// Формируем адрес текущего параметра
		PFUNC_blkRead((Int *)p->Params[Addr].Name, (Int *)p->HiString, 16);		// Считываем название текущего параметра
		if (p->Update) 
		{
			ReadDcrFull(p, Addr);									// Когда флаг update, то считываем описание параметра
			if ((p->Express.State == 5) || (p->Express.State == 7))
				{	p->Dcr.Min = 1;	p->Dcr.Max = 1;	}
		}
		if (p->Express.First)												// Выводим значение параметра на дисплей
			{	
				p->Rate = 0;	
				ShowValue(p, Addr, FALSE);	p->Express.First = false;
			}		
		else	ShowValue(p, Addr, TRUE);									// Выводим редактирование на дисплей
	}
}



__inline void ShowExpressChoice(MENU *p, Byte YesOne, Byte YesTwo, 
									Byte NoOne, Byte NoTwo, Bool Yes)
{
	Uns Timeout;

	Timeout = !p->Blink ? p->BlinkHiTime : p->BlinkLoTime;
	if (++p->BlinkTimer >= Timeout)
	{
		p->Blink = !p->Blink;
		p->BlinkTimer = 0;
	}

	if (p->Blink)	return;

	if (p->Express.First)
	{
		p->Express.Select = Yes;
		p->Express.First = false;
	}

	if (p->Express.Select)				// Выводим скобочки, обозначающие текущий выбор на индикаторе
	{
		p->LoString[YesOne] = '[';
		p->LoString[YesTwo] = ']';
	}
	else
	{
		if ((!NoOne) && (!NoTwo))
		{	p->Express.Select = true;	return;	}
		
			p->LoString[NoOne] = '[';
			p->LoString[NoTwo] = ']';
	}
}
