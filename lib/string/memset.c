#include <string.h>

void *memset(void *s, int c, size_t n)
{
    int *p = s;
    for(size_t counter = 0; counter < n; counter++)
    {
        p[counter] = c;
    }
    return s;
}
