#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint64_t multimod(uint64_t, uint64_t, uint64_t);

void test(uint64_t a, uint64_t b, uint64_t m) {
#define U64 "%" PRIu64
  printf(U64 " * " U64 " mod " U64 " = " U64 "\n", a, b, m, multimod(a, b, m));
}

int main() {
  /*
  // origin test frame
  test(123, 456, 789);
  test(123, 456, -1ULL);
  test(-2ULL, -2ULL, -1ULL); // should be 1
  // use python in c to test
  char buf[100];
  FILE *fp = popen("python3 multimod.py", "r");
  assert(fp);
  fscanf(fp, "%s", buf);
  printf("popen() returns: %s\n", buf);
  pclose(fp);
  //get the size of the array (the byte)
  uint64_t a[65];
  printf("%lu\n", sizeof(a));
  int a[65];
  printf("%lu\n", sizeof(a));
  */
}
