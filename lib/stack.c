#include <stack.h>

void stack64_push(stack64_t *stack, uint64_t value)
{
    stack->stack[stack->sp] = value;
    stack->sp++;
}

uint64_t stack64_pop(stack64_t *stack)
{
    stack->sp--;
    return stack->stack[stack->sp];
}
