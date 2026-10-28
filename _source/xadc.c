#include "xadc.h"
#include "xil_io.h"
#include "xil_printf.h"

uint16_t XADC_Read_DC_DET_SAU_mV(void)
{
    uint32_t reg_value;
    uint16_t adc_value;
    uint32_t adc_voltage_mV;
    uint32_t system_voltage_mV;

    /* Read VAUX1 register */
    reg_value = Xil_In32(XADC_BASE_ADDR + DC_DET_SAU_ADC_OFFSET);

    /* 12-bit ADC value */
    adc_value = (uint16_t)((reg_value >> 4) & 0x0FFF);

    /* ADC voltage: 0 ~ 1.8V */
    adc_voltage_mV =
        ((uint32_t)adc_value * DC_DET_SAU_ADC_MAX_MV)
        / XADC_MAX_VALUE;

    /* System voltage: 0 ~ 24V */
    system_voltage_mV =
        ((uint32_t)adc_value * DC_DET_SAU_SYSTEM_MAX_MV)
        / XADC_MAX_VALUE;

    xil_printf("DC_DET_SAU_ADC: REG=0x%08X ADC=%d ADC_V=%d mV SYS_V=%d mV\r\n",
               (unsigned int)reg_value,
               (int)adc_value,
               (int)adc_voltage_mV,
               (int)system_voltage_mV);

    return (uint16_t)system_voltage_mV;
}

char *XadcHelp[] = {
    "XADC Voltage",
    "",
    "Options:",
    "",
    "NOTE: none",
    0
};

int Xadc(int argc, char *argv[])
{
    uint16_t voltage;

    (void)argc;
    (void)argv;

    voltage = XADC_Read_DC_DET_SAU_mV();

    xil_printf("DC_DET_SAU = %d mV\r\n", voltage);

    return 0;
}
