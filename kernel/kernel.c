#include <stack.h>

void kernel_start(stack64_t *pageStack)
{
    stack64_pop(pageStack);
}
