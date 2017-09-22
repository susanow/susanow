
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <vector>

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

inline std::vector<size_t> coremask2vecor(uint8_t coremask)
{
  std::vector<size_t> vec;
  size_t bitlength = sizeof(uint8_t) * 8;
  for (size_t i=0; i<bitlength; i++) {
    if ((coremask & uint8_t(1<<i)) != 0) {
      vec.push_back(i);
    }
  }
  return vec;
}


