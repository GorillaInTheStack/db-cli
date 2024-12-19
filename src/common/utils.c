#include "utils.h"

unsigned long get_max_fd()
{
    struct rlimit limit;
    if (getrlimit(RLIMIT_NOFILE, &limit) != 0)
    {
        perror("getrlimit");
        exit(EXIT_FAILURE);
    }
    return limit.rlim_cur;
}
