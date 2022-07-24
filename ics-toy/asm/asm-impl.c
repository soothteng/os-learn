#include "asm.h"
#include <string.h>

int64_t asm_add(int64_t a, int64_t b) {
  int64_t res;
  asm("mov $0, %[t];"
      "add %1, %[t];"
      "add %2, %[t]"
      : [t] "=r"(res)
      : "r"(a), "r"(b));
  return res;
}

int asm_popcnt(uint64_t x) {
  int cnt;
  asm("mov $64, %%edx;"
      "mov $0, %[t];"
      ".L1:;"
      "mov %1, %%rcx;"
      "and $1, %%ecx;"
      "add %%ecx, %[t];"
      "shr %1;"
      "sub $1, %%edx;"
      "jne .L1;"
      : [t] "=r"(cnt)
      : "r"(x)
      : "%rdx", "%rcx");
  return cnt;
}

void *asm_memcpy(void *dest, const void *src, size_t n) {
  asm("mov %0, %[ret];"
      "test %2, %2;"
      "je .L3;"
      "mov $0, %%ecx;"
      ".L2:movzbl (%1,%%rcx,1), %%r8d;"
      "mov %%r8d, (%[ret],%%rcx,1);"
      "add $1, %%rcx;"
      "cmp %2, %%rcx;"
      "jne .L2;"
      ".L3:;"
      : [ret] "+r"(dest)
      : "r"(src), "r"(n)
      : "%rcx", "%r8");
  return dest;
}

int asm_setjmp(asm_jmp_buf env) {
  asm("mov %rbx, (%rdi);"
      "mov %rbp, 8(%rdi);"
      "mov %r12, 16(%rdi);"
      "mov %r13, 24(%rdi);"
      "mov %r14, 32(%rdi);"
      "mov %r15, 40(%rdi);"
      "lea 8(%rsp), %rcx;"
      "mov %rcx, 48(%rdi);"
      "mov (%rsp), %rcx;"
      "mov %rcx, 56(%rdi);");
  return 0;
}

void asm_longjmp(asm_jmp_buf env, int val) {
  asm("mov %rsi, %rax;"
      "test %rax, %rax;"
      "jnz .L4;"
      "mov $1, %rax;"
      ".L4:;"
      "mov (%rdi), %rbx;"
      "mov 8(%rdi), %rbp;"
      "mov 16(%rdi), %r12;"
      "mov 24(%rdi), %r13;"
      "mov 32(%rdi), %r14;"
      "mov 40(%rdi), %r15;"
      "mov 48(%rdi), %rsp;"
      "jmp *56(%rdi);");
}
