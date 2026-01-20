#pragma once


typedef enum  // UART index
{
    _UART1,
    _UART2,
    _UART3,
    _UART4,
    _UART5,
    _UART6,
    _UART7,
    _UART8
} UartIndex;

typedef struct {
    void (*init)(UartIndex uart, uint32_t baudrate, GpioIndex txPin, GpioIndex rxPin);
    void (*send_byte)(UartIndex uart, uint8_t data);
    void (*send_bytes)(UartIndex uart, u16 len, uint8_t* data);
    void (*send_string)(UartIndex uart, const char* str);
    uint8_t (*receive_byte)(UartIndex uart);
    int (*is_data_available)(UartIndex uart);
    void (*send_bytes_dma)(UartIndex uart, uint8_t* data, uint16_t size);
} hal_uart_ops_t;

extern const hal_uart_ops_t hal_uart;