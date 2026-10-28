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


static uint64_t g_led_data = 0;


// LED 색상 정의 (2컬러 조합)
typedef enum {
    LED_COLOR_OFF    		= 0x0, // 꺼짐
    LED_COLOR_GREEN  		= 0x1, // 그린 켜짐
    LED_COLOR_RED    		= 0x2, // 레드 켜짐
    LED_COLOR_YELLOW 		= 0x3,  // 둘 다 켜짐 (노란색/주황색 등)
	LED_COLOR_GREEN_BLINK  	= 0x4  // 그린 점멸 모드 추가    
} LED_Color_t;

// 전체 포트(블록) 정의
// - 첫 번째 그룹(SYNC, CAL, ISO, POWER 및 BR1~BR2): U1, U2, U3 (24비트)
// - 두 번째 그룹(BR3, BR4): U4, U5 (16비트)
// 총 20개 LED × 2비트(G, R) = 40비트
typedef enum {
    // 첫 번째 시프트 레지스터 그룹 (U1)
    PORT_SYNC,
    PORT_CAL,
    PORT_ISO,
    PORT_POWER,
    
    // BR1 블록 (U2)
    PORT_BR1_1,
    PORT_BR1_2,
    PORT_BR1_3,
    PORT_BR1_4,
    
    // BR2 블록 (U3)
    PORT_BR2_1,
    PORT_BR2_2,
    PORT_BR2_3,
    PORT_BR2_4,

    // BR3 블록 (U4)
    PORT_BR3_1,
    PORT_BR3_2,
    PORT_BR3_3,
    PORT_BR3_4,

    // BR4 블록 (U5)
    PORT_BR4_1,
    PORT_BR4_2,
    PORT_BR4_3,
    PORT_BR4_4,

    PORT_MAX
} LED_Port_t;


// 포트별 점멸 상태 관리를 위한 구조체 (시간 카운터 제거, 토글 상태만 유지)
typedef struct {
    uint8_t is_blinking;   // 점멸 모드 활성화 여부 (1: 점멸중, 0: 일반)
    uint8_t blink_state;   // 현재 토글 상태 (1: 켜짐, 0: 꺼짐)
} LED_State_Ctrl_t;


// 포트별 상태 제어 블록 배열
static LED_State_Ctrl_t g_led_ctrl[PORT_MAX];

// 각 포트별 Green 비트와 Red 비트의 40비트(0~39) 상 절대 위치 매핑 정보 구조체
typedef struct {
    uint8_t g_bit; // Green 핀에 연결된 시프트 레지스터 비트 번호
    uint8_t r_bit; // Red 핀에 연결된 시프트 레지스터 비트 번호
} LED_Pin_Map_t;

/**
 * @brief 포트별 G/R 비트 매핑 테이블
 * @note 회로도의 U1~U5 실제 핀 배치(QA~QH 순서)에 맞춰 g_bit와 r_bit 번호(0~39)를 지정하세요.
 *       (아래는 직렬 연결 순서에 따른 예시 배치입니다.)
 */
static const LED_Pin_Map_t c_led_map[PORT_MAX] = {
    [PORT_SYNC]    = {  0,  1 },
    [PORT_CAL]     = {  2,  3 },
    [PORT_ISO]     = {  4,  5 },
    [PORT_POWER]   = {  6,  7 },
    
    [PORT_BR1_1]   = {  8,  9 },
    [PORT_BR1_2]   = { 10, 11 },
    [PORT_BR1_3]   = { 12, 13 },
    [PORT_BR1_4]   = { 14, 15 },
    
    [PORT_BR2_1]   = { 16, 17 },
    [PORT_BR2_2]   = { 18, 19 },
    [PORT_BR2_3]   = { 20, 21 },
    [PORT_BR2_4]   = { 22, 23 },

    [PORT_BR3_1]   = { 24, 25 },
    [PORT_BR3_2]   = { 26, 27 },
    [PORT_BR3_3]   = { 28, 29 },
    [PORT_BR3_4]   = { 30, 31 },

    [PORT_BR4_1]   = { 32, 33 },
    [PORT_BR4_2]   = { 34, 35 },
    [PORT_BR4_3]   = { 36, 37 },
    [PORT_BR4_4]   = { 38, 39 }
};

/**
 * @brief 특정 포트(LED)의 2컬러 상태를 설정하는 함수
 * @param port 제어할 포트 (예: PORT_SYNC, PORT_BR1_1 등)
 * @param color 설정할 색상 (LED_COLOR_OFF, LED_COLOR_GREEN, LED_COLOR_RED, LED_COLOR_YELLOW)
 */
void LED_SetColor(LED_Port_t port, LED_Color_t color)
{
	if (port >= PORT_MAX) return;

	uint8_t g_bit = c_led_map[port].g_bit;
	uint8_t r_bit = c_led_map[port].r_bit;

	// 기존 비트 및 점멸 설정 초기화
	g_led_data &= ~((1ULL << g_bit) | (1ULL << r_bit));
	g_led_ctrl[port].is_blinking = 0;

	if (color == LED_COLOR_GREEN_BLINK) {
		g_led_ctrl[port].is_blinking = 1;
		g_led_ctrl[port].blink_state = 1; // 처음에 켜진 상태로 시작
		g_led_data |= (1ULL << g_bit);	  // 그린 켜기
	} else {
		if (color & 0x1) g_led_data |= (1ULL << g_bit);
		if (color & 0x2) g_led_data |= (1ULL << r_bit);
	}

	// 하드웨어로 즉시 전송
	LED_Write40(g_led_data);
}

/**
 * @brief 토글 이벤트 발생 시 호출하는 함수 (예: 외부 트리거 또는 수동 토글 시)
 * @param port 토글할 포트 (예: PORT_BR1_1)
 */
void LED_ToggleBlink(LED_Port_t port)
{
    if (port >= PORT_MAX) return;

    // 점멸 모드가 켜져 있는 포트인 경우에만 토글 수행
    if (g_led_ctrl[port].is_blinking) {
        g_led_ctrl[port].blink_state ^= 1; // 상태 반전 (1 <-> 0)

        uint8_t g_bit = c_led_map[port].g_bit;

        if (g_led_ctrl[port].blink_state) {
            g_led_data |= (1ULL << g_bit);  // 켜기
        } else {
            g_led_data &= ~(1ULL << g_bit); // 꺼기
        }

        // 상태가 변경되었으므로 하드웨어로 전송
        LED_Write40(g_led_data);
    }
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
//        LED_Write40(0xFFFFFFFFFFULL);
//		LED_Write40(0x5555555555ULL);

        LED_Write40(0x0000000001ULL);
        printf("LED ALL ON\n");
    }
	else if (strcmp(arg1, "an") == 0)
	{
//		  LED_Write40(0xFFFFFFFFFFULL);
		LED_Write40(0x5555555555ULL);

//		LED_Write40(0x00 00 00 00 10ULL);
		printf("LED ALL ON\n");
	}

    else if (strcmp(arg1, "off") == 0)
    {
        LED_Write40(0x0000000000ULL);

        printf("LED ALL OFF\n");
    }

	
    else if (strcmp(arg1, "test") == 0)
    {
#if 0
		// 1. SYNC LED를 초록색(Green)으로 켜기
		LED_SetColor(PORT_SYNC, LED_COLOR_GREEN_BLINK);
		
		// 2. CAL LED를 빨간색(Red)으로 켜기
		LED_SetColor(PORT_CAL, LED_COLOR_GREEN_BLINK);

		LED_SetColor(PORT_ISO, LED_COLOR_GREEN_BLINK);
		LED_SetColor(PORT_POWER, LED_COLOR_GREEN_BLINK);
		
		// 3. BR1의 첫 번째 채널(#1)을 노란색(Green + Red 동시 점등)으로 켜기
		LED_SetColor(PORT_BR1_1, LED_COLOR_GREEN_BLINK);
		LED_SetColor(PORT_BR1_2, LED_COLOR_GREEN_BLINK);
		LED_SetColor(PORT_BR1_3, LED_COLOR_GREEN_BLINK);
		LED_SetColor(PORT_BR1_4, LED_COLOR_GREEN_BLINK);

		LED_SetColor(PORT_BR2_1, LED_COLOR_GREEN_BLINK);
		LED_SetColor(PORT_BR2_2, LED_COLOR_GREEN_BLINK);
		LED_SetColor(PORT_BR2_3, LED_COLOR_GREEN_BLINK);
		LED_SetColor(PORT_BR2_4, LED_COLOR_GREEN_BLINK);

		LED_SetColor(PORT_BR3_1, LED_COLOR_GREEN);
		LED_SetColor(PORT_BR3_2, LED_COLOR_GREEN);
		LED_SetColor(PORT_BR3_3, LED_COLOR_GREEN);
		LED_SetColor(PORT_BR3_4, LED_COLOR_GREEN);

		LED_SetColor(PORT_BR4_1, LED_COLOR_RED);
		LED_SetColor(PORT_BR4_2, LED_COLOR_RED);
		LED_SetColor(PORT_BR4_3, LED_COLOR_RED);
		LED_SetColor(PORT_BR4_4, LED_COLOR_RED);
#endif
		LED_SetColor(PORT_SYNC, LED_COLOR_OFF);
		LED_SetColor(PORT_CAL, LED_COLOR_OFF);
		LED_SetColor(PORT_ISO, LED_COLOR_OFF);
		LED_SetColor(PORT_POWER, LED_COLOR_GREEN);

		Update_RU_LED(0,__LED_Green);
		Update_RU_LED(1,__LED_Green);
		Update_RU_LED(2,__LED_Green);
		Update_RU_LED(3,__LED_Green);

		Update_RU_LED(4,__LED_Green);
		Update_RU_LED(5,__LED_Green);
		Update_RU_LED(6,__LED_Green);
		Update_RU_LED(7,__LED_Green);
		Update_RU_LED(8,__LED_Green);
		Update_RU_LED(9,__LED_Green);
		Update_RU_LED(10,__LED_Green);
		Update_RU_LED(11,__LED_Green);
		Update_RU_LED(12,__LED_Green);
		Update_RU_LED(13,__LED_Green);
		Update_RU_LED(14,__LED_Green);
		Update_RU_LED(15,__LED_Green);


        printf("LED test \n");
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
//            LED_Write40(0xAAAAAAAAAAULL);
			LED_Write40(0x5555555555ULL);
        } else {
            LED_Clear();
        }
//        printf("led_toggle_state[%d]\n",led_toggle_state);
        break;
	}

}

/**
 * @brief RU_Num (0 ~ 15)에 해당하는 실제 포트를 찾아주는 매핑 함수
 */
LED_Port_t Get_Port_From_RU(int ru_num)
{
    // RU_Num 0~15를 BR1_1부터 BR4_4까지 순차적으로 매핑
    // (시작 포트를 PORT_BR1_1로 지정)
    return (LED_Port_t)(PORT_BR1_1 + ru_num);
}

/**
 * @brief LED 제어 메인 함수
 * @param RU_Num 0 ~ 15
 * @param Mode LED 모드 (__LED_Green, __LED_Red_Light, __LED_Red_Blink, __LED_Green_Blink 등)
 */
void Update_RU_LED(int RU_Num, int Mode)
{
    // RU_Num 범위 체크 (0 ~ 15)
    if (RU_Num < 0 || RU_Num > 15) return;

    // RU 번호별 토글 상태를 유지하기 위한 16개 배열 선언 (정적 변수)
    static char trg_array[16] = {0};

    // RU_Num에 해당하는 포트 획득
    LED_Port_t target_port = Get_Port_From_RU(RU_Num);

    if (Mode == __LED_Green)            // 녹색 상시 
    {
        LED_SetColor(target_port, LED_COLOR_GREEN);
    }
    else if (Mode == __LED_Red_Light)   // 적색 상시 
    {
        LED_SetColor(target_port, LED_COLOR_RED);
    }
    else if (Mode == __LED_Red_Blink)   // 적색 토글 점멸 
    {
        if (trg_array[RU_Num])
            LED_SetColor(target_port, LED_COLOR_OFF);
        else
            LED_SetColor(target_port, LED_COLOR_RED);
    }
    else if (Mode == __LED_Green_Blink) // 녹색 토글 점멸 (기존 그린 점멸 함수 활용)
    {
        LED_SetColor(target_port, LED_COLOR_GREEN_BLINK);
    }
    else                                // 소등 		
    {
        LED_SetColor(target_port, LED_COLOR_OFF);
    }

    // 해당 RU 번호의 토글 상태 반전
    trg_array[RU_Num] = !trg_array[RU_Num];
}



// Mode = 0 : 녹색 , Mode = 1 : 적색점등, Mode = 2 : 적색점멸,  Mode = 3 :  소등 
// Mode = 0 : 녹색 , Mode = 1 : 적색점등, Mode = 2 : 적색점멸,  Mode = 4 : 녹색점멸 ,  Mode = 3 :  소등 

void Led_display(char RU_Num,char Mode)
{
	struct repeater_struct *prpt;
	prpt = getRptDev();

	if(prpt->Tree.sau_pri_ru[0][0])
	{
		Update_RU_LED(0,Mode);
	}

	if(prpt->Tree.sau_pri_ru[0][1])
	{
		Update_RU_LED(1,Mode);
	}
	
	if(prpt->Tree.sau_pri_ru[0][2])
	{
		Update_RU_LED(2,Mode);
	}
	
	if(prpt->Tree.sau_pri_ru[0][3])
	{
		Update_RU_LED(3,Mode);
	}
///
	if(prpt->Tree.sau_pri_ru[1][0])
	{
		Update_RU_LED(4,Mode);
	}

	if(prpt->Tree.sau_pri_ru[1][1])
	{
		Update_RU_LED(5,Mode);
	}
	
	if(prpt->Tree.sau_pri_ru[1][2])
	{
		Update_RU_LED(6,Mode);
	}
	
	if(prpt->Tree.sau_pri_ru[1][3])
	{
		Update_RU_LED(7,Mode);
	}


/////
	if(prpt->Tree.sau_pri_ru[2][0])
	{
		Update_RU_LED(8,Mode);
	}

	if(prpt->Tree.sau_pri_ru[2][1])
	{
		Update_RU_LED(9,Mode);
	}
	
	if(prpt->Tree.sau_pri_ru[2][2])
	{
		Update_RU_LED(10,Mode);
	}
	
	if(prpt->Tree.sau_pri_ru[2][3])
	{
		Update_RU_LED(11,Mode);
	}
///
	if(prpt->Tree.sau_pri_ru[3][0])
	{
		Update_RU_LED(12,Mode);
	}

	if(prpt->Tree.sau_pri_ru[3][1])
	{
		Update_RU_LED(13,Mode);
	}
	
	if(prpt->Tree.sau_pri_ru[3][2])
	{
		Update_RU_LED(14,Mode);
	}
	
	if(prpt->Tree.sau_pri_ru[3][3])
	{
		Update_RU_LED(15,Mode);
	}

	#if 0
	if(RU_Num == 1) {
		static char trg = 0;
		if(Mode == __LED_Green)		// 녹색 
		{
			RU2_LED_G(1);
			RU2_LED_R(0);
		}
		else if(Mode == __LED_Red_Light) // 적색 점등 
		{
			RU2_LED_G(0);
			RU2_LED_R(1);
		}
		else if(Mode == __LED_Red_Blink) // 적색 점멸 
		{
			RU2_LED_G(0);
			RU2_LED_R(trg);
		}
		else if(Mode == __LED_Green_Blink) 	// 소등 녹색점멸 
		{
			RU2_LED_G(trg);
			RU2_LED_R(0);
		}
		else			    // 소등 		
		{
			RU2_LED_G(0);
			RU2_LED_R(0);
		}
		trg = !trg;
	}	

	if(RU_Num == 2) {
		static char trg = 0;
		if(Mode == __LED_Green)		// 녹색 
		{
			RU3_LED_G(1);
			RU3_LED_R(0);
		}
		else if(Mode == __LED_Red_Light) // 적색 점등 
		{
			RU3_LED_G(0);
			RU3_LED_R(1);
		}
		else if(Mode == __LED_Red_Blink) // 적색 점멸 
		{
			RU3_LED_G(0);
			RU3_LED_R(trg);
		}
		else if(Mode == __LED_Green_Blink) 	// 소등 녹색점멸 
		{
			RU3_LED_G(trg);
			RU3_LED_R(0);
		}
		else			    // 소등 		
		{
			RU3_LED_G(0);
			RU3_LED_R(0);
		}
		trg = !trg;
	}	

	if(RU_Num == 3) {
		static char trg = 0;
		if(Mode == __LED_Green)		// 녹색 
		{
			RU4_LED_G(1);
			RU4_LED_R(0);
		}
		else if(Mode == __LED_Red_Light) // 적색 점등 
		{
			RU4_LED_G(0);
			RU4_LED_R(1);
		}
		else if(Mode == __LED_Red_Blink) // 적색 점멸 
		{
			RU4_LED_G(0);
			RU4_LED_R(trg);
		}
		else if(Mode == __LED_Green_Blink) 	// 소등 녹색점멸 
		{
			RU4_LED_G(trg);
			RU4_LED_R(0);
		}
		else			    // 소등 		
		{
			RU4_LED_G(0);
			RU4_LED_R(0);
		}
		trg = !trg;
	}

	if(RU_Num == 4) {
		static char trg = 0;
		if(Mode == __LED_Green)		// 녹색 
		{
			RU5_LED_G(1);
			RU5_LED_R(0);
		}
		else if(Mode == __LED_Red_Light) // 적색 점등 
		{
			RU5_LED_G(0);
			RU5_LED_R(1);
		}
		else if(Mode == __LED_Red_Blink) // 적색 점멸 
		{
			RU5_LED_G(0);
			RU5_LED_R(trg);
		}
		else if(Mode == __LED_Green_Blink) 	// 소등 녹색점멸 
		{
			RU5_LED_G(trg);
			RU5_LED_R(0);
		}
		else			    // 소등 		
		{
			RU5_LED_G(0);
			RU5_LED_R(0);
		}
		trg = !trg;
	}

	if(RU_Num == 5) {
		static char trg = 0;
		if(Mode == __LED_Green)		// 녹색 
		{
			RU6_LED_G(1);
			RU6_LED_R(0);
		}
		else if(Mode == __LED_Red_Light) // 적색 점등 
		{
			RU6_LED_G(0);
			RU6_LED_R(1);
		}
		else if(Mode == __LED_Red_Blink) // 적색 점멸 
		{
			RU6_LED_G(0);
			RU6_LED_R(trg);
		}
		else if(Mode == __LED_Green_Blink) 	// 소등 녹색점멸 
		{
			RU6_LED_G(trg);
			RU6_LED_R(0);
		}
		else			    // 소등 		
		{
			RU6_LED_G(0);
			RU6_LED_R(0);
		}
		trg = !trg;
	}

	if(RU_Num == 6) {

		static char trg = 0;

		if(Mode == __LED_Green)		// 녹색 
		{
			RU7_LED_G(1);
			RU7_LED_R(0);
		}
		else if(Mode == __LED_Red_Light) // 적색 점등 
		{
			RU7_LED_G(0);
			RU7_LED_R(1);
		}
		else if(Mode == __LED_Red_Blink) // 적색 점멸 
		{
			RU7_LED_G(0);
			RU7_LED_R(trg);
		}
		else if(Mode == __LED_Green_Blink) 	// 소등 녹색점멸 
		{
			RU7_LED_G(trg);
			RU7_LED_R(0);
		}
		else			    // 소등 		
		{
			RU7_LED_G(0);
			RU7_LED_R(0);
		}
		trg = !trg;
	}

	if(RU_Num == 7) {
		static char trg = 0;
		if(Mode == __LED_Green)		// 녹색 
		{
			RU8_LED_G(1);
			RU8_LED_R(0);
		}
		else if(Mode == __LED_Red_Light) // 적색 점등 
		{
			RU8_LED_G(0);
			RU8_LED_R(1);
		}
		else if(Mode == __LED_Red_Blink) // 적색 점멸 
		{
			RU8_LED_G(0);
			RU8_LED_R(trg);
		}
		else if(Mode == __LED_Green_Blink) 	// 소등 녹색점멸 
		{
			RU8_LED_G(trg);
			RU8_LED_R(0);
		}
		else			    // 소등 		
		{
			RU8_LED_G(0);
			RU8_LED_R(0);
		}
		trg = !trg;
	}

	if(RU_Num == 8) {
		static char trg = 0;
		if(Mode == __LED_Green)		// 녹색 
		{
			RU9_LED_G(1);
			RU9_LED_R(0);
		}
		else if(Mode == __LED_Red_Light) // 적색 점등 
		{
			RU9_LED_G(0);
			RU9_LED_R(1);
		}
		else if(Mode == __LED_Red_Blink) // 적색 점멸 
		{
			RU9_LED_G(0);
			RU9_LED_R(trg);
		}
		else if(Mode == __LED_Green_Blink) 	// 소등 녹색점멸 
		{
			RU9_LED_G(trg);
			RU9_LED_R(0);
		}
		else			    // 소등 		
		{
			RU9_LED_G(0);
			RU9_LED_R(0);
		}
		trg = !trg;
	}

	if(RU_Num == 9) {
		static char trg = 0;
		if(Mode == __LED_Green)		// 녹색 
		{
			RU10_LED_G(1);
			RU10_LED_R(0);
		}
		else if(Mode == __LED_Red_Light) // 적색 점등 
		{
			RU10_LED_G(0);
			RU10_LED_R(1);
		}
		else if(Mode == __LED_Red_Blink) // 적색 점멸 
		{
			RU10_LED_G(0);
			RU10_LED_R(trg);
		}
		else if(Mode == __LED_Green_Blink) 	// 소등 녹색점멸 
		{
			RU10_LED_G(trg);
			RU10_LED_R(0);
		}		
		else			    // 소등 		
		{
			RU10_LED_G(0);
			RU10_LED_R(0);
		}
		trg = !trg;
	}	

	if(RU_Num == 10) {
		static char trg = 0;
		if(Mode == __LED_Green)		// 녹색 
		{
			RU11_LED_G(1);
			RU11_LED_R(0);
		}
		else if(Mode == __LED_Red_Light) // 적색 점등 
		{
			RU11_LED_G(0);
			RU11_LED_R(1);
		}
		else if(Mode == __LED_Red_Blink) // 적색 점멸 
		{
			RU11_LED_G(0);
			RU11_LED_R(trg);
		}
		else if(Mode == __LED_Green_Blink) 	// 소등 녹색점멸 
		{
			RU11_LED_G(trg);
			RU11_LED_R(0);
		}
		else			    // 소등 		
		{
			RU11_LED_G(0);
			RU11_LED_R(0);
		}
		trg = !trg;
	}	

	if(RU_Num == 11) {
		static char trg = 0;
		if(Mode == __LED_Green)		// 녹색 
		{
			RU12_LED_G(1);
			RU12_LED_R(0);
		}
		else if(Mode == __LED_Red_Light) // 적색 점등 
		{
			RU12_LED_G(0);
			RU12_LED_R(1);
		}
		else if(Mode == __LED_Red_Blink) // 적색 점멸 
		{
			RU12_LED_G(0);
			RU12_LED_R(trg);
		}
		else if(Mode == __LED_Green_Blink) 	// 소등 녹색점멸 
		{
			RU12_LED_G(trg);
			RU12_LED_R(0);
		}
		else			    // 소등 		
		{
			RU12_LED_G(0);
			RU12_LED_R(0);
		}
		trg = !trg;
	}		

	if(RU_Num == 12) {
		static char trg = 0;
		if(Mode == __LED_Green)		// 녹색 
		{
			RU13_LED_G(1);
			RU13_LED_R(0);
		}
		else if(Mode == __LED_Red_Light) // 적색 점등 
		{
			RU13_LED_G(0);
			RU13_LED_R(1);
		}
		else if(Mode == __LED_Red_Blink) // 적색 점멸 
		{
			RU13_LED_G(0);
			RU13_LED_R(trg);
		}
		else if(Mode == __LED_Green_Blink) 	// 소등 녹색점멸 
		{
			RU13_LED_G(trg);
			RU13_LED_R(0);
		}
		else			    // 소등 		
		{
			RU13_LED_G(0);
			RU13_LED_R(0);
		}
		trg = !trg;
	}	

	if(RU_Num == 13) {
		static char trg = 0;
		if(Mode == __LED_Green)		// 녹색 
		{
			RU14_LED_G(1);
			RU14_LED_R(0);
		}
		else if(Mode == __LED_Red_Light) // 적색 점등 
		{
			RU14_LED_G(0);
			RU14_LED_R(1);
		}
		else if(Mode == __LED_Red_Blink) // 적색 점멸 
		{
			RU14_LED_G(0);
			RU14_LED_R(trg);
		}
		else if(Mode == __LED_Green_Blink) 	// 소등 녹색점멸 
		{
			RU14_LED_G(trg);
			RU14_LED_R(0);
		}
		else			    // 소등 		
		{
			RU14_LED_G(0);
			RU14_LED_R(0);
		}
		trg = !trg;
	}	

	if(RU_Num == 14) {

		static char trg = 0;
		if(Mode == __LED_Green)		// 녹색 
		{
			RU15_LED_G(1);
			RU15_LED_R(0);
		}
		else if(Mode == __LED_Red_Light) // 적색 점등 
		{
			RU15_LED_G(0);
			RU15_LED_R(1);
		}
		else if(Mode == __LED_Red_Blink) // 적색 점멸 
		{
			RU15_LED_G(0);
			RU15_LED_R(trg);
		}
		else if(Mode == __LED_Green_Blink) 	// 소등 녹색점멸 
		{
			RU15_LED_G(trg);
			RU15_LED_R(0);
		}
		else			    // 소등 		
		{
			RU15_LED_G(0);
			RU15_LED_R(0);
		}
		trg = !trg;
	}	

	if(RU_Num == 15) {
		static char trg = 0;
		if(Mode == __LED_Green)		// 녹색 
		{
			RU16_LED_G(1);
			RU16_LED_R(0);
		}
		else if(Mode == __LED_Red_Light) // 적색 점등 
		{
			RU16_LED_G(0);
			RU16_LED_R(1);
		}
		else if(Mode == __LED_Red_Blink) // 적색 점멸 
		{
			RU16_LED_G(0);
			RU16_LED_R(trg);
		}
		else if(Mode == __LED_Green_Blink) 	// 소등 녹색점멸 
		{
			RU16_LED_G(trg);
			RU16_LED_R(0);
		}
		else			    // 소등 		
		{
			RU16_LED_G(0);
			RU16_LED_R(0);
		}
		trg = !trg;
	}	
	#endif
}

