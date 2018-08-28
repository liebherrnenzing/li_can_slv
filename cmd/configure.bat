rm -r ../build
mkdir ../build
cmake -DCMAKE_SH="CMAKE_SH-NOTFOUND" -B../build -H.. -G"MinGW Makefiles"
