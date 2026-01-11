#include "ppm.h"
#include "usart.h" // 用于打印调试信息


TIM_HandleTypeDef TIM3_Handler;      // TIM3句柄

// 发送缓冲区与状态控制
static uint8_t  ppm_buffer[64];      // 数据缓冲区
static volatile uint8_t  ppm_len = 0;         // 数据长度
static volatile uint8_t  ppm_byte_idx = 0;    // 当前字节索引
static volatile uint8_t  ppm_bit_idx = 0;     // 当前位索引
static volatile uint8_t  is_sending = 0;      // 发送状态标志

// 初始化TIM3 Channel 1 (PA6)
// APB1总线时钟42M，定时器时钟84M
void PPM_Init(void)
{
    TIM_OC_InitTypeDef TIM_OCInitStructure;
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_TIM3_CLK_ENABLE();    // 使能TIM3时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();   // 使能GPIOA时钟

    // 1. 配置GPIO PA6 复用为 TIM3
    GPIO_Initure.Pin = GPIO_PIN_6;
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;    // 复用推挽输出
    GPIO_Initure.Pull = GPIO_PULLDOWN;
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;
    GPIO_Initure.Alternate = GPIO_AF2_TIM3; // 复用为TIM3
    HAL_GPIO_Init(GPIOA, &GPIO_Initure);

    // 2. 配置TIM3基础参数
    // 目的: 计数频率 1MHz (1us/tick)
    // Prescaler = 84M / 1M - 1 = 83
    TIM3_Handler.Instance = TIM3;
    TIM3_Handler.Init.Prescaler = 83;       
    TIM3_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;
    TIM3_Handler.Init.Period = PPM_SYNC_FRAME - 1; // 默认周期 (Sync)
    TIM3_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&TIM3_Handler);

    // 3. 配置PWM通道
    TIM_OCInitStructure.OCMode = TIM_OCMODE_PWM1; 
    TIM_OCInitStructure.Pulse = PPM_PULSE_WIDTH;   // 固定脉宽 50us
    TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;
    TIM_OCInitStructure.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&TIM3_Handler, &TIM_OCInitStructure, TIM_CHANNEL_1);

    // 4. 配置中断
    HAL_NVIC_SetPriority(TIM3_IRQn, 1, 3);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);

    // 5. 开启更新中断和PWM输出
    __HAL_TIM_ENABLE_IT(&TIM3_Handler, TIM_IT_UPDATE);
    HAL_TIM_PWM_Start(&TIM3_Handler, TIM_CHANNEL_1);
}

// 开始发送数据
void PPM_Send_Data(uint8_t *data, uint8_t len)
{
    if(is_sending || len == 0 || len > 64) return; // 如果正在忙或数据无效，退出
    
    for(int i=0; i<len; i++)
    {
        ppm_buffer[i] = data[i];
    }
    ppm_len = len;
    ppm_byte_idx = 0;
    ppm_bit_idx = 0;
    is_sending = 1; // 标记开始发送
}

// 查询发送状态
uint8_t PPM_Is_Busy(void)
{
    return is_sending;
}

// 定时器更新中断回调 (核心逻辑)
// 这里的逻辑是：当前周期结束进入中断，设定 下一个 周期的时间
void TIM3_IRQHandler(void)
{
    // 检查更新中断标志
    if(__HAL_TIM_GET_FLAG(&TIM3_Handler, TIM_FLAG_UPDATE) != RESET)
    {
        if(__HAL_TIM_GET_IT_SOURCE(&TIM3_Handler, TIM_IT_UPDATE) !=RESET)
        {
            __HAL_TIM_CLEAR_IT(&TIM3_Handler, TIM_IT_UPDATE);

            uint32_t next_period = PPM_SYNC_FRAME; // 默认发同步/空闲帧

            if(is_sending)
            {
                // 取出当前要发送的位 (高位先发 MSB First)
                uint8_t current_bit = (ppm_buffer[ppm_byte_idx] >> (7 - ppm_bit_idx)) & 0x01;
                
                if(current_bit) next_period = PPM_LOGIC_1; // 1 -> 2000us
                else            next_period = PPM_LOGIC_0; // 0 -> 1000us

                // 索引移动
                ppm_bit_idx++;
                if(ppm_bit_idx >= 8)
                {
                    ppm_bit_idx = 0;
                    ppm_byte_idx++;
                    // 检查是否发完所有字节
                    if(ppm_byte_idx >= ppm_len)
                    {
                        is_sending = 0; // 发送结束，下一帧将是 Sync
                    }
                }
            }
            // 否则 (is_sending == 0)，保持发送 Sync 帧 (5ms)

            // 【关键】修改自动重装载值，影响下一个周期
            __HAL_TIM_SET_AUTORELOAD(&TIM3_Handler, next_period - 1);
        }
    }
}
