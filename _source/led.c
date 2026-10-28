#include <stdint.h>
#include <string.h>

#include "config.h"
#include "genlib.h"
#include "repeater.h"
#include "psa.h"
#include "fpga.h"
#include "gpio.h"

static inline void LED_GPIO_Set(unsigned int bit)
{
    volatile unsigned int *reg =
        (volatile unsigned int *)GPIO_DATA_1;

    *reg |= (1U << bit);
}


static inline void LED_GPIO_Clear(unsigned int bit)
{
    volatile unsigned int *reg =
        (volatile unsigned int *)GPIO_DATA_1;

    *reg &= ~(1U << bit);
}

void LED_GPIO_Init(void)
{
    volatile unsigned int *DIRM =
        (volatile unsigned int *)GPIO_DIRM_1;

    volatile unsigned int *OEN =
        (volatile unsigned int *)GPIO_OEN_1;

    unsigned int mask;

    mask = (1U << LED_SCLK_BIT) |
           (1U << LED_RCLK_BIT) |
           (1U << LED_MOSI_BIT) |
           (1U << LED_SCLR_BIT);

    /* GPIO output */
    *DIRM |= mask;

    /* Output enable */
    *OEN |= mask;

    /* Initial state */
    LED_GPIO_Clear(LED_SCLK_BIT);
    LED_GPIO_Clear(LED_RCLK_BIT);
    LED_GPIO_Clear(LED_MOSI_BIT);

    /* SCLR inactive = HIGH */
    LED_GPIO_Set(LED_SCLR_BIT);
}

static inline void LED_SCLK_Pulse(void)
{
    LED_GPIO_Set(LED_SCLK_BIT);
    LED_GPIO_Clear(LED_SCLK_BIT);
}


static inline void LED_RCLK_Pulse(void)
{
    LED_GPIO_Set(LED_RCLK_BIT);
    LED_GPIO_Clear(LED_RCLK_BIT);
}

void LED_Write40(uint64_t data)
{
    int i;

    for (i = 39; i >= 0; i--)
    {
        if (data & (1ULL << i))
            LED_GPIO_Set(LED_MOSI_BIT);
        else
            LED_GPIO_Clear(LED_MOSI_BIT);

        LED_SCLK_Pulse();
    }

    /* Latch 40 bits to output */
    LED_RCLK_Pulse();
}

void LED_Clear(void)
{
    LED_Write40(0x0000000000ULL);
}

char *LedHelp[] = {
    "LED Control",
    "-[on|off] [all]",
    "",
    "Options:",
    "on all     turn on all LEDs",
    "off all    turn off all LEDs",
    "",
    "",
    "NOTE: none",
    0
};


int Led(int argc, char *argv[])
{
    char *arg1;
    char *arg2;

    arg1 = argv[1];
    arg2 = argv[2];

    if (arg1 == NULL || arg2 == NULL)
        return 0;

    if (strcmp(arg2, "all") != 0)
        return 0;

    if (strcmp(arg1, "on") == 0)
    {
        LED_Write40(0xFFFFFFFFFFULL);

        printf("LED ALL ON\n");
    }
    else if (strcmp(arg1, "off") == 0)
    {
        LED_Write40(0x0000000000ULL);

        printf("LED ALL OFF\n");
    }
    else
    {
        return 0;
    }

    return 0;
}
void LED_Mode(uchar onoff){
	static uint8_t led_toggle_state = 0;
	switch(onoff){
	case _Off :
		LED_Clear();;
		break;
	case _On :
		 LED_Write40(0x5555555555ULL);
		 break;
	case _LED_MODE_TOGGLE :
        led_toggle_state = !led_toggle_state;
        if (led_toggle_state) {
            LED_Write40(0xAAAAAAAAAAULL);
        } else {
            LED_Clear();
        }
        break;
	}

}


