set RETURN_PATH=%cd%
cd source
call .\..\astyle.bat *.c %*
cd %RETURN_PATH%

set RETURN_PATH=%cd%
cd include
call .\..\astyle.bat *.h %*
cd %RETURN_PATH%

set RETURN_PATH=%cd%
cd verify
call .\..\astyle.bat *.h *.c %* --exclude=unity --exclude=xtfw
cd %RETURN_PATH%
