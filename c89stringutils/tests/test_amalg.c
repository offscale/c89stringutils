#define _GNU_SOURCE
#define C89STRINGUTILS_IMPLEMENTATION
/* clang-format off */
#include "c89stringutils_amalgamation.h"
#include <stdio.h>
#include <stdlib.h>
/* clang-format on */

int main(void) {
  char *x = NULL;
  int rc = c89stringutils_asprintf(&x, "hello %s", "world");
  if (rc == 0 && x != NULL) {
    printf("%s\n", x);
    free(x);
    return 0;
  }
  fprintf(stderr, "Failed: rc = %d, x = %p\n", rc, (void *)x);
  return 1;
}
