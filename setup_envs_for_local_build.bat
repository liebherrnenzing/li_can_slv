
REM setup environmemt variables for local build

SET PATH_LI_CAN_SLV_GCC=D:\toolchains\xpacks\@xpack-dev-tools\gcc\11.2.0-1.3\.content\bin
SET PATH_LI_CAN_SLV_MAKE=D:\toolchains\xpacks\@xpack-dev-tools\windows-build-tools\4.2.1-2.1\.content\bin
SET PATH_LI_CAN_SLV_CMAKE=D:\toolchains\xpacks\@xpack-dev-tools\cmake\3.20.6-2.1\.content\bin
SET PATH_LI_CAN_SLV_NINJA=D:\toolchains\xpacks\@xpack-dev-tools\ninja-build\1.10.2-5.1\.content\bin\
SET PATH_LI_CAN_SLV_RUBY=D:\toolchains\ruby\Ruby34-x64\bin\
SET PATH_LI_CAN_SLV_DOXYGEN=D:\tools\doxygen\bin\

SET PATH=%PATH_LI_CAN_SLV_GCC%;%PATH_LI_CAN_SLV_MAKE%;%PATH_LI_CAN_SLV_CMAKE%;%PATH_LI_CAN_SLV_NINJA%;%PATH_LI_CAN_SLV_RUBY%;%PATH_LI_CAN_SLV_DOXYGEN%;%PATH%;

REM use python environment or comment if not needed
CALL d:\toolchains\python_envs\py37_li_can_slv_2\Scripts\activate.bat
