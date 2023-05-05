#ifdef _MSC_VER
#endif /* _MSC_VER */
#define _CRT_SECURE_NO_WARNINGS 1

#include <greatest.h>

#include "test_string_extras.h"

/* Add definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(strnstr_suite);
  GREATEST_MAIN_END();
}
