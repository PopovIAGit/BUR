#ifndef _LOG_EV_H
#define _LOG_EV_H

#include "std.h"

#define NUMBER_OF_LOGS				3
#define LOG_EVENT_TYPE				0x01
#define LOG_CMD_TYPE				0x02
#define LOG_PARAMS_TYPE				0x03



#define FAULTS_COUNT_DEF			4				// Количество регистров дефектов
#define LOG_EV_BUF_DATA_COUNT		20				// Определяет емкость буфера журнала событий
#define LOG_CMD_BUF_DATA_COUNT		5				// Определяет емкость буфера журнала команд
#define LOG_PARAM_BUF_DATA_COUNT	5				// Определяет емкость буфера журнала изменения параметров
#define MB_PARAM_BUF_COUNT			5

// Структура для работы с журналом событий
typedef struct _TLogEv
{
	Bool Enable;							// Разрешение работы журнала событий
	Bool ExecFlag;							// Флаг формирования буфера журнала
	Bool WriteFlag;							// Флаг разрешения записи журнала в память
	Bool EventFlag;							// Флаг возникновения события
	Bool FirstEvent;						// Флаг первого события после включения питания
	Uns  FaultsCount;						// Количество регистров неисправностей
	Uns  FaultsState[FAULTS_COUNT_DEF];		// Буфер регистров неисправностей
	Uns  FaultsFiltr[FAULTS_COUNT_DEF];		// Буфер фильтрации регистров неисправностей
	Uns  FaultsFiltrPrev[FAULTS_COUNT_DEF];	// Буфер фильтрации регистров неисправностей
	Uns *Time;
	Uns *Date;
	Uns *Status;
	Int *PositionPr;
	Uns *Torque;
	Uns *Ur;
	Uns *Us;
	Uns *Ut;
	Uns *Iu;
	Uns *Iv;
	Uns *Iw;
	Int *Temper;
	Uns *Inputs;
	Uns *Outputs;
	Uns *Seconds;
	Uns Data[LOG_EV_BUF_DATA_COUNT];		// Буфер журнала событий
} TLogEv;



typedef struct _TLogCmd
{
	Bool Enable;							// Флаг разрешения работы журнала
	Bool ExecFlag;							// Флаг формирования буфера журнала
	Bool WriteFlag;							// Флаг разрешения записи журнала
	Uns *Time;								// Указатель на время
	Uns *Date;								// Указатель на дату
	Uns *Seconds;							// Указатель на секунды
	Uns *StatusReg;							// Указатель на статусный регистр блока
	Uns CmdReg;								// Регистр команд
	Uns CmdRegPrev;							// Предыдущее значение регистра команд 
	Uns Data[LOG_CMD_BUF_DATA_COUNT];		// Буфер журнала команд
} TLogCmd;

typedef struct _TLogParam
{
	Bool Enable;							// Флаг разрешения работы журнала
	Bool ExecFlag;							// Флаг формирования буфера журнала
	Bool WriteFlag;							// Флаг разрешения записи
	Uns *Time;								// Указатель на время
	Uns *Date;								// Указатель на дату
	Uns *Seconds;							// Указатель на секунды
	Uns Addr;								// Адрес параметра
	Uns NewValue;							// Новое значение параметра
//	Bool MbChangeParam;
	Uns MbIndex;
	Uns Data[LOG_PARAM_BUF_DATA_COUNT];		// Буфер журнала
	Uns MbBuffer[MB_PARAM_BUF_COUNT];		// Буфер параметров Модбас
} TLogParam;



void LogEvUpdate(TLogEv *);					// Функция формирования журнала событий
void LogCmdUpdate(TLogCmd *);				// Функция формирования журнала команд
void LogParamUpdate(TLogParam *);			// Функция формирования журнала параметров

#endif





