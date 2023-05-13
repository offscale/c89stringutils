@echo on

IF NOT DEFINED MINGW64INSTALLDIR (
  set MINGW64INSTALLDIR="C:\usr\msys64"
)

set build_dir="build_mingw64"
IF EXIST "%build_dir%" rd /s /q "%build_dir%"
md "%build_dir%"
cd "%build_dir%"

set pth=%CD:~2,99%
set pth=%pth:\=/%
set drv=%CD:~0,1%
set "fullpath=/%drv%%pth%"

call %MINGW64INSTALLDIR%"\msys2_shell.cmd" -mingw64 -c "cd '%fullpath%' && cmake -DCMAKE_BUILD_TYPE='Debug' .. && cmake --build . && ctest -C 'Debug' && cpack -G 'ZIP' -C 'Debug'"
cd ..
