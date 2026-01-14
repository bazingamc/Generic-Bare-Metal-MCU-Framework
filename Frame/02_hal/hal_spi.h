#pragma once


typedef enum  // SPI编号
{
    _SPI1,
    _SPI2,
    _SPI3,
    _SPI4,
    _SPI5,
    _SPI6
} SpiIndex;

typedef enum {
    SPI_MODE_MASTER,
    SPI_MODE_SLAVE
} SpiMode;

typedef enum {
    SPI_DIRECTION_2LINES_FULLDUPLEX,
    SPI_DIRECTION_2LINES_RXONLY,
    SPI_DIRECTION_1LINE_RX,
    SPI_DIRECTION_1LINE_TX
} SpiDirection;

typedef enum {
    SPI_DATASIZE_8BIT,
    SPI_DATASIZE_16BIT
} SpiDataSize;

typedef struct {
    void (*init)(SpiIndex spi, SpiMode mode, SpiDirection direction, SpiDataSize dataSize);
    void (*send_byte)(SpiIndex spi, uint8_t data);
    uint8_t (*receive_byte)(SpiIndex spi);
    void (*send_data)(SpiIndex spi, uint8_t* data, uint16_t size);
    void (*receive_data)(SpiIndex spi, uint8_t* data, uint16_t size);
} hal_spi_ops_t;

extern const hal_spi_ops_t hal_spi;