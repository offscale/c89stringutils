c89stringutils
==============
[![License](https://img.shields.io/badge/license-Apache--2.0%20OR%20MIT-blue.svg)](https://opensource.org/licenses/Apache-2.0)
![Doc Coverage](https://img.shields.io/badge/doc__coverage-100.00%25-brightgreen)
![Test Coverage](https://img.shields.io/badge/test__coverage-100.00%25-brightgreen)
[![CI for Linux, Windows, macOS](https://github.com/offscale/c89stringutils/actions/workflows/ci.yml/badge.svg)](https://github.com/offscale/c89stringutils/actions/workflows/ci.yml)
[![C89](https://img.shields.io/badge/C-89-blue)](https://en.wikipedia.org/wiki/C89_(C_version))

C89 is missing some nice things. As is MSVC.
`c89stringutils` provides portable string, formatting, and C11 Annex K ("Safe CRT") functionality across:

- **Windows**
    - MSVC 2005 through MSVC 2026+
    - MinGW
    - Cygwin
    - Open Watcom 2.0
- **DOS**
    - Open Watcom 2.0 — 16-bit real-mode (`I86`) and 32-bit protected-mode (`I386`) across MS-DOS 1.25, 2.00, 4.00, and FreeDOS 1.3+
- **SunOS / Solaris**
- **Linux**
- **\*BSD**
- **macOS**

All public symbols are namespaced under `c89stringutils_*` to prevent linkage collisions (with unprefixed `asprintf`, `vasprintf`, and `jasprintf` also provided when missing on the host). Feature detection via CMake (or `C89STRINGUTILS_NO_CMAKE_CONFIG` standalone mode) delegates to native OS/CRT routines whenever available and falls back to strict ISO C89 implementations otherwise.

### Architecture & Standards

- **Strict C89 Compliance:** The entire codebase strictly conforms to ISO C90 (C89) (`-std=c89 -Wpedantic -Wall -Wextra -Werror` on GCC/Clang, `/W4 /WX /permissive-` on MSVC), with 16-bit-clean size and far-pointer handling for real-mode DOS.
- **Header Guards & C++ Interop:** Public headers (`c89stringutils_string_extras.h`, `c89stringutils_safecrt.h`, `c89stringutils_log.h`, and `c89stringutils_amalgamation.h`) wrap declarations in `extern "C"` blocks placed cleanly after `#include` directives for direct C++98+ compatibility.
- **Windows & Safe CRT Security:** Avoids `<windows.h>` altogether, defaults to MSVC Safe CRT routines (e.g., `_vsnprintf_s`, `_vscprintf`, `strcpy_s`) when available, and provides configurable C11 Annex K runtime-constraint handlers.
- **Standalone (No-CMake) Mode:** Supports direct compilation without CMake by defining `C89STRINGUTILS_NO_CMAKE_CONFIG`, activating built-in feature detection for MSVC, GCC, Clang, Open Watcom, TinyCC, Oracle/SunPro, Intel, and Borland toolchains.
- **Quality Metrics:** Maintains 100% Doxygen documentation coverage and 100% line and branch test coverage.

## Integration Examples

### 1. Single-File Header-Only Amalgamation (STB-style)

If you don't want to link against a built `static` or `shared` library, you can generate a drop-in, STB-style single-header amalgamation!

When configuring the project via CMake, enable `C89STRINGUTILS_BUILD_AMALGAMATION=ON`:
```sh
cmake -S . -B build -DC89STRINGUTILS_BUILD_AMALGAMATION=ON
```
This generates `build/c89stringutils_amalgamation.h`.

**To use the amalgamation in your project:**
Drop it into your source tree and define `C89STRINGUTILS_IMPLEMENTATION` in **exactly one** `.c` or `.cpp` file before including the header:

```c
#define C89STRINGUTILS_IMPLEMENTATION
#include "c89stringutils_amalgamation.h"
#include <stdlib.h>

int main(void) {
    char *str = NULL;
    if (c89stringutils_asprintf(&str, "Hello %s", "World") == 0) {
        free(str);
    }
    return 0;
}
```

### 2. CMake FetchContent

You can automatically download and integrate `c89stringutils` into your CMake project using `FetchContent`:

```cmake
include(FetchContent)

FetchContent_Declare(
    c89stringutils
    GIT_REPOSITORY https://github.com/offscale/c89stringutils.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(c89stringutils)

# Link against the namespaced alias target (plain `c89stringutils` also works)
target_link_libraries(your_target PRIVATE c89stringutils::c89stringutils)
```

### 3. vcpkg (Custom Registry)

You can manage `c89stringutils` via vcpkg using a custom registry pointing to the `project0` branch of the `SamuelMarks/vcpkg` fork. In your project root, create a `vcpkg-configuration.json` file:

```json
{
  "default-registry": {
    "kind": "git",
    "repository": "https://github.com/SamuelMarks/vcpkg.git",
    "reference": "project0"
  },
  "registries": [
    {
      "kind": "git",
      "repository": "https://github.com/microsoft/vcpkg.git",
      "reference": "master",
      "packages": [ "*" ]
    }
  ]
}
```

Add the dependency to your `vcpkg.json`:

```json
{
  "name": "your-project",
  "version": "1.0.0",
  "dependencies": [
    "c89stringutils"
  ]
}
```

Then locate and link the package in your `CMakeLists.txt`:

```cmake
find_package(c89stringutils CONFIG REQUIRED)
target_link_libraries(your_target PRIVATE c89stringutils::c89stringutils)
```

## API Overview

### String Extras (`c89stringutils_string_extras.h`)

| Function | Origin / Behavior |
|---|---|
| [`c89stringutils_strcasestr`](https://www.freebsd.org/cgi/man.cgi?query=strcasestr) | Native `strcasestr` or MUSL C89 fallback (NULL-safe) |
| [`c89stringutils_strncasecmp`](https://www.freebsd.org/cgi/man.cgi?query=strncasecmp) | Delegates to `_strnicmp` / `strncasecmp` or C89 fallback (NULL-safe) |
| [`c89stringutils_strcasecmp`](https://www.freebsd.org/cgi/man.cgi?query=strcasecmp) | Delegates to `_stricmp` / `strcasecmp` or C89 fallback (NULL-safe) |
| [`c89stringutils_strnstr`](https://www.freebsd.org/cgi/man.cgi?query=strnstr) | Native `strnstr` or custom C89 polyfill (NULL-safe) |
| [`c89stringutils_snprintf`](https://www.freebsd.org/cgi/man.cgi?query=snprintf) / [`c89stringutils_vsnprintf`](https://www.freebsd.org/cgi/man.cgi?query=vsnprintf) | C99-semantics bounded formatting (`stb_sprintf` / Safe CRT / C89 fallback) |
| [`c89stringutils_asprintf`](https://www.freebsd.org/cgi/man.cgi?query=asprintf) / `c89stringutils_vasprintf` | Heap-allocating formatted output (`asprintf` / `vasprintf` also declared when missing on host) |
| `c89stringutils_jasprintf` / `jasprintf` | Appending `asprintf` variant that concatenates onto an existing heap string on successive calls |
| [`c89stringutils_strerrorlen_s`](https://en.cppreference.com/w/c/string/byte/strerror) | Length of the `strerror` description for an `errno_t` (from Safe C Library) |

### Safe CRT & C11 Annex K (`c89stringutils_string_extras.h` & `c89stringutils_safecrt.h`)

| Function | Behavior |
|---|---|
| `c89stringutils_strcpy_s` / `c89stringutils_strncpy_s` | Bounds-checked string copy (supports `_TRUNCATE`) |
| `c89stringutils_strcat_s` / `c89stringutils_strncat_s` | Bounds-checked string concatenation (supports `_TRUNCATE`) |
| `c89stringutils_snprintf_s` / `c89stringutils_vsnprintf_s` | Bounds-checked formatted string output |
| `c89stringutils_strerror_s` | Bounds-checked error string lookup (`strerror_s` / `strerror_r` / `strerror` fallback) |
| `c89stringutils_sscanf_s` / `c89stringutils_fscanf_s` | Safe formatted string and stream scanning with minimal C89 `vsscanf`/`vfscanf` fallbacks |
| `c89stringutils_fopen_s` / `c89stringutils_freopen_s` / `c89stringutils_tmpfile_s` | Parameter-validated `FILE *` stream wrappers |
| `c89stringutils_wcscpy_s` / `c89stringutils_wcsncpy_s` / `c89stringutils_swprintf_s` / `c89stringutils_vswprintf_s` | Bounds-checked wide-character copy and formatting (when `<wchar.h>` is available) |
| `c89stringutils_set_constraint_handler_s` / `c89stringutils_invoke_constraint_handler_s` / `c89stringutils_abort_handler_s` / `c89stringutils_ignore_handler_s` | Configurable C11 Annex K runtime-constraint violation handlers |

### Portable Format Specifiers, Error Codes & Logging (`c89stringutils_string_extras.h` & `c89stringutils_log.h`)

- **Fixed/Pointer Width Types & Format Macros:** `c89stringutils_int64_t`, `c89stringutils_uint64_t`, `C89STRINGUTILS_INT64_C`, `C89STRINGUTILS_UINT64_C`, `C89STRINGUTILS_PRId64` / `PRIu64` / `PRIx64`, `C89STRINGUTILS_PRId32` / `PRIu32` / `PRIx32`, `NUM_FORMAT`, `UNUM_FORMAT`, `SIZE_T_FORMAT`, `SIZE_T_HEX_FORMAT`, `PTRDIFF_T_FORMAT`, `C89STRINGUTILS_PRI_PTR`, `C89STRINGUTILS_PRI_LDBL`.
- **Error Enum & Types:** `c89stringutils_error_t` (`C89STRINGUTILS_SUCCESS`, `C89STRINGUTILS_EINVAL`, `C89STRINGUTILS_ERANGE`, `C89STRINGUTILS_STRUNCATE`, `C89STRINGUTILS_ENOMEM`, etc.), plus `errno_t` and `rsize_t` typedef fallbacks.
- **Debug Logging:** `c89stringutils_log_debug` (`LOG_DEBUG` macro in `c89stringutils_log.h`).

### Dependencies

- [CMake](https://cmake.org) (3.11 for MSVC 2005 or newer versions for other targets; optional when using `C89STRINGUTILS_NO_CMAKE_CONFIG` or the amalgamation)
- C89-compatible C compiler

### Configure, build, and test

```sh
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build -C Debug --output-on-failure
```

**CMake Configuration Options:**
* `C89STRINGUTILS_BUILD_TESTING` / `BUILD_TESTING_C89STRINGUTILS` / `BUILD_TESTING`: `ON` or `OFF` (defaults to `ON` when built as the top-level project).
* `C89STRINGUTILS_BUILD_AMALGAMATION`: `ON` or `OFF` (generates `c89stringutils_amalgamation.h` in the build directory).
* `C89STRINGUTILS_CHARSET`: `Unicode` (sets `UNICODE` and `_UNICODE`; default on non-DOS targets) or `ANSI` (default on DOS).
* `C89STRINGUTILS_CRT_LINKAGE`: `Static` (`/MT`, `/MTd`) or `Shared` (`/MD`, `/MDd`) on MSVC; also respects `VCPKG_CRT_LINKAGE` and `*-static` triplets.
* `C89STRINGUTILS_THREADING`: `Multi-threaded` (default) or `Single-threaded` (`OFF` / `FALSE` / `0`).
* `C89STRINGUTILS_LTO`: `ON` or `OFF` (checks IPO support and configures `CMAKE_INTERPROCEDURAL_OPTIMIZATION`).
* `C89STRINGUTILS_MSVC_RTC`: `RTC1`, `RTCs`, `RTCu`, `RTCsu`, `RTCus`, or `OFF` (configures `/RTC*` flags on Debug MSVC builds).
* `C89STRINGUTILS_FORCE_FALLBACKS`: `ON` or `OFF` (forces pure C89 fallback implementations over native symbols for testing and coverage).

### Script to automatically build

Build, test, and package for:

- Cygwin;
- MinGW (32-bit and 64-bit);
- MSVC 2005 (32-bit and 64-bit);
- MSVC 2022 / 2026 (32-bit and 64-bit);
- OpenWatcom's DOS targets (16-bit `I86` and 32-bit `I386`).

…by running in Command Prompt: https://github.com/offscale/win-cmake-multi-build

(also handles upload to GitHub Releases)

#### Instructions for MSVC 2005

With cmake-3.11.4 specified, do:

```sh
mkdir build_msvc2005 && cd build_msvc2005
cmake-3.11.4-win64-x64\bin\cmake -DCMAKE_WARN_VS8=OFF -DCMAKE_BUILD_TYPE="Debug" -G "Visual Studio 8 2005" ..
cmake-3.11.4-win64-x64\bin\cmake --build .
cmake-3.11.4-win64-x64\bin\ctest -C Debug
```

(the last two commands can be run by opening the solution in Visual Studio 2005)

Alternatively with newer versions of CMake (tested 3.26.3):

```sh
mkdir build_msvc_nmake2005 && cd build_msvc_nmake2005
cmake -DCMAKE_BUILD_TYPE="Debug" -G "NMake Makefiles" ..
cmake --build .
ctest -C Debug
```

#### Instructions for Open Watcom (DOS target)

With v2 from https://github.com/open-watcom/open-watcom-v2/releases installed:

```sh
[path]\WATCOM\owsetenv.bat
mkdir build_dos && cd build_dos
cmake -G "Watcom WMake" -DCMAKE_SYSTEM_NAME=DOS -DCMAKE_SYSTEM_PROCESSOR=I86 ..
cmake --build .
ctest -C Debug
```

(use `-DCMAKE_SYSTEM_PROCESSOR=I386` for 32-bit DOS; `ctest` requires a DOS host or emulator)

---

## License

Licensed under either of

- Apache License, Version 2.0 ([LICENSE-APACHE](LICENSE-APACHE) or <https://www.apache.org/licenses/LICENSE-2.0>)
- MIT license ([LICENSE-MIT](LICENSE-MIT) or <https://opensource.org/licenses/MIT>)

at your option.

### Contribution

Unless you explicitly state otherwise, any contribution intentionally submitted
for inclusion in the work by you, as defined in the Apache-2.0 license, shall be
dual licensed as above, without any additional terms or conditions.
