@echo on

IF NOT DEFINED VS2022INSTALLDIR (
  set VS2022INSTALLDIR="C:\PROGRA~1\Microsoft Visual Studio\2022\Enterprise"
)
call "%VS2022INSTALLDIR%\VC\Auxiliary\Build\vcvars64.bat"

set build_dir="build_msvc64_2022"
IF EXIST "%build_dir%" rd /s /q "%build_dir%"
md "%build_dir%"
cd "%build_dir%"
cmake -DCMAKE_BUILD_TYPE="Debug" -G "Visual Studio 17 2022" -A x64 .. && cmake --build . && ctest -C "Debug" && cpack -G "ZIP" -C "Debug" && ren *.zip *-msvc2022-win64.zip
cd ..