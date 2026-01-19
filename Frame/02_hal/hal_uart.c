#include "hal.h"

// 添加DMA相关变量和结构体定义
// DMA传输状态结构体
typedef struct {
    volatile bool is_busy;  // DMA传输忙标志
    DMA_Stream_TypeDef* dma_stream;  // 对应的DMA流
    uint32_t dma_channel;  // 对应的DMA通道
    IRQn_Type dma_irqn;  // 对应的DMA中断号
} uart_dma_config_t;

// 为每个UART分配DMA资源
static uart_dma_config_t uart_dma_configs[] = {
    [_UART1] = { .is_busy = false, .dma_stream = DMA1_Stream7, .dma_channel = DMA_Channel_4, .dma_irqn = DMA1_Stream7_IRQn },
    [_UART2] = { .is_busy = false, .dma_stream = DMA1_Stream6, .dma_channel = DMA_Channel_4, .dma_irqn = DMA1_Stream6_IRQn },
    [_UART3] = { .is_busy = false, .dma_stream = DMA1_Stream3, .dma_channel = DMA_Channel_4, .dma_irqn = DMA1_Stream3_IRQn },
    [_UART4] = { .is_busy = false, .dma_stream = DMA1_Stream4, .dma_channel = DMA_Channel_4, .dma_irqn = DMA1_Stream4_IRQn },
    [_UART5] = { .is_busy = false, .dma_stream = DMA1_Stream7, .dma_channel = DMA_Channel_4, .dma_irqn = DMA1_Stream7_IRQn },
    [_UART6] = { .is_busy = false, .dma_stream = DMA2_Stream6, .dma_channel = DMA_Channel_5, .dma_irqn = DMA2_Stream6_IRQn },
    [_UART7] = { .is_busy = false, .dma_stream = DMA1_Stream1, .dma_channel = DMA_Channel_5, .dma_irqn = DMA1_Stream1_IRQn },
    [_UART8] = { .is_busy = false, .dma_stream = DMA1_Stream0, .dma_channel = DMA_Channel_5, .dma_irqn = DMA1_Stream0_IRQn }
};

// 根据 UartIndex 枚举获取对应的 USART/UART 外设
USART_TypeDef* get_uart_peripheral(UartIndex uart) {
    switch(uart) {
        case _UART1: return USART1;
        case _UART2: return USART2;
        case _UART3: return USART3;
        case _UART4: return UART4;
        case _UART5: return UART5;
        case _UART6: return USART6;
        case _UART7: return UART7;
        case _UART8: return UART8;
        default: return NULL;
    }
}

// 获取对应的 DMA请求
uint32_t get_uart_dma_request(UartIndex uart) {
    switch(uart) {
        case _UART1: return USART_DMAReq_Tx;
        case _UART2: return USART_DMAReq_Tx;
        case _UART3: return USART_DMAReq_Tx;
        case _UART4: return USART_DMAReq_Tx;
        case _UART5: return USART_DMAReq_Tx;
        case _UART6: return USART_DMAReq_Tx;
        case _UART7: return USART_DMAReq_Tx;
        case _UART8: return USART_DMAReq_Tx;
        default: return 0;
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
        case _UART7: return RCC_APB1Periph_UART7;
        case _UART8: return RCC_APB1Periph_UART8;
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
        case _UART7:
            *tx_pin = PE8;   // PE8 = TX
            *rx_pin = PE7;   // PE7 = RX
            *gpio_af = GPIO_AF_UART7;
            break;
        case _UART8:
            *tx_pin = PE1;   // PE1 = TX
            *rx_pin = PE0;   // PE0 = RX
            *gpio_af = GPIO_AF_UART8;
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
void configure_uart_gpio(UartIndex uart, GpioIndex txPin, GpioIndex rxPin) {
    GpioIndex tx_pin, rx_pin;
    uint32_t gpio_af;
    
    get_uart_gpio_config(uart, &tx_pin, &rx_pin, &gpio_af);
    if(txPin != PIN_END || rxPin != PIN_END)
    {
        tx_pin = txPin;
        rx_pin = rxPin;
    }    
    
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

static void uart_init(UartIndex uart, uint32_t baudrate, GpioIndex txPin, GpioIndex rxPin) {
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
    configure_uart_gpio(uart, txPin, rxPin);
    
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

static void uart_send_bytes(UartIndex uart, u16 len, uint8_t* data) {
    if (data == NULL) return;

    for (uint16_t i = 0; i < len; i++)
    {
        uart_send_byte(uart, data[i]);
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

// DMA发送完成回调函数
static void uart_dma_tx_complete_callback(UartIndex uart) {
    uart_dma_configs[uart].is_busy = false;
    
    // 关闭DMA请求
    USART_DMACmd(get_uart_peripheral(uart), get_uart_dma_request(uart), DISABLE);
}

// DMA发送字节数组
static void uart_send_bytes_dma(UartIndex uart, uint8_t* data, uint16_t size) {
    USART_TypeDef* usart = get_uart_peripheral(uart);
    uart_dma_config_t* dma_config = &uart_dma_configs[uart];
    
    // 检查参数有效性
    if (usart == NULL || data == NULL || size == 0) {
        return;
    }
    
    // 检查DMA是否正在被使用
    if (dma_config->is_busy) {
        // 如果正在使用，则回退到普通发送
        for (int i = 0; i < size; i++) {
            uart_send_byte(uart, data[i]);
        }
        return;
    }
    
    // 设置DMA为忙碌状态
    dma_config->is_busy = true;
    
    // 使能DMA时钟
    if (dma_config->dma_stream < DMA2_Stream0) {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    } else {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
    }
    
    // 复位DMA流
    DMA_DeInit(dma_config->dma_stream);
    while(DMA_GetCmdStatus(dma_config->dma_stream) != DISABLE);  // 等待DMA流禁用
    
    // 配置DMA
    DMA_InitTypeDef DMA_InitStructure;
    DMA_StructInit(&DMA_InitStructure);
    
    DMA_InitStructure.DMA_Channel = dma_config->dma_channel;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&usart->DR;  // 外设地址设置为USART的数据寄存器
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)data;  // 内存地址设置为要发送的数据
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;  // 内存到外设
    DMA_InitStructure.DMA_BufferSize = size;  // 传输大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  // 外设地址不增加
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  // 内存地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  // 外设数据宽度为字节
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  // 内存数据宽度为字节
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  // 正常模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;  // 中等优先级
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;  // 禁用FIFO
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;  // FIFO阈值
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;  // 内存突发单次传输
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  // 外设突发单次传输
    
    DMA_Init(dma_config->dma_stream, &DMA_InitStructure);
    
    // 配置DMA中断
    DMA_ITConfig(dma_config->dma_stream, DMA_IT_TC, ENABLE);  // 使能传输完成中断
    
    // 使能UART的DMA发送请求
    USART_DMACmd(usart, get_uart_dma_request(uart), ENABLE);
    
    // 使能DMA流
    DMA_Cmd(dma_config->dma_stream, ENABLE);
}

// DMA中断处理函数
void DMA1_Stream7_IRQHandler(void) {
    if(DMA_GetITStatus(DMA1_Stream7, DMA_IT_TCIF7)) {
        DMA_ClearITPendingBit(DMA1_Stream7, DMA_IT_TCIF7);
        DMA_Cmd(DMA1_Stream7, DISABLE);  // 禁用DMA流
        
        // 查找是哪个UART使用的这个DMA流
        for(int i = 0; i < 8; i++) {
            if(uart_dma_configs[i].dma_stream == DMA1_Stream7) {
                uart_dma_tx_complete_callback((UartIndex)i);
                break;
            }
        }
    }
}

void DMA1_Stream6_IRQHandler(void) {
    if(DMA_GetITStatus(DMA1_Stream6, DMA_IT_TCIF6)) {
        DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TCIF6);
        DMA_Cmd(DMA1_Stream6, DISABLE);  // 禁用DMA流
        
        // 查找是哪个UART使用的这个DMA流
        for(int i = 0; i < 8; i++) {
            if(uart_dma_configs[i].dma_stream == DMA1_Stream6) {
                uart_dma_tx_complete_callback((UartIndex)i);
                break;
            }
        }
    }
}

void DMA1_Stream3_IRQHandler(void) {
    if(DMA_GetITStatus(DMA1_Stream3, DMA_IT_TCIF3)) {
        DMA_ClearITPendingBit(DMA1_Stream3, DMA_IT_TCIF3);
        DMA_Cmd(DMA1_Stream3, DISABLE);  // 禁用DMA流
        
        // 查找是哪个UART使用的这个DMA流
        for(int i = 0; i < 8; i++) {
            if(uart_dma_configs[i].dma_stream == DMA1_Stream3) {
                uart_dma_tx_complete_callback((UartIndex)i);
                break;
            }
        }
    }
}

void DMA1_Stream4_IRQHandler(void) {
    if(DMA_GetITStatus(DMA1_Stream4, DMA_IT_TCIF4)) {
        DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_TCIF4);
        DMA_Cmd(DMA1_Stream4, DISABLE);  // 禁用DMA流
        
        // 查找是哪个UART使用的这个DMA流
        for(int i = 0; i < 8; i++) {
            if(uart_dma_configs[i].dma_stream == DMA1_Stream4) {
                uart_dma_tx_complete_callback((UartIndex)i);
                break;
            }
        }
    }
}

void DMA2_Stream6_IRQHandler(void) {
    if(DMA_GetITStatus(DMA2_Stream6, DMA_IT_TCIF6)) {
        DMA_ClearITPendingBit(DMA2_Stream6, DMA_IT_TCIF6);
        DMA_Cmd(DMA2_Stream6, DISABLE);  // 禁用DMA流
        
        // 查找是哪个UART使用的这个DMA流
        for(int i = 0; i < 8; i++) {
            if(uart_dma_configs[i].dma_stream == DMA2_Stream6) {
                uart_dma_tx_complete_callback((UartIndex)i);
                break;
            }
        }
    }
}

void DMA1_Stream1_IRQHandler(void) {
    if(DMA_GetITStatus(DMA1_Stream1, DMA_IT_TCIF1)) {
        DMA_ClearITPendingBit(DMA1_Stream1, DMA_IT_TCIF1);
        DMA_Cmd(DMA1_Stream1, DISABLE);  // 禁用DMA流
        
        // 查找是哪个UART使用的这个DMA流
        for(int i = 0; i < 8; i++) {
            if(uart_dma_configs[i].dma_stream == DMA1_Stream1) {
                uart_dma_tx_complete_callback((UartIndex)i);
                break;
            }
        }
    }
}

void DMA1_Stream0_IRQHandler(void) {
    if(DMA_GetITStatus(DMA1_Stream0, DMA_IT_TCIF0)) {
        DMA_ClearITPendingBit(DMA1_Stream0, DMA_IT_TCIF0);
        DMA_Cmd(DMA1_Stream0, DISABLE);  // 禁用DMA流
        
        // 查找是哪个UART使用的这个DMA流
        for(int i = 0; i < 8; i++) {
            if(uart_dma_configs[i].dma_stream == DMA1_Stream0) {
                uart_dma_tx_complete_callback((UartIndex)i);
                break;
            }
        }
    }
}

const hal_uart_ops_t hal_uart = {
    .init = uart_init,
    .send_byte = uart_send_byte,
    .send_bytes = uart_send_bytes,
    .send_string = uart_send_string,
    .receive_byte = uart_receive_byte,
    .is_data_available = uart_is_data_available,
    .send_bytes_dma = uart_send_bytes_dma
    
};