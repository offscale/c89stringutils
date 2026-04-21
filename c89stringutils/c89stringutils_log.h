#ifndef C89STRINGUTILS_LOG_H
#define C89STRINGUTILS_LOG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* clang-format off */
#include "c89stringutils_export.h"
/* clang-format on */

#ifndef LOG_DEBUG
#ifdef DEBUG
/**
 * @brief Log debug message
 * @param fmt The format string.
 * @param ... The arguments.
 */
C89STRINGUTILS_EXPORT void c89stringutils_log_debug(const char *fmt, ...);
#define LOG_DEBUG c89stringutils_log_debug
#else
/**
 * @brief Log debug message
 * @param fmt The format string.
 * @param ... The arguments.
 */
C89STRINGUTILS_EXPORT void c89stringutils_log_debug(const char *fmt, ...);
#define LOG_DEBUG 1 ? (void)0 : c89stringutils_log_debug
#endif /* DEBUG */
#endif /* !LOG_DEBUG */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !C89STRINGUTILS_LOG_H */
