#include <string.h>

int strcmp(const char *s1, const char *s2)
{
    size_t length = strlen(s1);
    if(strlen(s2) > length) length = strlen(s2);

    for(size_t i = 0; i < length; i++)
    {
        if(s1[i] == s2[i]) continue;
        if(s1[i] < s2[i]) return -1;
        return 1;
    }
    return 0;
}
