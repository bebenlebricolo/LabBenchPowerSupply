#ifndef TEST_ISR_WRAPPER
#define TEST_ISR_WRAPPER

#ifdef __cplusplus
extern "C"
{
#endif

#define ISR(VEC) void test_isr_implementation(void)

/**
 * @brief wrapps the avr call to ISR
*/
ISR(VEC);

#ifdef __cplusplus
}
#endif

#endif /* TEST_ISR_WRAPPER */