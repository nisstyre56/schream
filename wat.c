#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  char *blah = malloc(2);
  blah[0] = 'a';
  blah[1] = '\0';
  printf("%zd\n", strlen(blah));
  free(blah);
  return 0;
}
