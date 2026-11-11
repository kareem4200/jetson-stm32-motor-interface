#include "gpio.h"

static bool gpio_is_valid(GPIO_TypeDef *port, uint8_t pin, gpio_mode_t mode) {
    return (port != NULL) && ((unsigned int)pin < 16U) && ((unsigned int)mode <= GPIO_MODE_ANALOG);
}

void gpio_init(GPIO_TypeDef *port, uint8_t pin, gpio_mode_t mode, gpio_otype_t otype, gpio_ospeed_t ospeed, gpio_pupd_t pupd) {
    if (!gpio_is_valid(port, pin, mode)) {
        return;
    }

    // set pin mode using MODER
    port->MODER &= ~(3U << (pin * 2));
    port->MODER |= (mode << (pin * 2));

    // set output type with OTYPER
    port->OTYPER &= ~(1U << pin);
    port->OTYPER |= (otype << pin);

    // set pin speed with OSPEEDR
    port->OSPEEDR &= ~(3U << (pin * 2));
    port->OSPEEDR |= (ospeed << (pin * 2));

    // set pull-up/pull-down resistors with PUPDR
    port->PUPDR &= ~(3U << (pin * 2));
    port->PUPDR |= (pupd << (pin * 2));
}

void gpio_write_pin(GPIO_TypeDef *port, uint8_t pin, gpio_pin_state_t state) {
    if (state == GPIO_PIN_HIGH) {
        port->BSRR = (1U << pin); // set pin high
    } else {
        port->BSRR = (1U << (pin + 16U)); // set pin low
    }
}

void gpio_toggle_pin(GPIO_TypeDef *port, uint8_t pin) {
    if (port->ODR & (1U << pin)) {
        port->BSRR = (1U << (pin + 16U)); // set pin low
    } else {
        port->BSRR = (1U << pin); // set pin high
    }
}

