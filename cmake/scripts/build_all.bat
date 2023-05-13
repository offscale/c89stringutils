@echo on

call "%~dp0\build_cygwin.bat"
call "%~dp0\build_mingw32.bat"
call "%~dp0\build_mingw64.bat"
call "%~dp0\build_msvc64_2005.bat"
call "%~dp0\build_msvc64_2022.bat"
call "%~dp0\build_msvc86_2005.bat"
call "%~dp0\build_msvc86_2022.bat"
call "%~dp0\build_openwatcom_dos.bat"
