#include <stack.h>
#include <string.h>

void kernel_start(stack64_t *pageStack)
{
    uint64_t *temp = (uint64_t *)stack64_pop(pageStack);
    memset(temp, 0, 4096);
}
