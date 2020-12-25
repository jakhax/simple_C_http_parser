#ifndef _C_HTTPS_CLIENT_COMMON_H__
#define _C_HTTPS_CLIENT_COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "log/log.h"

#define LOG_LEVEL LOG_TRACE

#define HTTP_RECEIVE_BUFFER_SIZE 8 * 1024
#define MAX_HEADERS_COUNT 100

enum {
    HTTPS_CLIENT_OK = 0,
    HTTPS_CLIENT_NO_MEMORY = -3001,
    HTTPS_CLIENT_INVALID_PARAMETER = -3002,
};

#ifndef bool
typedef enum boolean {
    false = 0,
    true = 1,
} bool;
#endif


#endif