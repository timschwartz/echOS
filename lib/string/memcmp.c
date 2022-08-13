#include <string.h>

int memcmp(const void *str1, const void *str2, size_t length)
{
    const char *s1 = str1;
    const char *s2 = str2;

    for(size_t i = 0; i < length; i++)
    {
        if(s1[i] == s2[i]) continue;
        if(s1[i] < s2[i]) return -1;
        return 1;
    }
    return 0;
}
