#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "ad9833.h"
#include "ppm.h"

uint8_t test_payload[] = {0xAA, 0x55, 0x12}; // 二进制: 10101010, 01010101, 00010010
uint8_t  Wave_Type = AD9833_Out_Sinus;
uint32_t Wave_Fre = 175000;
float CH1_Phase = 0;	// 范围0~360
float CH2_Phase = 0;	// 范围0~360


int main(void)
{
    u8 key;
    
    HAL_Init();                     // 初始化HAL库    
    Stm32_Clock_Init(336,8,2,7);    // 设置时钟,168Mhz
    delay_init(168);                // 初始化延时函数
    uart_init(115200);              // 初始化USART
    LED_Init();                     // 初始化LED
    KEY_Init();                     // 初始化按键
		AD9833_Init();								  // AD7606引脚初始化
    
		AD9833_Set_Frequency(AD9833_ALL, Wave_Type, (uint32_t)(Wave_Fre * 10.73742));		// 发送频率控制字
    AD9833_Set_Phase(Wave_Type, (uint32_t)(Wave_Fre * 10.73742), CH1_Phase, CH2_Phase);
	
    PPM_Init();                     // 初始化PPM模块 (TIM3 Channel 1 @ PA6)
	
    printf("STM32F407 PPM Generator Initialized.\r\n");
    printf("Pulse Width: 50us (10kHz equivalent)\r\n");
    printf("Press KEY0 to send data.\r\n");
		

    while(1)
    {
				

        key = KEY_Scan(0); // 扫描按键，不支持连按
        
        if(key == 1) // 你的代码逻辑里 KEY0 按下返回 KEY0_PRES (通常是1)
        {
            // 仅当PPM空闲时才允许发送新数据，防止冲突
            if(PPM_Is_Busy() == 0)
            {
                LED1 = 0; // 点亮LED1表示正在发送
                printf("KEY0 Pressed: Sending PPM Data...\r\n");
                
                // 发送数据
                PPM_Send_Data(test_payload, sizeof(test_payload));
                
                LED1 = 1; // 熄灭LED1
            }
            else
            {
                printf("System Busy.\r\n");
            }
        }
        LED0 = !LED0;
        delay_ms(100);

    }
}
