#include "hal.h"

// Add DMA related variables and structure definitions
// DMA transfer status structure
typedef struct {
    volatile bool is_busy;  // DMA transfer busy flag
    DMA_Stream_TypeDef* dma_stream;  // Corresponding DMA stream
    uint32_t dma_channel;  // Corresponding DMA channel
    IRQn_Type dma_irqn;  // Corresponding DMA interrupt number
} uart_dma_config_t;

// Allocate DMA resources for each UART
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

// Get the corresponding USART/UART peripheral according to UartIndex enumeration
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

// Get the corresponding DMA request
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

// Get the corresponding RCC clock enable register bit
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

// Get the corresponding GPIO pin configuration (determine TX/RX pins according to UART number)
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
            *tx_pin = PA0;   // Invalid value
            *rx_pin = PA0;
            *gpio_af = 0;
            break;
    }
}

// Get the RCC clock for the corresponding GPIO
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

// Configure GPIO pins for UART
void configure_uart_gpio(UartIndex uart, GpioIndex txPin, GpioIndex rxPin) {
    GpioIndex tx_pin, rx_pin;
    uint32_t gpio_af;
    
    get_uart_gpio_config(uart, &tx_pin, &rx_pin, &gpio_af);
    if(txPin != PIN_END || rxPin != PIN_END)
    {
        tx_pin = txPin;
        rx_pin = rxPin;
    }    
    
    // Get GPIO port and pin numbers
    GPIO_TypeDef* tx_port = get_gpio_port(tx_pin);
    uint16_t tx_pin_num = get_gpio_pin(tx_pin);
    GPIO_TypeDef* rx_port = get_gpio_port(rx_pin);
    uint16_t rx_pin_num = get_gpio_pin(rx_pin);
    
    // Enable GPIO port clocks
    RCC_AHB1PeriphClockCmd(get_uart_gpio_rcc_clock(tx_pin), ENABLE);
    if (tx_port != rx_port) {  // If TX and RX are on different ports, also enable RX port clock
        RCC_AHB1PeriphClockCmd(get_uart_gpio_rcc_clock(rx_pin), ENABLE);
    }
    
    // Configure TX pin as alternate function
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin = tx_pin_num;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;  // TX pin pull-up
    GPIO_Init(tx_port, &GPIO_InitStruct);
    
    // Configure RX pin as alternate function
    GPIO_InitStruct.GPIO_Pin = rx_pin_num;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;  // RX pin pull-up
    GPIO_Init(rx_port, &GPIO_InitStruct);
    
    // Connect GPIO pins to UART peripheral
    GPIO_PinAFConfig(tx_port, (uint8_t)(tx_pin - (tx_pin / 16) * 16), gpio_af);
    GPIO_PinAFConfig(rx_port, (uint8_t)(rx_pin - (rx_pin / 16) * 16), gpio_af);
}

static void uart_init(UartIndex uart, uint32_t baudrate, GpioIndex txPin, GpioIndex rxPin) {
    USART_TypeDef* usart = get_uart_peripheral(uart);
    uint32_t uart_clock = get_uart_rcc_clock(uart);
    
    if (usart == NULL) {
        return;  // Invalid UART
    }
    
    // Enable UART clock
    if (uart == _UART1 || uart == _UART6) {
        RCC_APB2PeriphClockCmd(uart_clock, ENABLE);
    } else {
        RCC_APB1PeriphClockCmd(uart_clock, ENABLE);
    }
    
    // Configure GPIO pins for UART
    configure_uart_gpio(uart, txPin, rxPin);
    
    // Initialize UART
    USART_InitTypeDef USART_InitStruct;
    USART_StructInit(&USART_InitStruct);
    
    USART_InitStruct.USART_BaudRate = baudrate;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    
    USART_Init(usart, &USART_InitStruct);
    
    // Enable UART
    USART_Cmd(usart, ENABLE);
}

static void uart_send_byte(UartIndex uart, uint8_t data) {
    USART_TypeDef* usart = get_uart_peripheral(uart);
    
    if (usart != NULL) {
        // Wait until transmit data register is empty
        while(USART_GetFlagStatus(usart, USART_FLAG_TXE) == RESET);
        
        // Send data
        USART_SendData(usart, data);
        
        // Wait for transmission to complete
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
        // Wait until receive data register is not empty
        while(USART_GetFlagStatus(usart, USART_FLAG_RXNE) == RESET);
        
        // Read data
        return (uint8_t)USART_ReceiveData(usart);
    }
    
    return 0;  // Invalid UART returns 0
}

static int uart_is_data_available(UartIndex uart) {
    USART_TypeDef* usart = get_uart_peripheral(uart);
    
    if (usart != NULL) {
        return (USART_GetFlagStatus(usart, USART_FLAG_RXNE) == SET) ? 1 : 0;
    }
    
    return 0;  // Invalid UART returns 0
}

// DMA send completion callback function
static void uart_dma_tx_complete_callback(UartIndex uart) {
    uart_dma_configs[uart].is_busy = false;
    
    // Disable DMA request
    USART_DMACmd(get_uart_peripheral(uart), get_uart_dma_request(uart), DISABLE);
}

// DMA send byte array
static void uart_send_bytes_dma(UartIndex uart, uint8_t* data, uint16_t size) {
    USART_TypeDef* usart = get_uart_peripheral(uart);
    uart_dma_config_t* dma_config = &uart_dma_configs[uart];
    
    // Check parameter validity
    if (usart == NULL || data == NULL || size == 0) {
        return;
    }
    
    // Check if DMA is currently in use
    if (dma_config->is_busy) {
        // If in use, fall back to normal sending
        for (int i = 0; i < size; i++) {
            uart_send_byte(uart, data[i]);
        }
        return;
    }
    
    // Set DMA to busy state
    dma_config->is_busy = true;
    
    // Enable DMA clock
    if (dma_config->dma_stream < DMA2_Stream0) {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    } else {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
    }
    
    // Reset DMA stream
    DMA_DeInit(dma_config->dma_stream);
    while(DMA_GetCmdStatus(dma_config->dma_stream) != DISABLE);  // Wait for DMA stream to be disabled
    
    // Configure DMA
    DMA_InitTypeDef DMA_InitStructure;
    DMA_StructInit(&DMA_InitStructure);
    
    DMA_InitStructure.DMA_Channel = dma_config->dma_channel;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&usart->DR;  // Peripheral address set to USART data register
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)data;  // Memory address set to data to be sent
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;  // Memory to peripheral
    DMA_InitStructure.DMA_BufferSize = size;  // Transfer size
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  // Peripheral address does not increment
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  // Memory address increments
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  // Peripheral data width is byte
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  // Memory data width is byte
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  // Normal mode
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;  // Medium priority
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;  // Disable FIFO
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;  // FIFO threshold
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;  // Memory burst single transfer
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  // Peripheral burst single transfer
    
    DMA_Init(dma_config->dma_stream, &DMA_InitStructure);
    
    // Configure DMA interrupt
    DMA_ITConfig(dma_config->dma_stream, DMA_IT_TC, ENABLE);  // Enable transfer complete interrupt
    
    // Enable UART DMA send request
    USART_DMACmd(usart, get_uart_dma_request(uart), ENABLE);
    
    // Enable DMA stream
    DMA_Cmd(dma_config->dma_stream, ENABLE);
}

// DMA interrupt handler
void DMA1_Stream7_IRQHandler(void) {
    if(DMA_GetITStatus(DMA1_Stream7, DMA_IT_TCIF7)) {
        DMA_ClearITPendingBit(DMA1_Stream7, DMA_IT_TCIF7);
        DMA_Cmd(DMA1_Stream7, DISABLE);  // Disable DMA stream
        
        // Find which UART used this DMA stream
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
        DMA_Cmd(DMA1_Stream6, DISABLE);  // Disable DMA stream
        
        // Find which UART used this DMA stream
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
        DMA_Cmd(DMA1_Stream3, DISABLE);  // Disable DMA stream
        
        // Find which UART used this DMA stream
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
        DMA_Cmd(DMA1_Stream4, DISABLE);  // Disable DMA stream
        
        // Find which UART used this DMA stream
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
        DMA_Cmd(DMA2_Stream6, DISABLE);  // Disable DMA stream
        
        // Find which UART used this DMA stream
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
        DMA_Cmd(DMA1_Stream1, DISABLE);  // Disable DMA stream
        
        // Find which UART used this DMA stream
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
        DMA_Cmd(DMA1_Stream0, DISABLE);  // Disable DMA stream
        
        // Find which UART used this DMA stream
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