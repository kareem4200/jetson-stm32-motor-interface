#include "gpio.h"
#include "stm32f401xc.h"
#include <stdbool.h>

static bool gpio_is_valid(GPIO_TypeDef *port, uint8_t pin, gpio_mode_t mode) {
    return (port != NULL) && ((unsigned int)pin < 16U) && ((unsigned int)mode <= GPIO_MODE_ANALOG);
}

void gpio_init(GPIO_TypeDef *port, uint8_t pin, gpio_mode_t mode, gpio_otype_t otype, gpio_ospeed_t ospeed, gpio_pupd_t pupd) {
    if (!gpio_is_valid(port, pin, mode)) {
        return;
    }

    // enable the peripheral clock for the GPIO port
    // the spacing between each GPIOx base address is 0x400 and to enable the clock, bits {0-4} in RCC->AHB1ENR are used for GPIOA-GPIOE respectively
    // TODO: Replace this with a lookup table that maps GPIO_TypeDef pointers to their corresponding RCC_AHB1ENR masks in stm32f401xc.h
    uintptr_t addr = (uintptr_t)port;
    uint32_t index = (addr - GPIOA_BASE) / 0x400U;
    RCC->AHB1ENR |= (1U << index);

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

gpio_pin_state_t gpio_read_pin(GPIO_TypeDef *port, uint8_t pin) {
    return (port->IDR & (1U << pin)) ? GPIO_PIN_HIGH : GPIO_PIN_LOW;
}

