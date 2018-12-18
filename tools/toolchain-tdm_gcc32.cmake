# the name of the target operating system
SET(CMAKE_SYSTEM_NAME Windows)

# which compilers to use for C and C++
SET(TOOLCHAIN_PATH "c://MinGW//bin//")
SET(CMAKE_C_COMPILER ${TOOLCHAIN_PATH}/gcc.exe)
SET(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/g++.exe)
SET(CMAKE_RC_COMPILER ${TOOLCHAIN_PATH}/windres.exe)

# here is the target environment located
SET(CMAKE_FIND_ROOT_PATH /MinGW/bin)

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search
# programs in the host environment
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)



