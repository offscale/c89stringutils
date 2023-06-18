/* for older targets and non CMake using build systems on MSVC */
#ifndef C89STRINGUTILS_EXPORT_H
#define C89STRINGUTILS_EXPORT_H

#ifdef C89STRINGUTILS_STATIC_DEFINE
#define C89STRINGUTILS_EXPORT
#define C89STRINGUTILS_NO_EXPORT
#else
#ifndef C89STRINGUTILS_EXPORT
#ifdef c89stringutils_EXPORTS
/* We are building this library */
#if defined(BUILD_SHARED)
#define C89STRINGUTILS_EXPORT __declspec(dllexport)
#else
#define C89STRINGUTILS_EXPORT
#endif
#endif
#else
/* We are using this library */
#if defined(BUILD_SHARED)
#define C89STRINGUTILS_EXPORT __declspec(dllimport)
#else
#define C89STRINGUTILS_EXPORT
#endif
#endif

#ifndef C89STRINGUTILS_NO_EXPORT
#define C89STRINGUTILS_NO_EXPORT
#endif
#endif

#ifndef C89STRINGUTILS_DEPRECATED
#if defined(BUILD_SHARED)
#define C89STRINGUTILS_DEPRECATED __declspec(deprecated)
#else
#define C89STRINGUTILS_DEPRECATED
#endif
#endif

#ifndef C89STRINGUTILS_DEPRECATED_EXPORT
#define C89STRINGUTILS_DEPRECATED_EXPORT                                       \
  C89STRINGUTILS_EXPORT C89STRINGUTILS_DEPRECATED
#endif

#ifndef C89STRINGUTILS_DEPRECATED_NO_EXPORT
#define C89STRINGUTILS_DEPRECATED_NO_EXPORT                                    \
  C89STRINGUTILS_NO_EXPORT C89STRINGUTILS_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#ifndef C89STRINGUTILS_NO_DEPRECATED
#define C89STRINGUTILS_NO_DEPRECATED
#endif
#endif

#endif /* C89STRINGUTILS_EXPORT_H */
