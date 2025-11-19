#define println(x)                                                             \
  _Generic((x),                                                                \
      int: printf("%d\n", (x)),                                                \
      long: printf("%ld\n", (x)),                                              \
      float: printf("%f\n", (x)),                                              \
      double: printf("%lf\n", (x)),                                            \
      char *: printf("%s\n", (x)),                                             \
      default: printf("%p\n", (void *)(x)))(x)
