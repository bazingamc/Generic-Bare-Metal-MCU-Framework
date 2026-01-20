#include "hal.h"



// Get the corresponding SPI peripheral according to SpiIndex enumeration
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

// Get the corresponding RCC clock enable register bit
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

// Get the corresponding GPIO pin configuration (determine SCK/MISO/MOSI pins according to SPI number)
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
            *sck_pin = PA0;    // Invalid value
            *miso_pin = PA0;
            *mosi_pin = PA0;
            *gpio_af = 0;
            break;
    }
}

// Get the RCC clock for the corresponding GPIO
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

// Configure GPIO pins for SPI
void configure_spi_gpio(SpiIndex spi) {
    GpioIndex sck_pin, miso_pin, mosi_pin;
    uint32_t gpio_af;
    
    get_spi_gpio_config(spi, &sck_pin, &miso_pin, &mosi_pin, &gpio_af);
    
    // Get GPIO port and pin numbers
    GPIO_TypeDef* sck_port = get_gpio_port(sck_pin);
    uint16_t sck_pin_num = get_gpio_pin(sck_pin);
    GPIO_TypeDef* miso_port = get_gpio_port(miso_pin);
    uint16_t miso_pin_num = get_gpio_pin(miso_pin);
    GPIO_TypeDef* mosi_port = get_gpio_port(mosi_pin);
    uint16_t mosi_pin_num = get_gpio_pin(mosi_pin);
    
    // Enable GPIO port clocks
    RCC_AHB1PeriphClockCmd(get_spi_gpio_rcc_clock(sck_pin), ENABLE);
    RCC_AHB1PeriphClockCmd(get_spi_gpio_rcc_clock(miso_pin), ENABLE);
    RCC_AHB1PeriphClockCmd(get_spi_gpio_rcc_clock(mosi_pin), ENABLE);
    
    // Configure SCK, MISO, MOSI pins as alternate function
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin = sck_pin_num;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;  // SPI pins pull-down
    GPIO_Init(sck_port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = miso_pin_num;
    GPIO_Init(miso_port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = mosi_pin_num;
    GPIO_Init(mosi_port, &GPIO_InitStruct);
    
    // Connect GPIO pins to SPI peripheral
    GPIO_PinAFConfig(sck_port, (uint8_t)(sck_pin - (sck_pin / 16) * 16), gpio_af);
    GPIO_PinAFConfig(miso_port, (uint8_t)(miso_pin - (miso_pin / 16) * 16), gpio_af);
    GPIO_PinAFConfig(mosi_port, (uint8_t)(mosi_pin - (mosi_pin / 16) * 16), gpio_af);
}

static void spi_init(SpiIndex spi, SpiMode mode, SpiDirection direction, SpiDataSize dataSize) {
    SPI_TypeDef* spi_dev = get_spi_peripheral(spi);
    uint32_t spi_clock = get_spi_rcc_clock(spi);
    
    if (spi_dev == NULL) {
        return;  // Invalid SPI
    }
    
    // Enable SPI clock
    if (spi == _SPI1 || spi == _SPI4 || spi == _SPI5 || spi == _SPI6) {
        RCC_APB2PeriphClockCmd(spi_clock, ENABLE);
    } else {
        RCC_APB1PeriphClockCmd(spi_clock, ENABLE);
    }
    
    // Configure GPIO pins for SPI
    configure_spi_gpio(spi);
    
    // Initialize SPI
    SPI_InitTypeDef SPI_InitStruct;
    SPI_StructInit(&SPI_InitStruct);
    
    // Set SPI mode (Master/Slave)
    SPI_InitStruct.SPI_Mode = (mode == SPI_MODE_MASTER) ? SPI_Mode_Master : SPI_Mode_Slave;
    
    // Set SPI direction
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
    
    // Set data size
    SPI_InitStruct.SPI_DataSize = (dataSize == SPI_DATASIZE_8BIT) ? SPI_DataSize_8b : SPI_DataSize_16b;
    
    // Set clock polarity and phase (CPOL=0, CPHA=0 - Mode 0)
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
    
    // Set software NSS management
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    
    // Set baud rate prescaler (assuming system clock is 84MHz, results in ~21MHz after division)
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    
    // Set whether the first clock edge captures or outputs data
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    
    // Set CRC calculation off
    SPI_InitStruct.SPI_CRCPolynomial = 7;
    
    // Initialize SPI
    SPI_Init(spi_dev, &SPI_InitStruct);
    
    // Enable SPI
    SPI_Cmd(spi_dev, ENABLE);
}

static void spi_send_byte(SpiIndex spi, uint8_t data) {
    SPI_TypeDef* spi_dev = get_spi_peripheral(spi);
    
    if (spi_dev != NULL) {
        // Wait until transmit buffer is empty
        while(SPI_I2S_GetFlagStatus(spi_dev, SPI_I2S_FLAG_TXE) == RESET);
        
        // Send data
        SPI_I2S_SendData(spi_dev, data);
        
        // Wait for transmission to complete
        while(SPI_I2S_GetFlagStatus(spi_dev, SPI_I2S_FLAG_TXE) == RESET);
    }
}

static uint8_t spi_receive_byte(SpiIndex spi) {
    SPI_TypeDef* spi_dev = get_spi_peripheral(spi);
    
    if (spi_dev != NULL) {
        // Wait until receive buffer is not empty
        while(SPI_I2S_GetFlagStatus(spi_dev, SPI_I2S_FLAG_RXNE) == RESET);
        
        // Read data
        return (uint8_t)SPI_I2S_ReceiveData(spi_dev);
    }
    
    return 0;  // Invalid SPI returns 0
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