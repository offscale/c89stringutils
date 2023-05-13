@echo on

IF NOT DEFINED VSINSTALLDIR (
  set VSINSTALLDIR="C:\PROGRA~2\Microsoft Visual Studio 8"
)
call "C:\PROGRA~2\Microsoft Visual Studio 8\VC\vcvarsall.bat"

set build_dir="build_msvc86_2005"
IF EXIST "%build_dir%" rd /s /q "%build_dir%"
md "%build_dir%"
cd "%build_dir%"
cmake -DCMAKE_BUILD_TYPE="Debug" -G "NMake Makefiles" .. && cmake --build . && ctest -C "Debug" && cpack -G "ZIP" -C "Debug" && ren *.zip *-msvc2005-win32.zip
cd ..
