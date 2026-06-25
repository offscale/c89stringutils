/**
 * @file c89stringutils_log.h
 * @brief Logging utilities for c89stringutils.
 */
#ifndef C89STRINGUTILS_LOG_H
#define C89STRINGUTILS_LOG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* clang-format off */
#include "c89stringutils_export.h"
#include "c89stringutilsConfig.h"
/* clang-format on */

#if defined(C89STRINGUTILS_HAVE_ATTR_FORMAT)
#define C89STRINGUTILS_LOG_FORMAT_PRINTF(fmt_idx, arg_idx)                     \
  __attribute__((format(printf, fmt_idx, arg_idx)))
#else
#define C89STRINGUTILS_LOG_FORMAT_PRINTF(fmt_idx, arg_idx)
#endif

#ifndef LOG_DEBUG
#ifdef DEBUG
/**
 * @brief Log debug message
 * @param fmt The format string.
 * @param ... The arguments.
 */
C89STRINGUTILS_EXPORT void c89stringutils_log_debug(const char *fmt, ...)
    C89STRINGUTILS_LOG_FORMAT_PRINTF(1, 2);
/** @brief Macro for debug logging */
#define LOG_DEBUG c89stringutils_log_debug
#else
/**
 * @brief Log debug message
 * @param fmt The format string.
 * @param ... The arguments.
 */
C89STRINGUTILS_EXPORT void c89stringutils_log_debug(const char *fmt, ...)
    C89STRINGUTILS_LOG_FORMAT_PRINTF(1, 2);
/** @brief Macro for debug logging (disabled) */
#define LOG_DEBUG 1 ? (void)0 : c89stringutils_log_debug
#endif /* DEBUG */
#endif /* !LOG_DEBUG */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !C89STRINGUTILS_LOG_H */
