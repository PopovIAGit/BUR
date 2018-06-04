/*======================================================================
Имя файла:          modbus_rtu.h
Автор:              Саидов В.С.
Версия файла:       01.03
Дата изменения:		19/02/10
Применяемость:      Совместно с библиотекой CSL
Описание:
Драйвер коммуникации по протоколу ModBus RTU
======================================================================*/

/*#ifndef MODBUS_RTU_
#define MODBUS_RTU_

#include "responses.h"

#ifdef __cplusplus
extern "C" {
#endif

// Режимы работы
#define MB_SLAVE             0        // Режим ведомого устройства
#define MB_MASTER            1        // Режим ведущего устройства

// Поддерживаемые функции
#define MB_READ_REGS         0x03     // Чтение регистров
#define MB_WRITE_REG         0x06     // Запись регистра
#define MB_DIAGNOSTICS       0x08     // Диагностика
#define MB_WRITE_REGS        0x10     // Запись регистров
#define MB_REPORT_ID         0x11     // Чтение идентификаторов устройства

// Поддерживаемые подфункции для диагностики
#define MB_RET_QUERY_DATA    0x0000   // Вернуть запрашиваемые данные
#define MB_RESTART_COMM      0x0001   // Сброс коммуникации
#define MB_RET_DIAGN_REG     0x0002   // Чтение регистра диагностики
#define MB_FORCE_LISTEN      0x0004   // Переход в режим "Listen Mode Only"
#define MB_CLEAR_DIAGN_REG   0x000A   // Сброс счетчиков и регистра диагностики
#define MB_RET_BUS_MSG       0x000B   // Чтение счетчика сообщений
#define MB_RET_BUS_ERR       0x000C   // Чтение счетчика ошибок связи (CRC)
#define MB_RET_BUS_EXCEPT    0x000D   // Чтение счетчика исключений
#define MB_RET_SLAVE_MSG     0x000E   // Чтение счетчика обработанных сообщений
#define MB_RET_SLAVE_NO_RESP 0x000F   // Чтение счетчика неответов
#define MB_RET_SLAVE_NAK     0x0010   // Чтение счетчика отрицательных подтверждений
#define MB_RET_SLAVE_BUSY    0x0011   // Чтение счетчика занятости устройства
#define MB_RET_BUS_OVERRUN   0x0012   // Чтение счетчика наложения данных
#define MB_CLEAR_OVERRUN     0x0014   // Очистка счетчика наложения и флага ошибки

// Структура параметров драйвера
typedef void (*TMbTrFunc)(Byte);

typedef struct _TMbParams {
	Byte UartID;               // Идентификатор UART-порта
	Byte Mode;                 // Режим работы
	Byte Slave;                // Адрес подчиненного устройства
	Uns  BaudRate;             // Скорость обмена / 100
	Uns  UartBaud;             // Расчитанная скорость для UART
	Byte Parity;               // Режим паритета
	Byte RetryCount;           // Количество повторов передач (в режиме MASTER)
	Byte Scale;                // Частота вызова таймингов / 1000
	Uns  RxDelay;              // Задержка при приеме кадров (в тактах)
	Uns  TxDelay;              // Задержка при передаче кадров (в мсек)
	Uns  ConnTimeout;          // Тайм-аут определения связи (в мсек)
	Uns  AckTimeout;           // Тайм-аут подтверждения (в мсек)
	TMbTrFunc TrEnable;   		// Функция разрешения передачи
} TMbParams;

// Структра пакета данных
typedef struct _TMbPacket {
	Byte Request;              // Код функции запроса
	Byte Response;             // Код функции ответа
	Uns  SubRequest;           // Код подфункции запроса
	Uns  Addr;                 // Начальный адрес данных
	Uns  Data[125];            // Буфер данных
	Uns  Count;                // Количество данных
	Byte Exception;            // Код исключения
	Bool Acknoledge;           // Флаг ожидания подтверждения
} TMbPacket;

// Структура кадра
typedef struct _TMbFrame {
	Bool       ListenMode;     // Признак нахождения в режиме "Listen Mode Only"
	Byte       RetryCounter;   // Количество повторов передач (в режиме MASTER)
	Bool       WaitResponse;   // Флаг ожидания ответа (в режиме MASTER)
	Bool       NewMessage;     // Флаг приема нового кадра
	TTimerList Timer1_5;       // Таймер для 1.5 символа
	TTimerList Timer3_5;       // Таймер для 3.5 символа
	TTimerList TimerPre;       // Таймер для преамбулы
	TTimerList TimerPost;      // Таймер для постамбулы
	TTimerList TimerConn;      // Таймер для определения наличия связи
	TTimerList TimerAck;       // Таймер для для потверждения
	Uns        RxLength;       // Длина принятого кадра
	Uns        TxLength;       // Длина передаваемого кадра
	Byte      *Data;           // Указатель в буфере данных кадара
	Byte       Buf[256];       // Буфер данных кадра
} TMbFrame;

// Струкутра статистики работы
typedef struct _TMbStat {
	Uns DiagnReg;              // Регистр диагностики
	Uns BusMsgCount;           // Счетчик сообщений
	Uns BusErrCount;           // Счетчик ошибок связи
	Uns BusExcCount;           // Счетчик исключений
	Uns SlaveMsgCount;         // Счетчик обработанных сообщений
	Uns SlaveNoRespCount;      // Счетчик неответов
	Uns SlaveNakCount;         // Счетчик отрицательных подтверждений
	Uns SlaveBusyCount;        // Счетчик занятости устройства
	Uns BusParityErrCount;		// Счетчик ошибок бита паритета
	Uns BusOverrunErrCount;    // Счетчик наложения данных
	Uns BusFrameErrCount;		// Счетчик ошибок отсутствия стопового бита
	Uns BusFrameLenErrCount;	// Счетчик ошибок приема слишком коротких кадров
	Uns BusFrameCrcErrCount;	// Счетчик ошибок CRC кадра
	Uns TxMsgCount;            // Счетчик переданных сообщений
	Uns RxBytesCount;          // Принятое количество байт
	Uns TxBytesCount;          // Отправление количество байт
} TMbStat;

// Структура объекта драйвера
typedef struct _TMbPort {
	TMbParams Params;          // Параметры драйвера
	TMbPacket Packet;          // Пакет данных
	TMbFrame  Frame;           // Структура кадра
	TMbStat   Stat;            // Статистика работы
} TMbPort;

typedef struct _TMbPort *TMbHandle;

// Прототипы функций
void ModBusInit(TMbHandle);
void ModBusInvoke(TMbHandle);
void ModBusTimings(TMbHandle);
void ModBusRxIsr(TMbHandle);
void ModBusTxIsr(TMbHandle);

#ifdef __cplusplus
}
#endif // extern "C"

#endif*/



