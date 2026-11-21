#include "unity.h"
#include "gpio.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_gpio_init_null_port(void)
{
    GPIO_TypeDef *port = NULL;
    uint16_t pin = 5;
    gpio_config_t config = {
        .mode = GPIO_MODE_OUTPUT,
        .otype = GPIO_OTYPE_PUSH_PULL,
        .ospeed = GPIO_OSPEED_LOW,
        .pupd = GPIO_PUPD_NONE,
        .af = 0
    };

    gpio_status_t status = gpio_init(port, pin, &config);

    TEST_ASSERT_EQUAL(GPIO_STATUS_ERROR, status);
}

void test_gpio_init_invalid_pin(void)
{
    GPIO_TypeDef *port = GPIOA;
    uint16_t pin = 16;
    gpio_config_t config = {
        .mode = GPIO_MODE_OUTPUT,
        .otype = GPIO_OTYPE_PUSH_PULL,
        .ospeed = GPIO_OSPEED_LOW,
        .pupd = GPIO_PUPD_NONE,
        .af = 0
    };

    gpio_status_t status = gpio_init(port, pin, &config);

    TEST_ASSERT_EQUAL(GPIO_STATUS_ERROR, status);
}

void test_gpio_init_null_config(void)
{
    GPIO_TypeDef *port = GPIOA;
    uint16_t pin = 5;
    gpio_config_t *config = NULL;

    gpio_status_t status = gpio_init(port, pin, config);

    TEST_ASSERT_EQUAL(GPIO_STATUS_ERROR, status);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_gpio_init_null_port);
    RUN_TEST(test_gpio_init_invalid_pin);
    RUN_TEST(test_gpio_init_null_config);

    return UNITY_END();
}