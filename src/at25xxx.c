#include "at25xxx.h"
#include "csl\csl_spi.h"
#include "csl\csl_memory.h"


// Коды функций
#define WRSR		0x01
#define WRITE		0x02
#define READ		0x03
#define WRDI		0x04
#define RDSR		0x05
#define WREN 		0x06

// Макросы выбора микросхем
#define SetCs()  p->CsFunc(0)
#define ClrCs()  p->CsFunc(1)

Uns MemDbg = 0;

void AT25XXX_Init(AT25XXX *p)
{
	p->BusyTimer = 0;
	SPI_init(p->SpiId, SPI_MASTER, 0, p->SpiBaud, 8);
	if (AT25XXX_ReadStatus(p)) p->Error = TRUE;				// Статусный регистр должен быть пустой
	asm(" NOP");
}

void AT25XXX_Update(AT25XXX *p)
{
	register Uns Tmp;

	if (!p->Func) {p->State = 0; return;}
	
	switch (++p->State)
	{
		case 1:
			if (p->Func & F_GETSTATUS)    p->State = 1;						// Проверяем какие функции присутствуют и назначаем State
			else if (p->Func & F_READ)    p->State = 2;						// Состояния на 1 больше, т.к. каждый проход switch'а, состояния инкрементируются
			else if (p->Func & F_WRITE)   p->State = 3;
			else {p->Func = 0; p->State = 0;}								// Если функций нет, то обнуляем State
			break;
//-----------------------------------------------------------------
		case 2:																// Проверка статуса
			SPI_init(p->SpiId, SPI_MASTER, 0, p->SpiBaud, 8);
			Tmp = AT25XXX_ReadStatus(p);									// Читаем статус
			
			if (!(p->Func & F_WRITE)) {p->RdData = Tmp; p->Func = 0;}		// Отсутствует функция записи, записываем байт статуса в RdData, обнуляем функции
			else if (!(Tmp & 0x01)) p->Func &= ~F_GETSTATUS;				// Проверяем бит RDY в статусном регистре, если отсутствует, то устройство готово для записи, отсекаем функцию чтения статуса
			else if (++p->BusyTimer < p->BusyTime) {p->State = 1; break;}	// Если функция записи и бит RDY выставлен (устройство еще занято), то проверяем таймер занятости устройства																			
			else {p->Error = TRUE; p->Func = 0;}							// В остальных случаях - ошибка
			
			MemDbg++;
			p->State = 0;
			break;
//-----------------------------------------------------------------
		case 3:															// Чтение байта
			SPI_init(p->SpiId, SPI_MASTER, 0, p->SpiBaud, 8);
			Tmp = AT25XXX_ReadByte(p, p->Addr);							// Считываем байт по указанному адресу во временную переменную

			if (!(p->Func & F_WRITE))									// Функция записи отсутствует, значит читаем байт
			{
				if (!(p->Addr & 0x1)) p->RdData = Tmp;					// Текущий адрес четный, значит младшие биты. Записываем в RdData
				else
				{
					p->RdData = (Tmp << 8) | p->RdData;					// Адрес нечетный. Биты старшие. Склеиваем младшие и старшие биты в RdData. Если не будет работать, то нужно будет сделать преведение к Uns
					*((Uns *)p->Buffer) = p->RdData;					// Приводим тип указателя к Uns и записываем данные из RdData в текущую ячейку буфера
				}
			}
			else if (Tmp != p->WrData)									// Если функция записи и считанное значение по адресу, по которому недавно проходила запись не совпадает с данными для записи
			{
				if (++p->RetryTimer < p->RetryCount) p->Func &= ~F_READ;	// Инкрементируем счетчик попыток и проверяем, если ли еще попытки. Если есть, то отсекаем функцию чтения,
																			// чтобы потом по статусу проверить готовность устройства и повторить цикл записи
				else {p->Error = TRUE; p->Func = 0;}						// Если попытки на перезапись кончились, то выставляем ошибку
				p->State = 0;
				break;														// Выходим из case
			}
			else {p->RetryTimer = 0; p->Func &= ~F_READ;}					// В остальных случаях обнуляем счетчик попыток и отсекаем функцию чтения
			
			if (p->Count > 1)												// Есть еще байты для записи
			{
				if (p->Buffer && (p->Addr & 0x1))	p->Buffer++;			// Проверяем, что буфер не пустой и что адрес нечетный (первыми считываются младшие биты, затем старшие)
				p->Addr++;
				p->Count--;
			}
			else
			{
				p->RetryTimer = 0;
				p->Count = 0;
				p->Func = 0;
			}

			p->State = 0;
			break;
//-------------------------------------------------------------------------
		case 4:
			if (!p->Buffer) Tmp = 0;					// Буфер пуст
			else Tmp = *((Uns *)p->Buffer);				// Записываем данные из буфера во временную переменную

			if (!(p->Addr & 0x1)) 						// Если 1 - то старшие биты, если 0, то младшие.
				 p->WrData = (Byte)(Tmp & 0xFF);		// Счет адресов идет вверх. Такого - 0011 0000 0000 быть не должно.
														// Продавливаем по маске, для отсечения старших бит и приводим тип к 8-битному
			else p->WrData = (Byte)(Tmp >> 8);			// Сдвигаем старшую часть и приводим к 8-битному типу

			SPI_init(p->SpiId, SPI_MASTER, 0, p->SpiBaud, 8);			// Инициализируем SPI
			AT25XXX_WriteEnable(p);										// Посылаем запрос на разрешение записи
			break;
		case 5:
			SPI_init(p->SpiId, SPI_MASTER, 0, p->SpiBaud, 8);			// Инициализируем SPI
			AT25XXX_WriteByte(p, p->Addr, p->WrData);					// Пишем байт
			p->Func |= (F_GETSTATUS|F_READ);							// Посылка на запись прошла, теперь необходимо проверить готовность устройва
																		// по статусному регистру, а так же считать записанные данные для проверки
			p->BusyTimer = 0;											// Сбрасываем таймер, т.к. будем заново проводить проверку готовности
			p->State = 0;
			break;

	}


}



Byte AT25XXX_ReadStatus(AT25XXX *p)
{
	Byte Status;
	
	SetCs();
	SPI_send(p->SpiId, RDSR);								// Посылаем код функции на чтение статуса
	Status = SPI_send(p->SpiId, 0);							// Принимаем биты статусного регистра
	ClrCs();
	
	return Status;
}

void AT25XXX_WriteStatus(AT25XXX *p, Byte Status)
{
	SetCs();
	SPI_send(p->SpiId, WRSR);								// Посылаем код функции на запись в статусный регистр
	SPI_send(p->SpiId, Status);								// Посылаем данные для записи в статусный регистр
	ClrCs();
}

void AT25XXX_WaitBusy(AT25XXX *p)
{
	while(AT25XXX_ReadStatus(p) & AT25XXX_RDY);				// Крутимся в цикле до тех пор пока не станет равным 0
}

Byte AT25XXX_ReadByte(AT25XXX *p, Uns Addr)
{
	Byte Data;
	
	SetCs();
	SPI_send(p->SpiId, READ);								// Посылаем код функции на чтение байта
	SPI_send(p->SpiId, Addr >> 8);							// Посылаем сперва старшие биты адреса
	SPI_send(p->SpiId, Addr);						// Затем младшие биты адреса
	Data = SPI_send(p->SpiId, 0);
	ClrCs();

	return Data;											// Возвращаем прочтенный байт данных
}

void AT25XXX_WriteByte(AT25XXX *p, Uns Addr, Byte Data)
{
	SetCs();
	DelayUs(250);
	SPI_send(p->SpiId, WRITE);								// Посылаем код функции на запись
	SPI_send(p->SpiId, Addr >> 8);							// Посылаем сперва старшие биты адреса
	SPI_send(p->SpiId, Addr);								// Затем младшие биты адреса
	SPI_send(p->SpiId, Data);								// Посылаем байт на запись
	ClrCs();
}

void AT25XXX_WriteEnable(AT25XXX *p)
{
	SetCs();
	SPI_send(p->SpiId, WREN);								// Посылаем код функции на включение записи
	ClrCs();
}

void AT25XXX_WriteDisable(AT25XXX *p)
{
	SetCs();
	SPI_send(p->SpiId, WRDI);								// Посылаем код функции на запрещение записи
	ClrCs();
}

