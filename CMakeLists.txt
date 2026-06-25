cmake_minimum_required(VERSION 3.11...3.28)
project(c89stringutils VERSION 0.0.2 LANGUAGES C)

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
set(CMAKE_C_STANDARD 90)

add_library("${PROJECT_NAME}_compiler_flags" INTERFACE)
if (NOT DEFINED MSVC_VERSION
        OR MSVC_VERSION STRGREATER "1900" # 2015
        OR NOT (CMAKE_C_COMPILER_ID STREQUAL "OpenWatcom"))
    target_compile_features("${PROJECT_NAME}_compiler_flags" INTERFACE "c_std_${CMAKE_C_STANDARD}")
elseif ((MSVC_VERSION STRLESS_EQUAL "1900"
         OR CMAKE_C_COMPILER_ID STREQUAL "OpenWatcom"
         OR CYGWIN)
        AND NOT EXISTS "${CMAKE_BINARY_DIR}/c89stringutils_export.h")
    file(COPY "${CMAKE_SOURCE_DIR}/c89stringutils/c89stringutils_export_pregen.h"
            DESTINATION "${CMAKE_BINARY_DIR}")
    file(RENAME "${CMAKE_BINARY_DIR}/c89stringutils_export_pregen.h"
            "${CMAKE_BINARY_DIR}/c89stringutils_export.h")
endif ()
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.15")
    set(gcc_like "$<COMPILE_LANG_AND_ID:C,CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
    set(msvc "$<COMPILE_LANG_AND_ID:C,CXX,MSVC>")
    target_compile_options(
            "${PROJECT_NAME}_compiler_flags"
            INTERFACE
            "$<$<AND:${gcc_like},$<CONFIG:Debug>>:$<BUILD_INTERFACE:-Wshadow;-Wformat=2;-Wall;-Wno-missing-braces;-Wno-long-long;-pedantic;-fprofile-arcs;-ftest-coverage>>"
            "$<${msvc}:$<BUILD_INTERFACE:-W4;-WX;-Zi;-permissive->>"
    )
    target_link_options(
            "${PROJECT_NAME}_compiler_flags"
            INTERFACE
            "$<$<AND:${gcc_like},$<CONFIG:Debug>>:$<BUILD_INTERFACE:--coverage>>"
    )
endif (CMAKE_VERSION VERSION_GREATER_EQUAL "3.15")
# Set the build directories
if (CMAKE_SYSTEM_NAME STREQUAL "Windows"
        OR CMAKE_SYSTEM_NAME STREQUAL "CYGWIN"
        OR CMAKE_SYSTEM_NAME MATCHES "MINGW.*")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
else ()
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
endif ()

include(CheckIncludeFile)
include(CheckSymbolExists)
include(CheckTypeSize)
include(CheckCSourceCompiles)

check_include_file(strings.h C89STRINGUTILS_HAVE_STRINGS_H)
check_include_file(sys/param.h C89STRINGUTILS_HAVE_SYS_PARAM_H)
check_include_file(wchar.h C89STRINGUTILS_HAVE_WCHAR_H)

check_symbol_exists(strncpy_s "string.h" C89STRINGUTILS_HAVE_STRNCPY_S)
check_symbol_exists(strcpy_s "string.h" C89STRINGUTILS_HAVE_STRCPY_S)
check_symbol_exists(strncat_s "string.h" C89STRINGUTILS_HAVE_STRNCAT_S)
check_symbol_exists(strcat_s "string.h" C89STRINGUTILS_HAVE_STRCAT_S)
check_symbol_exists(strlcpy "string.h" C89STRINGUTILS_HAVE_STRLCPY)
check_symbol_exists(strlcat "string.h" C89STRINGUTILS_HAVE_STRLCAT)
check_symbol_exists(strnlen "string.h" C89STRINGUTILS_HAVE_STRNLEN)
check_symbol_exists(strnlen_s "string.h" C89STRINGUTILS_HAVE_STRNLEN_S)
check_symbol_exists(memcpy_s "string.h" C89STRINGUTILS_HAVE_MEMCPY_S)
check_symbol_exists(set_constraint_handler_s "stdlib.h" C89STRINGUTILS_HAVE_SET_CONSTRAINT_HANDLER_S)
check_symbol_exists(reallocarray "stdlib.h" C89STRINGUTILS_HAVE_REALLOCARRAY)
check_symbol_exists(tmpfile_s "stdio.h" C89STRINGUTILS_HAVE_TMPFILE_S)
check_symbol_exists(fopen_s "stdio.h" C89STRINGUTILS_HAVE_FOPEN_S)
check_symbol_exists(freopen_s "stdio.h" C89STRINGUTILS_HAVE_FREOPEN_S)

check_symbol_exists(vsscanf "stdio.h;stdarg.h" C89STRINGUTILS_HAVE_VSSCANF)
check_symbol_exists(vsscanf_s "stdio.h;stdarg.h" C89STRINGUTILS_HAVE_VSSCANF_S)
check_symbol_exists(vfscanf "stdio.h;stdarg.h" C89STRINGUTILS_HAVE_VFSCANF)
check_symbol_exists(vfscanf_s "stdio.h;stdarg.h" C89STRINGUTILS_HAVE_VFSCANF_S)

check_symbol_exists(wcscpy "wchar.h" C89STRINGUTILS_HAVE_WCSCPY)
check_symbol_exists(wcsncpy "wchar.h" C89STRINGUTILS_HAVE_WCSNCPY)
check_symbol_exists(wcscpy_s "wchar.h" C89STRINGUTILS_HAVE_WCSCPY_S)
check_symbol_exists(wcsncpy_s "wchar.h" C89STRINGUTILS_HAVE_WCSNCPY_S)
check_symbol_exists(vswprintf "wchar.h;stdarg.h" C89STRINGUTILS_HAVE_VSWPRINTF)
check_symbol_exists(vswprintf_s "wchar.h;stdarg.h" C89STRINGUTILS_HAVE_VSWPRINTF_S)
check_symbol_exists(swprintf "wchar.h;stdarg.h" C89STRINGUTILS_HAVE_SWPRINTF)
check_symbol_exists(swprintf_s "wchar.h;stdarg.h" C89STRINGUTILS_HAVE_SWPRINTF_S)

check_symbol_exists(sprintf_s "stdio.h" C89STRINGUTILS_HAVE_SPRINTF_S)
check_symbol_exists(vsnprintf "stdio.h;stdarg.h" C89STRINGUTILS_HAVE_VSNPRINTF)
check_symbol_exists(_vsnprintf "stdio.h;stdarg.h" C89STRINGUTILS_HAVE__VSNPRINTF)
check_symbol_exists(vsnprintf_s "stdio.h;stdarg.h" C89STRINGUTILS_HAVE_VSNPRINTF_S)
check_symbol_exists(_vsnprintf_s "stdio.h;stdarg.h" C89STRINGUTILS_HAVE__VSNPRINTF_S)
check_symbol_exists(snprintf_s "stdio.h" C89STRINGUTILS_HAVE_SNPRINTF_S)
check_symbol_exists(_snprintf_s "stdio.h" C89STRINGUTILS_HAVE__SNPRINTF_S)
check_symbol_exists(_snprintf "stdio.h" C89STRINGUTILS_HAVE__SNPRINTF)
check_symbol_exists(vfprintf_s "stdio.h;stdarg.h" C89STRINGUTILS_HAVE_VFPRINTF_S)
check_symbol_exists(fprintf_s "stdio.h" C89STRINGUTILS_HAVE_FPRINTF_S)
check_symbol_exists(_vscprintf "stdio.h;stdarg.h" C89STRINGUTILS_HAVE__VSCPRINTF)
check_symbol_exists(snprintf "stdio.h" C89STRINGUTILS_HAVE_SNPRINTF)

check_c_source_compiles("
    #include <stdio.h>
    #include <stddef.h>
    int main(void) {
        char buf[32];
        size_t s = 0;
        sprintf(buf, \"%zu\", s);
        return 0;
    }
" C89STRINGUTILS_HAVE_PRINTF_ZU)

check_c_source_compiles("
    #include <stdio.h>
    int main(void) {
        char buf[32];
        long long s = 0;
        sprintf(buf, \"%lld\", s);
        return 0;
    }
" C89STRINGUTILS_HAVE_PRINTF_LLD)

check_c_source_compiles("
    #include <stdio.h>
    int main(void) {
        char buf[32];
        __int64 s = 0;
        sprintf(buf, \"%I64d\", s);
        return 0;
    }
" C89STRINGUTILS_HAVE_PRINTF_I64D)

check_c_source_compiles("
    #include <stdio.h>
    #include <stddef.h>
    int main(void) {
        char buf[32];
        size_t s = 0;
        sprintf(buf, \"%Iu\", s);
        return 0;
    }
" C89STRINGUTILS_HAVE_PRINTF_IU)

check_c_source_compiles("
    #include <stdio.h>
    #include <stdarg.h>
    int main(void) {
        /* This isn't a perfect runtime check, but checks if compiler complains */
        return 0;
    }
" C89STRINGUTILS_HAVE_VSNPRINTF_NULL)

set(STRING_HEADERS "string.h")
if(C89STRINGUTILS_HAVE_STRINGS_H)
    list(APPEND STRING_HEADERS "strings.h")
endif()

check_symbol_exists(strncasecmp "${STRING_HEADERS}" C89STRINGUTILS_HAVE_STRNCASECMP)
check_symbol_exists(_strnicmp "${STRING_HEADERS}" C89STRINGUTILS_HAVE__STRNICMP)

check_symbol_exists(strcasecmp "${STRING_HEADERS}" C89STRINGUTILS_HAVE_STRCASECMP)
check_symbol_exists(_stricmp "${STRING_HEADERS}" C89STRINGUTILS_HAVE__STRICMP)

set(CMAKE_REQUIRED_DEFINITIONS "-D_GNU_SOURCE")
check_symbol_exists(strnstr "string.h" C89STRINGUTILS_HAVE_STRNSTR)
check_symbol_exists(strcasestr "string.h" C89STRINGUTILS_HAVE_STRCASESTR)

check_symbol_exists(vasprintf "stdio.h" C89STRINGUTILS_HAVE_VASPRINTF)
check_symbol_exists(asprintf "stdio.h" C89STRINGUTILS_HAVE_ASPRINTF)
set(CMAKE_REQUIRED_DEFINITIONS)

check_symbol_exists(strerror_s "string.h" C89STRINGUTILS_HAVE_STRERROR_S)
check_symbol_exists(strerror_r "string.h" C89STRINGUTILS_HAVE_STRERROR_R)

option(C89STRINGUTILS_FORCE_FALLBACKS "Force fallbacks for coverage testing" OFF)
if(C89STRINGUTILS_FORCE_FALLBACKS)
  set(C89STRINGUTILS_HAVE_STRNCPY_S OFF)
  set(C89STRINGUTILS_HAVE_STRCPY_S OFF)
  set(C89STRINGUTILS_HAVE_STRNCAT_S OFF)
  set(C89STRINGUTILS_HAVE_STRCAT_S OFF)
  set(C89STRINGUTILS_HAVE_STRLCPY OFF)
  set(C89STRINGUTILS_HAVE_STRLCAT OFF)
  set(C89STRINGUTILS_HAVE_STRNLEN OFF)
  set(C89STRINGUTILS_HAVE_STRNLEN_S OFF)
  set(C89STRINGUTILS_HAVE_MEMCPY_S OFF)
  set(C89STRINGUTILS_HAVE_SET_CONSTRAINT_HANDLER_S OFF)
  set(C89STRINGUTILS_HAVE_REALLOCARRAY OFF)
  set(C89STRINGUTILS_HAVE_TMPFILE_S OFF)
  set(C89STRINGUTILS_HAVE_FOPEN_S OFF)
  set(C89STRINGUTILS_HAVE_FREOPEN_S OFF)
  set(C89STRINGUTILS_HAVE_VSSCANF_S OFF)
  set(C89STRINGUTILS_HAVE_VFSCANF_S OFF)
  set(C89STRINGUTILS_HAVE_WCSCPY_S OFF)
  set(C89STRINGUTILS_HAVE_WCSNCPY_S OFF)
  set(C89STRINGUTILS_HAVE_VSWPRINTF_S OFF)
  set(C89STRINGUTILS_HAVE_SWPRINTF_S OFF)
  set(C89STRINGUTILS_HAVE_SPRINTF_S OFF)
  set(C89STRINGUTILS_HAVE_VSNPRINTF_S OFF)
  set(C89STRINGUTILS_HAVE__VSNPRINTF_S OFF)
  set(C89STRINGUTILS_HAVE_SNPRINTF_S OFF)
  set(C89STRINGUTILS_HAVE__SNPRINTF_S OFF)
  set(C89STRINGUTILS_HAVE_VFPRINTF_S OFF)
  set(C89STRINGUTILS_HAVE_FPRINTF_S OFF)
  set(C89STRINGUTILS_HAVE_SNPRINTF OFF)
  set(C89STRINGUTILS_HAVE__SNPRINTF OFF)
  set(C89STRINGUTILS_HAVE_VSNPRINTF OFF)
  set(C89STRINGUTILS_HAVE__VSNPRINTF OFF)
  set(C89STRINGUTILS_HAVE_STRNCASECMP OFF)
  set(C89STRINGUTILS_HAVE__STRNICMP OFF)
  set(C89STRINGUTILS_HAVE_STRCASECMP OFF)
  set(C89STRINGUTILS_HAVE__STRICMP OFF)
  set(C89STRINGUTILS_HAVE_STRNSTR OFF)
  set(C89STRINGUTILS_HAVE_STRCASESTR OFF)
  set(C89STRINGUTILS_HAVE_VASPRINTF OFF)
  set(C89STRINGUTILS_HAVE_ASPRINTF OFF)
  set(C89STRINGUTILS_HAVE_STRERROR_S OFF)
  set(C89STRINGUTILS_HAVE_STRERROR_R OFF)
  add_compile_definitions(C89STRINGUTILS_FORCE_FALLBACKS)
endif()

if(C89STRINGUTILS_HAVE_STRERROR_R)
    set(CMAKE_REQUIRED_DEFINITIONS "-D_GNU_SOURCE")
    check_c_source_compiles("
        #include <string.h>
        int main(void) {
            char buf[256];
            char *res = strerror_r(0, buf, sizeof(buf));
            return res != 0;
        }
    " C89STRINGUTILS_STRERROR_R_CHAR_P)
    set(CMAKE_REQUIRED_DEFINITIONS)
endif()

check_c_source_compiles("
    #include <stdarg.h>
    int main(void) { va_list a, b; va_copy(a, b); return 0; }
" C89STRINGUTILS_HAVE_VA_COPY)

check_c_source_compiles("
    #include <stdarg.h>
    int main(void) { va_list a, b; __va_copy(a, b); return 0; }
" C89STRINGUTILS_HAVE___VA_COPY)

check_c_source_compiles("
    void my_printf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
    int main(void) { return 0; }
" C89STRINGUTILS_HAVE_ATTR_FORMAT)

check_c_source_compiles("
    void my_func(const char *s) __attribute__((nonnull(1)));
    int main(void) { return 0; }
" C89STRINGUTILS_HAVE_ATTR_NONNULL)

check_c_source_compiles("
    int main(void) { int a = 1; typeof(a) b = 2; return b; }
" C89STRINGUTILS_HAVE_TYPEOF)

check_c_source_compiles("
    #include <sal.h>
    _Ret_notnull_ void* my_func(_In_z_ const char* s) { return (void*)s; }
    int main(void) { return 0; }
" C89STRINGUTILS_HAVE_SAL)

check_c_source_compiles("
    #pragma warning(push)
    #pragma warning(disable: 4127)
    int main(void) { return 0; }
" C89STRINGUTILS_HAVE_PRAGMA_WARNING)

check_c_source_compiles("
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored \"-Wunused-parameter\"
    int main(void) { return 0; }
" C89STRINGUTILS_HAVE_PRAGMA_GCC_DIAGNOSTIC)

check_type_size("long long" SIZEOF_LONG_LONG)
if(HAVE_SIZEOF_LONG_LONG)
    set(C89STRINGUTILS_HAVE_LONG_LONG 1)
endif()
check_type_size("__int64" SIZEOF___INT64)
if(HAVE_SIZEOF___INT64)
    set(C89STRINGUTILS_HAVE___INT64 1)
endif()

check_c_source_compiles("
    #include <errno.h>
    #include <stddef.h>
    int main(void) { errno_t e = 0; return (int)e; }
" C89STRINGUTILS_HAVE_ERRNO_T)

check_c_source_compiles("
    #include <stddef.h>
    int main(void) { rsize_t s = 0; return (int)s; }
" C89STRINGUTILS_HAVE_RSIZE_T)

# configure a header file to pass the version number only
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.in"
        "${PROJECT_NAME}Config.h"
)

add_subdirectory("${PROJECT_NAME}")

option(BUILD_TESTING_${PROJECT_NAME} "Build the tests" ON)

set(CDD_CHARSET "Unicode" CACHE STRING "Charset to use: Unicode or ANSI")
set(CDD_THREADING "Multi-threaded" CACHE STRING "Threading: Multi-threaded or Single-threaded")
set(CDD_MSVC_RTC "" CACHE STRING "MSVC Runtime Checks: RTC1, RTCs, RTCu, or empty")
set(CDD_LTO OFF CACHE BOOL "Enable Link-Time Optimization (LTO)")
set(CDD_DEPS "System" CACHE STRING "Dependency resolution strategy")

set(CDD_CRT_LINKAGE "Shared" CACHE STRING "MSVC CRT Linkage: Static or Shared")
if (MSVC)
    if (CDD_CRT_LINKAGE STREQUAL "Static")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    else()
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    endif()
endif()


if (CDD_CHARSET STREQUAL "Unicode")
    add_compile_definitions(UNICODE _UNICODE)
endif()
if (MSVC AND CDD_MSVC_RTC)
    add_compile_options("/${CDD_MSVC_RTC}")
endif()
if (CDD_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported)
    if(ipo_supported)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
endif()

if (BUILD_TESTING_${PROJECT_NAME})
    include(CTest)
    if (BUILD_TESTING)
        add_subdirectory("${PROJECT_NAME}/tests")
    endif (BUILD_TESTING)
endif (BUILD_TESTING_${PROJECT_NAME})

###########
# Install #
###########

include(GNUInstallDirs)

install(
        FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
if(CMAKE_HOST_WIN32)
    include(InstallRequiredSystemLibraries)
endif()
set(CPACK_BUNDLE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "Samuel Marks")
set(CPACK_PACKAGE_DESCRIPTION "string functions from newer standards / common non-standards for C89")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${CPACK_PACKAGE_DESCRIPTION}")
if (APPLE)
    set(CPACK_BUNDLE_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Info.plist")
    set(CPACK_BUNDLE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Info.plist")
    set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CustomVolumeIcon.icns")
endif (APPLE)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/LICENSE.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${${PROJECT_NAME}_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${${PROJECT_NAME}_VERSION_MINOR}")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/cmake/README.txt")
set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Welcome.txt")
set(CPACK_PACKAGE_CONTACT "https://github.com/offscale/${PROJECT_NAME}")

include(CPack)
include(CMakePackageConfigHelpers)

# generate the config file that is includes the exports
configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}"
        NO_SET_AND_CHECK_MACRO
        NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

# generate the version file for the config file
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}"
        COMPATIBILITY AnyNewerVersion
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}")
