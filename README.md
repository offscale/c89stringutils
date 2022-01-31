c89stringutils
==============
[![License](https://img.shields.io/badge/license-Apache--2.0%20OR%20MIT-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![CI for Linux, Windows, macOS](https://github.com/offscale/libacquire/workflows/CI%20for%20Linux,%20Windows,%20macOS/badge.svg)](https://github.com/offscale/c89stringutils/actions)
[![C89](https://img.shields.io/badge/C-89-blue)](https://en.wikipedia.org/wiki/C89_(C_version))

C89 is missing some nice things. As is MSVC.
This adds the string related functionality for Windows (particularly: MSVC), SunOS, Linux, BSD, and macOS.

Everything is hidden behind `ifdef`s so if the compiler/OS supports the function, that function will be used instead of the one provided by this library.

Header only (to simplify including).

### String functions implemented

  | Function        | Notes                        |
  |-----------------|------------------------------|
  | `strcasestr`    | From MUSL                    |
  | `strncasecmp`   | Alias for MSVC's `_strnicmp` |
  | `strcasecmp`    | Alias for MSVC's `_stricmp`  |
  | `snprintf`      | Mostly from Chromium         |
  | `vsnprintf`     | Mostly from Chromium         |
  | `strnstr`       | Mostly from Chromium         |
  | `strerrorlen_s` | From Safe C Library          |
  | `asprintf`      | From libressl-portable       |

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
