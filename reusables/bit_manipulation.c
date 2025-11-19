
#include <stdio.h>

#define SET_BIT(x, n) ((x) |= (1U << (n)))
#define CLEAR_BIT(x, n) ((x) &= ~(1U << (n)))
#define TOGGLE_BIT(x, n) ((x) ^= (1U << (n)))
#define CHECK_BIT(x, n) (!!((x) & (1U << (n))))

int main() {
  unsigned int flags = 0;
  SET_BIT(flags, 1);
  TOGGLE_BIT(flags, 0);
  if (CHECK_BIT(flags, 1))
    printf("Bit 1 is set\n");
  return 0;
}
