#pragma once


typedef enum  // CAN编号
{
    _CAN1,
    _CAN2
} CanIndex;

typedef struct {
    void (*init)(CanIndex can, uint32_t baudrate);
    void (*transmit)(CanIndex can, uint32_t id, uint8_t *data, uint8_t len);
    uint8_t (*receive)(CanIndex can, uint32_t *id, uint8_t *data, uint8_t *len);
} hal_can_ops_t;

extern const hal_can_ops_t hal_can;