#include <stdlib.h>
#include <stdio.h>

int main(void) {
  char *foo = malloc(20);
  snprintf(foo, 5, "%s", "1234");
  char *bar = foo;
  *foo++ = *bar++;
  printf("%s\n", foo);
  return 0;
}
