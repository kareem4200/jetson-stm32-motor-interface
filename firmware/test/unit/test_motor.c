#include "unity.h"
#include "motor.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_motor_set_duty_cycle_invalid_motor(void)
{
    motor_status_t status = motor_set_duty_cycle(4, 50);
    TEST_ASSERT_EQUAL(MOTOR_STATUS_ERROR, status);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_motor_set_duty_cycle_invalid_motor);

    return UNITY_END();
}