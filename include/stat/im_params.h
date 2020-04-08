#ifndef _IM_PARAMS_H_
#define _IM_PARAMS_H_

#include "config.h"



#define IM_FIRMWARE_VERSION		(DEVICE_GROUP * 1000) + VERSION //VERSION					// Версия ПО
#define IM_DEVICE_ID			DEVICE_ID				// Идентификатор устройства
#define IM_PARAMS_CNT			RAM_DATA_SIZE			// Количество параметров устройства
#define IM_LOGS_CNT				LOGS_CNT				// Количество журналов устройства

#define IM_LOGEV_REC_ADDR		GrH->LogEvAddr			// Текущий адрес журнала событий
#define IM_LOGCMD_REC_ADDR		GrH->LogCmdAddr			// Текущий адрес журнала команд
#define IM_LOGPAR_REC_ADDR		GrH->LogParamAddr			// Текущий адрес журнала изм. параметров

#define IM_LOGEV_REC_CNT		GrH->LogEvCount			// Количество записей журнала событий
#define IM_LOGCMD_REC_CNT		GrH->LogCmdCount		// Количество записей журнала команд
#define IM_LOGPAR_REC_CNT		GrH->LogParamCount		// Количество записей журнала изм. параметров

#define IM_LOGEV_BUF_CELL_CNT			LOG_EV_BUF_CELL_COUNT		// Количество ячеек буфера журнала событий

#define IM_LOGEV_MAIN_CELL_DATA_CNT		LOG_EV_DATA_CNT				// Количество полей в основной ячейке журнала событий
#define IM_LOGEV_BUF_CELL_DATA_CNT		LOG_EV_BUF_DATA_CNT			// Количество полей в буферной ячейке журнала событий
#define IM_LOGCMD_CELL_DATA_CNT			LOG_CMD_DATA_CNT			// Количество полей в ячейке журнала команд
#define IM_LOGPAR_CELL_DATA_CNT			LOG_PARAM_DATA_CNT			// Количетсов полей в ячейке журнала изм. параметров

#define IM_DOWNLOAD_BUFFER_SIZE			IM_READ_BUF_SIZE

#define IM_PARAM_DOWNLOAD_NUM			20

#define NEW_PARAM_ADDR					60000
#define NEW_PARAM_VALUE_ADDR			60001
#define FIRST_RECORD_FLAG_ADDR			60002

//Uns *DeviceDataPtr 			= ((Uns *)&Ram);

Uns ImEvLogMainAddrsTable[]	= {
								GetAdr(GroupB.DevTime),
								GetAdr(GroupB.DevDate),
								GetAdr(GroupA.Status),
								GetAdr(GroupA.Faults.Proc),
								GetAdr(GroupA.Faults.Net),
								GetAdr(GroupA.Faults.Load),
								GetAdr(GroupA.Faults.Dev),
								GetAdr(GroupA.PositionPr),
								GetAdr(GroupA.Torque),
								GetAdr(GroupA.Ur),
								GetAdr(GroupA.Us),
								GetAdr(GroupA.Ut),
								GetAdr(GroupA.Iu),
								GetAdr(GroupA.Iv),
								GetAdr(GroupA.Iw),
								GetAdr(GroupA.Temper),
								GetAdr(GroupA.Inputs),
								GetAdr(GroupA.Outputs),
								GetAdr(GroupH.Seconds),
								FIRST_RECORD_FLAG_ADDR
};

Uns ImEvLogBufAddrsTable[] = {
								GetAdr(GroupA.Status),
								GetAdr(GroupA.PositionPr),
								GetAdr(GroupA.Torque),
								GetAdr(GroupA.Ur),
								GetAdr(GroupA.Us),
								GetAdr(GroupA.Ut),
								GetAdr(GroupA.Iu),
								GetAdr(GroupA.Iv),
								GetAdr(GroupA.Iw),
								GetAdr(GroupA.Temper),
								GetAdr(GroupA.Inputs),
								GetAdr(GroupA.Outputs)
};

Uns ImCmdLogAddrsTable[] = {
								GetAdr(GroupB.DevTime),
								GetAdr(GroupB.DevDate),
								GetAdr(GroupH.Seconds),
								GetAdr(GroupH.LogControlWord),
								GetAdr(GroupA.Status),
								GetAdr(GroupA.Outputs.all),
								GetAdr(GroupA.PosFix)
};

Uns ImParamLogAddrsTable[] = {
								GetAdr(GroupB.DevTime),
								GetAdr(GroupB.DevDate),
								GetAdr(GroupH.Seconds),
								NEW_PARAM_ADDR,
								NEW_PARAM_VALUE_ADDR
};



#endif

