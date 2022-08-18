#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

char hex_chars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

char *x32toa(uint32_t num, char *str)
{
    size_t pos = 0;
    for(size_t i = 28; i > 0; i -= 4)
    {
        str[pos++] = hex_chars[(num >> i) & 0xF];
    }
    str[pos] = hex_chars[num & 0xF];
}

int vsprintf(char *str, const char *format, va_list arg)
{
    char temp[32] = {0};
    int written = 0;
    for(size_t i = 0; i < strlen(format); i++)
    {
        if(format[i] == '%')
        {
            i++;
            switch(format[i])
            {
                case 'd':
                    i++;
                    int d_value = va_arg(arg, int);
                    itoa(d_value, temp, 10);
                    for(size_t counter = 0; counter < strlen(temp); counter++)
                    {
                        str[written] = temp[counter];
                        written++;
                    }
                    break;
                case 'x':
                    i++;
                    int x_value = va_arg(arg, int);
                    x32toa(x_value, temp);
                    for(size_t counter = 0; counter < strlen(temp); counter++)
                    {
                        str[written] = temp[counter];
                        written++;
                    }
                    break;
                case 's':
                    i++;
                    char *s_value = va_arg(arg, char *);
                    for(size_t counter = 0; counter < strlen(s_value); counter++)
                    {
                        str[written] = temp[counter];
                        written++;
                    }
                    break;
            }
        }

        str[written] = format[i];
        written++;
    }
    return written;
}