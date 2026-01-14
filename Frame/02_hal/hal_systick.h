#pragma once

typedef struct {
    void (*init)(uint32_t timeUs, void (*callback)(void));
} hal_systick_ops_t;

extern const hal_systick_ops_t hal_systick;