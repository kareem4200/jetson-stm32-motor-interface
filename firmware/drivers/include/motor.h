#include "stm32f401xc.h"
#include "pwm.h"
#include "gpio.h"
#include "utils.h"
#include <stdint.h>

#define PWM_FREQUENCY_HZ  10000U

typedef enum {
    MOTOR_FRONT_LEFT,
    MOTOR_FRONT_RIGHT,
    MOTOR_REAR_LEFT,
    MOTOR_REAR_RIGHT
} motor_id_t;

typedef enum {
    MOTOR_STATUS_ERROR,
    MOTOR_STATUS_OK
} motor_status_t;

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

typedef struct {
    int8_t applied_duty_cycle;
    int8_t target_duty_cycle;
} motor_state_t;

motor_status_t motors_init(void);
motor_status_t motor_set_duty_cycle(motor_id_t motor_id, int8_t duty);
motor_status_t motor_all_update(void);
motor_status_t motor_stop(motor_id_t motor_id);
void motors_stop_all(void);