/*
timings.h

��������
*/

#ifndef _TIMINGS_H_
#define _TIMINGS_H_


// rim_devices
#define EEPROM_RETRY			(5)					// ���������� ������� ���������� � EEPROM
#define DISPL_SCALE				(1.000 * PRD_2KHZ)		// ���������� ����. ����������
#define LOG_START_TOUT			(3.000 * PRD_200HZ)		// ������� ������ � ������
#define LOG_BUF_START_TOUT		(1.000 * PRD_200HZ)		// ������� ������ � ������
#define WRITE_TIME				(0.020 * PRD_2KHZ)		// ����� ������ EEPROM 20 ��

#define BLINK_HI_TOUT			(0.500 * PRD_10HZ)		// ����� ����������� ��� ��������������
#define BLINK_LO_TOUT			(0.300 * PRD_10HZ)		// ����� ������� ��� ��������������
#define BIT_TOUT				(1.000 * PRD_10HZ)		// ����� ��� ������ ����
#define TSENS_RETRY				(5)					// ���������� ����� ������� �� ����������� ������
#define BTN_TIME				(0.100 * PRD_50HZ)		// ����� ��������� ����� � ���������
#define TU_SCALE				(0.100 * PRD_50HZ)		// ������������� ��� �� (1 = 0.1 �)
#define TEMP_EN_TOUT			(5.000 * PRD_10HZ)		// �������� ��������� ����
#define CONT_TEST_TIME 			(1.500 * PRD_10HZ)		// ����� ��������� ��������� ����������� ��� �������� ��� �
#define DRV_TYPE_SHOW_TIME      (5.000 * PRD_10HZ)      // ����� ��������� ���� ������� ��� ����� �����
#define DRV_TEMPER_TOUT			(1.000 * PRD_50HZ)		// ����� �������� ������������ ������ �� ��������� ���������

// interface
#define MUFF_CLB_TIME			(3.000 * PRD_10HZ)		// ����� ��������� ����� ��� ����������
#define DEMO_TOUT				(5.000 * PRD_10HZ)		// ����� ����� � ����-������
#define BLINK_TOUT				(0.500 * PRD_10HZ)		// ����� ������� ��� ������� ������ ���/���
#define POWER_TOUT				(4.500 * PRD_50HZ)		// ����� ������� ����������
#define DSP_LED_TIMER			(0.700 * PRD_10HZ)		// ����� ���������� ����������
#define CANCEL_TOUT				(2.000 * PRD_10HZ)		// ����� ������ ��������� �� ������ �������
#define PULT_LED_TOUT			(0.200 * PRD_10HZ)		// ����� ������� ���������� ���
#define SLEEP_SCALE				(1.000 * PRD_10HZ)		// �������� ����� ����
#define IM_TIMEOUT				(10.00 * PRD_10HZ)		// ������� ��������������� ������

// motor_control				 
#define START_DELAY_TIME		(2.000 * PRD_50HZ)		// ����������� ������� ����� ����� ��������� � ����. ��������
#define ZAZOR_SCALE				(0.100 * PRD_200HZ)		// ������������� ������� ������ ������
#define TEST_STATE_TIME			(0.040 * PRD_200HZ)		// ����������� ������� ��������� ������ > ���� 1.2
#define UPOR_STATE_TIME			(2.000 * PRD_200HZ)		// ����������� ������� ������ ����� ���
#define MOVE_STATE_TIME			(1.000 * PRD_200HZ)		// ����������� ������� �������� �� ����
#define PAUSE_STATE_TIME		(0.040 * PRD_200HZ)		// ����������� ������� ������ �����
#define DYN_PAUSE_TIME			(0.040 * PRD_200HZ)		// ����������� ������� ����� ��
#define KICK_ST0_TIME			(0.040 * PRD_200HZ)		// ����������� ������� � ���� ������ ��� ���������� ����
#define KICK_ST1_TIME			(0.080 * PRD_200HZ)		// ������ ����������� ������� � ���� ������
#define SP_CALC_TOUT			(1.000 * PRD_10HZ)		// �������� ������� ��������
#define CTRLMODE_SCALE			(0.100 * PRD_200HZ)		// ������ ��� ������� ����
#define DMC_FLTR_TS				(1.000 / PRD_50HZ)		// ���������� ������������� �������� ���
#define DMC_TS					(1.000 / HZ)		// ���������� ������������� ������ ���������� ����������
#define DMC_TORQF_TS			(1.000 / PRD_200HZ)		// ���������� ������������� ������� ������� �������
#define START_PAUSE				(2.000 * PRD_200HZ)		// ���� ��������� ��
#define DEBUG_START_TIME		(2.000 * PRD_200HZ)      // ������ ��� ���������� ��������� �� ��� ���������

#define NET_MON_START_TIME		(1.000 * PRD_50HZ)		// �������� ����� ������ �� ������� ����������
#define NET_MON_STOP_TIME2 		(0.500 * PRD_50HZ)		// �������� ����� ����� ������ ��� ������� ����������
#define NET_MON_STOP_TIME 		(1.000 * PRD_50HZ)		// �������� ����� ����� ������ ��� ������� ����������
#define CONECTOR_TIME			(0.700 * PRD_50HZ)		// ����� ������ ������� �� ���������
#define CONECTOR_STOP_TIME		(1.000 * PRD_50HZ)		// ����� ������ ������� �� ���������
// protection
#define PRT_SCALE				(0.100 * PRD_50HZ)	// ������������� ��� ����� (1 = 0.1 �)
#define TB_SCALE				(60.00 * PRD_50HZ)	// ������������� ��� ������ �� ���������/�������������� �����
#define FLT_DEF_DELAY			(4.000 * PRD_50HZ)	// �������� �� ����������� ���� ������
#define VSK_TIME				(3.500 * PRD_50HZ)	// �������� ��� ������� ���������


#endif

