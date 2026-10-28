#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "protocol_private.h"
#include "protocol_ad4t.h"

#define CTLC    				0x03    /* control-c */
#define SYNC    				0x16

/* Protocol ���ɾ� ���� */
#define	PSA_STAT_REQ			0x09
#define PSA_STAT_ACK			0x89

#define	RPT_INFO_REQ			0x0A
#define RPT_INFO_ACK			0x8A

#define	MAIN_STAT_REQ			0x00
#define MAIN_STAT_ACK			0x80
#define MAIN_CTRL_REQ			0x01
#define MAIN_CTRL_ACK			0x81

#define ICS_STAT_REQ			0x02
#define ICS_STAT_ACK			0x82
#define ICS_CTRL_REQ			0x03
#define ICS_CTRL_ACK			0x83

#define TABLE_STAT_REQ			0x04
#define TABLE_STAT_ACK			0x84
#define TABLE_CTRL_REQ			0x05
#define TABLE_CTRL_ACK			0x85

#define ADC_REQ					0x06
#define ADC_ACK					0x86


#define UNINSTALL_REQ			0x07
#define UNINSTALL_ACK			0x87

#define M_MODEM_REQ				0x08
#define M_MODEM_ACK				0x88

#define LOG_LOAD_REQ			0x16
#define LOG_LOAD_ACK			0x96

#define LOG_ERASE_REQ			0x17
#define LOG_ERASE_ACK			0x97

#define LOG_CNT_REQ				0x18
#define LOG_CNT_ACK				0x98

#define SYS_RST_REQ				0x19
#define SYS_RST_ACK				0x99



#endif
