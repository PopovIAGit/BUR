/*======================================================================
Имя файла:          responses.h
Автор:              Саидов В.С.
Версия файла:       01.00
Описание:
Коды исключений ModBus
======================================================================*/

#ifndef RESPONSES_
#define RESPONSES_

#define FR_SUCCESS                       0x00
#define FR_ERR_BAD_PARAMS                0x0C
#define FR_ERR_NOT_RESPOND               0x0E
#define FR_ERR_FUNC_NOT_SUPPORTED        0x0F
#define FR_ERR_CRC_FAILED                0x11
#define FR_ERR_BAD_RESPONSE_LENGTH       0x12
#define FR_ERR_BAD_RESPONSE_ADDRESS      0x13
#define FR_ERR_BAD_RESPONSE_FUNC         0x14
#define FR_ERR_BAD_DATA_LENGTH           0x15
#define FR_ERR_BAD_DATA_ADDRESS          0x16
#define FR_ERR_BAD_DATA_NUM              0x17
#define FR_ERR_BAD_DATA_VALUE            0x18
#define FR_ERR_BAD_EXCERTION_RESPONSE    0x19

#define EX_ILLEGAL_FUNCTION              0x01
#define EX_ILLEGAL_DATA_ADDRESS          0x02
#define EX_ILLEGAL_DATA_VALUE            0x03
#define EX_SLAVE_DEVICE_FAILURE          0x04
#define EX_ACKNOWLEDGE                   0x05
#define EX_SLAVE_DEVICE_BUSY             0x06
#define EX_NEGATIVE_ACKNOWLEDGE          0x07
#define EX_MEMORY_PARITY_ERROR           0x08
#define EX_NO_CONNECTION                 0x09

#endif



