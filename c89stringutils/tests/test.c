/* clang-format off */
#include <greatest.h>
#include <c89stringutils_log.h>

#include "test_string_extras.h"
/* clang-format on */

/* Add definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(strnstr_suite);
  GREATEST_MAIN_END();
}
