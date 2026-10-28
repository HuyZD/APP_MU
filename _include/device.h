#ifndef __DEVICE_H__
#define __DEVICE_H__    

#include "FreeRTOS.h"
#include "uart.h"

#define DAU_NUM_MAX        3
#define SAU_S_NUM_MAX      3
#define MU_NUM_MAX         4
#define RU_NUM_MAX         4

typedef enum {
    DEV_TYPE_UNKNOWN = 0,
    DEV_TYPE_LMT     = 0xB0,
    DEV_TYPE_MODEM   = 0x10,
    DEV_TYPE_DAU     = 0x11,
    DEV_TYPE_SAU     = 0x12,
    DEV_TYPE_MU      = 0x13,
    DEV_TYPE_RU      = 0x14
} DEVICE_TYPE;

typedef enum {
    TOPOLOGY_UNKNOWN    = 0,
    TOPOLOGY_STARTED    = 1,
    TOPOLOGY_COMPLETED  = 2,
    TOPOLOGY_TIMEOUT    = 3, 
    TOPOLOGY_FAILED     = 4
} TOPOLOGY_STATUS;

typedef struct {
    DEVICE_TYPE type;
    short device_id;         // device ID 1, 2, 3, 4
    short branch_id; 
    short port_id; 
} dev_info;

typedef struct {    
    dev_info ru;                      // Info about current RU
    short children_ru_number;        // number of following RU children in same branch     
    dev_info children_ru[RU_NUM_MAX]; // RU children info in same  branch 
} ru_dev_info;

typedef struct {
    // Info about current MU
    dev_info mu;

    short br1_ru_number;  // number of RU children in branch 1
    short br2_ru_number;  // number of RU children in branch 2
    short br3_ru_number;  // number of RU children in branch 3
    short br4_ru_number;  // number of RU children in branch 4

    // RU children info in each branch 
    dev_info ru_br1[RU_NUM_MAX];
    dev_info ru_br2[RU_NUM_MAX];
    dev_info ru_br3[RU_NUM_MAX];
    dev_info ru_br4[RU_NUM_MAX];

    // Status parameter foreach branch used for topology building
    TOPOLOGY_STATUS top_status[RU_NUM_MAX];

    // Start tick of topology building, used for timeout calculation
    TickType_t top_start;
} mu_dev_info;

struct _sau_dev_info{
    dev_info    sau;                  // Info about current SAU
    mu_dev_info *p_mu;                   // MU info

    dev_info *p_dau1;                 // DAU 1
    dev_info *p_dau2;                 // DAU 2
    dev_info *p_dau3;                 // DAU 3
  
    struct _sau_dev_info *p_sau_s1;           // SAU-S 1 info if available
    struct _sau_dev_info *p_sau_s2;           // SAU-S 2 info if available
    struct _sau_dev_info *p_sau_s3;           // SAU-S 3 info if available
    struct _sau_dev_info *p_sau_s4;           // SAU-S 4 info if available
};

typedef struct _sau_dev_info sau_dev_info;

typedef char dev_type_name[16];
extern dev_type_name dev_map_array[MAX_UART_TYPE];

#endif
