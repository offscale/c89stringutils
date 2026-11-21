/**
 * @file c89stringutils_log.h
 * @brief Logging utilities for c89stringutils.
 */
#ifndef C89STRINGUTILS_LOG_H
#define C89STRINGUTILS_LOG_H

/* clang-format off */
#include "c89stringutils_export.h"
#include "c89stringutilsConfig.h"
#include <stdio.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined(C89STRINGUTILS_HAVE_ATTR_FORMAT)
#if defined(__MINGW_PRINTF_FORMAT)
/**
 * @brief Printf format-checking attribute macro for logging functions.
 * @param fmt_idx 1-based index of the format string parameter.
 * @param arg_idx 1-based index of the first variadic argument, or 0 for
 * va_list.
 */
#define C89STRINGUTILS_LOG_FORMAT_PRINTF(fmt_idx, arg_idx)                     \
  __attribute__((__format__(__MINGW_PRINTF_FORMAT, fmt_idx, arg_idx)))
#else
/**
 * @brief Printf format-checking attribute macro for logging functions.
 * @param fmt_idx 1-based index of the format string parameter.
 * @param arg_idx 1-based index of the first variadic argument, or 0 for
 * va_list.
 */
#define C89STRINGUTILS_LOG_FORMAT_PRINTF(fmt_idx, arg_idx)                     \
  __attribute__((__format__(__printf__, fmt_idx, arg_idx)))
#endif
#else
/**
 * @brief Printf format-checking attribute macro fallback for logging functions.
 * @param fmt_idx 1-based index of the format string parameter.
 * @param arg_idx 1-based index of the first variadic argument, or 0 for
 * va_list.
 */
#define C89STRINGUTILS_LOG_FORMAT_PRINTF(fmt_idx, arg_idx)
#endif

#ifndef LOG_DEBUG
/**
 * @brief Log debug message
 * @param fmt The format string.
 * @param ... The arguments.
 * @return Number of characters printed, or -1 on error.
 */
C89STRINGUTILS_EXPORT int c89stringutils_log_debug(const char *fmt, ...)
    C89STRINGUTILS_LOG_FORMAT_PRINTF(1, 2);
/** @brief Macro for debug logging */
#define LOG_DEBUG c89stringutils_log_debug
#endif /* !LOG_DEBUG */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !C89STRINGUTILS_LOG_H */
