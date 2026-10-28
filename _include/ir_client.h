#ifndef _IR_CLIENT_H_
#define _IR_CLIENT_H_

#include "protocol.h"
#include "device.h"

int forwardFrame(int cmd, char *rx_buf, unsigned int rx_len, mu_dev_info *p_mu, frame_info *fr_info);



#endif