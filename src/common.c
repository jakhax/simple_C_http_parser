#include "common.h"
#include <ctype.h>

int strcicmp(char const* a, char const* b) {
    for (;; a++, b++) {
        int d = tolower(*a) - tolower(*b);
        if (d != 0 || !*a) {
            return d;
        }
    }
}
