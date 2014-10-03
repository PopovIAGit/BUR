#ifndef _MENU_GLOBAL_H_
#define _MENU_GLOBAL_H_

#include "std.h"

#ifdef __cplusplus
extern "C" {
#endif

// ������ ����
#define MS_START		0		// ����� ������
#define MS_SELGR		1		// ����� ������ ������
#define MS_SELPAR		2		// ����� ������ ���������
#define MS_EDITPAR		3		// ����� �������������� ���������
#define MS_SHOWPAR		4		// ����� ��������� ���������
#define MS_SHOWBIN		5		// ����� ��������� �����������
#define MS_SETREF		6		// ����� �������������� �������
#define MS_EXPRESS		7		// ����� �������� ���������
#define MS_DRIVE_TYPE	8		// ����� �������� ���������

// ���� ����������
#define MT_DEC		0		// ����������
#define MT_STR		1		// ���������
#define MT_RUN		2		// ������� ������
#define MT_TIME		3		// �����
#define MT_DATE		4		// ����
#define MT_BIN		5		// ��������
#define MT_HEX		6		// �����������������
#define MT_RSVD     7     // ���������

// ��������� ������������ ���������
struct MENU_VAL_CFG {
	Uns Type:3;             // 0-2	��� ���������
	Uns ReadOnly:1;         // 3     ������� ��������� ������ ��� ������
	Uns PaswwPrt:2;         // 4-5   ������� �������� �� ������
	Uns Memory:1;			   // 6     ������� �������� � ������
	Uns Signed:1;			   // 7     ������� ��������� ���������
	Uns MinMax:1;			   // 8     ������� ������� ������������ ���������
	Uns Precision:3;		   // 9-11  ���������� ������ ����� �������
	Uns RateMax:4;			   // 12-15 ������������ ������
};

struct MENU_STR_CFG {
	Uns Type:3;             // 0-2	��� ���������
	Uns ReadOnly:1;         // 3     ������� ��������� ������ ��� ������
	Uns PaswwPrt:2;         // 4-5   ������� �������� �� ������
	Uns Memory:1;           // 6     ������� �������� � ������
	Uns Addr:9;             // 7-15	����� ���������� ��������
};

union MENU_CFG {
	Uns all;
	struct MENU_VAL_CFG Val;
	struct MENU_STR_CFG Str;
};

// ��������� �������� ���������
struct MENU_DCR {
	Uns   Min;              // ����������� �������� ���������
	Uns   Max;              // ������������ �������� ���������
	Uns   Def;              // �������� �� ���������
	union MENU_CFG Config;  // ������������ ���������
};

// ��������� ����������
struct MENU_CRD {
	Uns   Position;         // ������� ����� ��������
	Uns   Addr;             // ��������� ����� ���������
	Uns   Count;            // ���������� ���������
};

// ������� ��� ������ � ����
#define M_TYPE      0x0007                  // ����� ��� ���� ���������
#define M_RONLY     0x0008                  // ������ ��� ������
#define M_PWP1      0x0010                  // ������� ������� 1
#define M_PWP2      0x0020                  // ������� ������� 2
#define M_RC			0x0030						// ������� �� ��������� ���������� (��� M_RONLY)
#define M_NVM       0x0040                  // �������� � ����������������� ������
#define M_SIGN      0x0080                  // �������� ��������
#define M_KMM       0x0100                  // �������� � ������������� ���������
#define M_SADR(i)   ((i & 0x01FF) << 7)     // ����� ���������� ��������
#define M_PREC(i)   ((i & 0x0007) << 9)     // ���������� ������ ����� �������
#define M_RMAX(i)   ((i & 0x000F) << 12)    // ������������ ������
#define M_EDIT_PAR  (M_PWP1|M_NVM)          // �������� ���������
#define M_FACT_PAR  (M_PWP2|M_NVM)          // ��������� �������� ���������
#define M_PWP_MASK  (M_PWP1|M_PWP2)         // ����� ������������ ������ �������
#define M_CFG_MASK  (M_TYPE|M_RONLY|M_PWP_MASK|M_NVM) // ����� ����� ������������

#define M_SHOW      (MT_DEC|M_RONLY)        // ��������������� ��������
#define M_EDIT      (MT_DEC|M_PWP1|M_NVM)   // ������������� ���������� ��������
#define M_FACT      (MT_DEC|M_PWP2|M_NVM)   // ������������� ���������� ��������� ��������
#define M_DCOM      (MT_DEC|M_PWP1)         // ���������� ������� ����������
#define M_FDCOM		(MT_DEC|M_PWP2)         // ���������� ��������� ������� ����������
#define M_STAT      (MT_STR|M_RONLY)        // ��������� ���������
#define M_LIST      (MT_STR|M_PWP1|M_NVM)   // ������������� ��������� ��������
#define M_FLST      (MT_STR|M_PWP2|M_NVM)   // ������������� ��������� ��������� ��������
#define M_COMM      (MT_STR|M_PWP1)         // ��������� ������� ����������
#define M_FCOMM     (MT_STR|M_PWP2)         // ��������� ��������� ������� ����������
#define M_RUNS      (MT_RUN|M_RONLY)        // �������� ������� ������
#define M_TIME      (MT_TIME|M_RMAX(1))     // �������� �����
#define M_DATE      (MT_DATE|M_RMAX(2))     // �������� ����
#define M_BINS      (MT_BIN|M_RONLY)        // �������� � �������� �����������
#define M_BINE		(MT_BIN|M_PWP1|M_NVM)	// ������������� �������� ��������
#define M_BINF		(MT_BIN|M_PWP2|M_NVM)	// ������������� �������� ��������� ��������
#define M_BINC		(MT_BIN|M_PWP1)			// �������� ������� ����������
#define M_BINFC		(MT_BIN|M_PWP2)			// �������� ��������� ������� ����������
#define M_CODE      (MT_DEC|M_RMAX(4))      // ��� �������
#define M_HSHOW     (MT_HEX|M_RONLY)        // ��������������� ����������������� ��������
#define M_HEDIT     (MT_HEX|M_PWP1|M_NVM)   // ������������� ����������������� ��������
#define M_HFACT     (MT_HEX|M_PWP2|M_NVM)   // ������������� ����������������� ��������� ��������
#define M_HIDE      (MT_DEC|M_NVM)          // ������� ��������
#define M_RSVD      (MT_RSVD|M_RONLY)       // ��������� ��������
#define M_ESHOW     (MT_DEC|M_RONLY|M_NVM)  // ��������������� ���������� �������� �� ������
#define M_EHSHOW		(MT_HEX|M_RONLY|M_NVM)	// ��������������� ����������������� �������� �� ������
#define M_ETSHOW		(M_TIME|M_RONLY|M_NVM)	// ��������������� ����� �� ������
#define M_EDSHOW		(M_DATE|M_RONLY|M_NVM)	// ��������������� ���� �� ������

#define M_IS_KMM(v)	((((v) & M_TYPE) == MT_DEC) && (v & M_KMM))

#ifdef __cplusplus
}
#endif // extern "C"

#endif

