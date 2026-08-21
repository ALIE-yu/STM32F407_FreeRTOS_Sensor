# 5_pro — 环境监测系统项目概述

## 一、工程简介

基于 **STM32F407ZGTx + FreeRTOS** 的环境监测系统：

- 通过 **DHT11** 采集温湿度、**BH1750** 采集光照强度
- 数据经互斥信号量保护的共享结构体传递给各任务
- **OLED 显示屏** 本地实时显示，**蓝牙串口** 支持远程指令查询
- 支持 RGB 三色 LED 指示与按键输入

## 二、技术栈

| 类别 | 技术 |
|---|---|
| 主控 | STM32F407ZGTx（Cortex-M4F，168MHz，1024KB Flash / 192KB SRAM） |
| 固件库 | STM32 HAL 库 + CMSIS |
| 工程生成 | STM32CubeMX（`0_proj.ioc`） |
| 开发环境 | Keil MDK-ARM（`MDK-ARM/`） |
| 实时操作系统 | FreeRTOS（ARM_CM4F 移植层，任务 + 互斥信号量） |
| 协议 | UART 串口指令（蓝牙）、I2C（BH1750）、软件 I2C（OLED） |
| 语言 | C（C99） |

## 三、任务调度

所有任务由 `FreeRTOS_Start()`（[MyTask.c](MyTask/MyTask.c)）中的 `Start_Task` 统一创建，创建完成后 `Start_Task` 自删除。

| 任务 | 优先级 | 堆栈（字） | 周期 | 职责 |
|---|---|---|---|---|
| Start_Task | 1 | 128 | 一次性 | 创建 4 个子任务后自删除 |
| OLED_Task | 1 | 256 | 100 ms | 读取共享数据并刷新 OLED（温度/湿度/光照） |
| BH1750_Task | 1 | 128 | 100 ms | 通过 I2C1 读取 BH1750 光照，写入共享数据 |
| DHT11_Task | 1 | 128 | 1.5 s | GPIO 位带时序读取 DHT11 温湿度，写入共享数据 |
| BLUETOOTH_Task | 1 | 128 | 10 ms | 轮询解析 USART2 蓝牙指令并回显 |

## 四、外设与引脚明细

| 外设 | 引脚 | 配置 | 功能 |
|---|---|---|---|
| 系统时钟 | PH0/PH1（OSC_IN/OSC_OUT） | HSE 25MHz → PLL(M=25,N=336,P=2) → SYSCLK 168MHz | 系统主时钟 |
| USART2 | PA2 (TX) / PA3 (RX) | 接收中断 | 蓝牙模块通信 |
| I2C1 | PB6 (SCL) / PB7 (SDA)，AF4 | 硬件 I2C | BH1750 光照传感器（器件地址 0x23） |
| TIM6 | 内部时钟（84MHz / 84 = 1MHz） | 基础定时器 | DHT11 微秒级延时计数 |
| TIM7 | 内部时钟 | 基础定时器 | FreeRTOS 下的 HAL 时基（`HAL_IncTick`） |
| DHT11 数据线 | PC4 | GPIO 位带时序 | 温湿度传感器 |
| OLED SCL/SDA | PE13 / PE15 | 软件 I2C，从机地址 0x78，1.3 寸，内置中文字库 | 显示 |
| KEY1 | PA0 | 输入 | 按键 1 |
| KEY2 | PC13 | 输入 | 按键 2 |
| LED_R / LED_G / LED_B | PF6 / PF7 / PF8 | 推挽输出 | RGB 三色 LED |

## 五、数据流与同步

```
DHT11_Task ──┐                                    ┌──► OLED_Task（显示）
              ├─(写)─► g_sensor_data ─(读)─► 各任务
BH1750_Task ─┘        [g_sensor_mutex 保护]        └──► BLUETOOTH_Task（回显）
```

- 共享数据结构 `SensorData_t`（[MyTask.h](MyTask/MyTask.h)）：`light`(uint16) / `temp`(uint8) / `temp_0`(uint8) / `humi`(uint8)
- 所有读写均以 `xSemaphoreTake/give` 围绕互斥信号量 `g_sensor_mutex` 进行

## 六、蓝牙指令协议

指令格式：`key,<名称>,<动作>`（逗号分隔，`strtok` 解析），当前支持查询回显：

| 指令 | 响应 |
|---|---|
| `key,温度,up` | `当前温度：XX.X℃` |
| `key,湿度,up` | `当前湿度：XX%RH` |
| `key,光强,up` | `当前光强：XXXLX` |

## 七、目录结构

```
5_pro/
├── 0_proj.ioc            # CubeMX 工程配置
├── startup_stm32f407xx.s # 启动文件
├── Core/                 # CubeMX 生成代码（main/时钟/中断/HAL 外设初始化）
│   ├── Inc/              # main.h、FreeRTOSConfig.h、stm32f4xx_hal_conf.h 等
│   └── Src/              # main.c、gpio.c、usart.c、i2c.c、tim.c 等
├── Drivers/              # HAL 库 + CMSIS
├── FreeRTOS/             # FreeRTOS 内核源码（inc/portable/src）
├── HardWare/             # 外设驱动层（用户编写）
│   ├── bh1750.c/.h       # 光照传感器驱动（I2C1）
│   ├── dht11.c/.h        # 温湿度传感器驱动（GPIO 位带时序）
│   ├── oled.c/.h         # OLED 驱动（软件 I2C，含绘图/中文字库）
│   ├── oled_data.c/.h    # OLED 中文字模数据
│   ├── bluetooth.c/.h    # 蓝牙串口指令解析
│   ├── key.c/.h          # 按键消抖扫描
│   └── led.c/.h          # RGB LED 控制
├── MyTask/               # FreeRTOS 应用任务层
│   └── MyTask.c/.h       # 任务创建 + 共享数据定义
└── MDK-ARM/              # Keil 工程（STM32F407ZGTx）
```

## 八、启动流程

1. `HAL_Init()` → `SystemClock_Config()`（168MHz）
2. 外设初始化：GPIO / USART2 / TIM6 / I2C1
3. 关闭 RGB LED → 启动 TIM6 → 开启 USART2 接收
4. `FreeRTOS_Start()`：DHT11 → BH1750 → OLED 初始化 → 创建互斥锁 → 启动调度器
