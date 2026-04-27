c89stringutils
==============
[![License](https://img.shields.io/badge/license-Apache--2.0%20OR%20MIT-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![CI for Linux, Windows, macOS](https://github.com/offscale/c89stringutils/actions/workflows/ci.yml/badge.svg)](https://github.com/offscale/c89stringutils/actions/workflows/ci.yml)
[![C89](https://img.shields.io/badge/C-89-blue)](https://en.wikipedia.org/wiki/C89_(C_version))

C89 is missing some nice things. As is MSVC.
This adds the string related functionality for:

- **Windows**
    - MSVC 2005 through MSVC 2026+
    - MinGW
    - Cygwin
    - Open Watcom 2.0 (including **DOS** target)
- **SunOS**
- **Linux**
- ***BSD**
- **macOS**

Everything is hidden behind `ifdef`s so if the compiler/OS supports the function, that function will be used instead of
the one provided by this library.

### Architecture & Standards

- **Strict C89 Compliance:** The entire codebase strictly conforms to ISO C90 (C89), preventing compilation issues on extremely old enterprise toolchains.
- **Header Guards & C++ Interop:** Fully instrumented with `extern "C"` blocks and robust guards, playing nicely when directly `#include`d into C++ sources.
- **Windows Security:** Eradicates the inclusion of heavy platform headers like `<windows.h>`, and actively defaults to MSVC "Safe CRT" variants (e.g., `vsprintf_s`, `_vscprintf`) implicitly when building on Microsoft compilers.
- **Quality Metrics:** Maintains 100% Doxygen documentation and 100% test coverage locally.

### Single-File Header-Only Amalgamation (STB-style)

If you don't want to link against a built `static` or `shared` library, you can generate a drop-in, STB-style single-header amalgamation!

When building the project via CMake, toggle `C89STRINGUTILS_BUILD_AMALGAMATION=ON`:
```sh
cmake -B build -DC89STRINGUTILS_BUILD_AMALGAMATION=ON
```
This generates `build/c89stringutils/c89stringutils_amalgamation.h`.

**To use the amalgamation in your project:**
Drop it into your source tree and define `C89STRINGUTILS_IMPLEMENTATION` in **exactly one** `.c` or `.cpp` file before including the header:

```c
#define C89STRINGUTILS_IMPLEMENTATION
#include "c89stringutils_amalgamation.h"

int main(void) {
    char* str = NULL;
    asprintf(&str, "Hello %s", "World");
    free(str);
    return 0;
}
```

### String functions implemented

| Function                                                                | Citation                     |
|-------------------------------------------------------------------------|------------------------------|
| [`strcasestr`](https://www.freebsd.org/cgi/man.cgi?query=strcasestr)    | From MUSL                    |
| [`strncasecmp`](https://www.freebsd.org/cgi/man.cgi?query=strncasecmp)  | Alias for MSVC's `_strnicmp` |
| [`strcasecmp`](https://www.freebsd.org/cgi/man.cgi?query=strcasecmp)    | Alias for MSVC's `_stricmp`  |
| [`snprintf`](https://www.freebsd.org/cgi/man.cgi?query=snprintf)        | Mostly from WTF_StringExtras |
| [`vsnprintf`](https://www.freebsd.org/cgi/man.cgi?query=vsnprintf)      | Mostly from WTF_StringExtras |
| [`strnstr`](https://www.freebsd.org/cgi/man.cgi?query=strnstr)          | Mostly from WTF_StringExtras |
| [`strerrorlen_s`](https://en.cppreference.com/w/c/string/byte/strerror) | From Safe C Library          |
| [`asprintf`](https://www.freebsd.org/cgi/man.cgi?query=asprintf)        | From libressl-portable       |

Additionally `jasprintf`, a version of `asprintf` that concatenates on successive calls.

### Dependencies

- [CMake](https://cmake.org) (3.11 for MSVC 2005 or newer versions for other targets)
- C compiler (any that work with CMake, and were released within the last 30 years)

### Configure, build, and test

```sh
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build -C Debug
```

**Advanced CMake Toggles:**
The build environment seamlessly supports several CDD-style switches out of the box:
* `CDD_CHARSET`: `Unicode` or `ANSI` (Sets `UNICODE` and `_UNICODE` defines).
* `CDD_THREADING`: `ON` or `OFF`.
* `CDD_LTO`: `ON` or `OFF` (Validates and sets `CMAKE_INTERPROCEDURAL_OPTIMIZATION`).
* `CDD_MSVC_RTC`: `RTC1`, `RTCu`, or `RTCs` (Automatically injects `/RTCx` flags on MSVC).

### Script to automatically build

Build, test, and package for:

- Cygwin;
- MinGW (32-bit and 64-bit);
- MSVC 2005 (32-bit and 64-bit);
- MSVC 2022 / 2026 (32-bit and 64-bit);
- OpenWatcom's DOS target (16-bit).

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
cmake -G "Watcom WMake" -D CMAKE_SYSTEM_NAME "DOS" -D CMAKE_SYSTEM_PROCESSOR "I86" ..
cmake --build .
ctest -C Debug
```

(that test phase might fail if you're running this on a non-DOS host)

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
