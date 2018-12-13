call env_mingw32.bat
rm -r ../build_mingw
mkdir ../build_mingw
cmake -DCMAKE_SH="CMAKE_SH-NOTFOUND" -B../build_mingw -H.. -G"MinGW Makefiles"
