#define RED "\33[1;31m"
#define ORI "\33[0m"
#define LEN(_array) ( sizeof(_array)/sizeof(_array[0]) )

#define Assert(_con, _fmt, ...)                                                \
  do {                                                                         \
    if (!(_con)) {                                                             \
      fprintf(stderr, RED "Assertion failed:\nLine: %d" _fmt, __LINE__,        \
              ##__VA_ARGS__);                                                  \
      assert(0);                                                               \
    }                                                                          \
  } while (0)
