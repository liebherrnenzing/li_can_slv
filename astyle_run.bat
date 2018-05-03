set RETURN_PATH=%cd%
cd source
call .\..\astyle.bat *.h *.c %*
cd %RETURN_PATH%

set RETURN_PATH=%cd%
cd verify
call .\..\astyle.bat *.h *.c %* --exclude=unity
cd %RETURN_PATH%
