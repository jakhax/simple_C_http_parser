#include "test_common.h"

void run_tests(const char* test_name, int (*tests[])(void), int tests_count) {

    printf("\r\n\r\n+++++ %s UNIT TESTS +++++\r\n", test_name);
    int passes = 0;
    for (int i = 0; i < tests_count; i++) {
        if (tests[i]() == HTTPS_CLIENT_OK) {
            printf("\r\nPASS\r\n\r\n");
            passes++;
        } else {
            printf("\r\nFAIL\r\n\r\n");
        }
    }

    printf("%d PASS %d FAIL\r\n", passes, tests_count - passes);

    printf("----- %s UNIT TESTS ----\r\n\r\n", test_name);

}