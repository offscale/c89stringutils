/**
 * @file c89stringutils_string_extras.h
 * @brief string functions helpful on Linux (and sometimes BSD)
 * are now made available on other platforms (Windows, SunOS, &etc.)
 */

#ifndef C89STRINGUTILS_STRING_EXTRAS_H
#define C89STRINGUTILS_STRING_EXTRAS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* clang-format off */
#ifdef C89STRINGUTILS_NO_CMAKE_CONFIG
#ifndef C89STRINGUTILS_EXPORT
#if defined(_WIN32)
#define C89STRINGUTILS_EXPORT __declspec(dllexport)
#else
#define C89STRINGUTILS_EXPORT __attribute__((visibility("default")))
#endif
#endif

/* Standalone feature detection when CMake is not used */
#if defined(__INTEL_COMPILER)
# if defined(_WIN32)
#  define C89STRINGUTILS_HAVE__VSNPRINTF 1
#  define C89STRINGUTILS_HAVE__VSCPRINTF 1
#  define C89STRINGUTILS_HAVE__STRNICMP 1
#  define C89STRINGUTILS_HAVE__STRICMP 1
#  define C89STRINGUTILS_HAVE_PRINTF_I64D 1
#  define C89STRINGUTILS_HAVE_PRINTF_IU 1
#  define C89STRINGUTILS_HAVE___INT64 1
#  define C89STRINGUTILS_HAVE_PRAGMA_WARNING 1
#  define C89STRINGUTILS_HAVE_SAL 1
#  define C89STRINGUTILS_HAVE_VSSCANF 1
#  define C89STRINGUTILS_HAVE_VFSCANF 1
#  if defined(_MSC_VER) && _MSC_VER >= 1400
#   define C89STRINGUTILS_HAVE_SPRINTF_S 1
#   define C89STRINGUTILS_HAVE_VSNPRINTF_S 1
#   define C89STRINGUTILS_HAVE__VSNPRINTF_S 1
#   define C89STRINGUTILS_HAVE_SNPRINTF_S 1
#   define C89STRINGUTILS_HAVE__SNPRINTF_S 1
#   define C89STRINGUTILS_HAVE_STRERROR_S 1
#   define C89STRINGUTILS_HAVE_STRCPY_S 1
#   define C89STRINGUTILS_HAVE_STRNCPY_S 1
#   define C89STRINGUTILS_HAVE_STRCAT_S 1
#   define C89STRINGUTILS_HAVE_STRNCAT_S 1
#   define C89STRINGUTILS_HAVE_STRNLEN_S 1
#   define C89STRINGUTILS_HAVE_MEMCPY_S 1
#   define C89STRINGUTILS_HAVE_FOPEN_S 1
#   define C89STRINGUTILS_HAVE_FREOPEN_S 1
#   define C89STRINGUTILS_HAVE_TMPFILE_S 1
#   define C89STRINGUTILS_HAVE_VSSCANF_S 1
#   define C89STRINGUTILS_HAVE_VFSCANF_S 1
#  endif
# else
#  define C89STRINGUTILS_HAVE_VSNPRINTF 1
#  define C89STRINGUTILS_HAVE_SNPRINTF 1
#  define C89STRINGUTILS_HAVE_LONG_LONG 1
#  define C89STRINGUTILS_HAVE_STRNCASECMP 1
#  define C89STRINGUTILS_HAVE_STRCASECMP 1
#  define C89STRINGUTILS_HAVE_PRINTF_ZU 1
#  define C89STRINGUTILS_HAVE_PRINTF_LLD 1
# endif
#elif defined(__TINYC__)
# define C89STRINGUTILS_HAVE_VSNPRINTF 1
# define C89STRINGUTILS_HAVE_SNPRINTF 1
# define C89STRINGUTILS_HAVE_LONG_LONG 1
# define C89STRINGUTILS_HAVE_PRINTF_LLD 1
# if defined(_WIN32)
#  define C89STRINGUTILS_HAVE__VSNPRINTF 1
#  define C89STRINGUTILS_HAVE__VSCPRINTF 1
#  define C89STRINGUTILS_HAVE__STRNICMP 1
#  define C89STRINGUTILS_HAVE__STRICMP 1
#  define C89STRINGUTILS_HAVE_PRINTF_I64D 1
#  define C89STRINGUTILS_HAVE_PRINTF_IU 1
# endif
#elif defined(__SUNPRO_C)
# define C89STRINGUTILS_HAVE_VSNPRINTF 1
# define C89STRINGUTILS_HAVE_SNPRINTF 1
# define C89STRINGUTILS_HAVE_LONG_LONG 1
# define C89STRINGUTILS_HAVE_WCHAR_H 1
# define C89STRINGUTILS_HAVE_PRINTF_ZU 1
# define C89STRINGUTILS_HAVE_PRINTF_LLD 1
#elif defined(__WATCOMC__) || defined(__BORLANDC__)
/* Rely on fallbacks */
#elif defined(_MSC_VER)
#define C89STRINGUTILS_HAVE__VSNPRINTF 1
#define C89STRINGUTILS_HAVE__VSCPRINTF 1
#define C89STRINGUTILS_HAVE__STRNICMP 1
#define C89STRINGUTILS_HAVE__STRICMP 1
#if _MSC_VER >= 1400 /* MSVC 2005+ */
#define C89STRINGUTILS_HAVE_SPRINTF_S 1
#define C89STRINGUTILS_HAVE_VSNPRINTF_S 1
#define C89STRINGUTILS_HAVE__VSNPRINTF_S 1
#define C89STRINGUTILS_HAVE_SNPRINTF_S 1
#define C89STRINGUTILS_HAVE__SNPRINTF_S 1
#define C89STRINGUTILS_HAVE_STRERROR_S 1
#define C89STRINGUTILS_HAVE_STRCPY_S 1
#define C89STRINGUTILS_HAVE_STRNCPY_S 1
#define C89STRINGUTILS_HAVE_STRCAT_S 1
#define C89STRINGUTILS_HAVE_STRNCAT_S 1
#define C89STRINGUTILS_HAVE_STRNLEN_S 1
#define C89STRINGUTILS_HAVE_MEMCPY_S 1
#define C89STRINGUTILS_HAVE_FOPEN_S 1
#define C89STRINGUTILS_HAVE_FREOPEN_S 1
#define C89STRINGUTILS_HAVE_TMPFILE_S 1
#define C89STRINGUTILS_HAVE_VSSCANF_S 1
#define C89STRINGUTILS_HAVE_VFSCANF_S 1
#define C89STRINGUTILS_HAVE_WCSCPY_S 1
#define C89STRINGUTILS_HAVE_WCSNCPY_S 1
#define C89STRINGUTILS_HAVE_VSWPRINTF_S 1
#define C89STRINGUTILS_HAVE_SWPRINTF_S 1
#define C89STRINGUTILS_HAVE_VFPRINTF_S 1
#define C89STRINGUTILS_HAVE_FPRINTF_S 1
#endif
#define C89STRINGUTILS_HAVE_VSSCANF 1
#define C89STRINGUTILS_HAVE_VFSCANF 1
#define C89STRINGUTILS_HAVE_WCSCPY 1
#define C89STRINGUTILS_HAVE_WCSNCPY 1
#define C89STRINGUTILS_HAVE_VSWPRINTF 1
#define C89STRINGUTILS_HAVE_SWPRINTF 1
#define C89STRINGUTILS_HAVE_PRINTF_I64D 1
#define C89STRINGUTILS_HAVE_PRINTF_IU 1
#define C89STRINGUTILS_HAVE___INT64 1
#define C89STRINGUTILS_HAVE_PRAGMA_WARNING 1
#define C89STRINGUTILS_HAVE_SAL 1
#elif defined(__GNUC__) || defined(__clang__)
#define C89STRINGUTILS_HAVE_VSNPRINTF 1
#define C89STRINGUTILS_HAVE_SNPRINTF 1
#define C89STRINGUTILS_HAVE_VA_COPY 1
#define C89STRINGUTILS_HAVE_ATTR_FORMAT 1
#define C89STRINGUTILS_HAVE_ATTR_NONNULL 1
#define C89STRINGUTILS_HAVE_TYPEOF 1
#define C89STRINGUTILS_HAVE_PRINTF_ZU 1
#define C89STRINGUTILS_HAVE_PRINTF_LLD 1
#define C89STRINGUTILS_HAVE_LONG_LONG 1
#define C89STRINGUTILS_HAVE_WCHAR_H 1
#if __GNUC__ >= 7 || defined(__clang__)
#define C89STRINGUTILS_HAVE_PRAGMA_GCC_DIAGNOSTIC 1
#endif
#endif

/* Unix/POSIX detection */
#if defined(__unix__) || defined(__APPLE__) || defined(__linux__) || defined(__CYGWIN__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__) || defined(__sun) || defined(__BIONIC__)
#if !defined(C89STRINGUTILS_HAVE_STRNCASECMP)
#define C89STRINGUTILS_HAVE_STRNCASECMP 1
#endif
#if !defined(C89STRINGUTILS_HAVE_STRCASECMP)
#define C89STRINGUTILS_HAVE_STRCASECMP 1
#endif
#if defined(_GNU_SOURCE) || defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__)
#define C89STRINGUTILS_HAVE_VASPRINTF 1
#define C89STRINGUTILS_HAVE_ASPRINTF 1
#endif
#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__) || defined(__BIONIC__)
#define C89STRINGUTILS_HAVE_STRLCPY 1
#define C89STRINGUTILS_HAVE_STRLCAT 1
#define C89STRINGUTILS_HAVE_STRNSTR 1
#define C89STRINGUTILS_HAVE_STRCASESTR 1
#endif
#if defined(__linux__) || defined(__CYGWIN__)
#if (_POSIX_C_SOURCE >= 200112L) || defined(_XOPEN_SOURCE) || defined(_GNU_SOURCE)
#define C89STRINGUTILS_HAVE_STRERROR_R 1
#endif
#if defined(_GNU_SOURCE)
#define C89STRINGUTILS_STRERROR_R_CHAR_P 1
#define C89STRINGUTILS_HAVE_STRCASESTR 1
#endif
#define C89STRINGUTILS_HAVE_STRNLEN 1
#endif
#endif
#else
#include "c89stringutils_export.h"
#include "c89stringutilsConfig.h"
#endif

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifdef C89STRINGUTILS_HAVE_SYS_PARAM_H
#include <sys/param.h>
#elif defined(__unix__) || defined(__APPLE__)
#include <sys/param.h>
#endif

#ifdef C89STRINGUTILS_HAVE_STRINGS_H
#include <strings.h>
#elif defined(__unix__) || defined(__APPLE__)
#include <strings.h>
#endif
/* clang-format on */

#if defined(C89STRINGUTILS_HAVE_LONG_LONG)
#if defined(__GNUC__) || defined(__clang__)
__extension__ typedef long long c89stringutils_int64_t;
__extension__ typedef unsigned long long c89stringutils_uint64_t;
#else
/** @brief 64-bit signed integer type */
typedef long long c89stringutils_int64_t;
/** @brief 64-bit unsigned integer type */
typedef unsigned long long c89stringutils_uint64_t;
#endif
/** @brief 64-bit integer literal macro */
#define C89STRINGUTILS_INT64_C(c) c##LL
/** @brief 64-bit unsigned integer literal macro */
#define C89STRINGUTILS_UINT64_C(c) c##ULL
#elif defined(C89STRINGUTILS_HAVE___INT64)
/** @brief 64-bit signed integer type */
typedef __int64 c89stringutils_int64_t;
/** @brief 64-bit unsigned integer type */
typedef unsigned __int64 c89stringutils_uint64_t;
/** @brief 64-bit integer literal macro */
#define C89STRINGUTILS_INT64_C(c) c##i64
/** @brief 64-bit unsigned integer literal macro */
#define C89STRINGUTILS_UINT64_C(c) c##ui64
#else
/** @brief 64-bit fallback signed integer type (aliased to 32-bit long to
 * prevent printf crashes) */
typedef long c89stringutils_int64_t;
/** @brief 64-bit fallback unsigned integer type (aliased to 32-bit unsigned
 * long) */
typedef unsigned long c89stringutils_uint64_t;
/** @brief 32-bit fallback integer literal macro */
#define C89STRINGUTILS_INT64_C(c) c##L
/** @brief 32-bit fallback unsigned integer literal macro */
#define C89STRINGUTILS_UINT64_C(c) c##UL
#endif

#if defined(C89STRINGUTILS_HAVE_ATTR_FORMAT)
#define C89STRINGUTILS_FORMAT_PRINTF(fmt_idx, arg_idx)                         \
  __attribute__((format(printf, fmt_idx, arg_idx)))
#else
#define C89STRINGUTILS_FORMAT_PRINTF(fmt_idx, arg_idx)
#endif

#if defined(C89STRINGUTILS_HAVE_ATTR_NONNULL)
#define C89STRINGUTILS_NONNULL(args) __attribute__((nonnull args))
#else
#define C89STRINGUTILS_NONNULL(args)
#endif

#if defined(C89STRINGUTILS_HAVE_PRINTF_I64D) && defined(_WIN32)
/** @brief MSVC 64-bit format string */
#define NUM_FORMAT "%I64d"
/** @brief MSVC unsigned 64-bit format string */
#define UNUM_FORMAT "%I64u"
#define C89STRINGUTILS_PRId64 "I64d"
#define C89STRINGUTILS_PRIu64 "I64u"
#define C89STRINGUTILS_PRIx64 "I64x"
#elif defined(C89STRINGUTILS_HAVE_PRINTF_LLD)
/** @brief Standard 64-bit format string for non-MSVC */
#define NUM_FORMAT "%lld"
/** @brief Standard unsigned 64-bit format string for non-MSVC */
#define UNUM_FORMAT "%llu"
#define C89STRINGUTILS_PRId64 "lld"
#define C89STRINGUTILS_PRIu64 "llu"
#define C89STRINGUTILS_PRIx64 "llx"
#elif defined(C89STRINGUTILS_HAVE_LONG_LONG) && !defined(_WIN32)
/** @brief Fallback 64-bit format string */
#define NUM_FORMAT "%lld"
#define UNUM_FORMAT "%llu"
#define C89STRINGUTILS_PRId64 "lld"
#define C89STRINGUTILS_PRIu64 "llu"
#define C89STRINGUTILS_PRIx64 "llx"
#else
/** @brief Fallback format string for 32-bit targets */
#define NUM_FORMAT "%ld"
/** @brief Fallback unsigned format string for 32-bit targets */
#define UNUM_FORMAT "%lu"
#define C89STRINGUTILS_PRId64 "ld"
#define C89STRINGUTILS_PRIu64 "lu"
#define C89STRINGUTILS_PRIx64 "lx"
#endif

#if defined(_WIN32)
#define C89STRINGUTILS_PRId32 "I32d"
#define C89STRINGUTILS_PRIu32 "I32u"
#define C89STRINGUTILS_PRIx32 "I32x"
#if defined(_MSC_VER) && _MSC_VER < 1400
/** @brief Fallback pointer format for older MSVC */
#define C89STRINGUTILS_PRI_PTR "I"
#else
/** @brief Standard pointer format */
#define C89STRINGUTILS_PRI_PTR "p"
#endif
/** @brief Standard long double format for MSVC */
#define C89STRINGUTILS_PRI_LDBL "f"
#else
#define C89STRINGUTILS_PRId32 "d"
#define C89STRINGUTILS_PRIu32 "u"
#define C89STRINGUTILS_PRIx32 "x"
/** @brief Standard pointer format */
#define C89STRINGUTILS_PRI_PTR "p"
/** @brief Standard long double format */
#define C89STRINGUTILS_PRI_LDBL "Lf"
#endif

#if defined(C89STRINGUTILS_HAVE_PRINTF_ZU)
/** @brief Standard size_t format string */
#define SIZE_T_FORMAT "%zu"
/** @brief Standard size_t hex format string */
#define SIZE_T_HEX_FORMAT "%zx"
/** @brief Standard ptrdiff_t format string */
#define PTRDIFF_T_FORMAT "%td"
#elif defined(C89STRINGUTILS_HAVE_PRINTF_IU)
/** @brief MSVC size_t format string */
#define SIZE_T_FORMAT "%Iu"
/** @brief MSVC size_t hex format string */
#define SIZE_T_HEX_FORMAT "%Ix"
/** @brief MSVC ptrdiff_t format string */
#define PTRDIFF_T_FORMAT "%Id"
#else
/** @brief Fallback size_t format string */
#define SIZE_T_FORMAT "%lu"
/** @brief Fallback size_t hex format string */
#define SIZE_T_HEX_FORMAT "%lx"
/** @brief Fallback ptrdiff_t format string */
#define PTRDIFF_T_FORMAT "%ld"
#endif

#ifndef C89STRINGUTILS_HAVE_ERRNO_T
/** @brief POSIX errno_t alias for systems missing it */
typedef int errno_t;
#endif

#ifndef C89STRINGUTILS_HAVE_RSIZE_T
/** @brief POSIX rsize_t alias for systems missing it */
typedef size_t rsize_t;
#endif

#ifndef C89STRINGUTILS_INLINE
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define C89STRINGUTILS_INLINE inline
#elif defined(__GNUC__) || defined(__clang__) || defined(__TINYC__)
#define C89STRINGUTILS_INLINE __inline__
#elif defined(_MSC_VER) || defined(__INTEL_COMPILER)
#define C89STRINGUTILS_INLINE __inline
#elif defined(__SUNPRO_C)
#if __SUNPRO_C < 0x590
#define C89STRINGUTILS_INLINE
#else
#define C89STRINGUTILS_INLINE inline
#endif
#else
#define C89STRINGUTILS_INLINE
#endif
#endif
/**
 * @brief Get error string safely.
 * @param s Buffer to store the error string.
 * @param maxsize Maximum size of the buffer.
 * @param errnum Error number.
 * @return 0 on success, non-zero on error.
 */
#if defined(C89STRINGUTILS_HAVE_STRERROR_S)
#define c89stringutils_strerror_s strerror_s
#else
extern C89STRINGUTILS_EXPORT errno_t c89stringutils_strerror_s(char *s,
                                                               rsize_t maxsize,
                                                               errno_t errnum);
#endif

/**
 * @brief Copy string safely.
 * @param dest Destination buffer.
 * @param destsz Size of the destination buffer.
 * @param src Source string.
 * @return 0 on success, non-zero on error.
 */
#if defined(C89STRINGUTILS_HAVE_STRCPY_S)
#define c89stringutils_strcpy_s strcpy_s
#else
extern C89STRINGUTILS_EXPORT errno_t c89stringutils_strcpy_s(char *dest,
                                                             rsize_t destsz,
                                                             const char *src);
#endif

/**
 * @brief Copy string safely up to a maximum length.
 * @param dest Destination buffer.
 * @param destsz Size of the destination buffer.
 * @param src Source string.
 * @param count Maximum number of characters to copy.
 * @return 0 on success, non-zero on error.
 */
#if defined(C89STRINGUTILS_HAVE_STRNCPY_S)
#define c89stringutils_strncpy_s strncpy_s
#else
extern C89STRINGUTILS_EXPORT errno_t c89stringutils_strncpy_s(char *dest,
                                                              rsize_t destsz,
                                                              const char *src,
                                                              rsize_t count);
#endif

/**
 * @brief Concatenate string safely.
 * @param dest Destination buffer.
 * @param destsz Size of the destination buffer.
 * @param src Source string.
 * @return 0 on success, non-zero on error.
 */
#if defined(C89STRINGUTILS_HAVE_STRCAT_S)
#define c89stringutils_strcat_s strcat_s
#else
extern C89STRINGUTILS_EXPORT errno_t c89stringutils_strcat_s(char *dest,
                                                             rsize_t destsz,
                                                             const char *src);
#endif

/**
 * @brief Concatenate string safely up to a maximum length.
 * @param dest Destination buffer.
 * @param destsz Size of the destination buffer.
 * @param src Source string.
 * @param count Maximum number of characters to append.
 * @return 0 on success, non-zero on error.
 */
#if defined(C89STRINGUTILS_HAVE_STRNCAT_S)
#define c89stringutils_strncat_s strncat_s
#else
extern C89STRINGUTILS_EXPORT errno_t c89stringutils_strncat_s(char *dest,
                                                              rsize_t destsz,
                                                              const char *src,
                                                              rsize_t count);
#endif

/**
 * @brief Write formatted output to a bounded string safely.
 * @param s Buffer to write to.
 * @param n Maximum number of characters to write.
 * @param format Format string.
 * @param arg va_list of arguments.
 * @return Number of characters written, or negative on error.
 */
#if defined(C89STRINGUTILS_HAVE_VSNPRINTF_S) && !defined(_WIN32)
#define c89stringutils_vsnprintf_s vsnprintf_s
#else
extern C89STRINGUTILS_EXPORT int
c89stringutils_vsnprintf_s(char *s, rsize_t n, const char *format, va_list arg)
    C89STRINGUTILS_FORMAT_PRINTF(3, 0);
#endif

/**
 * @brief Write formatted output to a bounded string safely.
 * @param s Buffer to write to.
 * @param n Maximum number of characters to write.
 * @param format Format string.
 * @param ... Arguments.
 * @return Number of characters written, or negative on error.
 */
#if defined(C89STRINGUTILS_HAVE_SNPRINTF_S) && !defined(_WIN32)
#define c89stringutils_snprintf_s snprintf_s
#else
extern C89STRINGUTILS_EXPORT int
c89stringutils_snprintf_s(char *s, rsize_t n, const char *format, ...)
    C89STRINGUTILS_FORMAT_PRINTF(3, 4);
#endif

/**
 * @brief Write formatted output to a bounded string (snprintf fallback).
 * @param s Buffer to write to.
 * @param n Maximum number of characters to write.
 * @param format Format string.
 * @param ... Arguments.
 * @return Number of characters written, or negative on error.
 */
extern C89STRINGUTILS_EXPORT int
c89stringutils_snprintf(char *s, size_t n, const char *format, ...)
    C89STRINGUTILS_FORMAT_PRINTF(3, 4);

/**
 * @brief Write formatted output to a bounded string using a va_list (vsnprintf
 * fallback).
 * @param s Buffer to write to.
 * @param n Maximum number of characters to write.
 * @param format Format string.
 * @param arg va_list of arguments.
 * @return Number of characters written, or negative on error.
 */
extern C89STRINGUTILS_EXPORT int
c89stringutils_vsnprintf(char *s, size_t n, const char *format, va_list arg)
    C89STRINGUTILS_FORMAT_PRINTF(3, 0);

/**
 * @brief Compare at most n characters of two strings, ignoring case.
 * @param s1 The first string to compare.
 * @param s2 The second string to compare.
 * @param n The maximum number of characters to compare.
 * @return An integer less than, equal to, or greater than zero if s1 is found,
 * respectively, to be less than, to match, or be greater than s2.
 */
#if defined(C89STRINGUTILS_HAVE__STRNICMP)
#define c89stringutils_strncasecmp(s1, s2, n)                                  \
  ((((const void *)(s1)) == NULL && ((const void *)(s2)) == NULL) ? 0          \
   : (((const void *)(s1)) == NULL)                               ? -1         \
   : (((const void *)(s2)) == NULL)                               ? 1          \
                                    : _strnicmp((s1), (s2), (n)))
#elif defined(C89STRINGUTILS_HAVE_STRNCASECMP)
#define c89stringutils_strncasecmp(s1, s2, n)                                  \
  ((((const void *)(s1)) == NULL && ((const void *)(s2)) == NULL) ? 0          \
   : (((const void *)(s1)) == NULL)                               ? -1         \
   : (((const void *)(s2)) == NULL)                               ? 1          \
                                    : strncasecmp((s1), (s2), (n)))
#else
extern C89STRINGUTILS_EXPORT int
c89stringutils_strncasecmp(const char *s1, const char *s2, size_t n);
#endif

/**
 * @brief Compare two strings, ignoring case.
 * @param s1 The first string to compare.
 * @param s2 The second string to compare.
 * @return An integer less than, equal to, or greater than zero if s1 is found,
 * respectively, to be less than, to match, or be greater than s2.
 */
#if defined(C89STRINGUTILS_HAVE__STRICMP)
#define c89stringutils_strcasecmp(s1, s2)                                      \
  ((((const void *)(s1)) == NULL && ((const void *)(s2)) == NULL) ? 0          \
   : (((const void *)(s1)) == NULL)                               ? -1         \
   : (((const void *)(s2)) == NULL)                               ? 1          \
                                    : _stricmp((s1), (s2)))
#elif defined(C89STRINGUTILS_HAVE_STRCASECMP)
#define c89stringutils_strcasecmp(s1, s2)                                      \
  ((((const void *)(s1)) == NULL && ((const void *)(s2)) == NULL) ? 0          \
   : (((const void *)(s1)) == NULL)                               ? -1         \
   : (((const void *)(s2)) == NULL)                               ? 1          \
                                    : strcasecmp((s1), (s2)))
#else
extern C89STRINGUTILS_EXPORT int c89stringutils_strcasecmp(const char *s1,
                                                           const char *s2);
#endif

/**
 * @brief Locate a substring in a string, looking at no more than len
 * characters.
 * @param buffer The string to search.
 * @param target The substring to find.
 * @param bufferLength The maximum number of characters to search.
 * @return A pointer to the first occurrence of little in big, or NULL if not
 * found.
 */
#if defined(C89STRINGUTILS_HAVE_STRNSTR)
#define c89stringutils_strnstr(b, t, l)                                        \
  ((((const void *)(b)) == NULL || ((const void *)(t)) == NULL)                \
       ? NULL                                                                  \
       : strnstr((b), (t), (l)))
#else
extern C89STRINGUTILS_EXPORT char *c89stringutils_strnstr(const char *buffer,
                                                          const char *target,
                                                          size_t bufferLength);
#endif

/**
 * @brief Locate a substring in a string, ignoring case.
 * @param h The string to search.
 * @param n The substring to find.
 * @return A pointer to the first occurrence of little in big, or NULL if not
 * found.
 */
#if defined(C89STRINGUTILS_HAVE_STRCASESTR)
#define c89stringutils_strcasestr(h, n)                                        \
  ((((const void *)(h)) == NULL || ((const void *)(n)) == NULL)                \
       ? NULL                                                                  \
       : strcasestr((h), (n)))
#else
extern C89STRINGUTILS_EXPORT char *c89stringutils_strcasestr(const char *h,
                                                             const char *n);
#endif

/**
 * @brief Get the length of a string describing an error number.
 * @param errnum The error number.
 * @return The length of the string describing the error.
 */
extern C89STRINGUTILS_EXPORT size_t
c89stringutils_strerrorlen_s(errno_t errnum);

/**
 * @brief Write formatted output to a dynamically allocated string using a
 * va_list.
 * @param str A pointer to a string pointer where the allocated string will be
 * stored.
 * @param fmt The format string.
 * @param ap The va_list of arguments.
 * @return 0 on success, or -1 on error.
 */
extern C89STRINGUTILS_EXPORT int
c89stringutils_vasprintf(char **str, const char *fmt, va_list ap)
    C89STRINGUTILS_FORMAT_PRINTF(2, 0);

/**
 * @brief Write formatted output to a dynamically allocated string.
 * @param str A pointer to a string pointer where the allocated string will be
 * stored.
 * @param fmt The format string.
 * @param ... The arguments.
 * @return 0 on success, or -1 on error.
 */
extern C89STRINGUTILS_EXPORT int c89stringutils_asprintf(char **str,
                                                         const char *fmt, ...)
    C89STRINGUTILS_FORMAT_PRINTF(2, 3);

/**
 * @brief `jasprintf`, a version of `asprintf` that concatenates on successive
 * calls: char *s = NULL; c89stringutils_jasprintf(&s, "foo%s", "bar");
 * c89stringutils_jasprintf(&s, "can%s", "haz"); free(s);
 * @param unto The string to append to.
 * @param fmt The format string.
 * @param ... The arguments.
 * @return 0 on success, or -1 on error.
 */
extern C89STRINGUTILS_EXPORT int c89stringutils_jasprintf(char **unto,
                                                          const char *fmt, ...)
    C89STRINGUTILS_FORMAT_PRINTF(2, 3);

#if !defined(C89STRINGUTILS_HAVE_VASPRINTF) ||                                 \
    !defined(C89STRINGUTILS_HAVE_ASPRINTF)
/**
 * @brief Write formatted output to a dynamically allocated string using a
 * va_list.
 * @param str A pointer to a string pointer where the allocated string will be
 * stored.
 * @param fmt The format string.
 * @param ap The va_list of arguments.
 * @return 0 on success, or -1 on error.
 */
extern C89STRINGUTILS_EXPORT int vasprintf(char **str, const char *fmt,
                                           va_list ap)
    C89STRINGUTILS_FORMAT_PRINTF(2, 0);

/**
 * @brief Write formatted output to a dynamically allocated string.
 * @param str A pointer to a string pointer where the allocated string will be
 * stored.
 * @param fmt The format string.
 * @param ... The arguments.
 * @return 0 on success, or -1 on error.
 */
extern C89STRINGUTILS_EXPORT int asprintf(char **str, const char *fmt, ...)
    C89STRINGUTILS_FORMAT_PRINTF(2, 3);
#endif

/**
 * @brief `jasprintf`, a version of `asprintf` that concatenates on successive
 * calls.
 * @param unto The string to append to.
 * @param fmt The format string.
 * @param ... The arguments.
 * @return 0 on success, or -1 on error.
 */
extern C89STRINGUTILS_EXPORT int jasprintf(char **unto, const char *fmt, ...)
    C89STRINGUTILS_FORMAT_PRINTF(2, 3);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !C89STRINGUTILS_STRING_EXTRAS_H */
