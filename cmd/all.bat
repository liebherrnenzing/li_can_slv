@echo off
call env_tdmgcc32.bat
mingw32-make --directory=../build unittest_prepare
mingw32-make --directory=../build all
