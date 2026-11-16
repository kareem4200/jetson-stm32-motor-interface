#include "clock.h"

static const uint32_t AHB_PRESCALER_TABLE[16] = {
    1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U,
    2U, 4U, 8U, 16U, 64U, 128U, 256U, 512U
};

static const uint32_t APB_PRESCALER_TABLE[8] = {
    1U, 1U, 1U, 1U, 2U, 4U, 8U, 16U
};

clock_status_t clock_init(void) {
    uint32_t timeout = CLOCK_STARTUP_POLL_LIMIT;
    // enable HSI clock (16 MHz) and wait for it to be ready
    RCC->CR |= RCC_CR_HSION;
    while ((RCC->CR & RCC_CR_HSIRDY) == 0U) {
        if (timeout == 0U) {
            return CLOCK_ERROR_TIMEOUT;
        }
        timeout--;
    }

    // select HSI as system clock (SW = 00) and wait for it to be used
    timeout = CLOCK_STARTUP_POLL_LIMIT;
    RCC->CFGR &= ~RCC_CFGR_SW;
    // wont be applicable later when I allow all clock sources
    while ((RCC->CFGR & RCC_CFGR_SWS) != 0U) {
        if (timeout == 0U) {
            return CLOCK_ERROR_TIMEOUT;
        }
        timeout--;
    }

    // configure AHB, APB1, and APB2 prescalers (no division)
    RCC->CFGR &= ~RCC_CFGR_HPRE;
    RCC->CFGR &= ~RCC_CFGR_PPRE1;
    // RCC->CFGR |= (0x4U << RCC_CFGR_PPRE1_Pos);
    RCC->CFGR &= ~RCC_CFGR_PPRE2;

    return CLOCK_OK;
}

uint32_t clock_get_hclk_hz(void) {
    // TODO: check the current clock source and return the correct frequency based on that
    uint32_t sysclk_hz = 16000000U; // HSI frequency (hardcoded for now)
    uint32_t hpre = (RCC->CFGR & RCC_CFGR_HPRE) >> RCC_CFGR_HPRE_Pos;

    return sysclk_hz / AHB_PRESCALER_TABLE[hpre];
}

uint32_t clock_get_pclk_hz(clock_bus_t bus) {
    uint32_t hclk_hz = clock_get_hclk_hz();
    uint32_t pprex = 0U;
    if (bus == CLOCK_BUS_APB1) {
        pprex = (RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos;
    } else if (bus == CLOCK_BUS_APB2) {
        pprex = (RCC->CFGR & RCC_CFGR_PPRE2) >> RCC_CFGR_PPRE2_Pos;
    } else {
        return 0U; // Invalid bus
    }

    return hclk_hz / APB_PRESCALER_TABLE[pprex];
}

uint32_t clock_get_timerclk_hz(clock_bus_t bus) {
    uint32_t pclk_hz = clock_get_pclk_hz(bus);
    uint32_t pprex = 0U;
    if (bus == CLOCK_BUS_APB1) {
        pprex = (RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos;
    } else if (bus == CLOCK_BUS_APB2) {
        pprex = (RCC->CFGR & RCC_CFGR_PPRE2) >> RCC_CFGR_PPRE2_Pos;
    } else {
        return 0U; // Invalid bus
    }

    if (APB_PRESCALER_TABLE[pprex] > 1U) {
        return pclk_hz * 2U;
    } else {
        return pclk_hz;
    }
}