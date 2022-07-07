#include <stdint.h>

uint64_t multimod(uint64_t a, uint64_t b, uint64_t m) {
  int A[65] = {0}, len = 0;
  for (; a != 0; a /= 2) {
    A[len] = a % 2;
    ++len;
  }
  uint64_t res = 0, cur = b;
  for (int i = 0; i < len; i++) {
    if (A[i]) {
      res += cur;
      res %= m;
    }
    cur <<= 1;
    cur %= m;
  }
  return res;
}
