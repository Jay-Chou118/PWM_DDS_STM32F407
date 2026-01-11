# STM32F407 PPM 信号发生器 (模拟 10kHz 载波)

本项目基于正点原子 STM32F407 开发板（Explorer），使用 **TIM3** 定时器产生 PPM (Pulse Position Modulation) 信号。该信号专门用于模拟 10kHz 方波作为脉冲载体的通信协议。

## 1. 硬件引脚定义 (Pinout)

| 功能 | 引脚 | 描述 |
| :--- | :--- | :--- |
| **PPM 信号输出** | **PA6** | TIM3_CH1，输出 50us 脉宽的调制脉冲 |
| **发送触发按键** | **PE4** | KEY0，按下后发送一帧数据 |
| **状态指示灯** | **PF10** | LED1，发送数据时亮起 |
| **调试串口** | **PA9/PA10** | UART1 (115200)，打印调试信息 |

> **注意**：请务必将示波器探头连接到 **PA6** 引脚和 **GND** 以观察波形。

---

## 2. PPM 协议定义 (Protocol)

本工程实现了基于脉冲间隔的 PPM 调制。

* **时基 (Tick)**: $1 \mu s$ (定时器计数频率为 1MHz)
* **脉冲宽度 (Pulse Width)**: 固定 **50us** (对应 10kHz 方波的高电平持续时间)
* **信号逻辑**:
    * **Logic 0**: 脉冲间隔（周期）为 **1000us** (1ms)
    * **Logic 1**: 脉冲间隔（周期）为 **2000us** (2ms)
    * **Sync / Idle**: 脉冲间隔（周期）为 **5000us** (5ms)

### 波形示意图
```text
      |<-- 50us -->|                  |<-- 50us -->|
      +------------+                  +------------+
      |            |                  |            |
______|            |__________________|            |______
      ^                               ^
      |                               |
      |<-------- Period (T) --------->|

      If T = 1000us -> Data bit '0'
      If T = 2000us -> Data bit '1'

```

---

## 3. 计算公式与原理 (Calculations)

STM32F407 的 TIM3 挂载在 APB1 总线上。

* **System Core Clock**: 168 MHz
* **APB1 Clock**: 42 MHz
* **TIM3 Internal Clock**: 

### 3.1 定时器频率计算

我们需要将定时器计数频率配置为 **1 MHz** (即  计数一次)，以便于精确控制时间。

代入数值：

* **PSC (预分频系数)**: 83

### 3.2 脉冲宽度 (10kHz 模拟)

我们要产生 10kHz 的方波信号。

* 10kHz 完整周期 = 
* 10kHz 半周期 (高电平) = 

在代码中，`TIM_OCInitStructure.Pulse` (CCR寄存器) 决定了高电平持续时间。
由于时基是 ，所以设置 **CCR = 50** 即代表 。

---

## 4. 如何修改参数 (Modification Guide)

### 4.1 修改载波频率 (当前为 10kHz)

如果你想改变输出脉冲的宽度（模拟不同的频率），请修改 `ppm.c` 中的 `PPM_Init` 函数。

**公式**: 

* **例子**：如果你需要 **20kHz** 的信号。
* 目标脉宽 = 。
* 修改代码如下：



```c
// 文件: ppm.c -> PPM_Init 函数
#define PPM_PULSE_WIDTH  25   // 修改这里：25us 对应 20kHz
TIM_OCInitStructure.Pulse = PPM_PULSE_WIDTH;

```

### 4.2 修改 PPM 编码间隔

如果你想改变逻辑 0/1 代表的时间长度，请修改 `ppm.h` 中的宏定义：

```c
// 文件: ppm.h
#define PPM_LOGIC_0      1000  // 修改逻辑0的周期 (单位: us)
#define PPM_LOGIC_1      2000  // 修改逻辑1的周期 (单位: us)
#define PPM_SYNC_FRAME   5000  // 修改同步帧周期 (单位: us)

```

### 4.3 修改发送的数据

在 `main.c` 中修改 `test_payload` 数组：

```c
// 文件: main.c
uint8_t test_payload[] = {0xAA, 0x55, 0x12}; // 修改为你想要发送的数据

```

---

## 5. 快速开始

1. 确保工程中包含 `stm32f4xx_hal_tim.c`。
2. 确保 `stm32f4xx_hal_conf.h` 中开启了 `HAL_TIM_MODULE_ENABLED`。
3. 编译并下载代码到 STM32F407。
4. 将示波器接到 **PA6**。
5. 按下板子上的 **KEY0** 键，观察示波器上的脉冲序列。

```

```