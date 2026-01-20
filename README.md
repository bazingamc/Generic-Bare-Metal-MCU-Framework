# 通用裸机MCU框架

一个通用的裸机MCU基础框架，提供标准化的底层驱动与模块化软件结构，以STM32F4xx系列微控制器为例，可快速搭建MCU开发环境。

## 项目概述

本项目旨在简化裸机开发中的重复性工作（如启动代码配置、外设初始化），提供统一的硬件抽象层（HAL）和工具组件，提升代码可维护性和复用性。支持C++17开发，便于实现面向对象的设备管理与任务调度。

## 主要特性

- 基于ST标准外设库（StdPeriph Driver）的MCU初始化支持
- 硬件抽象层（HAL）封装常用外设（GPIO、UART、SPI、ADC、CAN、Timer等）
- C++实用工具：环形缓冲区、ASCII协议解析、任务调度机制
- 设备模型管理（输入/输出/日志/系统/UART设备）
- 架构化分层设计，易于扩展和移植
- 支持C++17面向对象编程
- 使用CMake构建系统，跨平台兼容性强

## 系统架构

项目采用典型的嵌入式分层架构模式：

```
应用层                → APP/
硬件抽象层            → Frame/02_hal
芯片级驱动层          → Frame/03_chip_driver
C++工具库            → Frame/04_utils_cpp
设备管理层            → Frame/05_device_cpp
MCU核心与BSP层       → Frame/01_mcu
```

## 目录结构

```
├── APP                 # 应用层，用户业务逻辑
│   ├── APP.hpp
│   ├── TaskDemo.cpp    # 示例任务实现
│   └── TaskDemo.hpp    # 示例任务声明
├── Frame               # 框架层
│   ├── 01_mcu          # MCU底层支持包
│   ├── 02_hal          # 硬件抽象层
│   ├── 03_chip_driver  # 芯片级驱动预留层
│   ├── 04_utils_cpp    # C++工具库
│   └── 05_device_cpp   # 设备管理层
└── main                # 主程序入口
    └── main.cpp
```

## 技术规格

- **主控芯片**: STM32F4xx / GD32F4xx
- **内核**: ARM Cortex-M4 (with FPU)
- **语言标准**: C++17, C11
- **编译器**: arm-none-eabi-gcc/g++
- **构建系统**: CMake ≥ v3.19
- **浮点支持**: 硬浮点（-mfloat-abi=hard -mfpu=fpv4-sp-d16）

## 快速开始

### 环境准备

1. 安装ARM GCC工具链（arm-none-eabi-gcc）
2. 安装CMake并确保版本 ≥ 3.19
3. 克隆项目到本地目录

### 构建项目

```bash
mkdir build
cd build
cmake ..
make
```

### 输出文件

构建完成后会生成以下文件：

- `${PROJECT_NAME}.elf`: 可执行镜像
- `${PROJECT_NAME}.srec`: S-Record格式烧录文件
- `${PROJECT_NAME}.map`: 内存映射分析文件

### 部署

使用OpenOCD、J-Link Commander或ST-Link Utility烧录`.srec`或`.elf`文件。

## 任务系统

本框架提供了一个基于状态机的任务调度系统，允许开发者轻松创建和管理多个并发任务：

```cpp
// 创建任务（推荐使用全局对象）
Task t1("t1", Task1);  // 创建任务1
Task t2("t2", Task2);  // 创建任务2
```

任务可以包含多个状态，通过`userState`进行管理，并支持延迟、子任务启动等功能。

## 设备管理系统

框架提供了面向对象的设备管理，包括：

- 输入设备（Input）
- 输出设备（Output）
- UART设备（Uart）
- 系统设备（System）
- 日志设备（Log）

这些设备可以通过统一的接口进行管理和使用。

## 示例代码

项目包含LED闪烁、串口初始化、log发送、状态机任务示例：

```cpp

Output led1;
// Protocol list
AsciiProtocol* protos[] = {&default_proto};
Uart uart1(1024, 1024, protos, 1);
void APP_Init() 
{
    System::init();

    // Program running indicator light
	OutputInitParam param;
	param.name = "LED1";
	param.pin = PH10;
	param.validLevel = GPIO_LEVEL_LOW;
	led1.init(param);
	led1.pulseOutputStart();// Start pulse output, default parameters: period 1000ms, duty cycle 50%

	// UART
	UartInitParam uartParam;
	uartParam.name = "UART1";
	uartParam.uart = _UART1;
	uartParam.baudrate = 115200;
	uart1.Init(uartParam);

	// log
	Logger::RegisterChannel(LOG_CH_UART, &uart1);
	Logger::SetTimeCallback(System::Time::getSysTime);
	LOG_INFO(LOG_CH_UART, "hello world !\r\n");
}

int main(void)
{
	APP_Init();

	t1.start();

	while(1)
	{
		System::run();
	}
}

```

## 许可证

本项目采用MIT许可证 - 详见[LICENSE](./LICENSE)文件

## 贡献

欢迎提交Pull Request来改进本框架！对于重大更改，请先开Issue讨论您想要改变的内容。

## 咨询
详细咨询、定制开发，联系：57612742@qq.com

