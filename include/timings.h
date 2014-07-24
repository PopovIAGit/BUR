/*
timings.h

��������
*/

#ifndef _TIMINGS_H_
#define _TIMINGS_H_


// rim_devices
#define EEPROM_RETRY			(5)					// ���������� ������� ���������� � EEPROM
#define DISPL_SCALE				(1.000 * PRD0)		// ���������� ����. ����������
#define LOG_START_TOUT			(3.000 * PRD1)		// ������� ������ � ������
#define LOG_BUF_START_TOUT		(1.000 * PRD1)		// ������� ������ � ������
#define WRITE_TIME				(0.020 * PRD0)		// ����� ������ EEPROM 20 ��

#define BLINK_HI_TOUT			(0.500 * PRD3)		// ����� ����������� ��� ��������������
#define BLINK_LO_TOUT			(0.300 * PRD3)		// ����� ������� ��� ��������������
#define BIT_TOUT				(1.000 * PRD3)		// ����� ��� ������ ����
#define TSENS_RETRY				(5)					// ���������� ����� ������� �� ����������� ������
#define BTN_TIME				(0.100 * PRD2)		// ����� ��������� ����� � ���������
#define TU_SCALE				(0.100 * PRD2)		// ������������� ��� �� (1 = 0.1 �)
#define TEMP_EN_TOUT			(5.000 * PRD3)		// �������� ��������� ����
#define CONT_TEST_TIME 			(1.500 * PRD3)		// ����� ��������� ��������� ����������� ��� �������� ��� �
#define DRV_TYPE_SHOW_TIME      (5.00 * PRD3)      // ����� ��������� ���� ������� ��� ����� �����


// interface
#define MUFF_CLB_TIME			(3.000 * PRD3)		// ����� ��������� ����� ��� ����������
#define DEMO_TOUT				(5.000 * PRD3)		// ����� ����� � ����-������
#define BLINK_TOUT				(0.500 * PRD3)		// ����� ������� ��� ������� ������ ���/���
#define POWER_TOUT				(4.500 * PRD2)		// ����� ������� ����������
#define DSP_LED_TIMER			(0.700 * PRD3)		// ����� ���������� ����������
#define CANCEL_TOUT				(2.000 * PRD3)		// ����� ������ ��������� �� ������ �������
#define PULT_LED_TOUT			(0.200 * PRD3)		// ����� ������� ���������� ���
#define SLEEP_SCALE				(1.000 * PRD3)		// �������� ����� ����
#define IM_TIMEOUT				(10.00 * PRD3)		// ������� ��������������� ������

// motor_control				 
#define START_DELAY_TIME		(2.000 * PRD2)		// ����������� ������� ����� ����� ��������� � ����. ��������
#define ZAZOR_SCALE				(0.100 * PRD1)		// ������������� ������� ������ ������
#define TEST_STATE_TIME			(0.040 * PRD1)		// ����������� ������� ��������� ������ > ���� 1.2
#define UPOR_STATE_TIME			(2.000 * PRD1)		// ����������� ������� ������ ����� ���
#define MOVE_STATE_TIME			(1.000 * PRD1)		// ����������� ������� �������� �� ����
#define PAUSE_STATE_TIME		(0.040 * PRD1)		// ����������� ������� ������ �����
#define DYN_PAUSE_TIME			(0.040 * PRD1)		// ����������� ������� ����� ��
#define KICK_ST0_TIME			(0.040 * PRD1)		// ����������� ������� � ���� ������ ��� ���������� ����
#define KICK_ST1_TIME			(0.080 * PRD1)		// ������ ����������� ������� � ���� ������
#define SP_CALC_TOUT			(1.000 * PRD3)		// �������� ������� ��������
#define CTRLMODE_SCALE			(0.100 * PRD1)		// ������ ��� ������� ����
#define DMC_FLTR_TS				(1.000 / PRD2)		// ���������� ������������� �������� ���
#define DMC_TS					(1.000 / HZ)		// ���������� ������������� ������ ���������� ����������
#define DMC_TORQF_TS			(1.000 / PRD1)		// ���������� ������������� ������� ������� �������
#define START_PAUSE				(2.000 * PRD1)		// ���� ��������� ��
#define DEBUG_START_TIME		(5.000 * PRD1)      // ������ ��� ���������� ��������� �� ��� ���������

#define NET_MON_START_TIME		(1.000 * PRD2)		// �������� ����� ������ �� ������� ����������
#define NET_MON_STOP_TIME2 		(1.000 * PRD2)		// �������� ����� ����� ������ ��� ������� ����������
#define NET_MON_STOP_TIME 		(1.000 * PRD2)		// �������� ����� ����� ������ ��� ������� ����������
#define CONECTOR_TIME			(0.700 * PRD2)		// ����� ������ ������� �� ���������
#define CONECTOR_STOP_TIME		(1.000 * PRD2)		// ����� ������ ������� �� ���������
// protection
#define PRT_SCALE				(0.100 * PRD2)	// ������������� ��� ����� (1 = 0.1 �)
#define TB_SCALE				(60.00 * PRD2)	// ������������� ��� ������ �� ���������/�������������� �����
#define FLT_DEF_DELAY			(2.000 * PRD4)	// �������� �� ����������� ���� ������
#define VSK_TIME				(3.500 * PRD4)	// �������� ��� ������� ���������


#endif

