#ifndef _MENU_H_
#define _MENU_H_

#include "menu_global.h"
#include "key_codes.h"

#ifdef __cplusplus
extern "C" {
#endif

// �������� ������ ����
struct MENU_GROUP {
	Char  HiString[16];  // ������� ������� ������
	Char  LoString[16];  // ������ ������� ������
	Uns   Addr;          // ��������� �����
	Uns   Count;         // ���������� ���������
};

// ��������� ��������� ����
struct MENU_PARAM {
	Char  Name[16];      // ��� ���������
	Char  Unit[4];       // ������� ��������� ���������
	struct MENU_DCR Dcr; // �������� ���������
};

// ��������� ���������� �������� ���������
struct MENU_STRING {
	Char  Str[16];       // ��������� �������� ���������
};

// ��������� ��� ������ � ����������� ��� Express ���������
struct MENU_EXPRESS_PARAMS {
	Char Cnt;			// ���������� ����������
	Uns  Buf[5];		// ����� ������� ���������� � ��������� Ram
};

// ��������� ��� ������ � Express ���������� � ����
struct MENU_EXPRESS {
	Bool Enable;							// ��������� ������� Express ���������
	Bool Select;							// ���� ������ ���������
	Byte State;								// ��� (���������) �������� ���������
	Byte Index;								// ������ �������� ��������� � ������� List'�
	Bool First;								// ���� ������� �����
	struct MENU_EXPRESS_PARAMS *List;		// ��������� �� ������� ������ ����������
	struct MENU_EXPRESS_PARAMS List1;		// ��������� �������� ���������
	struct MENU_EXPRESS_PARAMS List2;
	struct MENU_EXPRESS_PARAMS List3;
	struct MENU_EXPRESS_PARAMS List4;
	struct MENU_EXPRESS_PARAMS List5;
	struct MENU_EXPRESS_PARAMS List6;
	struct MENU_EXPRESS_PARAMS List7;
	struct MENU_EXPRESS_PARAMS List8;
	struct MENU_EXPRESS_PARAMS List9;
	struct MENU_EXPRESS_PARAMS List10;
	struct MENU_EXPRESS_PARAMS List11;
	struct MENU_EXPRESS_PARAMS List12;
};

// ��������� ����
typedef struct MENU {
	Byte   State;        // ����� ����
	Char   Key;          // ����������� �������
	Byte   EditType;     // ��� ������ ��������������
	Byte   Level;        // ������� ������� ����
	Byte   MaxLevel;     // ������� ����������� ���� (1 ��� 2)
	Bool   Indication;   // ���� ���������� ���������
	Bool   Update;       // ���� ���������� ������ �� ��������
	Bool   ShowReserved; // ���� ����������� ��������
	Uns   *MinMaxGain;   // ����������� ���������
	
	Uns    Value;        // ������� �������� ���������
	Byte   Rate;         // ������� ������
	Byte   EditRate;     // ������������ ������ ��� ��������������
	Bool   Blink;        // ��������� ������� ��������
	Uns    BlinkTimer;   // ������ ��� �������
	Uns    BlinkHiTime;  // ����� ����������� ��� ��������������
	Uns    BlinkLoTime;  // ����� ������� ��� ��������������
	Byte   Bit;          // ��������� ���� ����� � ���� ������� ������
	Uns    BitTimer;     // ������ ����� ��� ������ ����
	Uns    BitTime;      // ����� ��� ������ ����

	Bool   SleepActive;  // ��������� ������
	Uns    SleepTimer;   // ������ ������
	Uns    SleepTime;    // ����-��� ������

	Uns   *Data;         // ��������� �� ����� ������
	String HiString;     // ��������� �� ����� ������� ������� ����������
	String LoString;     // ��������� �� ����� ������ ������� ����������
	Uns    StartAddr;    // ����� ��������� ��� ������ � ������ ������
	Uns    StartOffset;  // �������� ������ ��� ������ � ������ ������
	Char   BufValue[6];  // ��������� ����� ��� �������������� � ������ 1
	Char   BufTmp[17];   // ��������� ����� ��� �������� ������� ���������/���������� ��������
	
	Bool   EvLogFlag;    // ���� ������ ������
	Bool   EvLogSelected;// ���� ������ ������
	Uns    EvLogGroup;   // ����� ������ �������
	Uns    EvLogAdress;  // ����� ��������� ������� ������ � ������
	Uns    EvLogTime;    // ����� ��������� ������� ������ �������
	Uns    EvLogDate;    // ����� ��������� ���� ������ �������
	Uns	   *EvLogCount;	 // ���������� ��������� ������� � ������
	
	struct MENU_CRD Group;     // ��������� ���������� ������
	struct MENU_CRD SubGroup;  // ��������� ���������� ���������
	struct MENU_CRD EvLog;     // ��������� ���������� ������ �������
	struct MENU_CRD Param;     // ��������� ���������� ���������
	struct MENU_DCR Dcr;       // ��������� �������� ���������

	struct MENU_EXPRESS Express;	// ��������� �������� ���������
	
	const struct MENU_GROUP  *Groups;   // ��������� �� ��������� �����
	const struct MENU_GROUP  *SubGroups;// ��������� �� ��������� ��������
	const struct MENU_PARAM  *Params;   // ��������� �� ��������� ����������
	const struct MENU_STRING *Values;   // ��������� �� ��������� ��������� ��������
	struct MENU_STRING        Rsvd;     // ��������� ������
	
	void (*StartDispl)(String Text);
	Bool (*EnableEdit)(Uns PaswwPrt, Uns ParAddr);
	Bool (*WriteValue)(Uns Memory, Uns ParAddr, Uns *Value);
	void (*GetExpressText)(Uns State);
	Byte (*GetExpressState)(Byte CurrentState, Uns State); 
} MENU;

void MENU_Update(MENU *);
void MENU_Display(MENU *);
void MENU_ReadDcr(MENU *, struct MENU_DCR *, Uns Addr);

#ifdef __cplusplus
}
#endif // extern "C"

#endif



