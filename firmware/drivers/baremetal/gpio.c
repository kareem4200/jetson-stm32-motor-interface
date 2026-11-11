#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "gpio.h"


static bool gpio_is_valid(GPIO_TypeDef *port, uint8_t pin, const gpio_config_t *config) {
    return (port != NULL) && 
            ((unsigned int)pin <= 15U) && 
            (config != NULL) && 
            ((unsigned int)config->mode <= GPIO_MODE_ANALOG) && 
            ((unsigned int)config->otype <= GPIO_OTYPE_OPEN_DRAIN) && 
            ((unsigned int)config->ospeed <= GPIO_OSPEED_VERY_HIGH) && 
            ((unsigned int)config->pupd <= GPIO_PUPD_PULL_DOWN) && 
            ((config->mode != GPIO_MODE_ALTERNATE) || ((unsigned int)config->af <= 15U));
}

static void gpio_init_af(GPIO_TypeDef *port, uint8_t pin, uint32_t af) {
    // set alternate function using AFR
    uint32_t afr_index = pin / 8U; // 0 for pins 0-7, 1 for pins 8-15
    // remainder of pin divided by 8 gives the position within AFR[0] or AFR[1]
    port->AFR[afr_index] &= ~(0xFU << ((pin % 8U) * 4U));
    port->AFR[afr_index] |= (af << ((pin % 8U) * 4U));
}

void gpio_init(GPIO_TypeDef *port, uint8_t pin, const gpio_config_t *config) {
    if (!gpio_is_valid(port, pin, config)) {
        return;
    }

    // enable the peripheral clock for the GPIO port
    // the spacing between each GPIOx base address is 0x400 and to enable the clock, 
    // bits {0-4} in RCC->AHB1ENR are used for GPIOA-GPIOE respectively
    // TODO: Replace this with a lookup table that maps GPIO_TypeDef pointers to their 
    // corresponding RCC_AHB1ENR masks in stm32f401xc.h
    uintptr_t addr = (uintptr_t)port;
    uint32_t index = (addr - GPIOA_BASE) / 0x400U;
    RCC->AHB1ENR |= (1U << index);
    (void)RCC->AHB1ENR;

    // handle alternate function initialization if the mode is set to alternate
    // Note: This function is called before setting the MODER register to ensure that the 
    // pin does not enter AF0 mode before the alternate function is configured
    if (config->mode == GPIO_MODE_ALTERNATE) {
        gpio_init_af(port, pin, config->af);
    }

    // set output type with OTYPER
    port->OTYPER &= ~(1U << pin);
    port->OTYPER |= (config->otype << pin);
    
    // set pin speed with OSPEEDR
    port->OSPEEDR &= ~(3U << (pin * 2));
    port->OSPEEDR |= (config->ospeed << (pin * 2));
    
    // set pull-up/pull-down resistors with PUPDR
    port->PUPDR &= ~(3U << (pin * 2));
    port->PUPDR |= (config->pupd << (pin * 2));
    
    // set pin mode using MODER
    port->MODER &= ~(3U << (pin * 2));
    port->MODER |= (config->mode << (pin * 2));
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

