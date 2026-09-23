#pragma once
#include <stdint.h>

/**
  *
  *
  */
char *itoa(int num, char *str, int base);

/**
  *
  *
  */
int memcmp(const void *str1, const void *str2, size_t length);

/**
  *
  *
  */
void *memcpy(void *dest, const void *src, size_t length);

/** Set all 'n' bytes of 's' to value 'c'.
  *
  *   \param s pointer to bytes.
  *   \param c value to set bytes.
  *   \param n number of bytes to write.
  */
void *memset(void *s, int c, size_t n);

/**
  *
  *
  */
int strcmp(const char *s1, const char *s2);

/**
  *
  *
  */
char *strcpy(char *dest, const char *src);

/**
  *
  *
  */
size_t strlen(const char *str);
