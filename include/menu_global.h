#ifndef _MENU_GLOBAL_H_
#define _MENU_GLOBAL_H_

#include "std.h"

#ifdef __cplusplus
extern "C" {
#endif

// Режимы меню
#define MS_START		0		// Режим старта
#define MS_SELGR		1		// Режим выбора группы
#define MS_SELPAR		2		// Режим выбора параметра
#define MS_EDITPAR		3		// Режим редактирования параметра
#define MS_SHOWPAR		4		// Режим просмотра параметра
#define MS_SHOWBIN		5		// Режим двоичного отображения
#define MS_SETREF		6		// Режим редактирования задания
#define MS_EXPRESS		7		// Режим экспресс настройки
#define MS_DRIVE_TYPE	8		// Режим экспресс настройки

// Типы параметров
#define MT_DEC		0		// Десятичный
#define MT_STR		1		// Строковый
#define MT_RUN		2		// Бегущая строка
#define MT_TIME		3		// Время
#define MT_DATE		4		// Дата
#define MT_BIN		5		// Двоичный
#define MT_HEX		6		// Шестнадцатеричный
#define MT_RSVD     7     // Резервный

// Структура конфигурации параметра
struct MENU_VAL_CFG {
	Uns Type:3;             // 0-2	Тип параметра
	Uns ReadOnly:1;         // 3     Признак параметра только для чтения
	Uns PaswwPrt:2;         // 4-5   Признак проверки на пароль
	Uns Memory:1;			   // 6     Признак хранения в памяти
	Uns Signed:1;			   // 7     Признак знакового параметра
	Uns MinMax:1;			   // 8     Признак наличия коэффициента диапазона
	Uns Precision:3;		   // 9-11  Количество знаков после запятой
	Uns RateMax:4;			   // 12-15 Максимальный разряд
};

struct MENU_STR_CFG {
	Uns Type:3;             // 0-2	Тип параметра
	Uns ReadOnly:1;         // 3     Признак параметра только для чтения
	Uns PaswwPrt:2;         // 4-5   Признак проверки на пароль
	Uns Memory:1;           // 6     Признак хранения в памяти
	Uns Addr:9;             // 7-15	Адрес строкового значения
};

union MENU_CFG {
	Uns all;
	struct MENU_VAL_CFG Val;
	struct MENU_STR_CFG Str;
};

// Структура описания параметра
struct MENU_DCR {
	Uns   Min;              // Минимальное значение параметра
	Uns   Max;              // Максимальное значение параметра
	Uns   Def;              // Значение по умолчанию
	union MENU_CFG Config;  // Конфигурация параметра
};

// Структура координаты
struct MENU_CRD {
	Uns   Position;         // Текущий номер элемента
	Uns   Addr;             // Начальный адрес элементов
	Uns   Count;            // Количество элементов
};

// Макросы при работе с меню
#define M_TYPE      0x0007                  // Маска для типа параметра
#define M_RONLY     0x0008                  // Только для чтения
#define M_PWP1      0x0010                  // Защищен паролем 1
#define M_PWP2      0x0020                  // Защищен паролем 2
#define M_RC			0x0030						// Зависит от состояния готовности (для M_RONLY)
#define M_NVM       0x0040                  // Хранится в энергонезависимой памяти
#define M_SIGN      0x0080                  // Знаковый параметр
#define M_KMM       0x0100                  // Параметр с коэффициентом диапазона
#define M_SADR(i)   ((i & 0x01FF) << 7)     // Адрес строкового значения
#define M_PREC(i)   ((i & 0x0007) << 9)     // Количество знаков после запятой
#define M_RMAX(i)   ((i & 0x000F) << 12)    // Максимальный разряд
#define M_EDIT_PAR  (M_PWP1|M_NVM)          // Параметр настройки
#define M_FACT_PAR  (M_PWP2|M_NVM)          // Заводской параметр настройки
#define M_PWP_MASK  (M_PWP1|M_PWP2)         // Маска конифгурации защиты паролем
#define M_CFG_MASK  (M_TYPE|M_RONLY|M_PWP_MASK|M_NVM) // Маска общей конфигурации

#define M_SHOW      (MT_DEC|M_RONLY)        // Нередактируемый параметр
#define M_EDIT      (MT_DEC|M_PWP1|M_NVM)   // Редактируемый десятичный параметр
#define M_FACT      (MT_DEC|M_PWP2|M_NVM)   // Редактируемый десятичный заводской параметр
#define M_DCOM      (MT_DEC|M_PWP1)         // Десятичная команда управления
#define M_FDCOM		(MT_DEC|M_PWP2)         // Десятичная заводская команда управления
#define M_STAT      (MT_STR|M_RONLY)        // Показание строковое
#define M_LIST      (MT_STR|M_PWP1|M_NVM)   // Редактируемый строковый параметр
#define M_FLST      (MT_STR|M_PWP2|M_NVM)   // Редактируемый строковый заводской параметр
#define M_COMM      (MT_STR|M_PWP1)         // Строковая команда управления
#define M_FCOMM     (MT_STR|M_PWP2)         // Строковая заводская команда управления
#define M_RUNS      (MT_RUN|M_RONLY)        // Параметр бегущая строка
#define M_TIME      (MT_TIME|M_RMAX(1))     // Параметр время
#define M_DATE      (MT_DATE|M_RMAX(2))     // Параметр дата
#define M_BINS      (MT_BIN|M_RONLY)        // Параметр в двоичном отображении
#define M_BINE		(MT_BIN|M_PWP1|M_NVM)	// Редактируемый двоичный параметр
#define M_BINF		(MT_BIN|M_PWP2|M_NVM)	// Редактируемый двоичный заводской параметр
#define M_BINC		(MT_BIN|M_PWP1)			// Двоичная команда управления
#define M_BINFC		(MT_BIN|M_PWP2)			// Двоичная заводская команда управления
#define M_CODE      (MT_DEC|M_RMAX(4))      // Код доступа
#define M_HSHOW     (MT_HEX|M_RONLY)        // Нередактируемый шестнадцатеричный параметр
#define M_HEDIT     (MT_HEX|M_PWP1|M_NVM)   // Редактируемый шестнадцатеричный параметр
#define M_HFACT     (MT_HEX|M_PWP2|M_NVM)   // Редактируемый шестнадцатеричный заводской параметр
#define M_HIDE      (MT_DEC|M_NVM)          // Скрытый параметр
#define M_RSVD      (MT_RSVD|M_RONLY)       // Резервный параметр
#define M_ESHOW     (MT_DEC|M_RONLY|M_NVM)  // Нередактируемый десятичный параметр из памяти
#define M_EHSHOW		(MT_HEX|M_RONLY|M_NVM)	// Нередактируемый шестнадцатеричный параметр из памяти
#define M_ETSHOW		(M_TIME|M_RONLY|M_NVM)	// Нередактируемое время из памяти
#define M_EDSHOW		(M_DATE|M_RONLY|M_NVM)	// Нередактируемая дата из памяти

#define M_IS_KMM(v)	((((v) & M_TYPE) == MT_DEC) && (v & M_KMM))

#ifdef __cplusplus
}
#endif // extern "C"

#endif

