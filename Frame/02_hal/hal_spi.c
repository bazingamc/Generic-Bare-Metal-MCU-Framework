#include "hal.h"



// 根据 SpiIndex 枚举获取对应的 SPI 外设
SPI_TypeDef* get_spi_peripheral(SpiIndex spi) {
    switch(spi) {
        case _SPI1: return SPI1;
        case _SPI2: return SPI2;
        case _SPI3: return SPI3;
        case _SPI4: return SPI4;
        case _SPI5: return SPI5;
        case _SPI6: return SPI6;
        default: return NULL;
    }
}

// 获取对应的 RCC 时钟使能寄存器位
uint32_t get_spi_rcc_clock(SpiIndex spi) {
    switch(spi) {
        case _SPI1: return RCC_APB2Periph_SPI1;
        case _SPI2: return RCC_APB1Periph_SPI2;
        case _SPI3: return RCC_APB1Periph_SPI3;
        case _SPI4: return RCC_APB2Periph_SPI4;
        case _SPI5: return RCC_APB2Periph_SPI5;
        case _SPI6: return RCC_APB2Periph_SPI6;
        default: return 0;
    }
}

// 获取对应的 GPIO 引脚配置（根据 SPI 编号确定 SCK/MISO/MOSI 引脚）
void get_spi_gpio_config(SpiIndex spi, GpioIndex* sck_pin, GpioIndex* miso_pin, GpioIndex* mosi_pin, uint32_t* gpio_af) {
    switch(spi) {
        case _SPI1:
            *sck_pin = PA5;    // PA5 = SCK
            *miso_pin = PA6;   // PA6 = MISO
            *mosi_pin = PA7;   // PA7 = MOSI
            *gpio_af = GPIO_AF_SPI1;
            break;
        case _SPI2:
            *sck_pin = PB13;   // PB13 = SCK
            *miso_pin = PB14;  // PB14 = MISO
            *mosi_pin = PB15;  // PB15 = MOSI
            *gpio_af = GPIO_AF_SPI2;
            break;
        case _SPI3:
            *sck_pin = PC10;   // PC10 = SCK
            *miso_pin = PC11;  // PC11 = MISO
            *mosi_pin = PC12;  // PC12 = MOSI
            *gpio_af = GPIO_AF_SPI3;
            break;
        case _SPI4:
            *sck_pin = PE12;   // PE12 = SCK
            *miso_pin = PE13;  // PE13 = MISO
            *mosi_pin = PE14;  // PE14 = MOSI
            *gpio_af = GPIO_AF_SPI4;
            break;
        case _SPI5:
            *sck_pin = PF7;    // PF7 = SCK
            *miso_pin = PF8;   // PF8 = MISO
            *mosi_pin = PF9;   // PF9 = MOSI
            *gpio_af = GPIO_AF_SPI5;
            break;
        case _SPI6:
            *sck_pin = PG13;   // PG13 = SCK
            *miso_pin = PG12;  // PG12 = MISO
            *mosi_pin = PG14;  // PG14 = MOSI
            *gpio_af = GPIO_AF_SPI6;
            break;
        default:
            *sck_pin = PA0;    // 无效值
            *miso_pin = PA0;
            *mosi_pin = PA0;
            *gpio_af = 0;
            break;
    }
}

// 获取对应 GPIO 的 RCC 时钟
uint32_t get_spi_gpio_rcc_clock(GpioIndex pin) {
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

// 配置 SPI 的 GPIO 引脚
void configure_spi_gpio(SpiIndex spi) {
    GpioIndex sck_pin, miso_pin, mosi_pin;
    uint32_t gpio_af;
    
    get_spi_gpio_config(spi, &sck_pin, &miso_pin, &mosi_pin, &gpio_af);
    
    // 获取 GPIO 端口和引脚号
    GPIO_TypeDef* sck_port = get_gpio_port(sck_pin);
    uint16_t sck_pin_num = get_gpio_pin(sck_pin);
    GPIO_TypeDef* miso_port = get_gpio_port(miso_pin);
    uint16_t miso_pin_num = get_gpio_pin(miso_pin);
    GPIO_TypeDef* mosi_port = get_gpio_port(mosi_pin);
    uint16_t mosi_pin_num = get_gpio_pin(mosi_pin);
    
    // 使能 GPIO 端口时钟
    RCC_AHB1PeriphClockCmd(get_spi_gpio_rcc_clock(sck_pin), ENABLE);
    RCC_AHB1PeriphClockCmd(get_spi_gpio_rcc_clock(miso_pin), ENABLE);
    RCC_AHB1PeriphClockCmd(get_spi_gpio_rcc_clock(mosi_pin), ENABLE);
    
    // 配置 SCK, MISO, MOSI 引脚为复用功能
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin = sck_pin_num;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;  // SPI引脚下拉
    GPIO_Init(sck_port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = miso_pin_num;
    GPIO_Init(miso_port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = mosi_pin_num;
    GPIO_Init(mosi_port, &GPIO_InitStruct);
    
    // 连接 GPIO 引脚到 SPI 外设
    GPIO_PinAFConfig(sck_port, (uint8_t)(sck_pin - (sck_pin / 16) * 16), gpio_af);
    GPIO_PinAFConfig(miso_port, (uint8_t)(miso_pin - (miso_pin / 16) * 16), gpio_af);
    GPIO_PinAFConfig(mosi_port, (uint8_t)(mosi_pin - (mosi_pin / 16) * 16), gpio_af);
}

static void spi_init(SpiIndex spi, SpiMode mode, SpiDirection direction, SpiDataSize dataSize) {
    SPI_TypeDef* spi_dev = get_spi_peripheral(spi);
    uint32_t spi_clock = get_spi_rcc_clock(spi);
    
    if (spi_dev == NULL) {
        return;  // 无效的 SPI
    }
    
    // 使能 SPI 时钟
    if (spi == _SPI1 || spi == _SPI4 || spi == _SPI5 || spi == _SPI6) {
        RCC_APB2PeriphClockCmd(spi_clock, ENABLE);
    } else {
        RCC_APB1PeriphClockCmd(spi_clock, ENABLE);
    }
    
    // 配置 SPI 的 GPIO 引脚
    configure_spi_gpio(spi);
    
    // 初始化 SPI
    SPI_InitTypeDef SPI_InitStruct;
    SPI_StructInit(&SPI_InitStruct);
    
    // 设置 SPI 模式 (Master/Slave)
    SPI_InitStruct.SPI_Mode = (mode == SPI_MODE_MASTER) ? SPI_Mode_Master : SPI_Mode_Slave;
    
    // 设置 SPI 方向
    switch(direction) {
        case SPI_DIRECTION_2LINES_FULLDUPLEX:
            SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
            break;
        case SPI_DIRECTION_2LINES_RXONLY:
            SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_RxOnly;
            break;
        case SPI_DIRECTION_1LINE_RX:
            SPI_InitStruct.SPI_Direction = SPI_Direction_1Line_Rx;
            break;
        case SPI_DIRECTION_1LINE_TX:
            SPI_InitStruct.SPI_Direction = SPI_Direction_1Line_Tx;
            break;
    }
    
    // 设置数据大小
    SPI_InitStruct.SPI_DataSize = (dataSize == SPI_DATASIZE_8BIT) ? SPI_DataSize_8b : SPI_DataSize_16b;
    
    // 设置时钟极性和相位 (CPOL=0, CPHA=0 - Mode 0)
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
    
    // 设置软件NSS管理
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    
    // 设置波特率预分频 (假设系统时钟为84MHz，分频后约为21MHz)
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    
    // 设置第一个时钟边沿是数据捕获还是时钟输出
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    
    // 设置 CRC 计算关闭
    SPI_InitStruct.SPI_CRCPolynomial = 7;
    
    // 初始化 SPI
    SPI_Init(spi_dev, &SPI_InitStruct);
    
    // 使能 SPI
    SPI_Cmd(spi_dev, ENABLE);
}

static void spi_send_byte(SpiIndex spi, uint8_t data) {
    SPI_TypeDef* spi_dev = get_spi_peripheral(spi);
    
    if (spi_dev != NULL) {
        // 等待发送缓冲区为空
        while(SPI_I2S_GetFlagStatus(spi_dev, SPI_I2S_FLAG_TXE) == RESET);
        
        // 发送数据
        SPI_I2S_SendData(spi_dev, data);
        
        // 等待发送完成
        while(SPI_I2S_GetFlagStatus(spi_dev, SPI_I2S_FLAG_TXE) == RESET);
    }
}

static uint8_t spi_receive_byte(SpiIndex spi) {
    SPI_TypeDef* spi_dev = get_spi_peripheral(spi);
    
    if (spi_dev != NULL) {
        // 等待接收缓冲区非空
        while(SPI_I2S_GetFlagStatus(spi_dev, SPI_I2S_FLAG_RXNE) == RESET);
        
        // 读取数据
        return (uint8_t)SPI_I2S_ReceiveData(spi_dev);
    }
    
    return 0;  // 无效的 SPI 返回 0
}

static void spi_send_data(SpiIndex spi, uint8_t* data, uint16_t size) {
    for(uint16_t i = 0; i < size; i++) {
        spi_send_byte(spi, data[i]);
    }
}

static void spi_receive_data(SpiIndex spi, uint8_t* data, uint16_t size) {
    for(uint16_t i = 0; i < size; i++) {
        data[i] = spi_receive_byte(spi);
    }
}

const hal_spi_ops_t hal_spi = {
    .init = spi_init,
    .send_byte = spi_send_byte,
    .receive_byte = spi_receive_byte,
    .send_data = spi_send_data,
    .receive_data = spi_receive_data
};