#ifndef _HTTPS_CLIENT_COMMON_H__
#define _HTTPS_CLIENT_COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "log/log.h"

#define LOG_LEVEL LOG_TRACE

enum {
    HTTPS_CLIENT_OK = 0,
    HTTPS_CLIENT_NO_MEMORY = -3001,
};

#endif