#include "hal.h"

// 根据 GpioIndex 枚举获取对应的 GPIO 端口
GPIO_TypeDef* get_gpio_port(GpioIndex pin) {
    if (pin >= PA0 && pin <= PA15) {
        return GPIOA;
    } else if (pin >= PB0 && pin <= PB15) {
        return GPIOB;
    } else if (pin >= PC0 && pin <= PC15) {
        return GPIOC;
    } else if (pin >= PD0 && pin <= PD15) {
        return GPIOD;
    } else if (pin >= PE0 && pin <= PE15) {
        return GPIOE;
    } else if (pin >= PF0 && pin <= PF15) {
        return GPIOF;
    } else if (pin >= PG0 && pin <= PG15) {
        return GPIOG;
    } else if (pin >= PH0 && pin <= PH15) {
        return GPIOH;
    } else if (pin >= PI0 && pin <= PI15) {
        return GPIOI;
    }
    return NULL;  // 无效的引脚
}

// 根据 GpioIndex 枚举获取对应的引脚号 (0-15)
uint16_t get_gpio_pin(GpioIndex pin) {
    if (pin >= PA0 && pin <= PA15) {
        return 1 << (pin - PA0);
    } else if (pin >= PB0 && pin <= PB15) {
        return 1 << (pin - PB0);
    } else if (pin >= PC0 && pin <= PC15) {
        return 1 << (pin - PC0);
    } else if (pin >= PD0 && pin <= PD15) {
        return 1 << (pin - PD0);
    } else if (pin >= PE0 && pin <= PE15) {
        return 1 << (pin - PE0);
    } else if (pin >= PF0 && pin <= PF15) {
        return 1 << (pin - PF0);
    } else if (pin >= PG0 && pin <= PG15) {
        return 1 << (pin - PG0);
    } else if (pin >= PH0 && pin <= PH15) {
        return 1 << (pin - PH0);
    } else if (pin >= PI0 && pin <= PI15) {
        return 1 << (pin - PI0);
    }
    return 0;  // 无效的引脚
}

// 获取对应的 RCC 时钟使能寄存器位
uint32_t get_gpio_rcc_clock(GpioIndex pin) {
    if (pin >= PA0 && pin <= PA15) {
        return RCC_AHB1Periph_GPIOA;
    } else if (pin >= PB0 && pin <= PB15) {
        return RCC_AHB1Periph_GPIOB;
    } else if (pin >= PC0 && pin <= PC15) {
        return RCC_AHB1Periph_GPIOC;
    } else if (pin >= PD0 && pin <= PD15) {
        return RCC_AHB1Periph_GPIOD;
    } else if (pin >= PE0 && pin <= PE15) {
        return RCC_AHB1Periph_GPIOE;
    } else if (pin >= PF0 && pin <= PF15) {
        return RCC_AHB1Periph_GPIOF;
    } else if (pin >= PG0 && pin <= PG15) {
        return RCC_AHB1Periph_GPIOG;
    } else if (pin >= PH0 && pin <= PH15) {
        return RCC_AHB1Periph_GPIOH;
    } else if (pin >= PI0 && pin <= PI15) {
        return RCC_AHB1Periph_GPIOI;
    }
    return 0;  // 无效的引脚
}

static void gpio_init(GpioIndex pin, GpioDir dir) {
    GPIO_InitTypeDef GPIO_InitStruct;
    
    // 获取 GPIO 端口和引脚
    GPIO_TypeDef* port = get_gpio_port(pin);
    uint16_t pin_number = get_gpio_pin(pin);
    uint32_t rcc_clock = get_gpio_rcc_clock(pin);
    
    if (port == NULL || pin_number == 0) {
        return;  // 无效的引脚
    }
    
    // 使能对应端口的时钟
    RCC_AHB1PeriphClockCmd(rcc_clock, ENABLE);
    
    // 初始化 GPIO 结构体
    GPIO_StructInit(&GPIO_InitStruct);
    
    // 设置引脚
    GPIO_InitStruct.GPIO_Pin = pin_number;
    
    // 根据 mode 参数设置 GPIO 模式
    // 这里假设 mode 是 STM32 标准外设库的 GPIO 模式
    GPIO_InitStruct.GPIO_Mode = (GPIOMode_TypeDef)dir;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;  // 默认速度
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;     // 默认推挽输出
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;  // 默认无上下拉
    
    // 初始化 GPIO
    GPIO_Init(port, &GPIO_InitStruct);
}

static void gpio_write(GpioIndex pin, GpioLevel val) {
    GPIO_TypeDef* port = get_gpio_port(pin);
    uint16_t pin_number = get_gpio_pin(pin);
    
    if (port != NULL && pin_number != 0) {
        if (val) {
            port->BSRRL = pin_number;  // 设置引脚
        } else {
            port->BSRRH = pin_number;  // 清除引脚
        }
    }
}

static uint8_t gpio_read(GpioIndex pin) {
    GPIO_TypeDef* port = get_gpio_port(pin);
    uint16_t pin_number = get_gpio_pin(pin);
    
    if (port != NULL && pin_number != 0) {
        return (uint8_t)((port->IDR & pin_number) != 0);
    }
    return 0;  // 无效的引脚返回 0
}

const hal_gpio_ops_t hal_gpio = {
    .init = gpio_init,
    .write = gpio_write,
    .read = gpio_read
};