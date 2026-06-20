/**
 * @file test.c
 * @brief Main entry point for greatest tests.
 */
/* clang-format off */
#include <greatest.h>
#include <c89stringutils_log.h>

#include "test_string_extras.h"
/* clang-format on */

/* Add definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

/**
 * @brief Main function for tests.
 * @param argc Number of arguments.
 * @param argv Argument values.
 * @return Exit status.
 */
int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(strnstr_suite);
  GREATEST_MAIN_END();
}
