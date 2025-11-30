#ifndef COMMON_HPP
#define COMMON_HPP

#include <cstdio>

#define CHECK_RETVAL(val)                                                       \
    do {                                                                        \
        if ((val) < 0) {                                                        \
            printf("Error at %s:%d. RetVal = %d\n", __FILE__, __LINE__, val);   \
        }                                                                       \
    } while (0)

#endif