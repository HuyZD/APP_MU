#ifndef __IR_SERVER_H__
#define __IR_SERVER_H__

#include "device.h"
#include "protocol_ad4t.h"

void updateRuInfo(char* rx_buf, unsigned int rx_len, frame_info *fr_info, mu_dev_info *mu_info, short uart_index);

#endif
