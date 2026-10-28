#ifndef __PROTOCOL_AD4T_H__
#define __PROTOCOL_AD4T_H__
#include <stdint.h>
#include "xil_types.h"
#include "protocol_private.h"
#include "device.h"

// Topology request message is 17byte long
#define TOPOLOGY_MSG_SIZE		17

#define pdTICKS_TO_MS( xTicks ) \
    ( ( uint32_t ) ( ( ( TickType_t ) ( xTicks ) * 1000U ) / ( TickType_t ) configTICK_RATE_HZ ) )

/*	
 *	Frame info
 */
typedef struct  {
	uint8_t    			srcID_type;
	uint8_t    			srcID_branch;
	uint8_t    			srcID_port;
	uint8_t    			srcID_id;
	
	uint8_t    			destID_type;
	uint8_t    			destID_branch;
	uint8_t    			destID_port;
	uint8_t    			destID_id;
} frame_info;

/*
	Create frame_info structure for response frame by switching
	source and destination IDs from incomming frame_info
*/
void setFrameInfo(frame_info *fr_info, frame_info *resp_fr_info);

/*
	Start topology building by send protocol message TOPOLOGY_REQ to all 4 branch
*/
void StartTopologyBuilding(mu_dev_info *p_mu_ptr);

/*
	Log current MU topology information
*/
void logMuInfo(mu_dev_info *mu_info);

/*
*/
int createForwardFrame(int cmd, char *rx_buf, unsigned int rx_len, u8 *tx_buf, const frame_info *fr_info);

/*
*/
void logHeader(short uart_index, char cmd, frame_info *fr_info);

#endif
