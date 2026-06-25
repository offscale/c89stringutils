/**
 * @file c89stringutils_export_pregen.h
 * @brief Export macros for older targets and non CMake using build systems on
 * MSVC
 */
#ifndef C89STRINGUTILS_EXPORT_H
#define C89STRINGUTILS_EXPORT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef C89STRINGUTILS_STATIC_DEFINE
/** @brief Define for static build */
#define C89STRINGUTILS_EXPORT
/** @brief Define for static build, explicitly no export */
#define C89STRINGUTILS_NO_EXPORT
#else
#ifndef C89STRINGUTILS_EXPORT
#ifdef c89stringutils_EXPORTS
/* We are building this library */
#if defined(BUILD_SHARED)
/** @brief Export macro when building the shared library */
#define C89STRINGUTILS_EXPORT __declspec(dllexport)
#else
/** @brief Export macro when building the library */
#define C89STRINGUTILS_EXPORT
#endif
#else
/* We are using this library */
#if defined(BUILD_SHARED)
/** @brief Export macro when using the shared library */
#define C89STRINGUTILS_EXPORT __declspec(dllimport)
#else
/** @brief Export macro when using the library */
#define C89STRINGUTILS_EXPORT
#endif
#endif
#endif

#ifndef C89STRINGUTILS_NO_EXPORT
/** @brief Macro for hidden symbols */
#define C89STRINGUTILS_NO_EXPORT
#endif
#endif

#ifndef C89STRINGUTILS_DEPRECATED
#if defined(BUILD_SHARED)
/** @brief Macro for deprecated symbols in shared build */
#define C89STRINGUTILS_DEPRECATED __declspec(deprecated)
#else
/** @brief Macro for deprecated symbols */
#define C89STRINGUTILS_DEPRECATED
#endif
#endif

#ifndef C89STRINGUTILS_DEPRECATED_EXPORT
/** @brief Macro for deprecated exported symbols */
#define C89STRINGUTILS_DEPRECATED_EXPORT                                       \
  C89STRINGUTILS_EXPORT C89STRINGUTILS_DEPRECATED
#endif

#ifndef C89STRINGUTILS_DEPRECATED_NO_EXPORT
/** @brief Macro for deprecated hidden symbols */
#define C89STRINGUTILS_DEPRECATED_NO_EXPORT                                    \
  C89STRINGUTILS_NO_EXPORT C89STRINGUTILS_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#ifndef C89STRINGUTILS_NO_DEPRECATED
/** @brief Macro to disable deprecation warnings */
#define C89STRINGUTILS_NO_DEPRECATED
#endif
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* C89STRINGUTILS_EXPORT_H */
