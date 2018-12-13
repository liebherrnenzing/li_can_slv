@echo off
rem Configuration file for Astyle. Can be downloaded from: https://sourceforge.net/projects/astyle/?source=typ_redirect
rem Example usage: C:\li_can_slv\source astyle.bat *.c *.h
set ASTYLE_BIN="c:\PortableApps\AStyle\bin\"
set ASTYLE_CODESTYLE_PARAMS=--mode=c --style=allman --indent=tab=4 --indent-switches --indent-col1-comments --pad-oper --pad-header --unpad-paren --lineend=windows --add-brackets --align-pointer=name --attach-extern-c
set ASTYLE_EXECUTION_PARAMS=--suffix=none --formatted --ignore-exclude-errors-x --recursive
echo.
echo ------------------ prepare astyle ---------------------------------------------
echo.
%ASTYLE_BIN%\AStyle.exe --version
echo   style params:  %ASTYLE_CODESTYLE_PARAMS%
echo   exec params :  %ASTYLE_EXECUTION_PARAMS%
echo   user params :  %*
echo   target dir  :  %CD%
echo.
echo ------------------ astyle execution -------------------------------------------
echo.
%ASTYLE_BIN%\AStyle.exe %ASTYLE_CODESTYLE_PARAMS% %ASTYLE_EXECUTION_PARAMS% %*
echo.
echo ------------------ astyle done ------------------------------------------------
