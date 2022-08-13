#include <string.h>

void *memcpy(void *dest, const void *src, size_t length)
{
    char *dest_ = dest;
    const char *src_ = src;
    size_t counter = 0;

    if(dest == src) return dest;

    while(counter < length)
    {
        dest_[counter] = src_[counter];
        counter++;
    }

    return dest;
}
