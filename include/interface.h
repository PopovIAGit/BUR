#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "LogEv.h"
#include "menu2x.h"			// Меню для двухстрочного индикатора
#include "value_conv.h"		// Библиотека для преоборазования чисел/строк
#include "rtc.h"
#include "VlvDrvCtrl.h"		// Алгоритмы управления приводом
#include "Calibs.h"			// Библиотека калибровок
#include "im.h"				// Информационный модуль

#define LED_ACTIVE			1

// Группа A (Адрес = 0, Количество = 40) - Диагностика - просмотр
typedef struct _TGroupA
{
	TStatusReg      Status;           	// 0.Статус работы
	TFltUnion	    Faults;				   // 1-4.Аварии
	TInputReg       Inputs;           	// 5.Состояние дискретных входов
	TOutputReg      Outputs;          	// 6.Состояние дискретных выходов
	Uns             Position;        	// 7.Положение
	Uns             Torque;           	// 8.Момент
	Int             Speed;				// 9.Скорость
	Uns             Ur;                 // 10.Напряжение фазы R
  	Uns             Us;                 // 11.Напряжение фазы S
 	Uns             Ut;                 // 12.Напряжение фазы T
	Uns             Iu;                 // 13.Ток фазы U
	Uns             Iv;                 // 14.Ток фазы V
	Uns             Iw;                 // 15.Ток фазы W
	Int             AngleUI;            // 16.Угол нагрузки
	TCalibState     CalibState;         // 17.Статус калибровки
	Int             PositionPr;         // 18.Положение %
	Uns             FullWay;            // 19.Полный ход
	Int             CurWay;             // 20.Текущий ход
	Uns             CycleCnt;           // 21.Счетчик циклов
	Int             Temper;             // 22.Температура блока
	Uns             MkuPoVersion;       // 23.Версия ПО
	Uns Rsvd[16];
} TGroupA;

// Группа B (Адрес = 40, Количество = 50) - Параметры пользователя
typedef struct _TGroupB
{
	Uns				MoveCloseTorque;	// 0.Момент закрытия
	Uns				MoveOpenTorque;		// 1.Момент открытия
	Uns				BreakCloseTorque;	// 2.Момент уплотнения на закрытие
	Uns				StartCloseTorque; 	// 3.Момент трогания на закрытие
	Uns				BreakOpenTorque;  	// 4.Момент уплотнения на открытие
	Uns				StartOpenTorque;  	// 5.Момент трогания на открытие
	TValveType		ValveType;			// 6.Тип задвижки (клин/шибер)
	TBreakMode      BreakMode;		   	// 7.Тип уплотнения
	TRodType        RodType;            // 8.Тип штока
	Uns				CloseZone;		 	// 9.Зона закрыто
	Uns				OpenZone;		 	// 10.Зона открыто
	Uns             PositionAcc;        // 11.Зона смещения
	Uns             MainCode;        	// 12.Код доступа	
	TTimeVar 		DevTime;			// 13.Время
	TDateVar 		DevDate;			// 14.Дата
	Int		 		TimeCorrection;		// 15.Корректеровка времени
	TIndicMode      IIndicMode;         // 16.Режим индикации тока
	TMuDuSetup      MuDuSetup;          // 17.Настройка режима МУ/ДУ
	TDuSource		DuSource;			// 18.Источник команд ДУ
	Uns             TuLockSeal;         // 19.Блокировка залипани
	Uns             TuTime;             // 20.Время команды
	#if BUR_M
	Uns 			Rsvd1[2];
	#else
	TInputType 		InputType;			// 21.Тип входного сигнала 24/220
	TInputMask	    InputMask;			// 22.Маска дискретных входов
	#endif
	TOutputMask 	OutputMask;			// 23.Маска дискретных выходов
	TBaudRate       RsBaudRate;         // 24.Скорость связи
	Uns             RsStation;          // 25.Адрес станции
	TParityMode		RsMode;				// 26.Режим связи
	TSettingPlace	SettingPlace;		// 27.Место установки БУР
	Uns 			Rsvd2[4];			// 28-31. 
	Uns				NoMoveTime;		   	// 32.Время отсутствия движения
	Uns				OverwayZone;		// 33.Макси
	Uns 			Rsvd3;				// 34.
 	Uns             SleepTime;          // 35.Дежурный режим
	Uns 			Rsvd[14];
} TGroupB;

// Группа C (Адрес = 90, Количество = 120) - Заводские параметры
typedef struct _TGroupC
{
	Uns             FactCode;           // 0.Код доступа
	TDriveType      DriveType;          // 1.Тип привода
    Uns        		ProductYear;        // 2.Год изготовления блока
  	Uns             FactoryNumber;      // 3.Заводской номер блока
  	Uns             MaxTorque;          // 4.Максимальный момент привода
	Uns             Inom;               // 5.Номинальный ток
  	Uns             GearRatio;          // 6.Передаточное число редуктора
	Int             BlockToGearRatio;   // 7.Коэффициент передачи блок-редуктор коэффициент скорости
	Uns             PosPrecision;       // 8.Точность положения
	Uns             PhOrdZone;      	// 9.Расстояние чередования фаз двигателя
	Uns             MuffZone;           // 10.Расстояние сброса муфты 
	Uns			    PosSensPow;			// 11.Тип датчика положения
	TPrtMode        CalibIndic;         // 12.Индикация калибровки
	Uns			    DisplResTout;		// 13.Время сброса индикатора
	// индикация
	Uns             OpenAngle;          // 14.Угол управления СИФУ
	Int             AngleUI;            // 15.Угол нагрузки
	TLedsReg        LedsReg;            // 16.Состояние светодиодов блока
	THallBlock      HallBlock;          // 17.Состояние датчиков холла блока
	Uns             SetDefaults;        // 18.Задание параметров по умолчанию
#if BUR_M
	TPrtMode       	RST_Err;           	// 19.Неверное чередование фаз сети для БУР М
#else
	Uns       	    Rsvd1;           	// 19.Резерв
#endif
	TPrtMode        DriveTemper;        // 20.Защита от перегрева двигателя. (дописать защиту перегрева блока в H)
	Uns             OvLevel_max;        // 21.Уровень превышения напряжения при 47% превышения (320В)
	Uns             OvTime_max;     	// 22.Время превышения напряжения при 47% превышения (1с)
	Uns             RevErrValue;       	// 23.Количество сбоев датчика положения
	Uns			    RevErrLevel;		// 24.Уровень сбоя датчика положения
	//-индикация конец--
	Uns             VoltAcc;            // 25.Интенсивность разгона
	Uns             BrakeAngle;         // 26.Угол торможения
	Uns             BrakeTime;          // 27.Время торможения
	Uns				KickCount;			// 28.Колличество ударов
	Uns             OvLevel;    		// 29.Уровень превышения напряжения
	Uns             OvTime;     		// 30.Время превышения напряжения
	Uns             UvLevel;   			// 31.Уровень понижения напряжения
	Uns             UvTime;    			// 32.Время понижения напряжения
	Uns             VSkLevel;          	// 33.Уровень асиметрии напряжения
	Uns             VSkTime;           	// 34.Время асиметрии напряжения
	Uns             BvLevel;   			// 35.Уровень обрыва входных фаз
	Uns             BvTime;    			// 36.Время обрыва входных фаз
	TPrtMode        Phl;           		// 37.Защита от обрыва фаз нагрузки
	Uns             PhlLevel;      		// 38.Уровень обрыва фаз
	Uns             PhlTime;       		// 39.Время определения обрыва фаз
	TPrtMode        I2t;                // 40.Время-токовая защита
	Uns             I2tMinCurrent;      // 41.Уровень минимального тока
	Uns             I2tMidCurrent;      // 42.Уровень среднего тока
	Uns             I2tMaxCurrent;      // 43.Уровень максимального тока
	Uns             I2tMinTime;         // 44.Время на минимальном токе
	Uns             I2tMidTime;         // 45.Время на среднем токе
	Uns             I2tMaxTime;         // 46.Время на максимальном токе
	Uns				PhOrdTime;			// 47.Время чередования фаз двигателя
	Uns				DrvTLevel;			// 48. Уровень срабатывания защиты по перегреву двигателя	
	Uns				DrvTInput;			// 49.
	TPrtMode        Ov;					// 50.Защита от превышения напряжения
	TPrtMode        Uv;					// 51.Защита от превышения напряжения
	TPrtMode        VSk;               	// 52.Защита от асиметрии напряжения
	TPrtMode        Bv;        			// 53.Защита от обрыва входных фаз
	TPrtMode        PhOrd;          	// 54.Защита от неверного чередования фаз двигателя	
	Uns             OvDTime;     		// 55.Время превышения напряжения
	Uns             UvDTime;     		// 56.Время превышения напряжения
	Uns             MkuPoSubVersion;    // 24.Подверсия ПО
	TPrtMode        IUnLoad;            // 58.Защита от малой нагрузки
	Uns             IUnLevel;           // 59.Ток малой нагрузки
	Uns             IUnTime;            // 60.Время малой нагрузки
	TPrtMode        ISkew;              // 61.Защита от асиметрии тока
	Uns             ISkewLevel;         // 62.Уровень асиметрии тока
	Uns             ISkewTime;          // 63.Время асиметрии тока
	TPrtMode        ShC;        		// 64.Защита от короткого замыкания
	Uns				SelectDynBr;		// 65.Выбор динамического торможения (старого образца или нового образца)
	Uns				IndicatorType;		// 66.Тип индикатора
	Uns				RTCErrOff;			// 67 Отключение индикации ошибки часов реального времени
	TPrtMode        ErrIndic;           // 68.Индикация аварий устройства
	TPrtMode        TemperTrack;        // 69.Защита от перегрева/переохлаждения блока
	Int             TemperHigh;         // 70.Уровень перегрева блока
	Int             TemperLow;          // 71.Уровень переохлаждения блока
	Int             TenOnValue;         // 72.Уровень включения ТЕНа
	Int             TenOffValue;        // 73.Уровень выключения ТЕНа
	Int             CorrTemper;         // 74.Корректировка температуры блока
	TPrtMode        MuDuDef;        	// 75.Защита ошибки входов Му/Ду
	Int				IU_Input;			// 76.параметр для подстройки оффсетов тока IU
	Int				IV_Input;			// 77.параметр для подстройки оффсетов тока IV
	Int				IW_Input;			// 78.параметр для подстройки оффсетов тока IW
	Uns             IU_Mpy;             // 79.Корректировка тока фазы U
	Uns             IV_Mpy;             // 80.Корректировка тока фазы V
	Uns             IW_Mpy;             // 81.Корректировка тока фазы W
	Uns             IU_Offset;          // 82.Смещение тока фазы U
	Uns             IV_Offset;          // 83.Смещение тока фазы V
	Uns             IW_Offset;          // 84.Смещение тока фазы W
	Uns             UR_Mpy;             // 85.Корректировка напряжения фазы R
	Uns             US_Mpy;             // 86.Корректировка напряжения фазы S
	Uns             UT_Mpy;             // 87.Корректировка напряжения фазы T
	Uns             UR_Offset;          // 88.Смещение напряжения фазы R
	Uns             US_Offset;          // 89.Смещение напряжения фазы S
	Uns             UT_Offset;          // 90.Смещение напряжения фазы T
	Uns			    CoefVoltFltr;		// 91.Коэффициент фильтрации входного напряжения
	Uns			    CoefCurrFltr;		// 92.Коэффициент фильтрации тока нагрузки
	Uns			    Dac_Mpy;			// 93.Корректировка ЦАП
	Int			    Dac_Offset;			// 94.Смещение ЦАП
	Uns             ClosePosition;      // 95.Положение закрыто
	Uns             OpenPosition;       // 96.Положение открыто
	TTenMode		Rsvd6[3];			// 97 - 99 Резерв
	Uns				ShC_Up; 			// 100 убрать
	Uns				ShC_Down;			// 101 уброать в H
	Uns             Position;           // 102.Положение
	Uns				LowPowerLvl;		// 103 Значение напряжения в попугаях для 3х секунд
	Int				Upor25;				// 104 Значение для добавлени/убавления значения упора при 220в 25% от максМ
	Int				Upor35;				// 105 Значение для добавлени/убавления значения упора при 220в 35% от максМ
	Int				Upor50;				// 106 Значение для добавлени/убавления значения упора при 220в 50% от максМ
	Int				Upor75;				// 107 Значение для добавлени/убавления значения упора при 220в 75% от максМ
	Int				Upor100;			// 108 Значение для добавлени/убавления значения упора при 220в 100% от максМ
	Int				Corr40Trq;			// 109 Параметр для корректировки индикации малых моментов (меньше 60%)
	Int				Corr60Trq;			// 110 Параметр для корректировки индикации больших моментов (больше 60%)
	Int				Corr80Trq;			// 111
	Int				Corr110Trq;			// 112
	Uns			    Rsvd2[7];			// 113 - 119 Резерв 
} TGroupC;

// Группа D (Адрес = 210, Количество = 20 )  	- Команды
typedef struct _TGroupD
{
	TTaskReset     TaskClose;           // 0.Задание закрыто
	TTaskReset     TaskOpen;            // 1.Задание открыто
	Uns            RevOpen;             // 2.Обороты на открытие
	Uns            RevClose;            // 3.Обороты на закрытие
	Uns            AutoCalib;           // 4.Автоматическая калибровка
	Uns            CalibReset;          // 5.Сброс калибровки
	TValveCmd	   ControlWord;         // 6.Команда управления	
	Uns			   PrtReset;      		// 7.Сброс защит
	Uns            SetDefaults;         // 8.Задание параметров по умолчанию
	Uns Rsvd1;//отчистка журнала событий 
	Uns            CycleReset;			// 10.Сброс счетчика циклов
	Uns			   RsReset;				// 11. Сброс связи
	Uns 		   Rsvd[8];				// 12-19.Резерв
} TGroupD;


// Группа G (Адрес 230, Количество 30)  - Предположительно заводской тест
typedef struct _TGroupG
{
	Uns            Mode;                // 0.Режим теста
	TLedsReg       LedsReg;             // 1.Тест светодиодов блока
	TOutputReg     OutputReg;			// 2.Тест дискретных выходов
	Int			   DacValue;			// 3.Значение ЦАП
	Uns			   DisplShow;			// 4.Старт дисплея
	Uns			   TestCamera;			// 5.Тест с камеры
	Uns			   ThyrGroup;			// 6.Выбор группы тиристоров
	Uns            SifuEnable;          // 7.Разрешение работы СИФУ
	Uns			   Rsvd2[10];			// 8-17.Резерв
	Uns			   ThyrOpenAngle;		// 18.Тестовый угол открытия
	Uns			   DiscrInTest;			// 19. Тест дискретных входов
	Uns			   DiscrOutTest;		// 20. Тест дискретных выходов
	Bool		   IsDiscrTestActive;	// 21. Активен ли тест ТС/ТУ
	Uns 		   Rsvd[8];			    // 22-29.Резерв
} TGroupG;

// Группа H (Адрес 260, Количество 140)
typedef struct _TGroupH
{
	TCalibState     CalibState;          // 0.Состояние калибровки
	Uns             CalibRsvd;           // 1.Резерв для калибровки
	Uns             ClosePosition[2];    // 2-3.Положение закрыто
	Uns             OpenPosition[2];     // 4-5.Положение открыто
										 // Дальше адрес +2
	Uns             Password1;           // 6.Основной пароль
	Uns             Password2;           // 7.Заводской пароль
	Uns             ScFaults;            // 8.Аварии КЗ
	Uns				UporOnly;			 // 9.Параметр включающий работу только на упоре
	Uns             CycleCnt;            // 10.Счетчик циклов
	Uns				Rsvd1[5];			 // 11-15.Резерв
	Uns				TransCurr;			 // 16.Ток перехода
	TCubArray		TqCurr;				 // 17-36.Ток поверхности
	TCubArray		TqAngUI;			 // 37-56.Углы нагрузки
	TCubArray		TqAngSf;			 // 57-76.Углы СИФУ
	Uns				ZazorTime;			 // 77.Время выборки зазора
	TNetReg         FaultsNet;           // 78.Диагностика сети
  	Uns             Ur;                  // 79.Напряжение фазы R
  	Uns             Us;                  // 80.Напряжение фазы S
  	Uns             Ut;                  // 81.Напряжение фазы T
	Uns             Umid;             	 // 82.Среднее напряжение
	Uns             VSkValue;          	 // 83.Асиметрия фаз питающей сети
  	Uns     	    PhOrdValue;        	 // 84.Чередование фаз сети
	TNetReg         DefectsNet;          // 85.Диагностика сети (для неисправностей)
	Uns             Rsvd7;         	 	 // 86. Резерв
	TLoadReg        FaultsLoad;          // 87.Диагностика нагрузки
	Uns             Iu;                  // 88.Ток фазы U
	Uns             Iv;                  // 89.Ток фазы V
	Uns             Iw;                  // 90.Ток фазы W
	Uns             Imid;				 // 91.Средний ток
  	Uns             ISkewValue;          // 92.Асиметрия токов нагрузки
	Uns             Position;            // 93.Положение
	Uns				Rsvd2[2];			 // 94 - 95 Резерв
	TReverseType	ReverseType;         // 96.Тип реверса
	Uns				DemoPosition1;       // 97.Демо положение 1
	Uns				DemoPosition2;       // 98.Демо положение 2
	TInputReg       Inputs;           	 // 99.Состояние дискретных входов
	TOutputReg      Outputs;          	 // 100.Состояние дискретных выходов
	Uns             TuReleMode;          // 101.Релейный режим
	TNormState	    NormState[3];		 // 102-104.Нормальное состояние входов    - Не используется но если не заработают маски
	TNormState 	    NormOut[8];			 // 105-112.Нормальное состояние выходов - то пользуем это
	TDeviceReg      FaultsDev;           // 113.Диагностика устройства
	Uns             StartIndic;			 // 114.Индикация в старте
 	Uns             SleepTime;           // 115.Дежурный режим  mb to C
	Uns             BusyValue;       	 // 116.Процент исполнения
	Uns 			LogEvAddr;			 // 117. Текущий адрес журнала событий
	Uns 			LogCmdAddr;			 // 118. Текущий адрес журнала команд
	Uns 			LogParamAddr;		 // 119. Текущий адрес журнала изменения параметров
	Uns				LogEvCount;			 // 120. Количество записанных ячеек журнала событий
	Uns				LogCmdCount;		 // 121. Количество записанных ячеек журнала команд
	Uns				LogParamCount;		 // 122. Количество записанных ячеек журнала изменения параметров
	Uns             Torque;           	 // 123.Момент
	Int             Speed;				 // 124.Скорость
	Uns				Seconds;			 // 125.Секунды
	TContactorGroup ContGroup;			 // 126. Управление контакторами
	TBurCmd 		LogControlWord;		 // 127. Команды БУР
	Uns				LogReset;			 // 128. Сброс журналов
	Uns 			Rsvd3[11];			 // 129-139.Резерв
} TGroupH;

// Группа E (Адрес 400, Количество 32)
typedef struct _TGroupE
{
	TTimeVar       LogTime;				// T.Время
	TDateVar       LogDate;				// D.Дата
	TStatusReg     LogStatus;           // 0.Статус работы
	TFltUnion	   LogFaults;			// 1-4.Аварии	
	Int            LogPositionPr;       // 5.Положение %
	Uns            LogTorque;           // 6.Момент
	Uns            LogUr;               // 7.Напряжение фазы R
  	Uns            LogUs;               // 8.Напряжение фазы S
  	Uns            LogUt;               // 9.Напряжение фазы T
	Uns            LogIu;               // 10.Ток фазы U
	Uns            LogIv;               // 11.Ток фазы V
	Uns            LogIw;               // 12.Ток фазы W
	Int            LogTemper;           // 13.Температура блока
	TInputReg      LogInputs;           // 14.Состояние дискретных входов
	TOutputReg     LogOutputs;          // 15.Состояние дискретных выходов
	Uns 		   Rsvd[14];			// 16-29.Резерв
} TGroupE;


typedef struct _TLogEvBuffer
{
	TStatusReg     	LogStatus;          // 0.Статус работы
	Int            	LogPositionPr;		// 1.Положение %
	Uns            	LogTorque;          // 2.Момент
	Uns            	LogUr;              // 3.Напряжение фазы R
  	Uns            	LogUs;              // 4.Напряжение фазы S
  	Uns            	LogUt;              // 5.Напряжение фазы T
	Uns            	LogIu;              // 6.Ток фазы U
	Uns            	LogIv;              // 7.Ток фазы V
	Uns            	LogIw;              // 8.Ток фазы W
	Int            	LogTemper;          // 9.Температура блока
	Uns      		LogInputs;        	// 10.Состояние дискретных входов
	Uns				LogOutputs;			// 11.Состояние дискретных выходов
} TLogEvBuffer;

// начальный адрес 4000
typedef struct _TGroupT
{
	TTEK_TechReg 	TechReg;			// 1 Технологический регистр
	TTEK_DefReg  	DefReg;				// 2 Регистр дефектов
	Uns 		 	PositionPr;			// 3 Текущее положение %
	TTEK_ComReg  	ComReg;				// 4 Регистр команд
	Uns 		 	CycleCnt;			// 5 Счетчик циклов
	Uns			 	Rsvd1;				// 6 Резерв
	Uns 		 	Iu;					// 7 Ток фазы U
	Uns 		 	Rsvd2[9];			// 8 - 16 Резерв
	Uns 			Ur;					// 17 Напряжение входной сети
	Uns				Rsvd3;				// 18 Резерв
	Uns 		 	   Speed;				// 19 Текущая скорость
	Uns			 	Rsvd4;				// 20 Резерв
	Uns 		 	Torque;				// 21 Текущий момент нагрузки Н*м
	TTEK_Discrete 	TsTu;				// 22 ТС/ТУ
	Uns				Rsvd6[4];			// 23 - 26 Резерв
	Uns				RsStation;			// 27 Адрес станции (только для чтения)
	Uns				Rsvd7;				// 28 Резерв
	Uns				Rsvd8[12];			// 29 Резерв
} TGroupT;



// Структура параметров устройства
typedef struct _TDriveData
{
	TGroupT			GroupT;				// TechReg
	TGroupB 		GroupB;				// 1.Пользовательские параметры
	TGroupC			GroupC;				// 2.Заводские параметры
	TGroupD			GroupD;				// 3.Команды управления
	TGroupG			GroupG;				// 4.Заводскиой тест
	TGroupH			GroupH;				// 5.Скрытые параметры
	TGroupA 		GroupA;				// 0.Диагностика привода
	TGroupE			GroupE;				// 6.Журнал событий

} TDriveData;


// Структура параметров устройства
/*typedef struct _TTEKDriveData
{
	TTEK_MB_GROUP	MainGroup;
} TTEKDriveData;*/

#ifdef CREATE_DRIVE_DATA					// Чтобы экземпляр создал только в одном месте, иначе будет писать "define multiple times"
TDriveData Ram;

TGroupA *GrA = &Ram.GroupA;
TGroupB *GrB = &Ram.GroupB;
TGroupC *GrC = &Ram.GroupC;
TGroupD *GrD = &Ram.GroupD;
TGroupE *GrE = &Ram.GroupE;
TGroupG *GrG = &Ram.GroupG;
TGroupH *GrH = &Ram.GroupH;
TGroupT *GrT = &Ram.GroupT;

#else
extern TDriveData Ram;
extern TGroupA *GrA;
extern TGroupB *GrB;
extern TGroupC *GrC;
extern TGroupD *GrD;
extern TGroupE *GrE;
extern TGroupG *GrG;
extern TGroupH *GrH;
extern TGroupT *GrT;

#endif

#if BUR_M
#define TEK_DEVICE_FAULT_MASK	0xF910				// Маска на регистр дефектов устройства для модбаса ТЭК
#else
#define TEK_DEVICE_FAULT_MASK	0xF990
#endif

#define RAM_ADR					0									// Адрес начала области памяти на запись
#define RAM_SIZE				sizeof(TDriveData)					// Размер области памяти параметров
#define RAM_DATA_SIZE			(RAM_SIZE - sizeof(TGroupE))		// Количество параметров для записи

#define MENU_GROUPS_COUNT		5
#define MENU_EVLOG_GROUP		4

#define LOGS_CNT				3									// Количество журналов устройства

#define LOG_EV_START_ADDR		12000								// Начальный адрес области памяти основной записи журнала
#define LOG_EV_DATA_CNT			20		//sizeof(TGroupE) + 2		// Количество полей в 1 ячейке основной записи журнала
#define LOG_EV_CNT				500									// Количество записей в журнал событий
#define LOG_EV_DATA_CELL		1									// Количество структур в основной записи журнала

#define LOG_EV_BUF_START_ADDR	0									// Начальный адрес буфера журнала
#define LOG_EV_BUF_DATA_CNT		sizeof(TLogEvBuffer)				// Количество полей в одной ячейке буфера
#define LOG_EV_BUF_DATA_CELL	5									// Количество ячеек буфера в одной записи

#define LOG_EV_BUF_CELL_COUNT	5									// Количество структур в буфере журнала событий

#define PREV_LEV_INDEX		0										//Индексы для запоминания предыдущего адреса журналов
#define PREV_LCMD_INDEX		1
#define PREV_LPAR_INDEX		2



#define LOG_EV_RAM_DATA_ADR		GetAdr(GroupE)						// Адрес группы журнала в Ram
#define LOG_EV_RAM_DATA_LADR	(LOG_EV_RAM_DATA_ADR * LOG_EV_DATA_CNT)
#define LOG_EV_SIZE				(LOG_EV_CNT * LOG_EV_DATA_CNT * LOG_EV_DATA_CELL)
//!!!!

#define LOG_CMD_START_ADDR		1000								// Начальный адрес журнала команд
#define LOG_CMD_DATA_CNT		5									// Количество записываемых в память полей
#define LOG_CMD_CNT				1500								// Емкость журнала команд (количество записей)

#define LOG_PARAM_START_ADDR	9000								// Начальный адрес журанала изменения параметров
#define LOG_PARAM_DATA_CNT		5									// Количество записываемых полей (поля структуры журнала: Дата, Время и т.д.)
#define LOG_PARAM_CNT			500									// Емкость журнала изменения параметров

#define IM_READ_BUF_SIZE		((LOG_EV_DATA_CNT * LOG_EV_DATA_CELL) + (LOG_EV_BUF_DATA_CNT * LOG_EV_BUF_DATA_CELL))

#define REG_TORQUE_ADDR		GetAdr(GroupA.Torque)
#define REG_START_IND			GetAdr(GroupH.StartIndic)
//#define REG_START_IND			GetAdr(GroupA.Torque)
#define REG_LOG_ADDR			GetAdr(GroupH.LogEvAddr)
#define REG_LOG_TIME			GetAdr(GroupE.LogTime)
#define REG_LOG_DATE			GetAdr(GroupE.LogDate)

#define REG_CODE				GetAdr(GroupB.MainCode)
#define REG_FCODE				GetAdr(GroupC.FactCode)
#define REG_PASSW1				GetAdr(GroupH.Password1)
#define REG_PASSW2				GetAdr(GroupH.Password2)

#define REG_CYCLE_CNT			GetAdr(GroupH.CycleCnt)
#define REG_CONTROL				GetAdr(GroupD.ControlWord)
#define REG_SC_FAULTS			GetAdr(GroupH.ScFaults)
#define REG_CALIB_STATE			GetAdr(GroupH.CalibState)

#define CMD_DEFAULTS_USER	0x0010	// Пользовательские параметры по умолчанию
#define CMD_RES_CLB			0x0020	// Сброс калибровки датчика положения
#define CMD_RES_FAULT		0x0040	// Сброс защит
#define CMD_CLR_LOG			0x0080	// Очистка журнала
#define CMD_RES_CYCLE		0x0100	// Сброс счетчика циклов
#define CMD_PAR_CHANGE		0x0200	// Изменение параметров
#define CMD_DEFAULTS_FACT 	0x0400	// Заводские параметры по умолчанию

#define TS_SIGNAL_MASK_M 	511		// Маска для отсекания лишних сигналов ри увеличении колличества ТСов поменять
#define TS_SIGNAL_MASK_T 	255		// 

#if BUR_M
#define LOG_EV_DEFAULTS { \
		false, false, false, false, true, 4, \
		0, 0, 0, 0, \
		0, 0, 0, 0, \
		0, 0, 0, 0, \
		&Ram.GroupB.DevTime.all, &Ram.GroupB.DevDate.all,\
		&Ram.GroupA.Status.all, &Ram.GroupA.PositionPr, &Ram.GroupA.Torque,				\
		&Ram.GroupA.Ur, &Ram.GroupA.Us, &Ram.GroupA.Ut,			\
		&Ram.GroupA.Iu, &Ram.GroupA.Iv, &Ram.GroupA.Iw,			\
		&Ram.GroupA.Temper, 0, &Ram.GroupA.Outputs.all,	\
		&Ram.GroupH.Seconds,		\
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0  \
		}
#else
#define LOG_EV_DEFAULTS { \
		false, false, false, false, true, 4, \
		0, 0, 0, 0, \
		0, 0, 0, 0, \
		0, 0, 0, 0, \
		&Ram.GroupB.DevTime.all, &Ram.GroupB.DevDate.all,\
		&Ram.GroupA.Status.all, &Ram.GroupA.PositionPr, &Ram.GroupA.Torque,				\
		&Ram.GroupA.Ur, &Ram.GroupA.Us, &Ram.GroupA.Ut,			\
		&Ram.GroupA.Iu, &Ram.GroupA.Iv, &Ram.GroupA.Iw,			\
		&Ram.GroupA.Temper, &Ram.GroupA.Inputs.all, &Ram.GroupA.Outputs.all,	\
		&Ram.GroupH.Seconds,		\
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0  \
		}
#endif

#define LOG_CMD_DEFAULTS	{	\
		FALSE, FALSE, FALSE,	\
		&Ram.GroupB.DevTime.all, &Ram.GroupB.DevDate.all, &Ram.GroupH.Seconds, \
		&Ram.GroupA.Status.all, 0, 0,\
		0,0,0,0,0				\
		}


#define LOG_PARAM_DEFAULTS	{	\
		false, false, false,	\
		&Ram.GroupB.DevTime.all, &Ram.GroupB.DevDate.all, &Ram.GroupH.Seconds, \
		0, 0,					\
		0,						\
		0, 0, 0, 0, 0,			\
		0, 0, 0, 0, 0			\
		}

// Ram.GroupH.LogEvCount
#define MENU_DEFAULT { \
	MS_START, 0, 0, 0, 1, True, True, False, \
	&Ram.GroupC.MaxTorque, \
	0, 0, 0, False, 0, BLINK_HI_TOUT, BLINK_LO_TOUT, 0xFF, 0, BIT_TOUT, \
	False, 0, 0, \
	ToPtr(&Ram), Display.HiStr, Display.LoStr, \
	REG_START_IND, RAM_ADR, "", "", \
	False, False, MENU_EVLOG_GROUP, REG_LOG_ADDR, REG_LOG_TIME, REG_LOG_DATE, &Ram.GroupH.LogEvCount, \
	0, 0, MENU_GROUPS_COUNT, \
	0, 0, 0, \
	0, 0, LOG_EV_CNT, \
	0, 0, 0, \
	0, 0, 0, 0, \
	FALSE, FALSE, 0, 0, FALSE, 0, \
	4, {GetAdr(GroupB.DevTime),GetAdr(GroupB.DevDate),GetAdr(GroupB.RodType),GetAdr(GroupB.ValveType),0},	\
	2, {GetAdr(GroupB.MoveCloseTorque),GetAdr(GroupB.MoveOpenTorque),0,0,0},	\
	1, {GetAdr(GroupD.TaskClose),0,0,0,0},	\
	1, {GetAdr(GroupD.TaskOpen),0,0,0,0},	\
	1, {GetAdr(GroupD.RevOpen),0,0,0,0},	\
	1, {GetAdr(GroupD.RevClose),0,0,0,0},	\
	1, {GetAdr(GroupB.BreakCloseTorque),0,0,0,0},	\
	1, {GetAdr(GroupB.StartCloseTorque),0,0,0,0},	\
	1, {GetAdr(GroupB.BreakOpenTorque),0,0,0,0},	\
	1, {GetAdr(GroupB.StartOpenTorque),0,0,0,0},	\
	1, {0,0,0,0,0},	\
	1, {GetAdr(GroupB.MuDuSetup),0,0,0,0},	\
	groups, Null, params, values, "РЕЗЕРВ", \
	StartDispl, EnableEdit, WriteValue, GetExpressText, GetExpressState\
}
//#define MENU_GROUPS_COUNT		5
//#define MENU_EVLOG_GROUP		4


#define DRIVE_CTRL_DEFAULT { \
	ToPtr(&Ram.GroupA.Status), \
	&Ram.GroupD.ControlWord, \
	&Ram.GroupB.MuDuSetup, \
	&Ram.GroupB.DuSource, \
	&Ram.GroupH.ReverseType, \
	True, 0, 0, False, \
	True, False, 0, True, False, True,\
	&Ram.GroupH.TuReleMode, \
	&Ram.GroupB.TuLockSeal, \
	False, False, \
	False, False, 0, DEMO_TOUT, 0, \
	&Ram.GroupH.DemoPosition2, \
	&Ram.GroupH.DemoPosition1, \
	True, False, POS_UNDEF, 0, \
	&Ram.GroupB.BreakMode, \
	&Calib, \
	0, 0, vcwNone, 0, 0, 0, 0,\
	&StopPowerControl, \
	&StartPowerControl \
}

#define CALIBS_DEFAULT { \
	False, False, 0, 0, REV_MAX, 0, 0, 0, 0, \
	MUFF_CLB_TIME, 0, 0, 0, 0, 0, 0, 0, \
	&Ram.GroupC.PosSensPow, \
	&Ram.GroupB.PositionAcc, \
	ToPtr(&Ram.GroupB.RodType), \
	&Encoder.Revolution, \
	&Ram.GroupA.PositionPr, \
	&Ram.GroupA.FullWay, \
	&Ram.GroupA.CurWay, \
	&Ram.GroupH.CycleCnt, \
	&MuffFlag, \
	ToPtr(&Ram.GroupD.TaskClose), \
	ToPtr(&Ram.GroupH.CalibState) \
}



#define IM_DEFAULT { \
	RECEIVE_FUNC_MODE, imInit, 0, 0, \
	False, &Bluetooth.TxBusy, 0, \
	IM_TIMEOUT, \
	False, False, False, False, \
	(Uns *)&Ram, &ImReadBuf[0] \
}


// Глобальные переменные модуля
extern MENU        	Menu;
extern TLogCmd		LogCmd;
extern TLogParam 	LogParam;
extern CALIBS       Calib;
extern TVlvDrvCtrl  Mcu;
extern TOutputReg   Dout;

extern Uns IrImpCount;
extern Uns LcdEnable;
extern Bool PowerEnable;
extern Bool DisplayRestartFlag;

extern TIM Im;
extern Uns TempMuDu;

// Прототипы функций
void InterfaceInit(void);
void InterfIndication(void);
void DataBufferPre(void);
void LogEvControl(void);
void GetCurrentCmd(void);
void LogCmdControl(void);
void LogParamControl(void);
void LogParamMbExec(void);
Bool InterfaceRefresh(void);
void StartDispl(String Str);
Bool EnableEdit(Uns Password, Uns Param);
Bool WriteValue(Uns Memory, Uns Param, Uns *Value);
void GetExpressText(Uns State);
Byte GetExpressState(Byte CurrentState, Uns State);
void RefreshData(void);
void DataSetting(void);
void AddControl(void);
void LocalControl(void);
void RemoteControl(void);
void AlgControl(void);
void ClbControl(void);
void BlkSignalization(void);
void TsSignalization(void);
void ShowDriveType(void);
void DisplayStart(void);
#if IR_IMP_TEST
void IrImpCalc(void);
#endif
void ImReadRecordsToBuffer(void);

#endif
