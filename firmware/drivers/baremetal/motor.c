#include "motor.h"
#include "gpio.h"
#include "pwm.h"
#include "stm32f401xc.h"
#include <stddef.h>

static const motor_config_t motor_configs[4] = {
    // Front left
    {MOTOR_FRONT_LEFT, GPIOA, GPIOA, GPIOA, 5U, 4U, 6U, TIM3, 1U, 2U},

    // Front right
    {MOTOR_FRONT_RIGHT, GPIOA, GPIOA, GPIOA, 3U, 2U, 1U, TIM2, 2U, 1U},

    // Rear left
    {MOTOR_REAR_LEFT, GPIOB, GPIOB, GPIOA, 14U, 13U, 8U, TIM1, 1U, 1U},

    // Rear right
    {MOTOR_REAR_RIGHT, GPIOA, GPIOB, GPIOA, 9U, 15U, 10U, TIM1, 3U, 1U}
};

static motor_state_t motor_states[4] = {
    {0, 0}, // MOTOR_FRONT_LEFT
    {0, 0}, // MOTOR_FRONT_RIGHT
    {0, 0}, // MOTOR_REAR_LEFT
    {0, 0}  // MOTOR_REAR_RIGHT
};

motor_status_t motors_init(void) {
    // initialize the timers separately
    TIM_TypeDef *timers_tracker[8] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

    for (uint8_t i = 0; i < 4; i++) {
        const motor_config_t *motor = &motor_configs[i];
        // I use only 3 timers, but the board has 8 overall. I chose to keep it generic

        // Initialize GPIO pins for motor control
        gpio_config_t in1_config = {
            .mode = GPIO_MODE_OUTPUT,
            .otype = GPIO_OTYPE_PUSH_PULL,
            .ospeed = GPIO_OSPEED_LOW,
            .pupd = GPIO_PUPD_PULL_DOWN, // ensure motor is off if pin is floating
            .af = 0U
        };
        gpio_config_t in2_config = {
            .mode = GPIO_MODE_OUTPUT,
            .otype = GPIO_OTYPE_PUSH_PULL,
            .ospeed = GPIO_OSPEED_LOW,
            .pupd = GPIO_PUPD_PULL_DOWN,
            .af = 0U
        };
        gpio_config_t en_config = {
            .mode = GPIO_MODE_ALTERNATE,
            .otype = GPIO_OTYPE_PUSH_PULL,
            .ospeed = GPIO_OSPEED_LOW,
            .pupd = GPIO_PUPD_PULL_DOWN,
            .af = motor->pwm_af
        };

        // init direction pins first to ensure motor is off at startup
        gpio_init(motor->in1_port, motor->in1_pin, &in1_config);
        gpio_init(motor->in2_port, motor->in2_pin, &in2_config);        
        gpio_write_pin(motor->in1_port, motor->in1_pin, GPIO_PIN_LOW);
        gpio_write_pin(motor->in2_port, motor->in2_pin, GPIO_PIN_LOW);
        
        // init enable pin for PWM output
        gpio_init(motor->en_port, motor->en_pin, &en_config);
        
        // Initialize PWM timer and channel for motor speed control and set duty cycle to 0%
        bool timer_initialized = false;
        for (size_t j = 0; j < sizeof(timers_tracker) / sizeof(timers_tracker[0]); j++) {
            if (timers_tracker[j] == motor->timer) {
                // Timer already initialized, skip
                timer_initialized = true;
                break;
            }
        }
        if (!timer_initialized) {
            pwm_status_t status = pwm_timer_init(motor->timer, PWM_FREQUENCY_HZ);
            if (status != PWM_STATUS_OK) {
                return MOTOR_STATUS_ERROR;
            }
            for (size_t j = 0; j < sizeof(timers_tracker) / sizeof(timers_tracker[0]); j++) {
                if (timers_tracker[j] == NULL) {
                    timers_tracker[j] = motor->timer;
                    break;
                }
            }
        }

        pwm_status_t channel_status = pwm_channel_init(motor->timer, motor->channel);
        if (channel_status != PWM_STATUS_OK) {
            return MOTOR_STATUS_ERROR;
        }
        pwm_status_t duty_cycle_status = pwm_set_duty_cycle(motor->timer, motor->channel, 0U);
        if (duty_cycle_status != PWM_STATUS_OK) {
            return MOTOR_STATUS_ERROR;
        }
    }
    return MOTOR_STATUS_OK;
}

static motor_status_t motor_set_direction(motor_id_t motor_id, bool forward) {
    if (motor_id < MOTOR_FRONT_LEFT || motor_id > MOTOR_REAR_RIGHT) {
        return MOTOR_STATUS_ERROR;
    }

    const motor_config_t *motor = &motor_configs[motor_id];

    if (forward) {
        gpio_write_pin(motor->in1_port, motor->in1_pin, GPIO_PIN_HIGH);
        gpio_write_pin(motor->in2_port, motor->in2_pin, GPIO_PIN_LOW);
    } else {
        gpio_write_pin(motor->in1_port, motor->in1_pin, GPIO_PIN_LOW);
        gpio_write_pin(motor->in2_port, motor->in2_pin, GPIO_PIN_HIGH);
    }

    return MOTOR_STATUS_OK;
}

motor_status_t motor_set_duty_cycle(motor_id_t motor_id, int8_t duty) {
    if (motor_id < MOTOR_FRONT_LEFT || motor_id > MOTOR_REAR_RIGHT) {
        return MOTOR_STATUS_ERROR;
    }
    
    if (duty < -100 || duty > 100) {
        return MOTOR_STATUS_ERROR;
    }
    
    if (duty == 0) {
        return motor_stop(motor_id);
    }

    motor_states[motor_id].target_duty_cycle = duty;
    
    return MOTOR_STATUS_OK;
}

motor_status_t motor_all_update(void) {
    for (uint8_t i = 0; i < 4; i++) {
        const motor_config_t *motor = &motor_configs[i];
        
        // the time it takes for the next CCR update.
        uint16_t delay_time_us = 1000000U / PWM_FREQUENCY_HZ; // convert to microseconds
        bool current_direction = (motor_states[i].applied_duty_cycle > 0);
        bool target_direction = (motor_states[i].target_duty_cycle > 0);
        
        // If the direction has changed, stop the motor first, wait for 100 us and then reverse direction
        if ((motor_states[i].applied_duty_cycle != 0) && 
            (motor_states[i].target_duty_cycle != 0) && 
            (current_direction ^ target_direction)) {
            pwm_status_t duty_cycle_status = pwm_set_duty_cycle(motor->timer, motor->channel, 0U);
            if (duty_cycle_status != PWM_STATUS_OK) {
                return MOTOR_STATUS_ERROR;
            }
            delay_us(delay_time_us);
        }
        uint8_t magnitude = (uint8_t)(target_direction ? motor_states[i].target_duty_cycle : -motor_states[i].target_duty_cycle);

        motor_status_t direction_status = motor_set_direction(motor->id, target_direction);
        if (direction_status != MOTOR_STATUS_OK) {
            return MOTOR_STATUS_ERROR;
        }

        pwm_status_t duty_cycle_status = pwm_set_duty_cycle(motor->timer, motor->channel, magnitude);
        if (duty_cycle_status != PWM_STATUS_OK) {
            return MOTOR_STATUS_ERROR;
        }

        motor_states[i].applied_duty_cycle = motor_states[i].target_duty_cycle;
    }
    return MOTOR_STATUS_OK;
}

motor_status_t motor_stop(motor_id_t motor_id) {
    if (motor_id < MOTOR_FRONT_LEFT || motor_id > MOTOR_REAR_RIGHT) {
        return MOTOR_STATUS_ERROR;
    }

    const motor_config_t *motor = &motor_configs[motor_id];

    // Set both direction pins low to stop the motor
    gpio_write_pin(motor->in1_port, motor->in1_pin, GPIO_PIN_LOW);
    gpio_write_pin(motor->in2_port, motor->in2_pin, GPIO_PIN_LOW);
    // Set duty cycle to 0%
    pwm_status_t duty_cycle_status = pwm_set_duty_cycle(motor->timer, motor->channel, 0U);
    if (duty_cycle_status != PWM_STATUS_OK) {
        return MOTOR_STATUS_ERROR;
    }

    motor_states[motor_id].applied_duty_cycle = 0;
    motor_states[motor_id].target_duty_cycle = 0;

    return MOTOR_STATUS_OK;
}

void motors_stop_all(void) {
    for (uint8_t i = 0; i < 4; i++) {
        motor_stop((motor_id_t)i);
    }
}