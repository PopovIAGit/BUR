#include "im.h"
#include "im_params.h"

Uns CurrentLogRec = 0;
Uns CurrentLogCnt = 0;

void ReceiveFunc(TIM *p);
void DownloadFunc(TIM *p);
void SendData(TIM *p);

__inline Bool FuncOne(TIM *p);
__inline Bool FuncTwo(TIM *p);
__inline Bool FuncThree(TIM *p, Byte LogType);
__inline Bool FuncFour(TIM *p, Byte LogType, Byte CellNum);
__inline Bool FuncFive(TIM *p, Byte LogType, Uns RecordNum);
__inline Bool FuncSix(TIM *p, Uns StartParamIndex);

void ImUpdate(TIM *p)
{
	switch (p->ControlMode)
	{
		case RECEIVE_FUNC_MODE:		ReceiveFunc(p);		break;
		case DOWNLOAD_FUNC_MODE: 	DownloadFunc(p);	break;
		case SEND_DATA_MODE:		SendData(p);		break;
	}

	if (++p->ControlMode > SEND_DATA_MODE)
		p->ControlMode = 0;
}

void ImReset(TIM *p)
{
/*
	if (!p->IsLogTransmit)
	{
		memset(&p->RdBuffer, 0, IM_RD_BUFFER_SIZE);
		memset(&p->WrBuffer, 0, IM_WR_BUFFER_SIZE);
	}
*/


	p->Timer = 0;
	p->Index = 0;
}

void ReceiveFunc(TIM *p)
{
	Bool IsFuncReceived = false;

	if (p->CanSendData)
		return;

	switch (p->RdBuffer[0])
	{
		// Device Info
		case 0x01: 	if (p->Index == 1)
					{
						 IsFuncReceived = true;
						 p->FuncState = imDeviceInfo;
					}
					break;
					
		// Summary Logs Info
		case 0x02:	if (p->Index == 1)
					{
						 IsFuncReceived = true;
						 p->FuncState = imSumLogsInfo;
					}
					break;
					
		// Concrete Logs Info
		case 0x03:	if (p->Index == 2)
					{
						 IsFuncReceived = true;
						
						// В зависимости от типа журнала
						 p->FuncState = imEvLogInfo + (p->RdBuffer[1] - 1);
					}
					break;
		
		// Logs Parameters Addresses
		case 0x04:	if (p->Index == 3)
					{
						 IsFuncReceived = true;
						 						 	
						 // В зависимости от типа журнала и номера ячейки					
						if ((p->RdBuffer[1] == IM_LOGEV_TYPE) && (p->RdBuffer[2] == 1))
								p->FuncState = imEvLogMainAddr;
						else if ((p->RdBuffer[1] == IM_LOGEV_TYPE) && (p->RdBuffer[2] > 1))
							p->FuncState = imEvLogBufAddr;
						else if (p->RdBuffer[1] == IM_LOGCMD_TYPE)
							p->FuncState = imCmdLogAddr;
						else if (p->RdBuffer[1] == IM_LOGPARAMS_TYPE)
							p->FuncState = imParLogAddr;
					}
					break;
					
		// Logs Data
		case 0x05:	if (p->Index == 4)
					{
						IsFuncReceived = true;
						p->FuncState = imLogDownload;

						CurrentLogCnt = (p->RdBuffer[2] << 8) | (p->RdBuffer[3]);
						CurrentLogRec = 0;
						p->CanReadNextRec = true;
					}

					break;

		// Parameters
		case 0x06:	if (p->Index == 3)
					{
						 IsFuncReceived = true;
						 p->FuncState = imParamDownload;
					}

					break;
		default: return;
	}

	if (IsFuncReceived)
	{
		memcpy(&p->WrBuffer, p->RdBuffer, p->Index);

		p->RdBuffer[0] = 0;
	}
}

void DownloadFunc(TIM *p)
{
		switch (p->FuncState)
		{
			case imInit:				p->EnableReceive();
										ImReset(p);
										p->FuncState = imIdle;
			break;

			case imDeviceInfo:			p->CanSendData = FuncOne(p);
			break;

			case imSumLogsInfo:			p->CanSendData = FuncTwo(p);
			break;

			case imEvLogInfo:			p->CanSendData = FuncThree(p, IM_LOGEV_TYPE);
			break;
			
			case imCmdLogInfo:			p->CanSendData = FuncThree(p, IM_LOGCMD_TYPE);
			break;

			case imParamLogInfo:		p->CanSendData = FuncThree(p, IM_LOGPARAMS_TYPE);
			break;

			case imEvLogMainAddr:		p->CanSendData = FuncFour(p, IM_LOGEV_TYPE, 1);
			break;

			case imEvLogBufAddr:		p->CanSendData = FuncFour(p, IM_LOGEV_TYPE, 2);
			break;

			case imCmdLogAddr:			p->CanSendData = FuncFour(p, IM_LOGCMD_TYPE, 1);
			break;

			case imParLogAddr:			p->CanSendData = FuncFour(p, IM_LOGPARAMS_TYPE, 1);
			break;

			case imLogDownload:

				if ((CurrentLogCnt > 0) && !p->CanSendData)
				{
					p->IsLogTransmit = true;
					p->CanSendData = FuncFive(p, p->RdBuffer[1], CurrentLogRec);
				}
			break;

			case imParamDownload:
				p->CanSendData = FuncSix(p, (p->RdBuffer[1] << 8) | (p->RdBuffer[2]));
			break;
			
		}

		if (p->CanSendData)
		{
			if (!p->IsLogTransmit)	
				p->FuncState = imIdle;

			p->Timer = p->Period;
			p->EnableTransmit();
		}


		//p->Timer = p->Period;
}

void ImReceiveData(TIM *p, Byte Data)
{
	p->RdBuffer[p->Index] = Data;

	if (p->Index < IM_RD_BUFFER_SIZE)
		p->Index++;

	// На всякий случай заводим таймер сброса
	p->Timer = p->Period;
}



void SendData(TIM *p)
{
	Byte Data = 0;

	if (!p->CanSendData)
		return;
	
	if (!(*p->IsTxBusy))
	{
		if (p->TxIndex > p->Index)
		{
			p->TxIndex = 0;
			p->Index = 0;
			p->CanSendData = false;
			p->Timer = 0;
			p->Index = 0;

			//p->IsLogTransmit = false;

			if (!p->IsLogTransmit)
				p->EnableReceive();

			//p->FuncState = imInit;
		}
		else
		{
			p->Timer = p->Period;
			*p->IsTxBusy = true;
			Data = p->WrBuffer[p->TxIndex++];
			p->TransmitByte(Data);
		}
	}
}

__inline Bool FuncOne(TIM *p)
{
	Uns StartIndex = p->Index;
	Uns CurrentIndex = StartIndex + 1;
	
	p->WrBuffer[CurrentIndex++] = IM_FIRMWARE_VERSION >> 8;
	p->WrBuffer[CurrentIndex++] = IM_FIRMWARE_VERSION & 0xFF;
	p->WrBuffer[CurrentIndex++] = IM_DEVICE_ID >> 8;
	p->WrBuffer[CurrentIndex++] = IM_DEVICE_ID & 0xFF;
	p->WrBuffer[CurrentIndex++] = IM_PARAMS_CNT >> 8;
	p->WrBuffer[CurrentIndex++] = IM_PARAMS_CNT & 0xFF;
	p->WrBuffer[CurrentIndex++] = IM_LOGS_CNT;
	p->WrBuffer[CurrentIndex++] = IM_LOGEV_TYPE;
	p->WrBuffer[CurrentIndex++] = IM_LOGCMD_TYPE;
	p->WrBuffer[CurrentIndex] 	= IM_LOGPARAMS_TYPE;

	p->WrBuffer[StartIndex]		= CurrentIndex - StartIndex;
	p->Index = CurrentIndex;

	return true;
}

__inline Bool FuncTwo(TIM *p)
{
	Uns StartIndex = p->Index;
	Uns CurrentIndex = StartIndex + 1;

	p->WrBuffer[CurrentIndex++] = IM_LOGEV_REC_ADDR >> 8;
	p->WrBuffer[CurrentIndex++] = IM_LOGEV_REC_ADDR & 0xFF;
	p->WrBuffer[CurrentIndex++] = IM_LOGEV_REC_CNT >> 8;
	p->WrBuffer[CurrentIndex++] = IM_LOGEV_REC_CNT & 0xFF;

	p->WrBuffer[CurrentIndex++] = IM_LOGCMD_REC_ADDR >> 8;
	p->WrBuffer[CurrentIndex++] = IM_LOGCMD_REC_ADDR & 0xFF;
	p->WrBuffer[CurrentIndex++] = IM_LOGCMD_REC_CNT >> 8;
	p->WrBuffer[CurrentIndex++] = IM_LOGCMD_REC_CNT & 0xFF;

	p->WrBuffer[CurrentIndex++] = IM_LOGPAR_REC_ADDR >> 8;
	p->WrBuffer[CurrentIndex++] = IM_LOGPAR_REC_ADDR & 0xFF;
	p->WrBuffer[CurrentIndex++] = IM_LOGPAR_REC_CNT >> 8;
	p->WrBuffer[CurrentIndex]	= IM_LOGPAR_REC_CNT & 0xFF;

	p->WrBuffer[StartIndex]		= CurrentIndex - StartIndex;
	p->Index = CurrentIndex;

	return true;
}



__inline Bool FuncThree(TIM *p, Byte LogType)
{
	Uns StartIndex = p->Index;
	Uns CurrentIndex = StartIndex + 1;

	Uns i;

	switch (LogType)
	{
		case IM_LOGEV_TYPE: 
			// Количество ячеек = основная + буферные
			p->WrBuffer[CurrentIndex++] = 1 + IM_LOGEV_BUF_CELL_CNT;
			p->WrBuffer[CurrentIndex++] = IM_LOGEV_MAIN_CELL_DATA_CNT;

			for (i = 0; i < IM_LOGEV_BUF_CELL_CNT; i++)
			{
				p->WrBuffer[CurrentIndex++] = IM_LOGEV_BUF_CELL_DATA_CNT;
			}

			CurrentIndex--;							// В цикле было лишнее инкрементирование
					
		break;

		case IM_LOGCMD_TYPE:
		
			p->WrBuffer[CurrentIndex++] = 1;
			p->WrBuffer[CurrentIndex] = LOG_CMD_DATA_CNT; 
		
		break;

		case IM_LOGPARAMS_TYPE:
			p->WrBuffer[CurrentIndex++] = 1;
			p->WrBuffer[CurrentIndex] = LOG_PARAM_DATA_CNT;
		
		break;
	}

	p->WrBuffer[StartIndex] = CurrentIndex - StartIndex;
	p->Index = CurrentIndex;

	return true;
}


__inline Bool FuncFour(TIM *p, Byte LogType, Byte CellNum)
{
	Uns *table;	Uns size = 0; Uns index = 0; Uns addr = 0;
	Uns StartIndex = p->Index;
	Uns CurrentIndex = StartIndex + 1;

	switch (LogType)
	{
		case IM_LOGEV_TYPE:
								if (CellNum == 1)
								{
									table = (Uns *)&ImEvLogMainAddrsTable;
									size = sizeof(ImEvLogMainAddrsTable);
								}
								else
								{
									table = (Uns *)&ImEvLogBufAddrsTable;
									size = sizeof(ImEvLogBufAddrsTable);
								}
		break;

		case IM_LOGCMD_TYPE:	
								table = (Uns *)&ImCmdLogAddrsTable;
								size = sizeof(ImCmdLogAddrsTable);
		break;

		case IM_LOGPARAMS_TYPE:	
								table = (Uns *)&ImParamLogAddrsTable;
								size = sizeof(ImParamLogAddrsTable);		
		break;

	}

	for (index = 0; index < size; index++)
	{
		addr = table[index];
		p->WrBuffer[CurrentIndex++] = addr >> 8;
		p->WrBuffer[CurrentIndex++] = addr & 0xFF;
	}

	CurrentIndex--;

	p->WrBuffer[StartIndex] = CurrentIndex - StartIndex;
	p->Index = CurrentIndex;

	return true;
}



__inline Bool FuncFive(TIM *p, Byte LogType, Uns RecordNum)
{
	Uns StartIndex = p->Index;
	Uns CurrentIndex = StartIndex;
	Uns CellIndex = 0;	Uns ImIndex = 0; Uns i;

	ImBufferReader(LogType, RecordNum);

	if (p->IsBufReady)
	{
		p->IsBufReady = false;

		// Добавляем номер текущей записи
		p->WrBuffer[CurrentIndex++] = CurrentLogRec >> 8;
		p->WrBuffer[CurrentIndex++] = CurrentLogRec & 0xFF;

		switch (LogType)
		{
			case IM_LOGEV_TYPE:

				for (CellIndex = 1; CellIndex <= (IM_LOGEV_BUF_CELL_CNT + 1); CellIndex++)
				{

					// Основная ячейка
					if (CellIndex == 1)
					{
						if (!(CurrentLogRec & 0x01))
						{	ImIndex = 0;	}
						else
						{	ImIndex = IM_DOWNLOAD_BUFFER_SIZE;	}

						for (i = 0; i < IM_LOGEV_MAIN_CELL_DATA_CNT; i++)
						{
							p->WrBuffer[CurrentIndex++] = p->DownloadBuffer[ImIndex + i] >> 8;
							p->WrBuffer[CurrentIndex++] = p->DownloadBuffer[ImIndex + i] & 0xFF;
						}
					}
					else if (CellIndex >= 2)
					{
				
						for (i = 0; i < IM_LOGEV_BUF_CELL_DATA_CNT; i++)
						{

							if (!(CurrentLogRec & 0x01))
							{	ImIndex = IM_LOGEV_MAIN_CELL_DATA_CNT + (IM_LOGEV_BUF_CELL_DATA_CNT * (CellIndex - 2));	}
							else
							{	ImIndex = IM_LOGEV_MAIN_CELL_DATA_CNT + IM_DOWNLOAD_BUFFER_SIZE + (IM_LOGEV_BUF_CELL_DATA_CNT * (CellIndex - 2));	}
								
							p->WrBuffer[CurrentIndex++] = p->DownloadBuffer[ImIndex + i] >> 8;
							p->WrBuffer[CurrentIndex++] = p->DownloadBuffer[ImIndex + i] & 0xFF;
						}
					}

					//DbgCellCnt = (IM_LOGEV_BUF_CELL_CNT + 1);
				}

			break;

			case IM_LOGCMD_TYPE:

				if (!(CurrentLogRec & 0x01))
				{	ImIndex = 0;	}
				else
				{	ImIndex = IM_LOGCMD_CELL_DATA_CNT;	}

				for (i = 0; i < IM_LOGCMD_CELL_DATA_CNT; i++)
				{
					p->WrBuffer[CurrentIndex++] = p->DownloadBuffer[ImIndex + i] >> 8;
					p->WrBuffer[CurrentIndex++] = p->DownloadBuffer[ImIndex + i] & 0xFF;
				}
				
			break;

			case IM_LOGPARAMS_TYPE:

				if (!(CurrentLogRec & 0x01))
				{	ImIndex = 0;	}
				else
				{	ImIndex = IM_LOGPAR_CELL_DATA_CNT;	}

				for (i = 0; i < IM_LOGPAR_CELL_DATA_CNT; i++)
				{
					p->WrBuffer[CurrentIndex++] = p->DownloadBuffer[ImIndex + i] >> 8;
					p->WrBuffer[CurrentIndex++] = p->DownloadBuffer[ImIndex + i] & 0xFF;
				}

			break;
		}

		CurrentIndex--;								// В циклах лишнее инкрементирование

		p->Index = CurrentIndex;

		if (++CurrentLogRec < CurrentLogCnt)
		{
			p->CanReadNextRec = true;
		}
		else
		{
			p->CanReadNextRec = false;
			p->IsLogTransmit = false;
			p->FuncState = imInit;
		}

		return true;
	}
		
	return false;

}	  


__inline Bool FuncSix(TIM *p, Uns StartParamIndex)
{
	Uns CurrentIndex = p->Index;
	Uns i;

	for (i = 0; i < IM_PARAM_DOWNLOAD_NUM; i++)
	{
		p->WrBuffer[CurrentIndex++] = (p->DeviceDataPtr[StartParamIndex + i]) >> 8;
		p->WrBuffer[CurrentIndex++] = (p->DeviceDataPtr[StartParamIndex + i]) & 0xFF;
	}

	CurrentIndex--;

	p->Index = CurrentIndex;

	return true;	
}


void ImTimer(TIM *p)
{
	if (p->Timer > 0)	p->Timer--; 

	// Сброс инф. модуля по таймауту
	if (p->Timer == 1)
	{
		p->FuncState = imInit;

		CurrentLogCnt = 0;
		CurrentLogRec = 0;

		p->IsLogTransmit = false;
	}
}
