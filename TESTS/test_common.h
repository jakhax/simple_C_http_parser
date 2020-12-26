#ifndef _C_HTTP_CLIENT_TEST_COMMON_H_
#define _C_HTTP_CLIENT_TEST_COMMON_H_

#include "common.h"

#define info(x, ...) printf("[TEST][INFO] " x "\r\n", ##__VA_ARGS__)
#define error(x, ...) printf("[TEST][ERROR] " x "\r\n", ##__VA_ARGS__)

int run_tests(const char* test_name, int (*tests[])(void), int tests_count);

#endif