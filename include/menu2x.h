#ifndef _MENU_H_
#define _MENU_H_

#include "menu_global.h"
#include "key_codes.h"

#ifdef __cplusplus
extern "C" {
#endif

// Структра группы меню
struct MENU_GROUP {
	Char  HiString[16];  // Верхняя строчка группы
	Char  LoString[16];  // Нижняя строчка группы
	Uns   Addr;          // Начальный адрес
	Uns   Count;         // Количество элементов
};

// Структура параметра меню
struct MENU_PARAM {
	Char  Name[16];      // Имя параметра
	Char  Unit[4];       // Единица измерения параметра
	struct MENU_DCR Dcr; // Описание параметра
};

// Структура строкового значения параметра
struct MENU_STRING {
	Char  Str[16];       // Строковое значение параметра
};

// Структура для работы с параметрами при Express настройке
struct MENU_EXPRESS_PARAMS {
	Char Cnt;			// Количество параметров
	Uns  Buf[5];		// Буфер адресов параметров в структуре Ram
};

// Структура для работы с Express настройкой в меню
struct MENU_EXPRESS {
	Bool Enable;							// Разрешние запуска Express настройки
	Bool Select;							// Флаг выбора настройки
	Byte State;								// Шаг (состояние) экспресс настройки
	Byte Index;								// Индекс текущего параметра в текущем List'е
	Bool First;								// Флаг первого такта
	struct MENU_EXPRESS_PARAMS *List;		// Указатель на текущий список параметров
	struct MENU_EXPRESS_PARAMS List1;		// Параметры экспресс настройки
	struct MENU_EXPRESS_PARAMS List2;
	struct MENU_EXPRESS_PARAMS List3;
	struct MENU_EXPRESS_PARAMS List4;
	struct MENU_EXPRESS_PARAMS List5;
	struct MENU_EXPRESS_PARAMS List6;
	struct MENU_EXPRESS_PARAMS List7;
	struct MENU_EXPRESS_PARAMS List8;
	struct MENU_EXPRESS_PARAMS List9;
	struct MENU_EXPRESS_PARAMS List10;
	struct MENU_EXPRESS_PARAMS List11;
	struct MENU_EXPRESS_PARAMS List12;
};

// Структура меню
typedef struct MENU {
	Byte   State;        // Режим меню
	Char   Key;          // Управляющая команда
	Byte   EditType;     // Тип режима редактирования
	Byte   Level;        // Текущий уровень меню
	Byte   MaxLevel;     // Уровень вложенности меню (1 или 2)
	Bool   Indication;   // Флаг разрешения индикации
	Bool   Update;       // Флаг обновления данных из структур
	Bool   ShowReserved; // Флаг отображения резервов
	Uns   *MinMaxGain;   // Коэффициент диапазона
	
	Uns    Value;        // Текущее значение параметра
	Byte   Rate;         // Текущий разряд
	Byte   EditRate;     // Максимальный разряд для редактирования
	Bool   Blink;        // Состояния мигания значения
	Uns    BlinkTimer;   // Таймер для мигания
	Uns    BlinkHiTime;  // Время отображения при редактировании
	Uns    BlinkLoTime;  // Время скрытия при редактировании
	Byte   Bit;          // Выводимый бита числа в виде бегущей строки
	Uns    BitTimer;     // Таймер паузы при выводе бита
	Uns    BitTime;      // Пауза при выводе бита

	Bool   SleepActive;  // Состояние режима
	Uns    SleepTimer;   // Таймер режима
	Uns    SleepTime;    // Тайм-аут режима

	Uns   *Data;         // Указатель на буфер данных
	String HiString;     // Указатель на буфер верхней строчки индикатора
	String LoString;     // Указатель на буфер нижней строчки индикатора
	Uns    StartAddr;    // Адрес параметра при выводе в режиме старта
	Uns    StartOffset;  // Смещение адреса при выводе в режиме старта
	Char   BufValue[6];  // Временный буфер при редактирование в режиме 1
	Char   BufTmp[17];   // Временный буфер для хранения единицы измерения/строкового значения
	
	Bool   EvLogFlag;    // Флаг чтения записи
	Bool   EvLogSelected;// Флаг выбора записи
	Uns    EvLogGroup;   // Номер группы журнала
	Uns    EvLogAdress;  // Адрес параметра текущей записи в журнал
	Uns    EvLogTime;    // Адрес параметра времени записи журнала
	Uns    EvLogDate;    // Адрес параметра даты записи журнала
	Uns	   *EvLogCount;	 // Количество сделанных записей в журнал
	
	struct MENU_CRD Group;     // Структура координаты группы
	struct MENU_CRD SubGroup;  // Структура координаты подгруппы
	struct MENU_CRD EvLog;     // Структура координаты записи журнала
	struct MENU_CRD Param;     // Структура координаты параметра
	struct MENU_DCR Dcr;       // Структура описания параметра

	struct MENU_EXPRESS Express;	// Структура экспресс настройки
	
	const struct MENU_GROUP  *Groups;   // Указатель на структуры групп
	const struct MENU_GROUP  *SubGroups;// Указатель на структуры подгрупп
	const struct MENU_PARAM  *Params;   // Указатель на структуры параметров
	const struct MENU_STRING *Values;   // Указатель на структуры строковых значений
	struct MENU_STRING        Rsvd;     // Резервная строка
	
	void (*StartDispl)(String Text);
	Bool (*EnableEdit)(Uns PaswwPrt, Uns ParAddr);
	Bool (*WriteValue)(Uns Memory, Uns ParAddr, Uns *Value);
	void (*GetExpressText)(Uns State);
	Byte (*GetExpressState)(Byte CurrentState, Uns State); 
} MENU;

void MENU_Update(MENU *);
void MENU_Display(MENU *);
void MENU_ReadDcr(MENU *, struct MENU_DCR *, Uns Addr);

#ifdef __cplusplus
}
#endif // extern "C"

#endif



