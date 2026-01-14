#include "hal.h"



// 根据 CanIndex 枚举获取对应的 CAN 外设
CAN_TypeDef* get_can_peripheral(CanIndex can) {
    switch(can) {
        case _CAN1: return CAN1;
        case _CAN2: return CAN2;
        default: return NULL;
    }
}

// 获取对应的 RCC 时钟使能寄存器位
uint32_t get_can_rcc_clock(CanIndex can) {
    switch(can) {
        case _CAN1: return RCC_APB1Periph_CAN1;
        case _CAN2: return RCC_APB1Periph_CAN2;
        default: return 0;
    }
}

// 获取对应的 GPIO 引脚配置（根据 CAN 编号确定 TX/RX 引脚）
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
            *tx_pin = PA0;   // 无效值
            *rx_pin = PA0;
            *gpio_af = 0;
            break;
    }
}

// 获取对应 GPIO 的 RCC 时钟
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

// 配置 CAN 的 GPIO 引脚
void configure_can_gpio(CanIndex can) {
    GpioIndex tx_pin, rx_pin;
    uint32_t gpio_af;
    
    get_can_gpio_config(can, &tx_pin, &rx_pin, &gpio_af);
    
    // 获取 GPIO 端口和引脚号
    GPIO_TypeDef* tx_port = get_gpio_port(tx_pin);
    uint16_t tx_pin_num = get_gpio_pin(tx_pin);
    GPIO_TypeDef* rx_port = get_gpio_port(rx_pin);
    uint16_t rx_pin_num = get_gpio_pin(rx_pin);
    
    // 使能 GPIO 端口时钟
    RCC_AHB1PeriphClockCmd(get_can_gpio_rcc_clock(tx_pin), ENABLE);
    if (tx_port != rx_port) {  // 如果TX和RX在不同端口，也要使能RX端口时钟
        RCC_AHB1PeriphClockCmd(get_can_gpio_rcc_clock(rx_pin), ENABLE);
    }
    
    // 配置 TX 引脚为复用功能
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin = tx_pin_num;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;  // CAN引脚不使用上下拉
    GPIO_Init(tx_port, &GPIO_InitStruct);
    
    // 配置 RX 引脚为复用功能
    GPIO_InitStruct.GPIO_Pin = rx_pin_num;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;  // CAN引脚不使用上下拉
    GPIO_Init(rx_port, &GPIO_InitStruct);
    
    // 连接 GPIO 引脚到 CAN 外设
    GPIO_PinAFConfig(tx_port, (uint8_t)(tx_pin - (tx_pin / 16) * 16), gpio_af);
    GPIO_PinAFConfig(rx_port, (uint8_t)(rx_pin - (rx_pin / 16) * 16), gpio_af);
}

static void can_init(CanIndex can, uint32_t baudrate) {
    CAN_TypeDef* can_dev = get_can_peripheral(can);
    uint32_t can_clock = get_can_rcc_clock(can);
    
    if (can_dev == NULL) {
        return;  // 无效的 CAN
    }
    
    // 使能 CAN 时钟
    RCC_APB1PeriphClockCmd(can_clock, ENABLE);
    
    // 配置 CAN 的 GPIO 引脚
    configure_can_gpio(can);
    
    // 初始化 CAN
    CAN_InitTypeDef CAN_InitStruct;
    CAN_StructInit(&CAN_InitStruct);
    
    // 设置 CAN 工作模式
    CAN_InitStruct.CAN_TTCM = DISABLE;  // 时间触发通信模式禁用
    CAN_InitStruct.CAN_ABOM = ENABLE;   // 自动离线管理
    CAN_InitStruct.CAN_AWUM = DISABLE;  // 自动唤醒模式
    CAN_InitStruct.CAN_NART = ENABLE;   // 禁止重传
    CAN_InitStruct.CAN_RFLM = DISABLE;  // 接收FIFO锁定模式
    CAN_InitStruct.CAN_TXFP = ENABLE;   // 发送FIFO优先级
    
    // 设置波特率
    // 波特率计算公式: 1 / (PRESCALER * (TS1 + TS2 + 1)) * APB1频率
    // 例如对于APB1=42MHz, 波特率=250Kbps, TS1=12, TS2=1, 则PRESCALER=12
    CAN_InitStruct.CAN_SJW = CAN_SJW_1tq;
    CAN_InitStruct.CAN_BS1 = CAN_BS1_12tq;
    CAN_InitStruct.CAN_BS2 = CAN_BS2_1tq;
    // 计算预分频值，假设APB1时钟为42MHz
    uint32_t apb1_freq = 42000000; // 这个值应该从系统时钟获取
    uint32_t prescaler = apb1_freq / (baudrate * 13); // 13 = (12 + 1 + 1)
    CAN_InitStruct.CAN_Prescaler = prescaler;
    
    // 初始化 CAN
    CAN_Init(can_dev, &CAN_InitStruct);
    
    // 配置过滤器
    CAN_FilterInitTypeDef CAN_FilterInitStruct;
    CAN_FilterInitStruct.CAN_FilterNumber = 0;  // 使用过滤器0
    CAN_FilterInitStruct.CAN_FilterMode = CAN_FilterMode_IdMask;  // 标识符掩码模式
    CAN_FilterInitStruct.CAN_FilterScale = CAN_FilterScale_32bit;  // 32位过滤器
    CAN_FilterInitStruct.CAN_FilterIdHigh = 0x0000;  // 过滤器ID高位
    CAN_FilterInitStruct.CAN_FilterIdLow = 0x0000;   // 过滤器ID低位
    CAN_FilterInitStruct.CAN_FilterMaskIdHigh = 0x0000;  // 掩码ID高位
    CAN_FilterInitStruct.CAN_FilterMaskIdLow = 0x0000;   // 掩码ID低位
    CAN_FilterInitStruct.CAN_FilterFIFOAssignment = (can == _CAN1) ? CAN_FIFO0 : CAN_FIFO1;  // 分配到FIFO
    CAN_FilterInitStruct.CAN_FilterActivation = ENABLE;  // 激活过滤器
    
    CAN_FilterInit(&CAN_FilterInitStruct);
}

static void can_transmit(CanIndex can, uint32_t id, uint8_t *data, uint8_t len) {
    CAN_TypeDef* can_dev = get_can_peripheral(can);
    
    if (can_dev == NULL || data == NULL || len > 8) {
        return;  // 参数错误
    }
    
    CanTxMsg TxMessage;
    TxMessage.StdId = id;          // 标准ID
    TxMessage.ExtId = id;          // 扩展ID
    TxMessage.RTR = CAN_RTR_DATA;  // 数据帧
    TxMessage.IDE = CAN_ID_STD;    // 标准帧
    TxMessage.DLC = len;           // 数据长度
    
    // 复制数据
    for(int i = 0; i < len; i++) {
        TxMessage.Data[i] = data[i];
    }
    
    // 发送消息
    uint8_t mailbox = CAN_Transmit(can_dev, &TxMessage);
    
    // 等待传输完成
    while(CAN_TransmitStatus(can_dev, mailbox) != CANTXOK);
}

static uint8_t can_receive(CanIndex can, uint32_t *id, uint8_t *data, uint8_t *len) {
    CAN_TypeDef* can_dev = get_can_peripheral(can);
    CanRxMsg RxMessage;
    
    if (can_dev == NULL) {
        return 0;  // 无效的 CAN
    }
    
    // 检查是否有消息在 FIFO0
    if (can == _CAN1) {
        if(CAN_MessagePending(can_dev, CAN_FIFO0) == 0) {
            return 0;  // 没有消息
        }
        
        // 接收消息
        CAN_Receive(can_dev, CAN_FIFO0, &RxMessage);
    } else {
        if(CAN_MessagePending(can_dev, CAN_FIFO1) == 0) {
            return 0;  // 没有消息
        }
        
        // 接收消息
        CAN_Receive(can_dev, CAN_FIFO1, &RxMessage);
    }
    
    // 获取ID
    if(RxMessage.IDE == CAN_ID_STD) {
        *id = RxMessage.StdId;
    } else {
        *id = RxMessage.ExtId;
    }
    
    // 获取数据长度
    *len = RxMessage.DLC;
    
    // 复制数据
    for(int i = 0; i < RxMessage.DLC; i++) {
        data[i] = RxMessage.Data[i];
    }
    
    return 1;  // 成功接收
}

const hal_can_ops_t hal_can = {
    .init = can_init,
    .transmit = can_transmit,
    .receive = can_receive
};