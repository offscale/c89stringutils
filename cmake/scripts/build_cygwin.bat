@echo on

IF NOT DEFINED CYGWININSTALLDIR (
  set CYGWININSTALLDIR="C:\usr\cygwin64"
)

set build_dir="build_cygwin"
IF EXIST "%build_dir%" rd /s /q "%build_dir%"
md "%build_dir%"
cd "%build_dir%"

set pth=%CD:~2,99%
set pth=%pth:\=/%
set drv=%CD:~0,1%
set "fullpath=/cygdrive/%drv%%pth%"

%CYGWININSTALLDIR%"\bin\bash" --login -c "cd '%fullpath%' && cmake -DCMAKE_BUILD_TYPE='Debug' -DCMAKE_MAKE_PROGRAM=$(cygpath $(where ninja)) -G 'Ninja' .. && cmake --build . && ctest -C 'Debug' && cpack -G 'ZIP' -C 'Debug'"
cd ..
