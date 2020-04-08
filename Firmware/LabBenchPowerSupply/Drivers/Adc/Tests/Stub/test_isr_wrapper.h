#ifndef TEST_ISR_WRAPPER
#define TEST_ISR_WRAPPER

#define ISR(VEC) void test_isr_implementation(void)

/**
 * @brief wrapps the avr call to ISR
*/
ISR(VEC);

#endif /* TEST_ISR_WRAPPER */