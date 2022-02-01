c89stringutils
==============
[![License](https://img.shields.io/badge/license-Apache--2.0%20OR%20MIT-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![CI for Linux, Windows, macOS, sunOS](https://github.com/offscale/c89stringutils/workflows/Linux,%20Windows,%20macOS%20,%20SunOS/badge.svg)](https://github.com/offscale/c89stringutils/actions)
[![CI for FreeBSD](https://api.cirrus-ci.com/github/offscale/c89stringutils.svg)](https://cirrus-ci.com/github/offscale/c89stringutils)
[![C89](https://img.shields.io/badge/C-89-blue)](https://en.wikipedia.org/wiki/C89_(C_version))

C89 is missing some nice things. As is MSVC.
This adds the string related functionality for Windows (particularly: MSVC) and is tested additionally on: SunOS, Linux, BSD, and macOS.

Everything is hidden behind `ifdef`s so if the compiler/OS supports the function, that function will be used instead of the one provided by this library.

Header only (to simplify including). Just `#define C89STRINGUTILS_IMPLEMENTATION` once-only in your program (before including the header).

### String functions implemented

  | Function                                                                | Citation                     |
----------------------------------------------------------------------------|------------------------------|
  | [`strcasestr`](https://www.freebsd.org/cgi/man.cgi?query=strcasestr)    | From MUSL                    |
  | [`strncasecmp`](https://www.freebsd.org/cgi/man.cgi?query=strncasecmp)  | Alias for MSVC's `_strnicmp` |
  | [`strcasecmp`](https://www.freebsd.org/cgi/man.cgi?query=strcasecmp)    | Alias for MSVC's `_stricmp`  |
  | [`snprintf`](https://www.freebsd.org/cgi/man.cgi?query=snprintf)        | Mostly from WTF_StringExtras |
  | [`vsnprintf`](https://www.freebsd.org/cgi/man.cgi?query=vsnprintf)      | Mostly from WTF_StringExtras |
  | [`strnstr`](https://www.freebsd.org/cgi/man.cgi?query=strnstr)          | Mostly from WTF_StringExtras |
  | [`strerrorlen_s`](https://en.cppreference.com/w/c/string/byte/strerror) | From Safe C Library          |
  | [`asprintf`](https://www.freebsd.org/cgi/man.cgi?query=asprintf)        | From libressl-portable       |

### Dependencies

- [CMake](https://cmake.org) (3.19 or later)
- C compiler (any that work with CMake, and were released within the last 30 years)

### Build

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

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
