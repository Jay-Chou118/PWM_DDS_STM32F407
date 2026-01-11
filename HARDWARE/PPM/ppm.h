#ifndef __PPM_H
#define __PPM_H
#include "sys.h"

// PPM 协议定义 (单位 us)
#define PPM_PULSE_WIDTH  50    // 脉冲宽度 50us
#define PPM_LOGIC_0      1000  // 逻辑0 周期
#define PPM_LOGIC_1      2000  // 逻辑1 周期
#define PPM_SYNC_FRAME   5000  // 同步/结束帧 周期

void PPM_Init(void);                // 初始化
void PPM_Send_Data(uint8_t *data, uint8_t len); // 发送数据
uint8_t PPM_Is_Busy(void);          // 查询是否正在发送

#endif
