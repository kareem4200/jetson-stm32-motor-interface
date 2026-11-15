#ifndef DRIVERS_CLOCK_H_
#define DRIVERS_CLOCK_H_

#include <stdint.h>

typedef enum {
    CLOCK_BUS_APB1,
    CLOCK_BUS_APB2
} clock_bus_t;

void clock_init(void);
uint32_t clock_get_hclk_hz(void);
uint32_t clock_get_pclk_hz(clock_bus_t bus);
uint32_t clock_get_timerclk_hz(clock_bus_t bus);

#endif // DRIVERS_CLOCK_H_