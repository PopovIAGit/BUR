#ifndef CONFIG_
#define CONFIG_

// ���������� ���������
#define VERSION					1113	// ������ �������
#define SUBVERSION				12		// ��������� �������
#define DEF_USER_PASS			2387	// ������ ��-��������� (����������������)
#define DEF_FACT_PASS			43072	// ������ ��-��������� (���������)
#define BT_AUTH_CODE_STRING		"5124"	// ������ � ����� ����������� Bluetooth


#define SYSCLK				100e6		// ��������� ������� (���-�� �������� � ���)
#define CLKIN				50e6		// ������� ������
#define CLK_VS_TRN_FREQ		20000U	// ������� ������������ ����������
#define CLK_VS_TRN_PERIOD	(SYSCLK / CLK_VS_TRN_FREQ)		// ������ ���
//#define IRDA_FREQ			((SYSCLK / 528e3) - 1)	// ������� IRDA
#define IRDA_FREQ			((SYSCLK / 528e3) - 1)	// ������� IRDA

// ������������ ������������ �������
#define HZ						18e3		// ������� ��������� ���������� RTOS
#define PRD0					2000		// ������������� ������ 2000 ��
#define PRD1					200			// ������������� ������ 200 ��
#define PRD2					50			// ������������� ������ 50 ��
#define PRD3					10			// ������������� ������ 10 ��
#define PRD4					50			// ������������� ������ 50 ��
#define PRD5					50			// ������������� ������ 50 ��

// �������� ������
#define TORQ_TEST			0			// ���� ������� �������
#define LOWPOW_TEST			0			// ���� ���������� ��� ������� ����������, 1 ��������� ����� ����������������, ���� 0 �� ������� ����
#define IR_IMP_TEST			0			// ���� IR

// ����������� ������������ ������
#include "device_modification.h"		// ����������� �����
#include "csl\csl.h"					// ���������� ��������� ����
#include "chip\DSP280x_Device.h"		// ���������� ����� ��������� ����
#include "IQmathLib.h"					// ������������ ���� ���������� IQmath
#include "timings.h"					// ���� ���������
#include "structs.h"					// �������� ����� � �������� �������
#include "macro.h"						// ���� ��������
#include "hardware.h"					// ������������ ������������
#include "serial_communication.h"		// ���������������� �����
#include "interface.h"					// ������������ ������
#include "rim_devices.h"				// ������ ���������
#include "motor_control.h"				// ���������� ����������
#include "protections.h"				// ������ �������
#include "drv_modbus.h"
#include "params.h"						// ���� ������������ ���������� 

#endif

