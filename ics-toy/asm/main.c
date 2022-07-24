#include "asm.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  asm_jmp_buf buf;
  int r = asm_setjmp(buf);
  if (r == 0) {
    assert(asm_add(1234, 5678) == 6912);
    assert(asm_popcnt(0x0123456789abcdefULL) == 32);
    assert(asm_add(0, 999) == 999);
    assert(asm_add(0xffffffffffffffff, 2) == 1);

    assert(asm_popcnt(0xffffffffffffffffull) == 64);
    assert(asm_popcnt(1) == 1);
    assert(asm_popcnt(2) == 1);
    assert(asm_popcnt(3) == 2);
    assert(asm_popcnt(4) == 1);

    assert(asm_memcpy(NULL, NULL, 0) == NULL);
    char s1[20] = "Peoples'";
    char s2[20] = " Republic";
    char s3[20] = "";
    char s4[20] = "Purple";
    asm_memcpy(s3, s1, 20);
    assert(strcmp(s3, "Peoples'") == 0);
    asm_memcpy(s3 + 8, s2, 20);
    assert(strcmp(s3, "Peoples' Republic") == 0);
    asm_memcpy(s3, s4, 3);
    assert(strcmp(s3, "Pur") == 0);

    asm_longjmp(buf, 123);
  } else {
    assert(r == 123);
    printf("PASSED.\n");
  }
}
