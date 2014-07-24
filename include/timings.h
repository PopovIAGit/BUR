/*
timings.h

Тайминги
*/

#ifndef _TIMINGS_H_
#define _TIMINGS_H_


// rim_devices
#define EEPROM_RETRY			(5)					// Количество попыток перезаписи в EEPROM
#define DISPL_SCALE				(1.000 * PRD0)		// Масштабный коэф. индикатора
#define LOG_START_TOUT			(3.000 * PRD1)		// Таймаут записи в журнал
#define LOG_BUF_START_TOUT		(1.000 * PRD1)		// Таймаут записи в журнал
#define WRITE_TIME				(0.020 * PRD0)		// Время записи EEPROM 20 мс

#define BLINK_HI_TOUT			(0.500 * PRD3)		// Время отображения при редактировании
#define BLINK_LO_TOUT			(0.300 * PRD3)		// Время скрытия при редактировании
#define BIT_TOUT				(1.000 * PRD3)		// Пауза при выводе бита
#define TSENS_RETRY				(5)					// Количество сбоев датчика до выставления ошибки
#define BTN_TIME				(0.100 * PRD2)		// Время удержании ручки в положении
#define TU_SCALE				(0.100 * PRD2)		// Масштабировка для ТУ (1 = 0.1 с)
#define TEMP_EN_TOUT			(5.000 * PRD3)		// Задержка включения тена
#define CONT_TEST_TIME 			(1.500 * PRD3)		// Время темтового замыкания контакторов при проверке БУР М
#define DRV_TYPE_SHOW_TIME      (5.00 * PRD3)      // Время индикации типа привода при пуске блока


// interface
#define MUFF_CLB_TIME			(3.000 * PRD3)		// Время удержания муфты для калибровок
#define DEMO_TOUT				(5.000 * PRD3)		// Время паузы в демо-режиме
#define BLINK_TOUT				(0.500 * PRD3)		// Время мигания при нажатии кнопки ПДУ/МПУ
#define POWER_TOUT				(4.500 * PRD2)		// Время провала напряжения
#define DSP_LED_TIMER			(0.700 * PRD3)		// Время светодиода процессора
#define CANCEL_TOUT				(2.000 * PRD3)		// Время вывода сообщения об отмене команды
#define PULT_LED_TOUT			(0.200 * PRD3)		// Время горения светодиода ПДУ
#define SLEEP_SCALE				(1.000 * PRD3)		// Дежурный режим меню
#define IM_TIMEOUT				(10.00 * PRD3)		// Таймаут информационного модуля

// motor_control				 
#define START_DELAY_TIME		(2.000 * PRD2)		// Ограничение времени паузы между остановом и след. запуском
#define ZAZOR_SCALE				(0.100 * PRD1)		// Масштабировка времени выбора зазора
#define TEST_STATE_TIME			(0.040 * PRD1)		// Ограничение времени тестового режима > было 1.2
#define UPOR_STATE_TIME			(2.000 * PRD1)		// Ограничение времени режиме упора для
#define MOVE_STATE_TIME			(1.000 * PRD1)		// Ограничение времени перехода на упор
#define PAUSE_STATE_TIME		(0.040 * PRD1)		// Ограничение времени режима паузы
#define DYN_PAUSE_TIME			(0.040 * PRD1)		// Ограничение времени паузы на
#define KICK_ST0_TIME			(0.040 * PRD1)		// Ограничение времени в удар режиме при отсутствии напр
#define KICK_ST1_TIME			(0.080 * PRD1)		// Полное ограничение времени в удар режиме
#define SP_CALC_TOUT			(1.000 * PRD3)		// Интервал расчета скорости
#define CTRLMODE_SCALE			(0.100 * PRD1)		// Масшаб для частоты сети
#define DMC_FLTR_TS				(1.000 / PRD2)		// Постоянная дискретизации фильтров рмс
#define DMC_TS					(1.000 / HZ)		// Постоянная дискретизации модуля управления двигателем
#define DMC_TORQF_TS			(1.000 / PRD1)		// Постоянная дискретизации фильтра расчета момента
#define START_PAUSE				(2.000 * PRD1)		// тест линейного КЗ
#define DEBUG_START_TIME		(5.000 * PRD1)      // таймер для повторного включения ПП без источника

#define NET_MON_START_TIME		(1.000 * PRD2)		// Задержка перед пуском по наличию напряжения
#define NET_MON_STOP_TIME2 		(1.000 * PRD2)		// Задержка перед перед стопом при пропаже напряжения
#define NET_MON_STOP_TIME 		(1.000 * PRD2)		// Задержка перед перед стопом при пропаже напряжения
#define CONECTOR_TIME			(0.700 * PRD2)		// Время выдачи сигнала на конекторы
#define CONECTOR_STOP_TIME		(1.000 * PRD2)		// Время выдачи сигнала на конекторы
// protection
#define PRT_SCALE				(0.100 * PRD2)	// Масштибировка для защит (1 = 0.1 с)
#define TB_SCALE				(60.00 * PRD2)	// Масштабировка для защиты от перегрева/переохлождения блока
#define FLT_DEF_DELAY			(2.000 * PRD4)	// Задержка на определение всех аварий
#define VSK_TIME				(3.500 * PRD4)	// задержка для расчета небаланса


#endif

