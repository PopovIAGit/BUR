#ifndef _MACRO_H_
#define _MACRO_H_

// Чтение всех параметров
#define ReadAllParams() 			ReadPar(RAM_ADR, &Ram, RAM_DATA_SIZE)
// Запись всех параметров
#define WriteAllParams()			WritePar(RAM_ADR, &Ram, RAM_DATA_SIZE)

// Макрос определения процента обновления данных в памяти
#define GetSetParPercent(A)			_IQrmpy(A, _IQ(50.0/RAM_DATA_SIZE))
#define GetWriteParPercent()		(100 - _IQrmpy(MemPar.Count, _IQ(25.0/RAM_DATA_SIZE)))
#define GetWriteLogPercent()		(100 - _IQrmpy(MemLog.Count, _IQ(50.0/LOG_EV_SIZE)))

// Адрес элемента в структуре параметров
#define GetAdr(Elem)				((LgUns)&(((TDriveData *)RAM_ADR)->Elem))
// Забираем "описание" параметра из меню
#define GetDcr(Adr, pDcr)			MENU_ReadDcr(&Menu, pDcr, Adr)

// Макросы вывода строк на двухстрочеый индикатор
#define PutAddHiStr(Addr)			DISPL_PutHiStr(&Display, ToPtr(&AddStr[Addr]))
#define PutAddLoStr(Addr)			DISPL_PutLoStr(&Display, ToPtr(&AddStr[Addr]))

// Макросы для работы со статусным регистром
#define IsStopped()					(GrA->Status.bit.Stop)
#define IsFaulted()					(GrA->Status.bit.Fault)
#define IsDefected()				(GrA->Status.bit.Defect)
#define IsClosing()					(GrA->Status.bit.Closing)
#define IsOpening()					(GrA->Status.bit.Opening)
#define IsClosed()					(GrA->Status.bit.Closed)
#define IsOpened()					(GrA->Status.bit.Opened)
#define IsProgramming()				(Menu.State > 0)
#define IsTested()					(GrA->Status.bit.Test)
#define IsMuffActive()				(GrA->Status.bit.Mufta)
#define IsMVOactive()				(GrA->Status.bit.Mufta & (lastDirection > 0))
#define IsMVZactive()				(GrA->Status.bit.Mufta & (lastDirection < 0))
#define IsLocalControl()			(GrA->Status.bit.MuDu)
#define IsBlockFault()				(GrA->Status.bit.BlkIndic)
#define IsTsFault()					(GrA->Status.bit.TsIndic)
#define IsTsDefect()				(GrA->Status.bit.TsDefect)
#define IsTenEnable()				(GrA->Status.bit.Ten)
#define IsSleeping()				(Menu.SleepActive)
#define IsBlockDefect()				(GrA->Status.bit.BlkDefect)
#define IsParamEditing()			(Menu.State == MS_EDITPAR)


#define IsPowerOn()					(((GrA->Ur > 100) && (GrA->Ur <400)) || ((GrA->Us > 100) && (GrA->Us <400)) || ((GrA->Ut > 100) && (GrA->Ut <400)))  //??? 
#define IsPowerOff()					((GrA->Ur < 100) && (GrA->Us <100) && (GrA->Ut <100)) //??? 
#define IsPowerLost()				((GrA->Ur < 100) || (GrA->Us <100) || (GrA->Ut <100))

// Макросы для определения состояния калибровки
#define IsNoCalib()					(GrH->CalibState != csCalib)
#define IsNoClosePos()				(!(GrH->CalibState & csClose))
#define IsNoOpenPos()				(!(GrH->CalibState & csOpen))

// Макросы преобразования токов и напряжений
#define I_ABS_CALC(A)				_IQ5mpy(A, 10)
#define I_REL_CALC(A)				_IQ5div(A, 10)
#define I_RMS_CALC(A)				_IQ5rmpy(A, 10)

// Макрос алгоритма работы защиты от КЗ
/*
#define SHC_ALG(PHASE) \
	Value = abs(I##PHASE##.Input); \
	if (Value < Level) \
		{if (Timer##PHASE## > 0) Timer##PHASE##--;} \
	else if (Timer##PHASE## < GrC->ShCTime) Timer##PHASE##++; \
	else GrH->FaultsLoad.bit.ShC##PHASE## = 1
*/
#define SHC_ALG(PHASE) \
	Value = ACP##PHASE##; \
	if ((Value < Level)||(Value > Level2)) \
		{if (Timer##PHASE## > 0) Timer##PHASE##--;} \
	else if (Timer##PHASE## < GrC->ShCTime) Timer##PHASE##++; \
	else GrH->FaultsLoad.bit.ShC##PHASE## = 1


// Макрос добавления Uns числа в буфер считывания информационного модуля
#define PutUnsToLogBuf(Variable)	\
	Buffer[CurrentIndex++] = Variable >> 8;		\
	Buffer[CurrentIndex++] = Variable & 0xFF	\

// Макросы управления состоянием ножки выбора микросхемы
#define SET_CS()						SPI_STE = 0
#define CLR_CS()						SPI_STE = 1

// Макрос нахождения в тестовом режиме
#define IsTestMode()				(GrG->Mode)

// Макрос перевода положения для индикации
//#define IndicPos(Position)		((Position) >> GrC->PosPrecision)

// Проверка наличия кода доступа
#define IsPassword1()				(GrH->Password1 != 0)
#define IsPassword2()				(GrH->Password2 != 0)

#endif
