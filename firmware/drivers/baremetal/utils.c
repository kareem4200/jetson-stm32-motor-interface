#include "utils.h"
#include "clock.h"
#include <stdint.h>

/*
source: https://www.youtube.com/watch?v=Fw-tKueeFHU&list=PLOAfeGId0YgbJykQvD8zGG1y5WmeubdYF&index=23
*/

void delay_init(void) {
    // get the clock frequency
    uint32_t timer_clock_freq = clock_get_timerclk_hz(CLOCK_BUS_APB1);
    uint32_t prescaler = (timer_clock_freq / 1000000U) - 1; // prescaler for 1 MHz timer clock (1 tick = 1 us)

    // enable the clock for TIM4
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

    // configure TIM4 / one tick = 1us
    TIM4->PSC = prescaler;
    TIM4->ARR = UINT16_MAX; // max value for 16-bit timer
    TIM4->CNT = 0;
    
    TIM4->EGR |= TIM_EGR_UG;
    TIM4->CR1 |= TIM_CR1_CEN; // enable the timer
}

void delay_us(uint32_t us) {
    uint32_t start = TIM4->CNT;
    // here since since we do 32 bit subtraction, when the timer overflows, the subtraction will wraparound 2^32
    // while the timer is only 16 bit. We need to cast the operation to uint16_t to ensure overflow around 2^16
    // (65530 + 6) % (2 ** 16) = 0 while (65530 + 6) % (2 ** 32) = 65536
    while ((uint16_t)(TIM4->CNT - start) < us) {
        // wait
    }
}

void delay_ms(uint32_t ms) {
    uint32_t start = TIM4->CNT;
    while ((uint16_t)(TIM4->CNT - start) < (ms * 1000)) {
        // wait
    }
}