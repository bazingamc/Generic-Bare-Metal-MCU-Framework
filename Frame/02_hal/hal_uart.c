#include "hal.h"


// 根据 UartIndex 枚举获取对应的 USART/UART 外设
USART_TypeDef* get_uart_peripheral(UartIndex uart) {
    switch(uart) {
        case _UART1: return USART1;
        case _UART2: return USART2;
        case _UART3: return USART3;
        case _UART4: return UART4;
        case _UART5: return UART5;
        case _UART6: return USART6;
        default: return NULL;
    }
}

// 获取对应的 RCC 时钟使能寄存器位
uint32_t get_uart_rcc_clock(UartIndex uart) {
    switch(uart) {
        case _UART1: return RCC_APB2Periph_USART1;
        case _UART2: return RCC_APB1Periph_USART2;
        case _UART3: return RCC_APB1Periph_USART3;
        case _UART4: return RCC_APB1Periph_UART4;
        case _UART5: return RCC_APB1Periph_UART5;
        case _UART6: return RCC_APB2Periph_USART6;
        default: return 0;
    }
}

// 获取对应的 GPIO 引脚配置（根据 UART 编号确定 TX/RX 引脚）
void get_uart_gpio_config(UartIndex uart, GpioIndex* tx_pin, GpioIndex* rx_pin, uint32_t* gpio_af) {
    switch(uart) {
        case _UART1:
            *tx_pin = PA9;   // PA9 = TX
            *rx_pin = PA10;  // PA10 = RX
            *gpio_af = GPIO_AF_USART1;
            break;
        case _UART2:
            *tx_pin = PA2;   // PA2 = TX
            *rx_pin = PA3;   // PA3 = RX
            *gpio_af = GPIO_AF_USART2;
            break;
        case _UART3:
            *tx_pin = PD8;   // PD8 = TX
            *rx_pin = PD9;   // PD9 = RX
            *gpio_af = GPIO_AF_USART3;
            break;
        case _UART4:
            *tx_pin = PC10;  // PC10 = TX
            *rx_pin = PC11;  // PC11 = RX
            *gpio_af = GPIO_AF_UART4;
            break;
        case _UART5:
            *tx_pin = PC12;  // PC12 = TX
            *rx_pin = PD2;   // PD2 = RX
            *gpio_af = GPIO_AF_UART5;
            break;
        case _UART6:
            *tx_pin = PC6;   // PC6 = TX
            *rx_pin = PC7;   // PC7 = RX
            *gpio_af = GPIO_AF_USART6;
            break;
        default:
            *tx_pin = PA0;   // 无效值
            *rx_pin = PA0;
            *gpio_af = 0;
            break;
    }
}

// 获取对应 GPIO 的 RCC 时钟
uint32_t get_uart_gpio_rcc_clock(GpioIndex pin) {
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
    return 0;
}

// 配置 UART 的 GPIO 引脚
void configure_uart_gpio(UartIndex uart) {
    GpioIndex tx_pin, rx_pin;
    uint32_t gpio_af;
    
    get_uart_gpio_config(uart, &tx_pin, &rx_pin, &gpio_af);
    
    // 获取 GPIO 端口和引脚号
    GPIO_TypeDef* tx_port = get_gpio_port(tx_pin);
    uint16_t tx_pin_num = get_gpio_pin(tx_pin);
    GPIO_TypeDef* rx_port = get_gpio_port(rx_pin);
    uint16_t rx_pin_num = get_gpio_pin(rx_pin);
    
    // 使能 GPIO 端口时钟
    RCC_AHB1PeriphClockCmd(get_uart_gpio_rcc_clock(tx_pin), ENABLE);
    if (tx_port != rx_port) {  // 如果TX和RX在不同端口，也要使能RX端口时钟
        RCC_AHB1PeriphClockCmd(get_uart_gpio_rcc_clock(rx_pin), ENABLE);
    }
    
    // 配置 TX 引脚为复用功能
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin = tx_pin_num;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;  // TX 引脚上拉
    GPIO_Init(tx_port, &GPIO_InitStruct);
    
    // 配置 RX 引脚为复用功能
    GPIO_InitStruct.GPIO_Pin = rx_pin_num;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;  // RX 引脚上拉
    GPIO_Init(rx_port, &GPIO_InitStruct);
    
    // 连接 GPIO 引脚到 UART 外设
    GPIO_PinAFConfig(tx_port, (uint8_t)(tx_pin - (tx_pin / 16) * 16), gpio_af);
    GPIO_PinAFConfig(rx_port, (uint8_t)(rx_pin - (rx_pin / 16) * 16), gpio_af);
}

static void uart_init(UartIndex uart, uint32_t baudrate) {
    USART_TypeDef* usart = get_uart_peripheral(uart);
    uint32_t uart_clock = get_uart_rcc_clock(uart);
    
    if (usart == NULL) {
        return;  // 无效的 UART
    }
    
    // 使能 UART 时钟
    if (uart == _UART1 || uart == _UART6) {
        RCC_APB2PeriphClockCmd(uart_clock, ENABLE);
    } else {
        RCC_APB1PeriphClockCmd(uart_clock, ENABLE);
    }
    
    // 配置 UART 的 GPIO 引脚
    configure_uart_gpio(uart);
    
    // 初始化 UART
    USART_InitTypeDef USART_InitStruct;
    USART_StructInit(&USART_InitStruct);
    
    USART_InitStruct.USART_BaudRate = baudrate;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    
    USART_Init(usart, &USART_InitStruct);
    
    // 使能 UART
    USART_Cmd(usart, ENABLE);
}

static void uart_send_byte(UartIndex uart, uint8_t data) {
    USART_TypeDef* usart = get_uart_peripheral(uart);
    
    if (usart != NULL) {
        // 等待发送数据寄存器空
        while(USART_GetFlagStatus(usart, USART_FLAG_TXE) == RESET);
        
        // 发送数据
        USART_SendData(usart, data);
        
        // 等待发送完成
        while(USART_GetFlagStatus(usart, USART_FLAG_TC) == RESET);
    }
}

static void uart_send_string(UartIndex uart, const char* str) {
    if (str == NULL) return;
    
    while(*str) {
        uart_send_byte(uart, *str++);
    }
}

static uint8_t uart_receive_byte(UartIndex uart) {
    USART_TypeDef* usart = get_uart_peripheral(uart);
    
    if (usart != NULL) {
        // 等待接收数据寄存器非空
        while(USART_GetFlagStatus(usart, USART_FLAG_RXNE) == RESET);
        
        // 读取数据
        return (uint8_t)USART_ReceiveData(usart);
    }
    
    return 0;  // 无效的 UART 返回 0
}

static int uart_is_data_available(UartIndex uart) {
    USART_TypeDef* usart = get_uart_peripheral(uart);
    
    if (usart != NULL) {
        return (USART_GetFlagStatus(usart, USART_FLAG_RXNE) == SET) ? 1 : 0;
    }
    
    return 0;  // 无效的 UART 返回 0
}

const hal_uart_ops_t hal_uart = {
    .init = uart_init,
    .send_byte = uart_send_byte,
    .send_string = uart_send_string,
    .receive_byte = uart_receive_byte,
    .is_data_available = uart_is_data_available
};