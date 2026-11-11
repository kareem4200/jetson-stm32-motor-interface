#include "stm32f4xx.h"

// define the used pins (PORT A)
#define FLM_IN1 5U
#define FLM_IN2 4U
#define FLM_PWM 	6U

#define PWM_PSC 15U   // 16 MHz / (PSC + 1) = 1 MHz timer tick
#define PWM_ARR 99U   // 100 ticks per period -> f_pwm = f_effective / counter_period = 10 kHz, so CCR1 = duty in %

int main(void) {
	// Set the clocks for GPIOA and TIMER3 and do dummy read before the 1st GPIO access
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	(void)RCC->AHB1ENR;
	(void)RCC->APB1ENR;

	// set pins modes using MODER
	GPIOA->MODER &= ~(3U << (FLM_IN1 * 2));
	GPIOA->MODER &= ~(3U << (FLM_IN2 * 2));
	GPIOA->MODER |= (1U << (FLM_IN1 * 2));
	GPIOA->MODER |= (1U << (FLM_IN2 * 2));

	// alternate fn for PWM pin
	GPIOA->MODER &= ~(3U << (FLM_PWM * 2));
	GPIOA->MODER |=  (2U << (FLM_PWM * 2));

	// set output type with OTYPER
	GPIOA->OTYPER &= ~(1U << FLM_IN1);
	GPIOA->OTYPER &= ~(1U << FLM_IN2);
	GPIOA->OTYPER &= ~(1U << FLM_PWM);

	// set pin speed with OSPEEDR
	GPIOA->OSPEEDR &= ~(3U << (FLM_IN1 * 2));
	GPIOA->OSPEEDR &= ~(3U << (FLM_IN2 * 2));
	GPIOA->OSPEEDR &= ~(3U << (FLM_PWM * 2));

	// we need no pull up/down resistors
	GPIOA->PUPDR &= ~(3U << (FLM_IN1 * 2));
	GPIOA->PUPDR &= ~(3U << (FLM_IN2 * 2));
	GPIOA->PUPDR &= ~(3U << (FLM_PWM * 2));

	// PA6 -> AF2 (TIM3_CH1). Pins 0..7 are in AFR[0] (AFRL), 4 bits per pin
	GPIOA->AFR[0] &= ~(0xFU << (FLM_PWM * 4));
	GPIOA->AFR[0] |=  (2U   << (FLM_PWM * 4));

	// TIM3 time base
	TIM3->PSC = PWM_PSC;
	TIM3->ARR = PWM_ARR;
	TIM3->CCR1 = 0U;                          // start with 0 % duty

	// Channel 1: output (CC1S = 00), PWM mode 1 (OC1M = 110), preload on CCR1 (OC1PE)
	TIM3->CCMR1 &= ~(0xFFU);
	TIM3->CCMR1 |=  (6U << 4) | (1U << 3);

	// enable the channel 1 output (CC1E), active high (CC1P = 0)
	TIM3->CCER &= ~(1U << 1);
	TIM3->CCER |=  (1U << 0);

	// ARR preload on, force an update so PSC/ARR take effect, then start the counter
	TIM3->CR1 |= (1U << 7);
	TIM3->EGR |= (1U << 0);
	TIM3->CR1 |= (1U << 0);

	// start with IN1 high and IN2 low
	GPIOA->BSRR = (1U << FLM_IN1);
	GPIOA->BSRR = (1U << (FLM_IN2 + 16U));

	TIM3->CCR1 = 70U;

	while (1) { }

}
