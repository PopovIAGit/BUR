#ifndef _RIM_DEVICES_H_
#define _RIM_DEVICES_H_

#include "at25xxx.h"
#include "display.h"
#include "displhal.h"
#include "dac7513.h"
#include "encoder.h"
#include "adt7301.h"
//#include "ds1305.h" // Заменено на ds3234
#include "ds3234.h"
#include "pult.h"
#include "Inputs.h"		// Обработка входных сигналов


// Определение ножек процессора
#define ENB_TRN			GpioDataRegs.GPBDAT.bit.GPIO32
#define CS_AT			GpioDataRegs.GPADAT.bit.GPIO23
#define RS_DIR			GpioDataRegs.GPADAT.bit.GPIO24
#define IR_IN			GpioDataRegs.GPADAT.bit.GPIO26
#define TEN_OFF			GpioDataRegs.GPADAT.bit.GPIO27
#define TEN_ON_STATUS	GpioDataRegs.GPADAT.bit.GPIO13

#define CS_PORT			GpioDataRegs.GPADAT.all
#define CS0				(1UL<<31)
#define CS4				(1UL<<5)
#define CS2				(1UL<<12)
#define CS3				(1UL<<30)
#define CS_MASK			(CS0|CS4|CS2|CS3)


#define LCD_PORT		GpioDataRegs.GPADAT.all
#define LCD_RW			7
#define LCD_EN			9
#define LCD_RS			19
//#define LCD_ON			GpioDataRegs.GPBDAT.bit.GPIO???

#define THYR_PORT		GpioDataRegs.GPADAT.all
#define THYR_MASK		0x1F
/*
#define ADC_IW			AdcRegs.ADCRESULT2
#define ADC_IV			AdcRegs.ADCRESULT0
#define ADC_IU			AdcRegs.ADCRESULT1

#define ADC_UR			AdcRegs.ADCRESULT3
#define ADC_US			AdcRegs.ADCRESULT5
#define ADC_UT			AdcRegs.ADCRESULT4

#define ADC_IW			AdcRegs.ADCRESULT1
#define ADC_IV			AdcRegs.ADCRESULT0
#define ADC_IU			AdcRegs.ADCRESULT2

#define ADC_UR			AdcRegs.ADCRESULT3
#define ADC_US			AdcRegs.ADCRESULT5
#define ADC_UT			AdcRegs.ADCRESULT4
*/
// 
#define ADC_IW			AdcRegs.ADCRESULT0
#define ADC_IV			AdcRegs.ADCRESULT2
#define ADC_IU			AdcRegs.ADCRESULT1

#define ADC_UR			AdcRegs.ADCRESULT3
#define ADC_US			AdcRegs.ADCRESULT4
#define ADC_UT			AdcRegs.ADCRESULT5 

#define ADC_IU_MIN		AdcRegs.ADCRESULT13
#define ADC_IV_MIN		AdcRegs.ADCRESULT15
#define ADC_IW_MIN		AdcRegs.ADCRESULT14

#define HALL_SENS1		AdcRegs.ADCRESULT8
#define HALL_SENS2		AdcRegs.ADCRESULT9
#define HALL_SENS3		AdcRegs.ADCRESULT10
#define HALL_SENS4		AdcRegs.ADCRESULT11

#define DRIVE_TEMPER	AdcRegs.ADCRESULT6

#define POWER_CONTROL 	GpioDataRegs.GPADAT.bit.GPIO13 

// Константы
#define MB_SCIID		SCIB

// Глобальные константы
#define PLIS_SPI			SPIA
#define PLIS_BRR			SPI_BRR(4000)
#define REV_MAX				0x3FFF

// Дискретные входы
#define DIN_REVERS		0
#define SBEXT_OPEN		0
#define SBEXT_CLOSE		1
#define SBEXT_STOP		2
#define SBEXT_MU		3
#define SBEXT_DU		4

#define SBEXT_TEN		7

// обработка датчиков холла
#define BTN_OPEN			BIT0
#define BTN_CLOSE			BIT1
#define BTN_STOP_MU			BIT2
#define BTN_STOP_DU			BIT3
#define BTN_STOP			(BTN_STOP_MU|BTN_STOP_DU)
#define BTN_LEVEL			1000


#define CS_RESET			0x00
#define CS_LCD				0x01		// Дисплей
#define CS_LED				0x02		// Светодиоды
#define CS_EE1				0x03		// Eeprom1
#define CS_EE2				0x04		// Eeprom2
#define CS_RTC				0x05		// Часы
#define CS_TEMP				0x06		// Температурный датчик
#define CS_ENC				0x07		// Энкодер
#define CS_AOUT				0x08		//
#define CS_LCD_ON			0x0A		// LCD ON/OFF
#define CS_NONE				0x0F		// Нет микросхемы


// Функция выбора микросхем
extern void SetPlisAddr(Uns Addr);
__inline void Eeprom1CsSet(Byte Lev)  	{SetPlisAddr(!Lev ? CS_EE1    : CS_NONE);}
__inline void Eeprom2CsSet(Byte Lev)  	{SetPlisAddr(!Lev ? CS_EE2    : CS_NONE);}
__inline void DisplayCsSet(Byte Lev) 	{SetPlisAddr(!Lev ? CS_LCD    : CS_NONE);}
__inline void TsensCsSet(Byte Lev)   	{SetPlisAddr(!Lev ? CS_TEMP   : CS_NONE);}
__inline void RtcCsSet(Byte Lev)     	{SetPlisAddr(!Lev ? CS_RTC    : CS_NONE);}
__inline void DacCsSet(Byte Lev)     	{SetPlisAddr(!Lev ? CS_AOUT   : CS_NONE);}
__inline void EncCsSet(Byte Lev)     	{SetPlisAddr(!Lev ? CS_ENC    : CS_NONE);}
__inline void AtCsSet(Byte Lev)		 	{CS_AT = Lev;}

// Значения инициализации для структур

#define AT25XXX_DEFAULT1 { \
	PLIS_SPI, SPI_BRR(4000), \
	0, 0, 0, Null, 0, 0, 0, False, \
	0, EEPROM_RETRY,\
	0, WRITE_TIME,\
	Eeprom1CsSet\
}

#define AT25XXX_DEFAULT2 { \
	PLIS_SPI, SPI_BRR(4000), \
	0, 0, 0, Null, 0, 0, 0, False, \
	0, EEPROM_RETRY,\
	0, WRITE_TIME,\
	Eeprom2CsSet\
}

#define DISPLAY_DEFAULT { \
	PLIS_SPI, SPI_BRR(4000), \
	False, False, False, 0, 0, 0, \
	0, 0, 0, "", "", Null, WINSTAR_RusTable, &Ram.GroupC.IndicatorType,\
	IO_INIT(LCD_PORT, LCD_EN), \
	IO_INIT(LCD_PORT, LCD_RS), \
	IO_INIT(LCD_PORT, LCD_RW), \
	Null, 0, 0UL, \
	DisplayCsSet \
}

#define ADT7301_DEFAULT { \
	PLIS_SPI, SPI_BRR(2000), \
	&BlockTemper, 0, TSENS_RETRY, False, \
	TsensCsSet \
}

#define DS1305_DEFAULT { \
	PLIS_SPI, SPI_BRR(2000), \
	ToPtr(&Rtc), \
	0, 0, 0, False, False, False, \
	RtcCsSet \
}

#define DS3234_DEFAULT { \
	PLIS_SPI, SPI_BRR(2000), \
	ToPtr(&Rtc), \
	0, 0, 0, False, False, False, \
	RtcCsSet \
}

#define RTC_DEFAULT { 0,0,0,1,1,1,0,0xFF }

#define DAC7513_DEFAULT { \
	SPIC, SPI_BRR(4000), \
	DAC7513_DIRECT, DAC7513_NORMAL, \
	0, &CsAtBusy, 0, 0, 0, \
	DacCsSet \
}

#define ENCODER_DEFAULT { \
	PLIS_SPI, SPI_BRR(1000), \
	14, 0, REV_MAX, 0UL, \
	&Ram.GroupC.RevErrValue, \
	&Ram.GroupC.RevErrLevel, \
	0, 0UL, 5, 3, 0, 0, 0, false, false, \
	EncCsSet \
}


#define PULT_DEFAULTS		{	\
		false, false, 0, 0, 0, 0, 0 	\
}



#define BTN_DEFAULT(Bit, Match) { \
	ltAnMin, True, \
	Null, &BtnStatus, \
	&BtnLevel, &BtnTout, \
	(1<<Bit), 1, 0, Match, False \
}

#define TU_DEFAULT(Port, Index, Bit) { \
	ltDigital, True, \
	ToPtr(Port), &Mcu.Tu.State, \
	DIN_LEVEL(Index, 1), &Ram.GroupB.TuTime, \
	(1<<Bit), TU_SCALE, 0, False, False \
}

#define TU_DEFAULT2(Port, Index, Bit) { \
	ltDigital, True, \
	ToPtr(Port), &TempMuDu, \
	DIN_LEVEL(Index, 1), &Ram.GroupB.TuTime, \
	(1<<Bit), TU_SCALE, 0, False, False \
}

// Переопределение для работы с памятью
#define MemPar	Eeprom1								// Структуры памяти параметров
#define MemLog	Eeprom2
#define ParFunc	EEPROM_Func							// Функция для работы с данными структуры параметров
#define LogFunc EEPROM_Func


// Глобальные структуры
extern AT25XXX 		Eeprom1;
extern AT25XXX		Eeprom2;
extern ADT7301		TempSens;
extern ENCODER		Encoder;
extern DAC7513  	Dac;
extern TDisplay 	Display;
//extern DS1305		Ds1305;
extern DS3234		Ds3234;
extern RTC_Obj		Rtc;
extern Bool 		RtcStart;
extern TPult 		Pult;
extern Int  		BlockTemper;
extern Uns  		BtnStatus;
extern Uns  		BtnTout;
extern Uns  		BtnLevel;
extern Uns  		ExtReg;
extern Byte 		RimRefrState;
extern LOG_INPUT	BtnOpen;
extern LOG_INPUT	BtnClose;
extern LOG_INPUT	BtnStop_MU;
extern LOG_INPUT	BtnStop_DU;
extern LOG_INPUT	TuOpen;
extern LOG_INPUT	TuClose;
extern LOG_INPUT	TuStop;
extern LOG_INPUT	TuMu;
extern LOG_INPUT	TuDu;
//--- Тест ручек ---
extern Uns 			Test_Open;
extern Uns 			Test_Close;
extern Uns 			Test_Stop1;
extern Uns 			Test_Stop2;

// Прототипы функций
void RimDevicesInit(void);
Bool RimDevicesRefresh(void);
void ReadParams(void);
void RtcControl(void);
void EEPROM_Update(AT25XXX *Eeprom);
void EEPROM_Func(Byte Memory, Byte Func, 
	Uns Addr, Uns *Data, Uns Count);
void RimIndication(void);
void SetPlisData(void);
void DiscrInOutTestObserver(void);
void TekModbusParamsUpdate(void);

#endif
