#pragma once


typedef enum  // UART编号
{
    _UART1,
    _UART2,
    _UART3,
    _UART4,
    _UART5,
    _UART6
} UartIndex;

typedef struct {
    void (*init)(UartIndex uart, uint32_t baudrate, GpioIndex txPin, GpioIndex rxPin);
    void (*send_byte)(UartIndex uart, uint8_t data);
    void (*send_string)(UartIndex uart, const char* str);
    uint8_t (*receive_byte)(UartIndex uart);
    int (*is_data_available)(UartIndex uart);
} hal_uart_ops_t;

extern const hal_uart_ops_t hal_uart;