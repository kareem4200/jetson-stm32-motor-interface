#include "unity.h"
#include "clock.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_clock_get_pclk_hz_invalid_bus(void)
{
    uint32_t pclk_hz = clock_get_pclk_hz(3);
    TEST_ASSERT_EQUAL(CLOCK_ERROR, pclk_hz);
}

void test_clock_get_timerclk_hz_invalid_bus(void)
{
    uint32_t timerclk_hz = clock_get_timerclk_hz(3);
    TEST_ASSERT_EQUAL(CLOCK_ERROR, timerclk_hz);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_clock_get_pclk_hz_invalid_bus);
    RUN_TEST(test_clock_get_timerclk_hz_invalid_bus);

    return UNITY_END();
}