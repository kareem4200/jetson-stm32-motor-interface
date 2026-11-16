#ifndef DRIVERS_CLOCK_H_
#define DRIVERS_CLOCK_H_

#include "stm32f401xc.h"
#include <stdint.h>

#define CLOCK_STARTUP_POLL_LIMIT 1000000U

typedef enum {
    CLOCK_BUS_APB1,
    CLOCK_BUS_APB2
} clock_bus_t;

// typedef enum {
//     CLOCK_SOURCE_HSI,
//     CLOCK_SOURCE_HSE,
//     CLOCK_SOURCE_PLL
// } clock_source_t;

typedef enum {
    CLOCK_OK,
    CLOCK_ERROR_TIMEOUT,
} clock_status_t;

clock_status_t clock_init(void);
uint32_t clock_get_hclk_hz(void);
uint32_t clock_get_pclk_hz(clock_bus_t bus);
uint32_t clock_get_timerclk_hz(clock_bus_t bus);

#endif // DRIVERS_CLOCK_H_