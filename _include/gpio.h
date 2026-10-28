#include "xgpiops.h"
#include "xparameters.h"

extern XGpioPs Gpio;

#define BR_COUNT    4

// PLL control
#define MIO_PLL_B3_LD			27
#define MIO_PLL_B3_EN			31
#define MIO_PLL_DATA			51
#define MIO_PLL_CLK				52
#define MIO_PLL_B28_EN			53
#define MIO_PLL_B28_LD			26
                            	
// ATT Control              	
#define MIO_ATT1_B3_EN      	45
#define MIO_ATT2_B3_EN      	46
#define MIO_ATT_DATA			41
#define MIO_ATT_CLK				42
#define MIO_ATT1_B28_EN			43
#define MIO_ATT2_B28_EN			50

/* PWR_ENABLE */
#define BR1_PWR_ENABLE_GPIO    12
#define BR2_PWR_ENABLE_GPIO    13
#define BR3_PWR_ENABLE_GPIO    14
#define BR4_PWR_ENABLE_GPIO    15

/* PWR_MON */
#define BR1_PWR_MON_GPIO       34
#define BR2_PWR_MON_GPIO       35
#define BR3_PWR_MON_GPIO       36
#define BR4_PWR_MON_GPIO       37

static const u32 br_pwr_enable_gpio[BR_COUNT] = {
    BR1_PWR_ENABLE_GPIO,
    BR2_PWR_ENABLE_GPIO,
    BR3_PWR_ENABLE_GPIO,
    BR4_PWR_ENABLE_GPIO
};

static const u32 br_pwr_mon_gpio[BR_COUNT] = {
    BR1_PWR_MON_GPIO,
    BR2_PWR_MON_GPIO,
    BR3_PWR_MON_GPIO,
    BR4_PWR_MON_GPIO
};

#define LED_SCLR       49
#define LED_RCLK       39
#define LED_SCLK       38
#define LED_MOSI       48

#define LED_SCLR_BIT   17
#define LED_RCLK_BIT    7
#define LED_SCLK_BIT    6
#define LED_MOSI_BIT   16

#define GPIO_BASE       0xE000A000U

#define GPIO_DATA_1     (GPIO_BASE + 0x0044U)
#define GPIO_DIRM_1     (GPIO_BASE + 0x0244U)
#define GPIO_OEN_1      (GPIO_BASE + 0x0248U)

extern int gpio_init();



