#include "board.h"


const motor_config_t motor_configs[MOTOR_COUNT] = {
    // Front left
    {MOTOR_FRONT_LEFT, GPIOA, GPIOA, GPIOA, 5U, 4U, 6U, TIM3, 1U, 2U},

    // Front right
    {MOTOR_FRONT_RIGHT, GPIOA, GPIOA, GPIOA, 3U, 2U, 1U, TIM2, 2U, 1U},

    // Rear left
    {MOTOR_REAR_LEFT, GPIOB, GPIOB, GPIOA, 14U, 13U, 8U, TIM1, 1U, 1U},

    // Rear right
    {MOTOR_REAR_RIGHT, GPIOA, GPIOB, GPIOA, 9U, 15U, 10U, TIM1, 3U, 1U}
};