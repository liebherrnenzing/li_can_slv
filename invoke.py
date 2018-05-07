tasks = {
    "auto_dash_names": False
}

path = {
    "gnu_arm": "c:\\Program Files (x86)\\GNU Tools ARM Embedded\\4.9 2015q2\\bin",
    "mingw32": "c:\\MinGW32\bin",
    "gnu_arm_tools": "c:\\Program Files\\GNU ARM Eclipse\\Build Tools\\2.6-201507152002\\bin",
    "cmake": "c:\\Program Files\\CMake\\bin",
    "ninja": "c:\\PortableApps\\ninja-win",
}

make = {
    "gnu_arm_make": "{}\\make.exe".format(path["gnu_arm_tools"]),
    "mingw32_make": "{}\\mingw32-make.exe".format(path["mingw32"]),
    "ninja_make": "{}\\ninja.exe".format(path["ninja"]),
}

tools = {
    "doxygen": "c:\\Program Files\\doxygen\\bin\\doxygen.exe",
    "ant_bat": "c:\\v_infrastructure\\ant\\bin\\ant.bat",
    "cmake": "{}\\cmake.exe".format(path["cmake"]),
}
