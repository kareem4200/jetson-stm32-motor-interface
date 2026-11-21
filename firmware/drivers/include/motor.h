#include "stm32f401xc.h"
#include "pwm.h"
#include "gpio.h"
#include "board.h"
#include "utils.h"
#include <stdint.h>

#define PWM_FREQUENCY_HZ  10000U

typedef enum {
    MOTOR_STATUS_ERROR,
    MOTOR_STATUS_OK
} motor_status_t;

typedef struct {
    int8_t applied_duty_cycle;
    int8_t target_duty_cycle;
} motor_state_t;

motor_status_t motors_init(void);
motor_status_t motor_set_duty_cycle(motor_id_t motor_id, int8_t duty);
motor_status_t motor_all_update(void);
motor_status_t motor_stop(motor_id_t motor_id);
void motors_stop_all(void);