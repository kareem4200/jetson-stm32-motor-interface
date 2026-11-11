#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>"

#include "gpio.h"

static void gpio_init_af(GPIO_TypeDef *port, uint8_t pin, uint32_t af, gpio_otype_t otype, gpio_ospeed_t ospeed, gpio_pupd_t pupd);

static bool gpio_is_valid(GPIO_TypeDef *port, uint8_t pin, gpio_mode_t mode) {
    return (port != NULL) && ((unsigned int)pin < 16U) && ((unsigned int)mode <= GPIO_MODE_ANALOG);
}

void gpio_init(GPIO_TypeDef *port, uint8_t pin, gpio_mode_t mode, gpio_otype_t otype, gpio_ospeed_t ospeed, gpio_pupd_t pupd, uint32_t af) {
    if (!gpio_is_valid(port, pin, mode)) {
        return;
    }

    if (mode == GPIO_MODE_ALTERNATE) {
        gpio_init_af(port, pin, af, otype, ospeed, pupd);
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

static void gpio_init_af(GPIO_TypeDef *port, uint8_t pin, uint32_t af, gpio_otype_t otype, gpio_ospeed_t ospeed, gpio_pupd_t pupd) {
    if (!gpio_is_valid(port, pin, GPIO_MODE_ALTERNATE) || (af > 15U)) {
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
    port->MODER |= (GPIO_MODE_ALTERNATE << (pin * 2));

    // set alternate function using AFR
    uint32_t afr_index = pin / 8U; // 0 for pins 0-7, 1 for pins 8-15
    // remainder of pin divided by 8 gives the position within AFR[0] or AFR[1]
    port->AFR[afr_index] &= ~(0xFU << ((pin % 8U) * 4U));
    port->AFR[afr_index] |= (af << ((pin % 8U) * 4U));

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

