#ifndef BSP_BOARD_H_
#define BSP_BOARD_H_

#include "stm32f401xc.h"

typedef enum {
    MOTOR_FRONT_LEFT,
    MOTOR_FRONT_RIGHT,
    MOTOR_REAR_LEFT,
    MOTOR_REAR_RIGHT,
    MOTOR_COUNT
} motor_id_t;

typedef struct {
    motor_id_t id;
    GPIO_TypeDef *in1_port;
    GPIO_TypeDef *in2_port;
    GPIO_TypeDef *en_port;

    uint8_t in1_pin;
    uint8_t in2_pin;
    uint8_t en_pin;

    TIM_TypeDef *timer;
    uint8_t channel;
    uint8_t pwm_af;
} motor_config_t;

extern const motor_config_t motor_configs[MOTOR_COUNT];

#endif // BSP_BOARD_H_