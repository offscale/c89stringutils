cmake_minimum_required(VERSION 3.11...3.28)
if (POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif ()

if ((CMAKE_SYSTEM_NAME STREQUAL "Windows" OR CMAKE_SYSTEM_NAME STREQUAL "DOS")
        AND CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    set(CMAKE_OSX_ARCHITECTURES "")
endif ()
if (CMAKE_SYSTEM_NAME STREQUAL "Windows" AND CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
endif ()

project(c89stringutils VERSION 0.0.4 LANGUAGES C)

string(TOUPPER "${PROJECT_NAME}" PROJECT_PREFIX)
string(REPLACE "-" "_" PROJECT_PREFIX "${PROJECT_PREFIX}")

# Consume optional shared CI variables to prevent unused-variable warnings
set(_c89su_unused_ci_vars
        "${CMAKE_BUILD_TYPE}"
        "${FETCHCONTENT_SOURCE_DIR_AUTO-WIN-MSVC}"
        "${FETCHCONTENT_SOURCE_DIR_AUTO_WIN_MSVC}"
        "${FETCHCONTENT_SOURCE_DIR_C-ABSTRACT-HTTP}"
        "${FETCHCONTENT_SOURCE_DIR_C_ABSTRACT_HTTP}"
        "${FETCHCONTENT_SOURCE_DIR_CDD-C}"
        "${FETCHCONTENT_SOURCE_DIR_CDD_C}"
        "${FETCHCONTENT_SOURCE_DIR_C-ORM}"
        "${FETCHCONTENT_SOURCE_DIR_C_ORM}"
        "${FETCHCONTENT_SOURCE_DIR_C-STR-SPAN}"
        "${FETCHCONTENT_SOURCE_DIR_C_STR_SPAN}"
        "${FETCHCONTENT_SOURCE_DIR_C89STRINGUTILS}"
        "${FETCHCONTENT_SOURCE_DIR_CFS}"
        "${FETCHCONTENT_SOURCE_DIR_C_FS}"
        "${FETCHCONTENT_SOURCE_DIR_PARSON}")
unset(_c89su_unused_ci_vars)

if (CMAKE_SYSTEM_NAME STREQUAL "DOS")
    if (NOT DEFINED CMAKE_INSTALL_LIBDIR)
        set(CMAKE_INSTALL_LIBDIR "lib" CACHE PATH "Object code libraries")
    endif ()
endif ()

if (CMAKE_SYSTEM_NAME STREQUAL "DOS" AND CMAKE_C_COMPILER_ID STREQUAL "OpenWatcom")
    get_filename_component(_ow_cc_name "${CMAKE_C_COMPILER}" NAME)
    if (_ow_cc_name MATCHES "^wcl(\\.exe)?$")
        if (NOT CMAKE_C_FLAGS MATCHES "(^| )-m[smlchf]( |$)")
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mm -zm -zc -zt=128")
        endif ()
        if (NOT CMAKE_EXE_LINKER_FLAGS MATCHES "stack=")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} option stack=16384")
        endif ()
    endif ()
endif ()

if (MSVC)
    string(REGEX REPLACE "/W[0-4]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    string(REGEX REPLACE "-W[0-4]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
endif ()

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
set(CMAKE_C_STANDARD 90)
if (NOT CMAKE_C_COMPILER_ID STREQUAL "OpenWatcom")
    set(CMAKE_C_EXTENSIONS OFF)
    if (NOT DEFINED MSVC_VERSION OR MSVC_VERSION STRGREATER "1900")
        set(CMAKE_C_STANDARD_REQUIRED ON)
    endif ()
endif ()

add_library("${PROJECT_NAME}_compiler_flags" INTERFACE)
if ((NOT DEFINED MSVC_VERSION OR MSVC_VERSION STRGREATER "1900")
        AND NOT CMAKE_C_COMPILER_ID STREQUAL "OpenWatcom")
    target_compile_features("${PROJECT_NAME}_compiler_flags" INTERFACE "c_std_${CMAKE_C_STANDARD}")
endif ()
if (((DEFINED MSVC_VERSION AND MSVC_VERSION STRLESS_EQUAL "1900")
        OR CMAKE_C_COMPILER_ID STREQUAL "OpenWatcom"
        OR CYGWIN)
        AND NOT EXISTS "${PROJECT_BINARY_DIR}/c89stringutils_export.h")
    file(COPY "${PROJECT_SOURCE_DIR}/c89stringutils/c89stringutils_export_pregen.h"
            DESTINATION "${PROJECT_BINARY_DIR}")
    file(RENAME "${PROJECT_BINARY_DIR}/c89stringutils_export_pregen.h"
            "${PROJECT_BINARY_DIR}/c89stringutils_export.h")
endif ()
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.15")
    set(gcc_like "$<OR:$<C_COMPILER_ID:ARMClang>,$<C_COMPILER_ID:AppleClang>,$<C_COMPILER_ID:Clang>,$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:LCC>>")
    set(msvc "$<C_COMPILER_ID:MSVC>")
    target_compile_options(
            "${PROJECT_NAME}_compiler_flags"
            INTERFACE
            "$<${gcc_like}:$<BUILD_INTERFACE:-Werror;-Wall;-Wextra;-Wpedantic;-Wshadow;-Wformat=2;-Wconversion;-Wcast-align;-Wstrict-prototypes;-Wold-style-definition;-Wmissing-prototypes;-Wmissing-declarations;-Wundef;-Wpointer-arith;-Wcast-qual;-Wwrite-strings>>"
            "$<${msvc}:$<BUILD_INTERFACE:-W4;-WX;$<$<VERSION_GREATER_EQUAL:$<C_COMPILER_VERSION>,19.10>:-permissive->>>"
    )
    target_link_options(
            "${PROJECT_NAME}_compiler_flags"
            INTERFACE
            "$<$<AND:${gcc_like},$<CONFIG:Debug>>:$<BUILD_INTERFACE:>>"
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

if (NOT MSVC AND NOT CMAKE_C_COMPILER_ID STREQUAL "OpenWatcom")
    set(CMAKE_REQUIRED_DEFINITIONS "-D_GNU_SOURCE")
endif ()

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

set(_saved_cmake_required_flags "${CMAKE_REQUIRED_FLAGS}")
if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -Wformat -Werror")
elseif (MSVC)
    set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} /W4 /WX")
endif ()

check_c_source_compiles("
    #include <stdio.h>
    #include <stddef.h>
    int main(void) {
        char buf[32];
        size_t s = 0;
        #if defined(_MSC_VER) && _MSC_VER >= 1400
        return sprintf_s(buf, sizeof(buf), \"%zu\", s);
        #else
        return sprintf(buf, \"%zu\", s);
        #endif
    }
" C89STRINGUTILS_HAVE_PRINTF_ZU)

check_c_source_compiles("
    #include <stdio.h>
    #if defined(__GNUC__) || defined(__clang__)
    __extension__ typedef long long test_ll_t;
    #else
    typedef long long test_ll_t;
    #endif
    int main(void) {
        char buf[32];
        test_ll_t s = 0;
        #if defined(_MSC_VER) && _MSC_VER >= 1400
        return sprintf_s(buf, sizeof(buf), \"%lld\", s);
        #else
        return sprintf(buf, \"%lld\", s);
        #endif
    }
" C89STRINGUTILS_HAVE_PRINTF_LLD)

check_c_source_compiles("
    #include <stdio.h>
    int main(void) {
        char buf[32];
        __int64 s = 0;
        #if defined(_MSC_VER) && _MSC_VER >= 1400
        return sprintf_s(buf, sizeof(buf), \"%I64d\", s);
        #else
        return sprintf(buf, \"%I64d\", s);
        #endif
    }
" C89STRINGUTILS_HAVE_PRINTF_I64D)

check_c_source_compiles("
    #include <stdio.h>
    #include <stddef.h>
    int main(void) {
        char buf[32];
        size_t s = 0;
        #if defined(_MSC_VER) && _MSC_VER >= 1400
        return sprintf_s(buf, sizeof(buf), \"%Iu\", s);
        #else
        return sprintf(buf, \"%Iu\", s);
        #endif
    }
" C89STRINGUTILS_HAVE_PRINTF_IU)

set(CMAKE_REQUIRED_FLAGS "${_saved_cmake_required_flags}")

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
if (MSVC OR CMAKE_C_COMPILER_ID STREQUAL "OpenWatcom")
    set(CMAKE_REQUIRED_DEFINITIONS)
endif ()

check_symbol_exists(strerror_s "string.h" C89STRINGUTILS_HAVE_STRERROR_S)
check_symbol_exists(strerror_r "string.h" C89STRINGUTILS_HAVE_STRERROR_R)
set(CMAKE_REQUIRED_DEFINITIONS)

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
  set(C89STRINGUTILS_HAVE_VFSCANF OFF)
  set(C89STRINGUTILS_HAVE_VSSCANF OFF)
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
            char c = *strerror_r(0, buf, sizeof(buf));
            return (int)c;
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
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.h"
)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    option(BUILD_TESTING_${PROJECT_PREFIX} "Build the tests" ON)
    option(${PROJECT_PREFIX}_BUILD_TESTING "Build the tests" ON)
else()
    option(BUILD_TESTING_${PROJECT_PREFIX} "Build the tests" OFF)
    option(${PROJECT_PREFIX}_BUILD_TESTING "Build the tests" OFF)
endif()

if (DEFINED CDD_CHARSET AND NOT CDD_CHARSET STREQUAL "")
    set(_default_charset "${CDD_CHARSET}")
elseif (CMAKE_SYSTEM_NAME STREQUAL "DOS")
    set(_default_charset "ANSI")
else ()
    set(_default_charset "Unicode")
endif ()
set(${PROJECT_PREFIX}_CHARSET "${_default_charset}" CACHE STRING "Charset to use: Unicode or ANSI")

if (DEFINED CDD_THREADING AND NOT CDD_THREADING STREQUAL "")
    set(_default_threading "${CDD_THREADING}")
else ()
    set(_default_threading "Multi-threaded")
endif ()
set(${PROJECT_PREFIX}_THREADING "${_default_threading}" CACHE STRING "Threading: Multi-threaded or Single-threaded")

if (DEFINED CDD_MSVC_RTC)
    set(_default_msvc_rtc "${CDD_MSVC_RTC}")
else ()
    set(_default_msvc_rtc "")
endif ()
set(${PROJECT_PREFIX}_MSVC_RTC "${_default_msvc_rtc}" CACHE STRING "MSVC Runtime Checks: RTC1, RTCs, RTCu, OFF, or empty")

if (DEFINED CDD_LTO)
    set(_default_lto "${CDD_LTO}")
else ()
    set(_default_lto OFF)
endif ()
set(${PROJECT_PREFIX}_LTO "${_default_lto}" CACHE BOOL "Enable Link-Time Optimization (LTO)")

if (DEFINED CDD_DEPS AND NOT CDD_DEPS STREQUAL "")
    set(_default_deps "${CDD_DEPS}")
else ()
    set(_default_deps "System")
endif ()
set(${PROJECT_PREFIX}_DEPS "${_default_deps}" CACHE STRING "Dependency resolution strategy")

if (DEFINED CDD_CRT_LINKAGE)
    set(_default_crt_linkage "${CDD_CRT_LINKAGE}")
else ()
    set(_default_crt_linkage "")
endif ()
set(${PROJECT_PREFIX}_CRT_LINKAGE "${_default_crt_linkage}" CACHE STRING "MSVC CRT Linkage: Static or Shared")

set(_effective_crt_linkage "${${PROJECT_PREFIX}_CRT_LINKAGE}")
if (_effective_crt_linkage STREQUAL "" AND DEFINED CDD_CRT_LINKAGE)
    set(_effective_crt_linkage "${CDD_CRT_LINKAGE}")
endif ()

set(_effective_msvc_rtc "${${PROJECT_PREFIX}_MSVC_RTC}")
if (_effective_msvc_rtc STREQUAL "" AND DEFINED CDD_MSVC_RTC)
    set(_effective_msvc_rtc "${CDD_MSVC_RTC}")
endif ()

if (MSVC)
    foreach(flag_var
            CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
        string(REGEX REPLACE "[/-]M[TD]d?" "" ${flag_var} "${${flag_var}}")
    endforeach()

    string(TOUPPER "${_effective_crt_linkage}" _crt_linkage_upper)
    if (_crt_linkage_upper STREQUAL "STATIC")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    elseif (_crt_linkage_upper STREQUAL "SHARED")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    elseif (NOT DEFINED CMAKE_MSVC_RUNTIME_LIBRARY OR CMAKE_MSVC_RUNTIME_LIBRARY STREQUAL "")
        if ((DEFINED VCPKG_CRT_LINKAGE AND VCPKG_CRT_LINKAGE STREQUAL "static")
                OR (DEFINED VCPKG_TARGET_TRIPLET AND VCPKG_TARGET_TRIPLET MATCHES "-static$" AND NOT DEFINED VCPKG_CRT_LINKAGE))
            set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        else ()
            set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
        endif ()
    endif ()

    string(TOUPPER "${_effective_msvc_rtc}" _msvc_rtc_upper)
    if (_msvc_rtc_upper STREQUAL "OFF" OR _msvc_rtc_upper STREQUAL "NONE")
        foreach(flag_var
                CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
                CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
            string(REGEX REPLACE "[/-]RTC[1su]+" "" ${flag_var} "${${flag_var}}")
        endforeach()
    elseif (_effective_msvc_rtc MATCHES "^RTC(1|s|u|su|us)$")
        foreach(flag_var
                CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
                CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
            string(REGEX REPLACE "[/-]RTC[1su]+" "" ${flag_var} "${${flag_var}}")
        endforeach()
        add_compile_options("$<$<CONFIG:Debug>:/${_effective_msvc_rtc}>")
    endif ()
endif ()

string(TOUPPER "${${PROJECT_PREFIX}_CHARSET}" _charset_upper)
if (_charset_upper STREQUAL "UNICODE")
    add_compile_definitions(UNICODE _UNICODE)
endif ()

string(TOUPPER "${${PROJECT_PREFIX}_THREADING}" _threading_upper)
if (_threading_upper STREQUAL "SINGLE-THREADED"
        OR _threading_upper STREQUAL "SINGLE"
        OR _threading_upper STREQUAL "OFF"
        OR _threading_upper STREQUAL "FALSE"
        OR _threading_upper STREQUAL "0")
    add_compile_definitions(C89STRINGUTILS_SINGLE_THREADED=1)
else ()
    add_compile_definitions(C89STRINGUTILS_MULTI_THREADED=1)
endif ()

if (CMAKE_INTERPROCEDURAL_OPTIMIZATION OR ${PROJECT_PREFIX}_LTO OR CDD_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported)
    if (ipo_supported)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    else ()
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION FALSE)
    endif ()
endif ()

add_subdirectory("${PROJECT_NAME}")

if (BUILD_TESTING_${PROJECT_PREFIX} OR ${PROJECT_PREFIX}_BUILD_TESTING)
    enable_testing()
    include(CTest)
    file(WRITE "${PROJECT_BINARY_DIR}/CTestCustom.cmake"
        "set(CTEST_CUSTOM_COVERAGE_EXCLUDE \".*tests/.*\" \".*test_.*\" \".*CMakeFiles.*\" \".*greatest.*\" \".*stb_sprintf\\\\.h\")\n"
    )
    if (BUILD_TESTING)
        add_subdirectory("${PROJECT_NAME}/tests")
    endif()
endif()

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
        VERSION "${${PROJECT_NAME}_VERSION}"
        COMPATIBILITY AnyNewerVersion
)
install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}")
