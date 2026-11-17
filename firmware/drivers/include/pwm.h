#ifndef DRIVERS_PWM_H_
#define DRIVERS_PWM_H_

#include "stm32f401xc.h"
#include "clock.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// TODO: use better error codes later
typedef enum {
    PWM_STATUS_OK,
    PWM_STATUS_ERROR,
} pwm_status_t;

struct timer_clk_bus_mapping {
    TIM_TypeDef *timer;
    clock_bus_t bus;
    uint32_t rcc_apbenr_mask;
};

pwm_status_t pwm_timer_init(TIM_TypeDef *timer, uint32_t pwm_freq_hz);
pwm_status_t pwm_channel_init(TIM_TypeDef *timer, uint8_t channel);
pwm_status_t pwm_set_duty_cycle(TIM_TypeDef *timer, uint8_t channel, uint8_t duty_cycle);
// void pwm_start(TIM_TypeDef *timer);

#endif // DRIVERS_PWM_H_