#include "hal.h"



// Get the corresponding CAN peripheral according to CanIndex enumeration
CAN_TypeDef* get_can_peripheral(CanIndex can) {
    switch(can) {
        case _CAN1: return CAN1;
        case _CAN2: return CAN2;
        default: return NULL;
    }
}

// Get the corresponding RCC clock enable register bit
uint32_t get_can_rcc_clock(CanIndex can) {
    switch(can) {
        case _CAN1: return RCC_APB1Periph_CAN1;
        case _CAN2: return RCC_APB1Periph_CAN2;
        default: return 0;
    }
}

// Get the corresponding GPIO pin configuration (determine TX/RX pins according to CAN number)
void get_can_gpio_config(CanIndex can, GpioIndex* tx_pin, GpioIndex* rx_pin, uint32_t* gpio_af) {
    switch(can) {
        case _CAN1:
            *tx_pin = PD1;   // PD1 = CAN1_TX
            *rx_pin = PD0;   // PD0 = CAN1_RX
            *gpio_af = GPIO_AF_CAN1;
            break;
        case _CAN2:
            *tx_pin = PB13;  // PB13 = CAN2_TX
            *rx_pin = PB12;  // PB12 = CAN2_RX
            *gpio_af = GPIO_AF_CAN2;
            break;
        default:
            *tx_pin = PA0;   // Invalid value
            *rx_pin = PA0;
            *gpio_af = 0;
            break;
    }
}

// Get the RCC clock for the corresponding GPIO
uint32_t get_can_gpio_rcc_clock(GpioIndex pin) {
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

// Configure GPIO pins for CAN
void configure_can_gpio(CanIndex can) {
    GpioIndex tx_pin, rx_pin;
    uint32_t gpio_af;
    
    get_can_gpio_config(can, &tx_pin, &rx_pin, &gpio_af);
    
    // Get GPIO port and pin numbers
    GPIO_TypeDef* tx_port = get_gpio_port(tx_pin);
    uint16_t tx_pin_num = get_gpio_pin(tx_pin);
    GPIO_TypeDef* rx_port = get_gpio_port(rx_pin);
    uint16_t rx_pin_num = get_gpio_pin(rx_pin);
    
    // Enable GPIO port clocks
    RCC_AHB1PeriphClockCmd(get_can_gpio_rcc_clock(tx_pin), ENABLE);
    if (tx_port != rx_port) {  // If TX and RX are on different ports, also enable RX port clock
        RCC_AHB1PeriphClockCmd(get_can_gpio_rcc_clock(rx_pin), ENABLE);
    }
    
    // Configure TX pin as alternate function
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin = tx_pin_num;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;  // CAN pins don't use pull-up/pull-down
    GPIO_Init(tx_port, &GPIO_InitStruct);
    
    // Configure RX pin as alternate function
    GPIO_InitStruct.GPIO_Pin = rx_pin_num;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;  // CAN pins don't use pull-up/pull-down
    GPIO_Init(rx_port, &GPIO_InitStruct);
    
    // Connect GPIO pins to CAN peripheral
    GPIO_PinAFConfig(tx_port, (uint8_t)(tx_pin - (tx_pin / 16) * 16), gpio_af);
    GPIO_PinAFConfig(rx_port, (uint8_t)(rx_pin - (rx_pin / 16) * 16), gpio_af);
}

static void can_init(CanIndex can, uint32_t baudrate) {
    CAN_TypeDef* can_dev = get_can_peripheral(can);
    uint32_t can_clock = get_can_rcc_clock(can);
    
    if (can_dev == NULL) {
        return;  // Invalid CAN
    }
    
    // Enable CAN clock
    RCC_APB1PeriphClockCmd(can_clock, ENABLE);
    
    // Configure GPIO pins for CAN
    configure_can_gpio(can);
    
    // Initialize CAN
    CAN_InitTypeDef CAN_InitStruct;
    CAN_StructInit(&CAN_InitStruct);
    
    // Set CAN working mode
    CAN_InitStruct.CAN_TTCM = DISABLE;  // Time-triggered communication mode disabled
    CAN_InitStruct.CAN_ABOM = ENABLE;   // Automatic bus-off management
    CAN_InitStruct.CAN_AWUM = DISABLE;  // Automatic wake-up mode
    CAN_InitStruct.CAN_NART = ENABLE;   // No automatic retransmission
    CAN_InitStruct.CAN_RFLM = DISABLE;  // Receive FIFO locked mode
    CAN_InitStruct.CAN_TXFP = ENABLE;   // Transmit FIFO priority
    
    // Set baud rate
    // Baud rate calculation formula: 1 / (PRESCALER * (TS1 + TS2 + 1)) * APB1 frequency
    // For example, for APB1=42MHz, baud rate=250Kbps, TS1=12, TS2=1, then PRESCALER=12
    CAN_InitStruct.CAN_SJW = CAN_SJW_1tq;
    CAN_InitStruct.CAN_BS1 = CAN_BS1_12tq;
    CAN_InitStruct.CAN_BS2 = CAN_BS2_1tq;
    // Calculate prescaler value, assuming APB1 clock is 42MHz
    uint32_t apb1_freq = 42000000; // This value should be obtained from system clock
    uint32_t prescaler = apb1_freq / (baudrate * 13); // 13 = (12 + 1 + 1)
    CAN_InitStruct.CAN_Prescaler = prescaler;
    
    // Initialize CAN
    CAN_Init(can_dev, &CAN_InitStruct);
    
    // Configure filter
    CAN_FilterInitTypeDef CAN_FilterInitStruct;
    CAN_FilterInitStruct.CAN_FilterNumber = 0;  // Use filter 0
    CAN_FilterInitStruct.CAN_FilterMode = CAN_FilterMode_IdMask;  // Identifier mask mode
    CAN_FilterInitStruct.CAN_FilterScale = CAN_FilterScale_32bit;  // 32-bit filter
    CAN_FilterInitStruct.CAN_FilterIdHigh = 0x0000;  // Filter ID high
    CAN_FilterInitStruct.CAN_FilterIdLow = 0x0000;   // Filter ID low
    CAN_FilterInitStruct.CAN_FilterMaskIdHigh = 0x0000;  // Mask ID high
    CAN_FilterInitStruct.CAN_FilterMaskIdLow = 0x0000;   // Mask ID low
    CAN_FilterInitStruct.CAN_FilterFIFOAssignment = (can == _CAN1) ? CAN_FIFO0 : CAN_FIFO1;  // Assign to FIFO
    CAN_FilterInitStruct.CAN_FilterActivation = ENABLE;  // Activate filter
    
    CAN_FilterInit(&CAN_FilterInitStruct);
}

static void can_transmit(CanIndex can, uint32_t id, uint8_t *data, uint8_t len) {
    CAN_TypeDef* can_dev = get_can_peripheral(can);
    
    if (can_dev == NULL || data == NULL || len > 8) {
        return;  // Parameter error
    }
    
    CanTxMsg TxMessage;
    TxMessage.StdId = id;          // Standard ID
    TxMessage.ExtId = id;          // Extended ID
    TxMessage.RTR = CAN_RTR_DATA;  // Data frame
    TxMessage.IDE = CAN_ID_STD;    // Standard frame
    TxMessage.DLC = len;           // Data length
    
    // Copy data
    for(int i = 0; i < len; i++) {
        TxMessage.Data[i] = data[i];
    }
    
    // Send message
    uint8_t mailbox = CAN_Transmit(can_dev, &TxMessage);
    
    // Wait for transmission completion
    while(CAN_TransmitStatus(can_dev, mailbox) != CANTXOK);
}

static uint8_t can_receive(CanIndex can, uint32_t *id, uint8_t *data, uint8_t *len) {
    CAN_TypeDef* can_dev = get_can_peripheral(can);
    CanRxMsg RxMessage;
    
    if (can_dev == NULL) {
        return 0;  // Invalid CAN
    }
    
    // Check if there is a message in FIFO0
    if (can == _CAN1) {
        if(CAN_MessagePending(can_dev, CAN_FIFO0) == 0) {
            return 0;  // No message
        }
        
        // Receive message
        CAN_Receive(can_dev, CAN_FIFO0, &RxMessage);
    } else {
        if(CAN_MessagePending(can_dev, CAN_FIFO1) == 0) {
            return 0;  // No message
        }
        
        // Receive message
        CAN_Receive(can_dev, CAN_FIFO1, &RxMessage);
    }
    
    // Get ID
    if(RxMessage.IDE == CAN_ID_STD) {
        *id = RxMessage.StdId;
    } else {
        *id = RxMessage.ExtId;
    }
    
    // Get data length
    *len = RxMessage.DLC;
    
    // Copy data
    for(int i = 0; i < RxMessage.DLC; i++) {
        data[i] = RxMessage.Data[i];
    }
    
    return 1;  // Successfully received
}

const hal_can_ops_t hal_can = {
    .init = can_init,
    .transmit = can_transmit,
    .receive = can_receive
};