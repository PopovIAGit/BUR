#ifndef CONFIG_
#define CONFIG_

// Глобальные настройки
#define VERSION					1113	// Версия проекта
#define SUBVERSION				12		// Подверсия проекта
#define DEF_USER_PASS			2387	// Пароль по-умолчанию (пользовательский)
#define DEF_FACT_PASS			43072	// Пароль по-умолчанию (заводской)
#define BT_AUTH_CODE_STRING		"5124"	// Строка с кодом авторизации Bluetooth


#define SYSCLK				100e6		// Системная частота (кол-во операций в сек)
#define CLKIN				50e6		// Частота кварца
#define CLK_VS_TRN_FREQ		20000U	// Частота тактирования тиристоров
#define CLK_VS_TRN_PERIOD	(SYSCLK / CLK_VS_TRN_FREQ)		// Период ШИМ
//#define IRDA_FREQ			((SYSCLK / 528e3) - 1)	// Частота IRDA
#define IRDA_FREQ			((SYSCLK / 528e3) - 1)	// Частота IRDA

// Конфигурация операционной системы
#define HZ						18e3		// Частота основного прерывания RTOS
#define PRD0					2000		// Периодическая задача 2000 Гц
#define PRD1					200			// Периодическая задача 200 Гц
#define PRD2					50			// Периодическая задача 50 Гц
#define PRD3					10			// Периодическая задача 10 Гц
#define PRD4					50			// Периодическая задача 50 Гц
#define PRD5					50			// Периодическая задача 50 Гц

// Тестовые режимы
#define TORQ_TEST			0			// Тест расчета момента
#define LOWPOW_TEST			0			// Тест выключение при провале напряжения, 1 выключаем режим энергосбережения, если 0 то включен тест
#define IR_IMP_TEST			0			// Тест IR

// Подключение заголовочных файлов
#include "device_modification.h"		// Модификация блока
#include "csl\csl.h"					// Библиотека поддержки чипа
#include "chip\DSP280x_Device.h"		// Заголовные файлы переферии чипа
#include "IQmathLib.h"					// Заголовояный файл библиотеки IQmath
#include "timings.h"					// Файл таймингов
#include "structs.h"					// Описание типов и структур проекта
#include "macro.h"						// Файл макросов
#include "hardware.h"					// Конфигурация оборудования
#include "serial_communication.h"		// Последовательная связь
#include "interface.h"					// Интерфейсный модуль
#include "rim_devices.h"				// Модуль периферии
#include "motor_control.h"				// Управление двигателем
#include "protections.h"				// Защиты привода
#include "drv_modbus.h"
#include "params.h"						// Файл конфигурации параметров 

#endif

