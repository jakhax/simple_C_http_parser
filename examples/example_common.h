#ifndef _C_HTTP_EXAMPLES_COMMON_H_
#define _C_HTTP_EXAMPLES_COMMON_H_

#include "common.h"

#define info(x, ...) printf("[INFO] " x "\r\n", ##__VA_ARGS__)
#define error(x, ...) printf("[ERROR] " x "\r\n", ##__VA_ARGS__)

#endif