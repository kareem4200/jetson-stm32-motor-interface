#ifndef DRIVERS_GPIO_H_
#define DRIVERS_GPIO_H_

#include "stm32f401xc.h"
#include <stdint.h>

typedef enum {
    GPIO_MODE_INPUT = 0U,
    GPIO_MODE_OUTPUT = 1U,
    GPIO_MODE_ALTERNATE = 2U,
    GPIO_MODE_ANALOG = 3U
} gpio_mode_t;

typedef enum {
    GPIO_OTYPE_PUSH_PULL = 0U,
    GPIO_OTYPE_OPEN_DRAIN = 1U
} gpio_otype_t;

typedef enum {
    GPIO_OSPEED_LOW = 0U,
    GPIO_OSPEED_MEDIUM = 1U,
    GPIO_OSPEED_HIGH = 2U,
    GPIO_OSPEED_VERY_HIGH = 3U
} gpio_ospeed_t;

typedef enum {
    GPIO_PUPD_NONE = 0U,
    GPIO_PUPD_PULL_UP = 1U,
    GPIO_PUPD_PULL_DOWN = 2U
} gpio_pupd_t;

typedef enum {
    GPIO_PIN_LOW = 0U,
    GPIO_PIN_HIGH = 1U
} gpio_pin_state_t;

// GPIO function declarations
void gpio_init(GPIO_TypeDef *port, uint8_t pin, gpio_mode_t mode, gpio_otype_t otype, gpio_ospeed_t ospeed, gpio_pupd_t pupd, uint32_t af);
void gpio_write_pin(GPIO_TypeDef *port, uint8_t pin, gpio_pin_state_t state);
void gpio_toggle_pin(GPIO_TypeDef *port, uint8_t pin);
gpio_pin_state_t gpio_read_pin(GPIO_TypeDef *port, uint8_t pin);

#endif // DRIVERS_GPIO_H_