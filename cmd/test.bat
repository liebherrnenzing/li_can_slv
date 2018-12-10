@echo off
call env_mingw32.bat
mingw32-make --directory=../build_mingw unittest_prepare
mingw32-make --directory=../build_mingw all
mingw32-make --directory=../build_mingw unittest_run
