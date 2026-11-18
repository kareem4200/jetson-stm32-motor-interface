#include "stm32f401xc.h"
#include "clock.h"
#include <stdint.h>

void delay_init(void);
void delay_ms(uint32_t ms);
void delay_us(uint32_t us);