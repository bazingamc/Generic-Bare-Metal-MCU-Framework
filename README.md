# Generic Bare Metal MCU Framework

**A C++ Oriented, Scalable Bare-Metal Architecture for Multi-MCU Embedded Systems**

Generic Bare Metal MCU Framework 是一个面向工程实践的通用裸机嵌入式软件框架，基于 C/C++ 构建，采用分层架构与面向对象设计思想，旨在为多 MCU 平台提供一套可复用、可扩展、可长期演进的基础软件骨架。

该框架不依赖 RTOS，运行于纯裸机环境，通过统一的系统抽象、设备模型、任务机制与构建体系，实现从底层 MCU 启动、外设驱动到上层应用逻辑的完整软件栈组织方式。

本项目面向以下目标用户：

- 具有一定经验的嵌入式软件工程师
- 希望构建长期可维护裸机平台的个人或团队
- 对 C++ 裸机架构设计、模块化与系统抽象感兴趣的开源社区开发者

## 1. 设计目标

### 1.1 工程化而非示例化

本框架的目标不是提供零散的外设示例代码，而是构建一套具备工程属性的基础平台，包括：

- 统一的启动与系统初始化流程
- 清晰的软件分层与模块边界
- 可复用的设备抽象与驱动封装方式
- 可扩展的任务与调度模型
- 标准化的构建与多 MCU 选择机制

### 1.2 面向对象的裸机架构

在保持裸机可控性与确定性的前提下，引入 C++ 面向对象思想：

- 使用类描述设备（UART、GPIO、外设模块等）
- 使用对象封装协议、缓冲区、日志系统
- 使用统一接口与基类定义模块边界
- 通过组合而非继承构建系统功能

### 1.3 多 MCU 可扩展性

框架在架构层面考虑多 MCU 支持：

- MCU 相关代码集中于独立层级
- 启动文件、链接脚本、寄存器定义可替换
- HAL 接口保持稳定，上层逻辑无需感知具体芯片型号
- 通过 CMake 配置选择不同目标平台

### 1.4 长期演进与二次开发

框架设计面向长期维护与扩展：

- 支持逐步引入更多外设抽象
- 支持从裸机平滑演进到 RTOS
- 支持封装为静态库供多个项目复用
- 支持单元测试、仿真与持续集成的后期扩展

## 2. 总体软件架构

框架采用自底向上的分层结构，各层职责明确、依赖单向。

```
+--------------------------------------------------+
|                    Application (APP)             |
+--------------------------------------------------+
|            Device Abstraction & Services         |
|      (Device, Logger, Protocol, Task, System)   |
+--------------------------------------------------+
|          HAL - Hardware Abstraction Layer        |
|        (GPIO, UART, SPI, Timer, ADC, etc.)       |
+--------------------------------------------------+
|        MCU Support Package (Startup, BSP)        |
|   (Clock, Interrupt, Linker Script, Registers)  |
+--------------------------------------------------+
|                     Hardware                     |
+--------------------------------------------------+
```

各层说明：

**MCU Support Package（MCU 层）**
- 负责芯片相关内容：启动代码、时钟配置、中断向量表、链接脚本、寄存器定义。

**HAL（硬件抽象层）**
- 对外设进行统一接口封装，屏蔽寄存器细节，向上提供稳定的驱动 API。

**Device & Service Layer（设备与系统服务层）**
- 建立统一的设备模型、通信协议、缓冲区、日志系统、时间系统与任务框架。

**Application Layer（应用层）**
- 用户业务逻辑，仅依赖抽象接口，不直接操作寄存器。

## 3. 代码目录结构

当前工程的典型结构如下：

```
Generic-Bare-Metal-MCU-Framework
│
├── APP/                    # 应用层（用户任务与业务逻辑）
│
├── Frame/
│   ├── 01_mcu/             # MCU 启动、BSP、链接脚本
│   ├── 02_hal/             # 硬件抽象层（GPIO, UART, SPI, Timer...）
│   ├── 03_chip_driver/     # 外部芯片或复杂器件驱动
│   ├── 04_utils_cpp/       # 工具库（RingBuffer, Time, Protocol 等）
│   └── 05_device_cpp/      # 设备模型与系统服务（Device, Logger, Task）
│
├── main/                   # 系统入口与初始化流程
│
├── CMakeLists.txt          # 顶层构建脚本
└── README.md
```

依赖关系遵循自下而上原则：

```
MCU -> HAL -> Utils / Device -> APP
```

上层模块不得直接依赖下两层以上的实现细节。

## 4. 系统启动流程与生命周期

本框架在裸机环境下构建一套完整、可控的系统生命周期管理流程。从 MCU 上电复位到应用任务运行，整体过程可划分为以下阶段：

```
Reset_Handler
    |
    v
SystemInit()            // 时钟、内存、底层硬件初始化
    |
    v
C/C++ Runtime Init      // .data/.bss 初始化，全局对象构造
    |
    v
main()
    |
    v
System::Init()          // 框架级初始化
    |
    v
Device Initialization  // 设备注册与外设配置
    |
    v
Task Start              // 任务创建与启动
    |
    v
while(1) System::Run() // 主循环调度
```

### 4.1 启动阶段（MCU 层）

该阶段位于 [Frame/01_mcu](Frame/01_mcu)，典型职责包括：

- 堆栈初始化
- 中断向量表设置
- 时钟系统配置（HSE/HSI/PLL）
- SRAM、Flash 等存储器配置
- 跳转至 main()

该层完全与具体芯片相关，是移植新 MCU 时的主要修改点。

### 4.2 C/C++ 运行时初始化

在进入 main() 前，编译器运行时完成：

- .data 段拷贝到 RAM
- .bss 段清零
- 全局 / 静态对象构造函数执行

这使得框架可以安全使用：

- 全局单例对象
- 静态设备实例
- 构造函数完成注册与绑定

### 4.3 System 模块初始化

在 main() 中，首先调用：

```
System::Init();
```

该接口通常负责：

- 系统时钟抽象初始化
- Tick 计数器与时间基准建立
- 中断管理初始化
- 内存管理与缓冲区系统初始化
- 日志系统基础设施就绪

System 模块是整个框架的"内核服务层"，对上层提供统一的时间、调度和系统状态接口。

### 4.4 设备初始化与注册

所有外设与逻辑设备以"设备对象"形式存在，例如：

- [Uart](dev_uart.cpp)
- Gpio Output / Input
- SPI Device
- Sensor / Actuator Device

典型流程：

```
Uart uart1;
UartInitParam param;
uart1.Init(param);

DeviceManager::Register(&uart1);
```

设备初始化阶段完成：

- 硬件资源配置（GPIO 复用、时钟、DMA、中断）
- 软件对象与硬件实例绑定
- 注册到系统设备表，供日志、协议、任务访问

### 4.5 任务系统启动

任务模型位于 Device/Service 层，提供：

- 任务对象封装
- 状态机式运行模型
- 延时与周期调度
- 统一调度入口

示例：

```
Task task1("Task1", Task1Entry);
task1.start();
```

每个任务本质是一个函数指针 + 状态控制结构，由系统调度器统一调度。

### 4.6 主循环与调度机制

在裸机环境中，框架采用"集中调度式主循环"模型：

```
while (1)
{
    System::Run();
}
```

System::Run() 内部通常执行：

- 轮询已注册任务
- 判断任务状态与时间条件
- 调用就绪任务回调
- 处理协议解析、缓冲区收发
- 刷新软件定时器
- 执行低优先级后台服务

该模型具有以下特点：

- 确定性强
- 易于调试
- 无上下文切换开销
- 适合对实时性可控的中小型系统

## 5. 任务模型设计

### 5.1 设计思想

框架的任务系统并非线程，而是：

- 基于状态机的协作式调度
- 运行于单一主循环
- 通过时间片与状态条件触发执行

适用于：

- 通信协议处理
- 外设控制状态机
- 运动控制流程
- 周期采样与计算任务

### 5.2 Task 基本结构

典型 Task 定义形式：

```
void Task1Entry(Task& task)
{
    switch (task.state())
    {
    case 0:
        // 初始化状态
        task.delay(100);
        task.nextState(1);
        break;

    case 1:
        // 周期执行状态
        DoSomething();
        task.delay(10);
        break;
    }
}
```

调度特性：

- delay(ms)：非阻塞延时
- nextState(n)：状态迁移
- 支持一次性任务与周期性任务

### 5.3 与 RTOS 的关系

该任务模型定位为：

- RTOS 之前的架构基础
- RTOS 之下的最小调度内核
- 可作为将来接入 FreeRTOS、RT-Thread 的适配层

上层业务若遵循该 Task 抽象，将来迁移至 RTOS 只需替换调度实现。

## 6. 模块级架构与职责划分

### 6.1 01_mcu —— MCU 支持层（Startup & BSP）

该层完全与具体芯片相关，负责把"芯片"抽象成"可运行 C++ 系统"。

主要包含：

- 启动文件（startup_xxx.s / .c）
- 中断向量表
- 时钟系统初始化（HSE / HSI / PLL / Bus 分频）
- 链接脚本（Flash / RAM 布局）
- CMSIS 头文件与寄存器定义
- 系统滴答定时器（SysTick 或 TIM）

设计原则：

- 只做"上电到 main()"所必需的事情
- 不包含任何业务逻辑
- 不包含设备抽象
- 为上层提供：中断、时间基准、寄存器访问能力

移植新 MCU 时，通常只需修改本层。

### 6.2 02_hal —— 硬件抽象层（HAL）

HAL 层的目标是：

用统一的 C++ 接口屏蔽不同 MCU 外设寄存器差异。

典型模块：

- GPIO
- UART
- SPI
- I2C
- Timer
- ADC
- DMA（可选）

接口风格示例：

```
class HalUart
{
public:
    bool init(const HalUartConfig& cfg);
    int  write(const uint8_t* buf, uint16_t len);
    int  read(uint8_t* buf, uint16_t len);
    void enableRxInterrupt();
};
```

特点：

- 只做"驱动级抽象"，不涉及协议
- 不包含缓冲区策略
- 不关心日志、任务、调度
- 可被多个 Device 复用

### 6.3 04_utils_cpp —— 通用工具库

该层提供平台无关的基础设施：

#### 6.3.1 RingBuffer（环形缓冲区）

用途：

- 串口收发缓存
- 协议解析缓存
- 日志缓存

特性：

- 支持动态或静态大小
- 无锁（裸机环境）
- 支持 peek / pop / push / available

#### 6.3.2 Protocol（协议解析框架）

如：

- AsciiProtocol
- BinaryFrameProtocol

职责：

- 帧同步
- CRC 校验
- 字段解析
- 回调分发

与 UART 组合形成完整通信栈。

#### 6.3.3 Time 与 SystemTick

统一提供：

- 毫秒计时
- 微秒计时（如支持）
- 超时判断
- 任务延时基准

### 6.4 05_device_cpp —— 设备与系统服务层（核心）

这是整个框架的"中枢层"，将 HAL、Utils 组织为可用的系统对象。

#### 6.4.1 Device 基类

所有设备统一继承自：

```
class Device
{
public:
    virtual bool init(const DeviceInitParam& param) = 0;
    virtual void poll();      // 轮询处理
    virtual const char* name();
};
```

特性：

- 统一生命周期管理
- 统一注册与遍历
- 支持系统级调度与监控

#### 6.4.2 Uart 设备类

结构关系：

```
Uart(Device)
 ├─ HalUart       (硬件驱动)
 ├─ RingBuffer    (RX/TX缓存)
 ├─ Protocol[]    (协议解析器)
 └─ LoggerChannel (日志通道)
```

初始化示例：

```
AsciiProtocol proto;
Protocol* protos[] = { &proto };

Uart uart1(1024, 1024, protos, 1);

UartInitParam p;
p.name = "UART1";
p.baudrate = 115200;
p.port = UART_PORT1;

uart1.Init(p);
```

#### 6.4.3 Logger 日志系统

设计目标：

- 与具体输出介质解耦
- 支持多通道（UART / RTT / Flash / TCP 等）
- 支持时间戳与级别

结构：

```
Logger
 ├─ Channel[ ]  -> 绑定 Device (如 Uart)
 ├─ Formatter
 └─ TimeProvider
```

使用：

```
Logger::RegisterChannel(LOG_CH_UART1, &uart1);
Logger::SetTimeCallback(System::Time::getSysTime);

LOG_INFO(LOG_CH_UART1, "System started\r\n");
```

## 6.5 APP —— 应用层规范

应用层只做三件事：

- 定义设备实例
- 定义任务状态机
- 组织系统流程

不得：

- 直接操作寄存器
- 直接访问 HAL 结构体
- 破坏 Device 抽象边界

典型结构：

```
void APP_Init()
{
    InitDevices();
    InitTasks();
    StartTasks();
}
```

## 7. 关键类设计说明

### 7.1 System 类

职责：

- 系统初始化
- Tick 管理
- 主循环调度
- 全局服务入口

核心接口：

```
class System
{
public:
    static void Init();
    static void Run();
    static uint64_t GetTickMs();
};
```

### 7.2 Task 类

封装协作式任务：

```
class Task
{
public:
    Task(const char* name, TaskEntry entry);
    void start();
    void delay(uint32_t ms);
    void nextState(uint8_t s);
};
```

### 7.3 Protocol 类族

统一通信帧处理接口：

```
class Protocol
{
public:
    virtual void input(uint8_t byte) = 0;
    virtual bool frameReady() = 0;
    virtual void onFrame(const uint8_t* frame, uint16_t len) = 0;
};
```

## 8. 新增模块的标准流程

### 8.1 新增一个 MCU

步骤：

1. 复制 [01_mcu/stm32f4xx](Frame/01_mcu) 为 [01_mcu/stm32f1xx](Frame/01_mcu)
2. 替换：
   - startup
   - linker script
   - system_xxx.c
3. 实现：
   - SystemTick
   - IRQ 桥接层
4. 在 CMake 中新增 target 选择项

### 8.2 新增一个外设设备（如电机驱动）

推荐分层：

```
MotorDevice (Device)
 ├─ HalPwm
 ├─ HalGpio
 ├─ ControlStateMachine
 └─ Task
```

### 8.3 新增一个应用任务

流程：

1. 定义 TaskEntry
2. 注册 Task
3. 在 APP_Init 中 start()

## 9. CMake 构建体系与多 MCU 支持

本框架采用 CMake 作为唯一构建系统，目标是实现：

- 跨平台（Windows / Linux / macOS）
- 跨 IDE（VSCode / CLion / VS / 命令行）
- 跨 MCU（F1 / F4 / F7 / H7 / 其他 Cortex-M）

### 9.1 工程组织原则

每个 MCU 平台对应一个独立的 BSP 目录

启动文件、链接脚本、系统时钟配置与芯片强绑定

HAL 与上层代码完全复用

通过 CMake 变量选择目标平台

示意：

```
set(TARGET_MCU STM32F429)
add_subdirectory(Frame/01_mcu/${TARGET_MCU})
```

### 9.2 多 MCU 切换机制

推荐做法：

```
cmake -DTARGET_MCU=STM32F103 ..
cmake -DTARGET_MCU=STM32F429 ..
```

每个 MCU 子目录需提供：

- startup_xxx.s
- linker_xxx.ld
- system_xxx.c
- mcu.cmake（导出 include 路径与启动文件）

## 10. 移植与扩展规范

### 10.1 新增 MCU 的标准步骤

1. 建立新 BSP 目录：[Frame/01_mcu/YourMCU](Frame/01_mcu)
2. 提供：
   - 启动文件
   - 链接脚本
   - CMSIS 头文件
   - 时钟与中断初始化
3. 实现 SystemTick 接口
4. 在顶层 CMake 中注册该平台
5. 验证最小系统：串口打印 + SysTick 计时

该流程保证上层 HAL / Device / APP 无需改动。

### 10.2 新增 HAL 外设驱动

规则：

- 不直接暴露寄存器给 APP
- 接口以"功能抽象"而非"寄存器位"命名
- 提供阻塞与非阻塞两种工作模式（如适用）

例如新增 CAN：

```
02_hal/
 └── can/
     ├── hal_can.hpp
     └── hal_can_stm32f4.cpp
```

### 10.3 新增 Device 设备类

设备类应满足：

- 继承 Device 基类
- 持有一个或多个 HAL 实例
- 内部可包含协议、缓冲区、控制状态机
- 对外只暴露"设备级语义接口"

例如：

```
class Motor : public Device
{
public:
    bool init(const MotorParam& p);
    void setSpeed(int rpm);
    void stop();
};
```

### 10.4 新增任务的设计规范

任务应：

- 表达"流程"而非"中断处理"
- 以状态机形式组织
- 不阻塞主循环
- 只通过 Device 接口操作硬件

推荐结构：

```
void MotorTask(Task& t)
{
    switch (t.state())
    {
    case INIT:
        motor.init();
        t.nextState(RUN);
        break;

    case RUN:
        motor.control();
        t.delay(10);
        break;
    }
}
```

## 11. 框架演进方向

该框架定位为长期演进的平台型工程，后续可自然扩展至：

### 11.1 RTOS 适配层

- 保留 Device / HAL 抽象
- 用 RTOS Task 替换当前协作式 Task
- Logger、Protocol、RingBuffer 继续复用

### 11.2 仿真与单元测试

- HAL Mock 化
- 在 PC 端运行 Protocol / Task / Device 状态机
- 引入 GoogleTest / Catch2

### 11.3 静态库化

- Frame 层编译为 libgbmcu.a
- APP 作为独立工程链接
- 支持多项目共用同一底层平台

### 11.4 多核与高性能 MCU

- 支持 M7 / M33 / RISC-V
- 引入 Cache / MPU 抽象
- 支持高速 DMA 与多总线架构

## 12. 贡献与维护

建议的协作模式：

- 主分支保持可编译、可运行
- 新功能在 feature 分支开发
- 每个 MCU 平台维护独立文档
- 每个新增模块提供最小示例

## 13. 总结

Generic Bare Metal MCU Framework 的核心目标是：

在不依赖 RTOS 的前提下，构建一套结构清晰、可扩展、工程化的 C++ 裸机软件架构，使嵌入式系统开发从"堆驱动代码"升级为"平台化软件工程"。

它既可作为个人长期技术积累的基础平台，也可作为开源社区中研究裸机架构、面向对象嵌入式设计、多 MCU 抽象方法的参考实现。