#ifndef C89STRINGUTILS_SAFECRT_H
#define C89STRINGUTILS_SAFECRT_H

/**
 * @file c89stringutils_safecrt.h
 * @brief Implementations of C11 Annex K safe CRT functions and constraint
 * handlers.
 */

#include "c89stringutils_string_extras.h"
#include <stddef.h>
#include <stdio.h>
#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(C89STRINGUTILS_HAVE_SET_CONSTRAINT_HANDLER_S)
#include <stdlib.h>
#define c89stringutils_constraint_handler_t constraint_handler_t
#define c89stringutils_set_constraint_handler_s set_constraint_handler_s
#define c89stringutils_abort_handler_s abort_handler_s
#define c89stringutils_ignore_handler_s ignore_handler_s
#else
/**
 * @brief Constraint handler callback type.
 * @param msg Constraint violation message.
 * @param ptr Pointer to additional info (usually NULL).
 * @param error The error code.
 */
typedef void (*c89stringutils_constraint_handler_t)(const char *msg, void *ptr,
                                                    errno_t error);

/**
 * @brief Sets a new constraint handler.
 * @param handler The new handler to set.
 * @return The previously configured handler.
 */
C89STRINGUTILS_EXPORT c89stringutils_constraint_handler_t
c89stringutils_set_constraint_handler_s(
    c89stringutils_constraint_handler_t handler);

/**
 * @brief Default constraint handler that prints to stderr and aborts.
 * @param msg Constraint violation message.
 * @param ptr Pointer to additional info (usually NULL).
 * @param error The error code.
 */
C89STRINGUTILS_EXPORT void
c89stringutils_abort_handler_s(const char *msg, void *ptr, errno_t error);

/**
 * @brief Constraint handler that silently ignores the violation.
 * @param msg Constraint violation message.
 * @param ptr Pointer to additional info (usually NULL).
 * @param error The error code.
 */
C89STRINGUTILS_EXPORT void
c89stringutils_ignore_handler_s(const char *msg, void *ptr, errno_t error);
#endif

/**
 * @brief Invokes the currently configured constraint handler.
 * @param msg Constraint violation message.
 * @param ptr Pointer to additional info (usually NULL).
 * @param error The error code.
 */
C89STRINGUTILS_EXPORT void
c89stringutils_invoke_constraint_handler_s(const char *msg, void *ptr,
                                           errno_t error);

/**
 * @brief Safe sscanf wrapper.
 * @param buffer The input string.
 * @param format The format string.
 * @param ... Additional arguments.
 * @return The number of items scanned.
 */
C89STRINGUTILS_EXPORT int c89stringutils_sscanf_s(const char *buffer,
                                                  const char *format, ...);

/**
 * @brief Safe fscanf wrapper.
 * @param stream The input stream.
 * @param format The format string.
 * @param ... Additional arguments.
 * @return The number of items scanned.
 */
C89STRINGUTILS_EXPORT int c89stringutils_fscanf_s(FILE *stream,
                                                  const char *format, ...);

/**
 * @brief Safe fopen wrapper.
 * @param pFile Pointer to the file pointer.
 * @param filename The filename.
 * @param mode The access mode.
 * @return 0 on success, or an error code.
 */
C89STRINGUTILS_EXPORT errno_t c89stringutils_fopen_s(FILE **pFile,
                                                     const char *filename,
                                                     const char *mode);

/**
 * @brief Safe freopen wrapper.
 * @param pFile Pointer to the file pointer.
 * @param filename The filename.
 * @param mode The access mode.
 * @param stream The stream to reopen.
 * @return 0 on success, or an error code.
 */
C89STRINGUTILS_EXPORT errno_t c89stringutils_freopen_s(FILE **pFile,
                                                       const char *filename,
                                                       const char *mode,
                                                       FILE *stream);

/**
 * @brief Safe tmpfile wrapper.
 * @param pFile Pointer to the file pointer.
 * @return 0 on success, or an error code.
 */
C89STRINGUTILS_EXPORT errno_t c89stringutils_tmpfile_s(FILE **pFile);

/* Wide character support */
#if defined(C89STRINGUTILS_HAVE_WCHAR_H)
#if defined(C89STRINGUTILS_HAVE_WCSCPY_S)
#define c89stringutils_wcscpy_s wcscpy_s
#else
/**
 * @brief Safe wcscpy wrapper.
 * @param dest The destination wide string.
 * @param destsz The destination size.
 * @param src The source wide string.
 * @return 0 on success, or an error code.
 */
C89STRINGUTILS_EXPORT errno_t c89stringutils_wcscpy_s(wchar_t *dest,
                                                      rsize_t destsz,
                                                      const wchar_t *src);
#endif

#if defined(C89STRINGUTILS_HAVE_WCSNCPY_S)
#define c89stringutils_wcsncpy_s wcsncpy_s
#else
/**
 * @brief Safe wcsncpy wrapper.
 * @param dest The destination wide string.
 * @param destsz The destination size.
 * @param src The source wide string.
 * @param count The number of characters to copy.
 * @return 0 on success, or an error code.
 */
C89STRINGUTILS_EXPORT errno_t c89stringutils_wcsncpy_s(wchar_t *dest,
                                                       rsize_t destsz,
                                                       const wchar_t *src,
                                                       rsize_t count);
#endif

#if defined(C89STRINGUTILS_HAVE_SWPRINTF_S)
#define c89stringutils_swprintf_s swprintf_s
#else
/**
 * @brief Safe swprintf wrapper.
 * @param buffer The destination buffer.
 * @param sizeOfBuffer The destination size.
 * @param format The format string.
 * @param ... Additional arguments.
 * @return The number of characters written, or -1 on error.
 */
C89STRINGUTILS_EXPORT int c89stringutils_swprintf_s(wchar_t *buffer,
                                                    rsize_t sizeOfBuffer,
                                                    const wchar_t *format, ...);
#endif

#if defined(C89STRINGUTILS_HAVE_VSWPRINTF_S)
#define c89stringutils_vswprintf_s vswprintf_s
#else
/**
 * @brief Safe vswprintf wrapper.
 * @param buffer The destination buffer.
 * @param sizeOfBuffer The destination size.
 * @param format The format string.
 * @param argptr The va_list of arguments.
 * @return The number of characters written, or -1 on error.
 */
C89STRINGUTILS_EXPORT int c89stringutils_vswprintf_s(wchar_t *buffer,
                                                     rsize_t sizeOfBuffer,
                                                     const wchar_t *format,
                                                     va_list argptr);
#endif
#endif /* C89STRINGUTILS_HAVE_WCHAR_H */

#ifdef __cplusplus
}
#endif

#endif
