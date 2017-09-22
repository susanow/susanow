
#pragma once
#include <stdint.h>
#include <stddef.h>

inline bool is_power_of2(size_t num)
{
  while (num != 1) {
    int ret = num % 2;
    if (ret == 0) {
      num = num/2;
      continue;
    } else {
      return false;
    }
  }
  return true;
}

