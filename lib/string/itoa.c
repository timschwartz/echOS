#include <string.h>

void reverse_string(char *s)
{
    size_t c1 = 0, c2, length;
    c2 = length = strlen(s) - 1;

    char temp[length + 1];
    strcpy(temp, s);

    while(c1 <= length)
    {
        s[c1++] = temp[c2--];
    }
}

char *itoa(int num, char *str, int base)
{
    int pos = 0;
    int negative = 0;

    if(num == 0)
    {
        strcpy(str, "0\0");
        return str;
    }

    if((num < 0) && (base == 10))
    {
        negative = 1;
        num = -num;
    }

    while(num)
    {
        int value = num % base;
        str[pos++] = value + '0';
        num = num / base;
    }

    if(negative) str[pos++] = '-';
    str[pos] = '\0';

    reverse_string(str);

    return str;
}
