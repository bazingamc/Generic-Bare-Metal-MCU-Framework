#pragma once

typedef struct {
    void (*init)();
    void (*delay)(uint32_t timeUs);
    uint32_t (*getCYCCNT)(void);
} hal_dwt_ops_t;

extern const hal_dwt_ops_t hal_dwt;