#include "pwm.h"

// my application only needs TIM1, TIM2, TIM3. Timers 9-11 have less than 4 channels and
// the switch statement below does not handle the case if a user inputs a wrong channel
static const struct timer_clk_bus_mapping timer_clk_bus_map[8] = {
    {TIM1, 4, CLOCK_BUS_APB2, RCC_APB2ENR_TIM1EN},
    {TIM2, 4, CLOCK_BUS_APB1, RCC_APB1ENR_TIM2EN},
    {TIM3, 4, CLOCK_BUS_APB1, RCC_APB1ENR_TIM3EN},
    {TIM4, 4, CLOCK_BUS_APB1, RCC_APB1ENR_TIM4EN},
    {TIM5, 4, CLOCK_BUS_APB1, RCC_APB1ENR_TIM5EN},
    {TIM9, 2, CLOCK_BUS_APB2, RCC_APB2ENR_TIM9EN},
    {TIM10, 1, CLOCK_BUS_APB2, RCC_APB2ENR_TIM10EN},
    {TIM11, 1, CLOCK_BUS_APB2, RCC_APB2ENR_TIM11EN}
};

pwm_status_t pwm_timer_init(TIM_TypeDef *timer, uint32_t pwm_freq_hz) {
    if (timer == NULL || pwm_freq_hz == 0U) {
        return PWM_STATUS_ERROR;
    }

    // find the clock bus for the given timer
    clock_bus_t bus = CLOCK_BUS_APB2;
    bool timer_found = false;
    uint8_t timer_index = 0;
    for (size_t i = 0; i < sizeof(timer_clk_bus_map) / sizeof(timer_clk_bus_map[0]); i++) {
        if (timer_clk_bus_map[i].timer == timer) {
            bus = timer_clk_bus_map[i].bus;
            timer_found = true;
            timer_index = i;
            break;
        }
    }

    // invalid timer was passed
    if (!timer_found) {
        return PWM_STATUS_ERROR;
    }

    // calculate the prescaler value based on the desired frequency and the timer input clock frequency
    // I only use only pwm frequencies that divide evenly into the timer clock frequency. 
    // This is not necessay and I could use PSC=0 and ARR = timer_clk_hz // pwm_freq_hz with a small error
    uint32_t timer_clk_hz = clock_get_timerclk_hz(bus);
    if (timer_clk_hz == 0U) {
        return PWM_STATUS_ERROR;
    }

    if ((timer_clk_hz % pwm_freq_hz) != 0U) {
        return PWM_STATUS_ERROR; // cannot achieve the desired frequency with integer prescaler and arr
    }

    // determine the multiplicative factor to get the desired frequency division and 
    // accept the first valid prescaler and arr pair that fits within the 16-bit limit
    uint16_t prescaler = 0U;
    uint32_t arr = 0U;
    uint32_t freq_div = timer_clk_hz / pwm_freq_hz;
    bool found_valid_pair = false;
    for (uint32_t i = 1U; i <= 65536U; i++) {
        if ((freq_div % i) == 0U) {
            prescaler = i - 1U;
            arr = (freq_div / i) - 1U;
            if (arr <= 65535U) {
                found_valid_pair = true;
                break; // found a valid prescaler and arr pair
            }
        }
    }

    if (found_valid_pair == false || arr == 0U || arr > 65535U) {
        return PWM_STATUS_ERROR; // no valid pair found or arr is out of range
    }

    // enable the timer clock in the appropriate RCC register
    if (bus == CLOCK_BUS_APB2) {
        RCC->APB2ENR |= timer_clk_bus_map[timer_index].rcc_apbenr_mask;
        (void)RCC->APB2ENR;
    } else if (bus == CLOCK_BUS_APB1) {
        RCC->APB1ENR |= timer_clk_bus_map[timer_index].rcc_apbenr_mask;
        (void)RCC->APB1ENR;
    }

    // reset cntr register, by default we are counting up, 
    // buffer the arr value in preload register. But since it is buffered we need to trigger
    // an update event to load the value into the active register
    timer->CR1 = 0U;
    timer->CR1 |= TIM_CR1_ARPE;
    timer->PSC = prescaler;
	timer->ARR = arr;
    timer->CNT = 0U;
    timer->EGR |= TIM_EGR_UG;
    timer->CR1 |= (1U << 0);

    if (timer == TIM1) {
        // enable the main output bit for advanced TIM1
        timer->BDTR |= TIM_BDTR_MOE;
    }

    return PWM_STATUS_OK;
}

pwm_status_t pwm_channel_init(TIM_TypeDef *timer, uint8_t channel) {
    if (timer == NULL || channel == 0U) {
        return PWM_STATUS_ERROR;
    }

    for (size_t i = 0; i < sizeof(timer_clk_bus_map) / sizeof(timer_clk_bus_map[0]); i++) {
        if (timer_clk_bus_map[i].timer == timer) {
            if (channel > timer_clk_bus_map[i].num_channels) {
                return PWM_STATUS_ERROR; // channel number exceeds the number of channels for this timer
            }
            break;
        }
    }

    // configure the timer channel as output and pwm mode 1
    // polatity is active high by default on CCxP
    switch (channel) {
        case 1U:
            timer->CCER &= ~TIM_CCER_CC1E; // disable output for channel
            timer->CCER &= ~TIM_CCER_CC1P; // set polarity to active high
            timer->CCR1 = 0U; // initialize duty cycle to 0%
            timer->CCMR1 &= ~TIM_CCMR1_CC1S; // set the channel to output mode
            timer->CCMR1 &= ~TIM_CCMR1_OC1M; // clear the OC1M bits
            timer->CCMR1 |= (6U << TIM_CCMR1_OC1M_Pos); // set to PWM mode 1
            timer->CCMR1 |= TIM_CCMR1_OC1PE; // enable preload for CCR1
            timer->CCER |= TIM_CCER_CC1E; // enable output for channel
            break;
        case 2U:
            timer->CCER &= ~TIM_CCER_CC2E;
            timer->CCER &= ~TIM_CCER_CC2P;
            timer->CCR2 = 0U;
            timer->CCMR1 &= ~TIM_CCMR1_CC2S;
            timer->CCMR1 &= ~TIM_CCMR1_OC2M;
            timer->CCMR1 |= (6U << TIM_CCMR1_OC2M_Pos);
            timer->CCMR1 |= TIM_CCMR1_OC2PE;
            timer->CCER |= TIM_CCER_CC2E;
            break;
        case 3U:
            timer->CCER &= ~TIM_CCER_CC3E;
            timer->CCER &= ~TIM_CCER_CC3P;
            timer->CCR3 = 0U;
            timer->CCMR2 &= ~TIM_CCMR2_CC3S;
            timer->CCMR2 &= ~TIM_CCMR2_OC3M;
            timer->CCMR2 |= (6U << TIM_CCMR2_OC3M_Pos);
            timer->CCMR2 |= TIM_CCMR2_OC3PE;
            timer->CCER |= TIM_CCER_CC3E;
            break;
        case 4U:
            timer->CCER &= ~TIM_CCER_CC4E;
            timer->CCER &= ~TIM_CCER_CC4P;
            timer->CCR4 = 0U;
            timer->CCMR2 &= ~TIM_CCMR2_CC4S;
            timer->CCMR2 &= ~TIM_CCMR2_OC4M;
            timer->CCMR2 |= (6U << TIM_CCMR2_OC4M_Pos);
            timer->CCMR2 |= TIM_CCMR2_OC4PE;
            timer->CCER |= TIM_CCER_CC4E;
            break;
    }

    return PWM_STATUS_OK;
}

pwm_status_t pwm_set_duty_cycle(TIM_TypeDef *timer, uint8_t channel, uint8_t duty_cycle) {
    if (timer == NULL || channel < 1U || channel > 4U || duty_cycle > 100U) {
        return PWM_STATUS_ERROR;
    }

    // convert duty cycle percentage to CCRx value based on the timer's ARR value
    uint32_t ccr_value = (duty_cycle * (timer->ARR + 1U)) / 100U;

    switch (channel) {
        case 1:
            timer->CCR1 = ccr_value;
            break;
        case 2:
            timer->CCR2 = ccr_value;
            break;
        case 3:
            timer->CCR3 = ccr_value;
            break;
        case 4:
            timer->CCR4 = ccr_value;
            break;
    }

    return PWM_STATUS_OK;
}