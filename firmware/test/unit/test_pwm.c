#include "unity.h"
#include "pwm.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_pwm_timer_init_null_timer(void)
{
    TIM_TypeDef *timer = NULL;
    uint32_t pwm_freq_hz = 1000;

    pwm_status_t status = pwm_timer_init(timer, pwm_freq_hz);

    TEST_ASSERT_EQUAL(PWM_STATUS_ERROR, status);
}

void test_pwm_timer_init_zero_freq(void)
{
    TIM_TypeDef *timer = TIM1;
    uint32_t pwm_freq_hz = 0;

    pwm_status_t status = pwm_timer_init(timer, pwm_freq_hz);

    TEST_ASSERT_EQUAL(PWM_STATUS_ERROR, status);
}

void test_pwm_timer_init_invalid_timer(void)
{
    TIM_TypeDef *timer = (TIM_TypeDef *)0xDEADBEEF; // Invalid timer address
    uint32_t pwm_freq_hz = 1000;

    pwm_status_t status = pwm_timer_init(timer, pwm_freq_hz);

    TEST_ASSERT_EQUAL(PWM_STATUS_ERROR, status);
}

void test_pwm_channel_init_null_timer(void)
{
    TIM_TypeDef *timer = NULL;
    uint8_t channel = 1;

    pwm_status_t status = pwm_channel_init(timer, channel);

    TEST_ASSERT_EQUAL(PWM_STATUS_ERROR, status);
}

void test_pwm_channel_init_invalid_channel(void)
{
    TIM_TypeDef *timer = TIM9;
    uint8_t channel = 3;

    pwm_status_t status = pwm_channel_init(timer, channel);

    TEST_ASSERT_EQUAL(PWM_STATUS_ERROR, status);
}

void test_pwm_channel_init_zero_channel(void)
{
    TIM_TypeDef *timer = TIM1;
    uint8_t channel = 0;

    pwm_status_t status = pwm_channel_init(timer, channel);

    TEST_ASSERT_EQUAL(PWM_STATUS_ERROR, status);
}

void test_pwm_set_duty_cycle_null_timer(void)
{
    TIM_TypeDef *timer = NULL;
    uint8_t channel = 1;
    uint8_t duty_cycle = 50;

    pwm_status_t status = pwm_set_duty_cycle(timer, channel, duty_cycle);

    TEST_ASSERT_EQUAL(PWM_STATUS_ERROR, status);
}

void test_pwm_set_duty_cycle_invalid_channel(void)
{
    TIM_TypeDef *timer = TIM1;
    uint8_t channel = 5;
    uint8_t duty_cycle = 50;

    pwm_status_t status = pwm_set_duty_cycle(timer, channel, duty_cycle);

    TEST_ASSERT_EQUAL(PWM_STATUS_ERROR, status);
}

void test_pwm_set_duty_cycle_zero_duty_cycle(void)
{
    TIM_TypeDef *timer = TIM1;
    uint8_t channel = 1;
    uint8_t duty_cycle = 0;

    pwm_status_t status = pwm_set_duty_cycle(timer, channel, duty_cycle);

    TEST_ASSERT_EQUAL(PWM_STATUS_OK, status);
}

void test_pwm_set_duty_cycle_invalid_duty_cycle(void)
{
    TIM_TypeDef *timer = TIM1;
    uint8_t channel = 1;
    uint8_t duty_cycle = 150;

    pwm_status_t status = pwm_set_duty_cycle(timer, channel, duty_cycle);

    TEST_ASSERT_EQUAL(PWM_STATUS_ERROR, status);
}

void test_duty_value(void)
{
    int16_t duty = 50;

    TEST_ASSERT_EQUAL_INT16(50, duty);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_duty_value);

    return UNITY_END();
}