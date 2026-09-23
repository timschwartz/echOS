#include <string.h>

void *memset(void *s, int c, size_t n)
{
    unsigned char *p = s;
    for(size_t counter = 0; counter < n; counter++)
    {
        p[counter] = (unsigned char)c;
    }
    return s;
}
