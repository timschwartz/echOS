#pragma once
#include <stdint.h>

void tims_function(void);

/** Set all 'n' elements of 's' to value 'c'.
  *
  *   \param s pointer to elements.
  *   \param c value to set elements.
  *   \param n number of elements to write.
  */
void *memset(void *s, int c, size_t n);

/**
  *
  *
  */
size_t strlen(const char *str);
