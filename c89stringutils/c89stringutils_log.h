#ifndef C89STRINGUTILS_LOG_H
#define C89STRINGUTILS_LOG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* clang-format off */
#include <stdio.h>
/* clang-format on */

#ifndef LOG_DEBUG
#ifdef DEBUG
#if defined(_MSC_VER) && _MSC_VER < 1400
/* MSVC older than 2005 doesn't support variadic macros well */
#define LOG_DEBUG(fmt, ...)
#else
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvariadic-macros"
#endif /* defined(__GNUC__) || defined(__clang__) */
#define LOG_DEBUG(fmt, ...) fprintf(stderr, "[DEBUG] " fmt, ##__VA_ARGS__)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif /* defined(__GNUC__) || defined(__clang__) */
#endif
#else
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvariadic-macros"
#endif /* defined(__GNUC__) || defined(__clang__) */
#define LOG_DEBUG(fmt, ...)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif /* defined(__GNUC__) || defined(__clang__) */
#endif /* DEBUG */
#endif /* !LOG_DEBUG */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !C89STRINGUTILS_LOG_H */
