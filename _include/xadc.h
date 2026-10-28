#ifndef XADC_H
#define XADC_H

#include "xil_types.h"

#define XADC_BASE_ADDR              0x43C00000U
#define DC_DET_SAU_ADC_OFFSET       0x244U

#define XADC_MAX_VALUE              4095U
#define DC_DET_SAU_ADC_MAX_MV       1800U
#define DC_DET_SAU_SYSTEM_MAX_MV    24000U

uint16_t XADC_Read_DC_DET_SAU_mV(void);

#endif
